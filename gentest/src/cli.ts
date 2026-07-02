/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import * as fs from 'node:fs/promises';
import {dirname, parse, relative, resolve} from 'path';
import * as process from 'node:process';
import {fileURLToPath, pathToFileURL} from 'url';
import {stdin, stdout} from 'node:process';
import {randomUUID} from 'node:crypto';
import minimist from 'minimist';
import readline from 'node:readline/promises';
import signedsource from 'signedsource';
import {glob} from 'glob';

import type {LayoutNode} from './types.ts';
import {createChromePool} from './ChromePool.ts';
import buildLayoutTree from './buildLayoutTree.ts';
import {CppEmitter} from './emitters/CppEmitter.ts';
import {JavaEmitter} from './emitters/JavaEmitter.ts';
import {JavascriptEmitter} from './emitters/JavascriptEmitter.ts';

function addSignatureToSourceCode(sourceCode: string): string {
  const codeWithToken = sourceCode.replace(
    'MAGIC_PLACEHOLDER',
    signedsource.getSigningToken(),
  );

  return signedsource.signFile(codeWithToken);
}

const argv = minimist(process.argv.slice(2));
const specificFixture = argv.f || argv.fixture;
const suspend = argv.s || argv.suspend;
const poolSize = parseInt(argv['pool-size'] || '8', 10);

const gentestDir = dirname(dirname(fileURLToPath(import.meta.url)));
const yogaDir = dirname(gentestDir);
const fixturesDir = `${gentestDir}/fixtures`;
const fontPath = pathToFileURL(
  resolve(gentestDir, 'fonts/Ahem.ttf'),
).toString();

// Discover fixtures
let fixtures: string[];
try {
  if (specificFixture != null) {
    const fixturePath = `${fixturesDir}/${specificFixture}.html`;
    await fs.access(fixturePath, fs.constants.F_OK);
    fixtures = [fixturePath];
  } else {
    fixtures = await glob(`${fixturesDir}/**/*.html`);
  }
} catch (e) {
  const errorMessage = e instanceof Error ? e.message : '';
  console.log(
    `Trying to access ${specificFixture}.html threw an exception. Executing against all fixtures. ${errorMessage}`,
  );
  fixtures = await glob(`${fixturesDir}/**/*.html`);
}

// Read template
const template = await fs.readFile(`${gentestDir}/test-template.html`, 'utf8');
const templateWithFont = template.replace('%FONT_PATH%', fontPath);

// Create Chrome pool
const pool = createChromePool(poolSize);

// Process all fixtures in parallel (limited by pool size)
async function processFixture(fixturePath: string): Promise<void> {
  // Acquire Chrome instance
  const driver = await pool.acquire();

  const fixture = await fs.readFile(fixturePath, 'utf8');
  const relativePath = relative(fixturesDir, fixturePath);
  const fileNameNoExtension = parse(relativePath).name;

  // Inject fixture into template
  const html = templateWithFont.replace('%FIXTURE%', fixture);

  // Write to temp file
  const tmpFile = `/tmp/gentest-${randomUUID()}.html`;
  await fs.writeFile(tmpFile, html);

  let ltrTree: LayoutNode[];
  let rtlTree: LayoutNode[];
  try {
    await driver.get('file://' + tmpFile);

    // Wait for @font-face fonts (Ahem) to finish loading before measuring
    await driver.executeAsyncScript(`
      var callback = arguments[arguments.length - 1];
      document.fonts.ready.then(callback);
    `);

    // Build LTR layout tree
    ltrTree = await buildLayoutTree(driver, 'ltr');

    // Build RTL layout tree (same page, just change direction)
    rtlTree = await buildLayoutTree(driver, 'rtl');
  } finally {
    pool.release(driver);
    // Clean up temp file
    await fs.unlink(tmpFile).catch(() => undefined);
  }

  // Build test cases from top-level elements
  const testCases = ltrTree.map((ltrNode, i) => ({
    name: ltrNode.id || 'INSERT_NAME_HERE',
    ltrLayout: ltrNode,
    rtlLayout: rtlTree[i],
    experiments: ltrNode.experiments,
    disabled: ltrNode.disabled,
  }));

  const cppCode = new CppEmitter().generateFixture(
    fileNameNoExtension,
    testCases,
  );
  const javaCode = new JavaEmitter().generateFixture(
    fileNameNoExtension,
    testCases,
  );
  const jsCode = new JavascriptEmitter().generateFixture(
    fileNameNoExtension,
    testCases,
  );

  // Sign and write output files
  await Promise.all([
    fs.writeFile(
      `${yogaDir}/tests/generated/${fileNameNoExtension}.cpp`,
      addSignatureToSourceCode(cppCode),
    ),
    fs.writeFile(
      `${yogaDir}/java/tests/generated/com/facebook/yoga/${fileNameNoExtension}.java`,
      addSignatureToSourceCode(javaCode),
    ),
    fs.writeFile(
      `${yogaDir}/javascript/tests/generated/${fileNameNoExtension}.test.ts`,
      addSignatureToSourceCode(jsCode),
    ),
  ]);

  console.log('Generated', fileNameNoExtension);

  if (suspend) {
    const rl = readline.createInterface({input: stdin, output: stdout});
    await rl.question('');
    rl.close();
  }
}

try {
  await Promise.all(fixtures.map(f => processFixture(f)));
} finally {
  await pool.shutdown();
}
