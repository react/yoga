---
slug: announcing-yoga-2.0
title: Announcing Yoga 2.0
authors:
  - NickGerleman
---

We are excited to announce a new major (breaking) version of Yoga. This release contains 198 new commits from 64 contributors, and includes the first significant changes to Yoga in open-source since 2018.

While there has been a [long gap in development](https://github.com/facebook/yoga/issues/1151), the React Native team believes Yoga is a critical tool in delivering the future of React, and we have resumed development of the engine. This release of Yoga is focused on:

1. Delivering the features we have shipped to React Native to standalone Yoga users
2. Updating toolchains, packaging, and build systems to make Yoga easier to consume
3. API changes to allow us to evolve the engine towards conformance

Yoga continuously ships to thousands of surfaces across multiple frameworks at Meta. This version of Yoga corresponds to the version which will be included in React Native 0.73 shipping this fall.

## Flexbox gap

The most significant new feature for users on an older stable release of Yoga is the addition of Flexbox gap support. This powers `gap`, `rowGap`, and `columnGap` in React Native 0.71.

```cpp
// Example.cpp
YGNodeStyleSetGap(node, YGGutterRow, 2.0f);
```

```java
// Example.java
node.setGap(YogaGutter.ROW, 2.0f);
```

```ts
// Example.ts
node.setGap(Gutter.Row, 2);
```

## Toolchain

Meta uses [Buck](https://buck.build/) across its monorepo, but we recognize that Buck has acted as a barrier to be able to use Yoga outside of Meta. Yoga no longer ships build logic for Buck to open-source. We have instead added over 20 new validation jobs to GitHub Actions to continually validate that Yoga builds correctly in common systems and scenarios where Yoga is used in OSS.

New toolchain support includes:

1. A reference CMake build for Yoga and its unit tests
1. A modern Gradle build and published AARs
1. Compatibility with XCode 14.3+
1. Compatibility with Node 16+
1. Support for WebAssembly in both Node and browsers
1. Support for MSVC, higher warning levels, and building without exceptions

## Aiming for Conformance

Our team wants to enable engineers to be able to create a single style which renders faithfully across Yoga and web. Conformance is a moving target, with browsers like Chromium regularly making behavior changes to better achieve it. This requires making behavior changes to Yoga which break existing behaviors, for better consistency with the web.

In Yoga 2.0, we’ve generalized `UseLegacyStretchBehaviour` to a new [Errata API](https://github.com/facebook/yoga/issues/1247), to allow different parts of a Yoga tree to target different conformance levels. This allows rendering part of the tree to be compatible with styles written for web, with other parts compatible with styles written for older versions of Yoga.

Yoga's default behavior going forward is W3C compliance. We recommend users sensitive to the change to set `YGErrataClassic,` or `YGErrataAll` if you were already setting `UseLegacyStretchBehaviour`.

```cpp
// Example.cpp
YGConfigRef config = YGConfigNew();
YGConfigSetErrata(config, YGErrataClassic);

YGNodeRef node = YGNodeNewWithConfig(config);
```

```java
// Example.java
YogaConfig config = YogaConfigFactory.create();
config.setErrata(YogaErrata.CLASSIC);

YogaNode node = YogaNodeFactory.create(config);
```

```ts
// Example.ts
const config = Config.create();
config.setErrata(Errata.Classic);

const node = Node.create(config);
```

## Yoga for JavaScript

Yoga’s previous JavaScript bindings are not installable when using Node 12+, making them effectively unusable in today’s JavaScript ecosystem. We now ship a new package with prebuilt binaries and first-class support for TypeScript and modern bundlers.

Two variants are shipped:

1. An asmjs variant for compatibility
1. A JS module with embedded WebAssembly (~50% faster)

Both are about 45KB when gzipped.

WebAssembly binaries must be asynchronously compiled and loaded in Chrome. In the absence of universal support for top-level await, we have made the breaking change to require explicitly asynchronously loading Yoga before using it.

```ts
import {loadYoga, Align} from 'yoga-layout';

const Yoga = await loadYoga();
const node = Yoga.Node.create();
node.setAlignContent(Align.Center);
```

The previous behavior of blocking to load the binary can be replicated by importing from the `yoga-layout/sync` entrypoint, but this is not recommended for new usages, and does not allow using WebAssembly on browsers.

```ts
import Yoga, {ALIGN_CENTER} from 'yoga-layout/sync';

const node = Yoga.Node.create();
node.setAlignContent(ALIGN_CENTER);
```

`yoga-layout` and `yoga-layout/sync` try to pick between `asmjs` and `WebAssembly` automatically based on the [target environment](https://webpack.js.org/guides/package-exports/#target-environment) set by your bundler, but you can choose this explicitly as well.

```ts
import {loadYoga} from 'yoga-layout/wasm-async';
```

> Note: the `yoga-layout` package requires your bundler and typechecker to configured to be able to follow the package `exports `field.

## Deprecations and removals

### YogaKit and YogaLayout ViewGroup

We are deprecating, `YogaKit` and the `YogaLayout` ViewGroup. These libraries allow initegrating Yoga directly with UIKit and the Android view system, but are not widely used by Meta in production. We are instead focusing on higher-level libraries using Yoga like [Litho](https://fblitho.com/) and [React Native](https://reactnative.dev/). Because we aren’t in a place to continue development, or validate contributions, we are discontinuing development. These libraries will not receive future updates beyond the Yoga `release-v2.0` branch.

### UseLegacyStretchBehaviour

The functions to manipulate `UseLegacyStretchBehaviour` have been deprecated. Previous users of the API should now set an appropriate errata level, like `YGErrataAll` to opt-out of all future conformance fixes.

### C# bindings

C# bindings were contributed to the Yoga repo but have since degraded. The bindings have not had working build validation, or a consistent contributor. We have removed them from the Yoga repo, but we will continue to provide a public C ABI for others to build bindings on top of.

### Private C++ APIs

Yoga’s header structure has historically allowed the inclusion of concrete internal structures like `YGStyle` or `YGNode`. We will begin to enforce that users instead rely on the public C APIs provided by `#include <yoga/Yoga.h>`. Other C++ APIs may change without notice.

```cpp
// Public API (GOOD)
#include <yoga/Yoga.h>

YGConfigRef config = YGConfigNew();
YGConfigSetPointScaleFactor(config, 1.0f);
```

```cpp
// Private API (BAD)
#include <yoga/YGConfig.h>

YGConfig config{yogaLogger_};
config.pointScaleFactor = 1.0f;
```

### C++ 11 Support

Yoga now requires a compiler which supports C++ 14. This will likely be bumped to C++ 17 in a future minor release.

## How do I get it?

For users who don’t want to build from source, new Yoga packages have been published to the npmjs registry, Maven Central, and CocoaPods.

```json5
// package.json
dependencies: {
  "yoga-layout": "^2.0.0"
}
```

```gradle
// build.gradle
dependencies {
  implementation("com.facebook.yoga:yoga:2.0.0")
}
```

```rb
# Podfile
pod 'Yoga', '~> 2.0.0'
```

## Acknowledgements

Yoga 2.0 contains major external contributions from [@intergalacticspacehighway](https://github.com/intergalacticspacehighway), [@jacobp100](https://github.com/jacobp100), [@jeetiss](https://github.com/jeetiss) and [@nicoburns](https://github.com/nicoburns).
