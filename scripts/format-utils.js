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

const {spawnSync} = require('node:child_process');
const fs = require('node:fs');
const path = require('node:path');

const REPO_ROOT = path.resolve(__dirname, '..');
const FBSOURCE_ROOT = path.resolve(REPO_ROOT, '../..');

function findMetaTool(...relativePath) {
  const manifest = path.join(FBSOURCE_ROOT, ...relativePath);
  const dotslash = ['/usr/bin/dotslash', '/usr/local/bin/dotslash'].find(
    candidate => fs.existsSync(candidate),
  );
  return dotslash != null && fs.existsSync(manifest)
    ? {command: dotslash, prefixArguments: [manifest]}
    : null;
}

function javaMajorVersion(command) {
  const result = spawnSync(command, ['-version'], {encoding: 'utf8'});
  const output = `${result.stdout ?? ''}\n${result.stderr ?? ''}`;
  const version = /version "(?:1\.)?(\d+)/.exec(output);
  return result.status === 0 && version != null ? Number(version[1]) : null;
}

function findJava(minimumVersion) {
  if (process.env.JAVA != null && process.env.JAVA !== '') {
    const version = javaMajorVersion(process.env.JAVA);
    if (version == null) {
      throw new Error(
        `JAVA=${process.env.JAVA} did not run or report a recognizable Java version.`,
      );
    }
    if (version < minimumVersion) {
      throw new Error(
        `JAVA=${process.env.JAVA} is Java ${version}; Java ${minimumVersion} or newer is required.`,
      );
    }
    return process.env.JAVA;
  }

  const candidates = ['java'];
  if (process.platform === 'darwin') {
    for (const version of [17, 21]) {
      const result = spawnSync(
        '/usr/libexec/java_home',
        ['-v', String(version)],
        {
          encoding: 'utf8',
        },
      );
      if (result.status === 0 && result.stdout.trim() !== '') {
        candidates.unshift(path.join(result.stdout.trim(), 'bin', 'java'));
      }
    }
  } else {
    candidates.unshift(
      '/usr/local/java-runtime/impl/17/bin/java',
      '/usr/local/java-runtime/17/bin/java',
      '/usr/local/java-runtime/impl/21/bin/java',
      '/usr/local/java-runtime/21/bin/java',
    );
  }

  return (
    candidates.find(command => javaMajorVersion(command) >= minimumVersion) ??
    null
  );
}

module.exports = {findJava, findMetaTool, REPO_ROOT};
