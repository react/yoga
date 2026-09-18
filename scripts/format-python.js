/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @noflow
 * @format
 */

'use strict';

const {findMetaTool, REPO_ROOT} = require('./format-utils');
const {spawnSync} = require('node:child_process');
const fs = require('node:fs');
const path = require('node:path');
const {globSync} = require('glob');

const RUFF_VERSION = '0.14.0';
const RUFF_ROOT = path.join(
  REPO_ROOT,
  'node_modules',
  '.cache',
  'yoga-format',
  `ruff-${RUFF_VERSION}`,
);
const PYTHON_GLOB = '**/*.py';
const IGNORE = ['**/build/**', '**/node_modules/**'];

function run(command, arguments_, options = {}) {
  const result = spawnSync(command, arguments_, {
    cwd: REPO_ROOT,
    stdio: options.quiet === true ? 'ignore' : 'inherit',
    ...options,
    env: {...(options.env ?? process.env), PWD: REPO_ROOT},
  });
  if (result.error != null) {
    if (
      options.allowMissingCommand === true &&
      result.error.code === 'ENOENT'
    ) {
      return 1;
    }
    throw result.error;
  }
  if (result.signal != null) {
    throw new Error(`${command} was terminated by ${result.signal}`);
  }
  return result.status ?? 1;
}

function findPython() {
  const candidates =
    process.platform === 'win32'
      ? [
          ['py', ['-3']],
          ['python', []],
        ]
      : [
          ['python3', []],
          ['python', []],
        ];
  return (
    candidates.find(
      ([command, prefixArguments]) =>
        run(
          command,
          [
            ...prefixArguments,
            '-c',
            'import sys; raise SystemExit(sys.version_info.major != 3)',
          ],
          {allowMissingCommand: true, quiet: true},
        ) === 0,
    ) ?? null
  );
}

function runRuff(command, prefixArguments, check, environment) {
  const files = globSync(PYTHON_GLOB, {cwd: REPO_ROOT, ignore: IGNORE});
  return run(
    command,
    [...prefixArguments, 'format', ...(check ? ['--check'] : []), ...files],
    {env: environment},
  );
}

function main() {
  const check = process.argv.includes('--check');
  if (process.env.RUFF != null && process.env.RUFF !== '') {
    process.exitCode = runRuff(process.env.RUFF, [], check, process.env);
    return;
  }

  const metaRuff = findMetaTool('tools', 'third-party', 'ruff', 'ruff');
  if (metaRuff != null) {
    process.exitCode = runRuff(
      metaRuff.command,
      metaRuff.prefixArguments,
      check,
      process.env,
    );
    return;
  }

  const python = findPython();
  if (python == null) {
    throw new Error('Python formatting requires Python 3 with pip.');
  }
  const [command, prefixArguments] = python;
  const pythonPath = [RUFF_ROOT, process.env.PYTHONPATH]
    .filter(Boolean)
    .join(path.delimiter);
  const environment = {...process.env, PYTHONPATH: pythonPath};
  if (
    run(
      command,
      [
        ...prefixArguments,
        '-c',
        `from importlib.metadata import version; raise SystemExit(version('ruff') != '${RUFF_VERSION}')`,
      ],
      {env: environment, quiet: true},
    ) !== 0
  ) {
    fs.mkdirSync(RUFF_ROOT, {recursive: true});
    const installStatus = run(command, [
      ...prefixArguments,
      '-m',
      'pip',
      'install',
      '--disable-pip-version-check',
      '--only-binary=:all:',
      `--target=${RUFF_ROOT}`,
      `ruff==${RUFF_VERSION}`,
    ]);
    if (installStatus !== 0) {
      process.exitCode = installStatus;
      return;
    }
  }
  process.exitCode = runRuff(
    command,
    [...prefixArguments, '-m', 'ruff'],
    check,
    environment,
  );
}

main();
