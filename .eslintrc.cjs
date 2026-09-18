/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

module.exports = {
  root: true,
  ignorePatterns: ['**/binaries/**', '**/build/**', '**/generated/**'],
  overrides: [
    // Catch-all
    {
      files: ['**/*.?(m|c){j,t}s?(x)'],
      extends: ['eslint:recommended', 'plugin:prettier/recommended'],
      plugins: ['prettier'],
      rules: {
        'no-unused-vars': ['error', {argsIgnorePattern: '^_'}],
        'no-var': 'error',
        'prefer-arrow-callback': 'error',
        'prefer-const': 'error',
        'prefer-object-spread': 'error',
        'prefer-spread': 'error',
        'require-await': 'error',
      },
      env: {
        es2020: true,
        'shared-node-browser': true,
      },
      parserOptions: {
        ecmaVersion: 'latest',
      },
    },
    // ES Modules
    {
      files: ['**/*.?(m){j,t}s?(x)'],
      parserOptions: {
        sourceType: 'module',
      },
    },
    // CommonJS Modules
    {
      files: ['**/*.c{j,t}s?(x)'],
      env: {
        commonjs: true,
      },
      parserOptions: {
        sourceType: 'commonjs',
      },
    },
    // TypeScript
    {
      files: ['**/*.?(m|c)ts?(x)'],
      extends: ['plugin:@typescript-eslint/recommended'],
      parser: '@typescript-eslint/parser',
      parserOptions: {
        project: true,
      },
      plugins: ['@typescript-eslint'],
      rules: {
        '@typescript-eslint/ban-ts-comment': 'off',
        '@typescript-eslint/no-unused-vars': [
          'error',
          {argsIgnorePattern: '^_'},
        ],
        '@typescript-eslint/no-var-requires': 'off',
      },
    },
    // Node
    {
      files: ['**/.*rc.(c){j,t}s', '**/*.config.?(c){j,t}s', 'scripts/**/*.js'],
      env: {
        node: true,
      },
    },
    // Jest
    {
      files: ['**/tests/**'],
      extends: ['plugin:jest/recommended'],
    },
  ],
};
