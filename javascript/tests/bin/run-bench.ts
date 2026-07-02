#!/usr/bin/env ts-node
/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import path from 'path';

const WARMUP_ITERATIONS = 3;
const BENCHMARK_ITERATIONS = 10;

const testFiles = process.argv.slice(2);

const testResults = new Map<string, Map<string, number>>();

for (const type of ['wasm']) {
  for (const file of testFiles) {
    globalThis.YGBENCHMARK = (name: string, fn: () => void) => {
      let testEntry = testResults.get(name);

      if (testEntry === undefined)
        testResults.set(name, (testEntry = new Map()));

      for (let t = 0; t < WARMUP_ITERATIONS; ++t) fn();

      const start = Date.now();

      for (let t = 0; t < BENCHMARK_ITERATIONS; ++t) fn();

      const end = Date.now();

      testEntry.set(type, (end - start) / BENCHMARK_ITERATIONS);
    };

    const modulePath = path.resolve(file);
    await import(modulePath);
  }
}

console.log(
  `Note: those tests are independent; there is no time relation to be expected between them`,
);

for (const [name, results] of testResults) {
  console.log();

  const min = Math.min(Infinity, ...results.values());

  console.log(name);

  for (const [type, result] of results) {
    console.log(
      `  - ${type}: ${result}ms (${Math.round((result / min) * 10000) / 100}%)`,
    );
  }
}
