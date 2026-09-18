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

const {findJava, REPO_ROOT} = require('./format-utils');
const {spawnSync} = require('node:child_process');
const fs = require('node:fs');
const path = require('node:path');
const {globSync} = require('glob');

const GENERATED_MARKER = Buffer.from('@' + 'generated');
const MAX_HEADER_BYTES = 4096;
const MAX_FILES_PER_PROCESS = 100;

function isGenerated(file) {
  let fd;
  try {
    fd = fs.openSync(path.resolve(REPO_ROOT, file), 'r');
    const header = Buffer.alloc(MAX_HEADER_BYTES);
    const bytesRead = fs.readSync(fd, header, 0, header.length, 0);
    return header.subarray(0, bytesRead).includes(GENERATED_MARKER);
  } catch (error) {
    console.warn(`Unable to inspect ${file}: ${String(error)}`);
    return false;
  } finally {
    if (fd != null) {
      fs.closeSync(fd);
    }
  }
}

function main() {
  const check = process.argv.includes('--check');
  const ktfmt =
    process.env.KTFMT != null && process.env.KTFMT !== ''
      ? {command: process.env.KTFMT, prefixArguments: []}
      : (() => {
          const java = findJava(17);
          if (java == null) {
            throw new Error(
              'Kotlin formatting requires Java 17 or newer. Install a JDK or set JAVA=/path/to/java.',
            );
          }
          const jar =
            process.env.KTFMT_JAR != null && process.env.KTFMT_JAR !== ''
              ? process.env.KTFMT_JAR
              : require.resolve('ktfmt/lib/ktfmt.jar');
          return {command: java, prefixArguments: ['-jar', jar]};
        })();
  const files = globSync('**/*.{kt,kts}', {
    cwd: REPO_ROOT,
    ignore: ['**/.gradle/**', '**/build/**', '**/node_modules/**'],
  }).filter(file => !isGenerated(file));
  let exitStatus = 0;

  for (let index = 0; index < files.length; index += MAX_FILES_PER_PROCESS) {
    const result = spawnSync(
      ktfmt.command,
      [
        ...ktfmt.prefixArguments,
        '--do-not-remove-unused-imports',
        ...(check ? ['--dry-run', '--set-exit-if-changed'] : []),
        ...files.slice(index, index + MAX_FILES_PER_PROCESS),
      ],
      {cwd: REPO_ROOT, stdio: 'inherit'},
    );
    if (result.error != null) {
      throw result.error;
    }
    if (result.signal != null) {
      throw new Error(`ktfmt was terminated by ${result.signal}`);
    }
    if (result.status !== 0) {
      exitStatus = result.status ?? 1;
    }
  }
  process.exitCode = exitStatus;
}

main();
