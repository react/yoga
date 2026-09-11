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

const OSS_CLANG_FORMAT_DOTSLASH = path.join(__dirname, 'clang-format');
const GENERATED_MARKER = Buffer.from('@' + 'generated');
const MAX_HEADER_BYTES = 4096;
const MAX_FILES_PER_PROCESS = 30;
const SOURCE_GLOB = '**/*.{c,cc,cpp,cu,cuh,cxx,h,hh,hpp,hxx,m,mm,proto,tcc}';
const IGNORE = [
  '**/.cxx/**',
  '**/build/**',
  '**/node_modules/**',
  '**/Pods/**',
  'lib/**',
];

function findClangFormat() {
  if (process.env.CLANG_FORMAT != null && process.env.CLANG_FORMAT !== '') {
    return {command: process.env.CLANG_FORMAT, prefixArguments: []};
  }
  return (
    findMetaTool('tools', 'third-party', 'clang-format', 'clang-format') ?? {
      command: require('fb-dotslash'),
      prefixArguments: [OSS_CLANG_FORMAT_DOTSLASH],
    }
  );
}

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
  const files = globSync(SOURCE_GLOB, {
    cwd: REPO_ROOT,
    ignore: IGNORE,
  }).filter(file => !isGenerated(file));
  const clangFormat = findClangFormat();
  let exitStatus = 0;

  for (let index = 0; index < files.length; index += MAX_FILES_PER_PROCESS) {
    const result = spawnSync(
      clangFormat.command,
      [
        ...clangFormat.prefixArguments,
        ...(check ? ['--dry-run', '--Werror'] : ['-i']),
        ...files.slice(index, index + MAX_FILES_PER_PROCESS),
      ],
      {cwd: REPO_ROOT, stdio: 'inherit'},
    );
    if (result.error != null) {
      throw result.error;
    }
    if (result.signal != null) {
      throw new Error(`clang-format was terminated by ${result.signal}`);
    }
    if (result.status !== 0) {
      exitStatus = result.status ?? 1;
    }
  }
  process.exitCode = exitStatus;
}

main();
