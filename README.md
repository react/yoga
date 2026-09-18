# Yoga [![npm](https://img.shields.io/npm/v/yoga-layout.svg)](https://www.npmjs.com/package/yoga-layout) [![Maven Central](https://img.shields.io/maven-central/v/com.facebook.yoga/yoga)](https://search.maven.org/artifact/com.facebook.yoga/yoga) ![SPM](https://img.shields.io/badge/SPM-Supported-blue.svg)

Yoga is an embeddable and performant flexbox layout engine with bindings for multiple languages.

## Building

Yoga's main implementation targets C++ 20 with accompanying build logic in CMake. A wrapper is provided to build the main library and run unit tests.

```sh
./unit_tests Debug
./unit_tests Release
```

While not required, this script will use [ninja](https://ninja-build.org/) if it is installed for faster builds.

Yoga is additionally part of the [vcpkg](https://github.com/Microsoft/vcpkg/) collection of ports maintained by Microsoft and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

## Adding Tests

Many of Yoga's tests are automatically generated, using HTML fixtures describing node structure. These are rendered in Chrome to generate an expected layout result for the tree. New fixtures can be added to `gentest/fixtures`.

```html
<div id="my_test" style="width: 100px; height: 100px; align-items: center;">
  <div style="width: 50px; height: 50px;"></div>
</div>
```

To generate new tests from added fixtures:

1. Ensure you have [yarn classic](https://classic.yarnpkg.com) installed.
2. Run `yarn install` to install dependencies for the test generator.
3. Run `yarn gentest` in the `yoga` directory.

## Debugging

Yoga provides a VSCode "launch.json" configuration which allows debugging unit tests. Simply add your breakpoints, and run "Debug C++ Unit tests (lldb)" (or "Debug C++ Unit tests (vsdbg)" on Windows).
