---
slug: announcing-yoga-3.2
title: Announcing Yoga 3.2
authors:
  - NickGerleman
---

import Playground from '@site/src/components/Playground';

Yoga 3.2 is a new minor version of Yoga, used by React Native 0.77.

## Highlights

1. Support for `box-sizing`
2. Support for `display: contents`
3. Bug fixes and improvements

## `box-sizing`

Yoga [now supports](https://github.com/facebook/yoga/commit/671ae61a39d02091d1e73fe773d6a09f2f93cda4) [`box-sizing`](https://developer.mozilla.org/en-US/docs/Web/CSS/box-sizing) on styles, allowing sizing values to influence the content box instead of the border box.

:::warning

To preserve compatibility, Yoga nodes default to `box-sizing: border-box`, even if `UseWebDefaults` is set. We recommend manually setting node defaults instead of using the `UseWebDefaults` API.

:::

<Playground code={`<Layout config={{useWebDefaults: true}}>
  <Node
    style={{
      width: 100,
      height: 100,
      padding: 50,
      boxSizing: "border-box",
    }}>
  </Node>
</Layout>`} />

<Playground code={`<Layout config={{useWebDefaults: true}}>
    <Node
    style={{
      width: 100,
      height: 100,
      padding: 50,
      boxSizing: "content-box",
    }}>
  </Node>
</Layout>`} />

## `display: contents`

Yoga nodes [may now be set to `display: contents`](https://github.com/facebook/yoga/commit/68bb2343d2b470962065789d09016bba8e785340) to remove them from the layout flow, while preserving and hoisting the node's children. This may be used by the higher level UI framework to allow more easily composing wrapper components (such as those which may need to handle events, without influencing child layout). Thanks [@j-piasecki](https://github.com/j-piasecki) for the contribution!

<Playground code={`<Layout config={{useWebDefaults: false}}>
<Node
style={{
      width: 100,
      height: 100,
      gap: 10,
    }}

>

    <Node style={{display: "contents"}}>

      <Node style={{flexGrow: 1}} />
      <Node style={{flexGrow: 1}} />
      <Node style={{flexGrow: 1}} />
    </Node>

  </Node>
</Layout>`} />

## Removal of legacy absolute positioning

Yoga 3.0 introduced a new algorithm used for absolute positioning. This algorithm is more correct than the one previously used, but led to observed compatibility issues with existing code, so we left the option to disable it via the `AbsolutePositioningIncorrect` erratum (enabled by default in frameworks like React Native). Yoga 3.2 removes the legacy absolute positioning path, but ports over the main compatibility quirk under a new erratum `AbsolutePositionWithoutInsetsExcludesPadding` (where the previous incorrect behavior would omit padding when a position was not specified on the absolute node). Errata users should see more correct absolute positioning behavior, while preserving compatibility with existing code.

## Fixed non-global YogaConfig in Java bindings

Yoga would previously allow garbage collection of a `YogaConfig` if it was not retained outside of the Yoga tree. This could result in confusing errors caused by use-after free. Yoga nodes [now correctly retain the configs they are using](https://github.com/facebook/yoga/commit/22b018c957e930de950338ad87f4ef8d59e8a169). Thanks [@michaeltroger](https://github.com/michaeltroger) for this fix!

## Fixed behavior when combining `align-items` with `align-content`

A regression [was fixed](https://github.com/facebook/yoga/commit/77c99870127e9c2d46a07264fa372025334d8fd0) in how Yoga handles some combinations of `align-content` and `align-items`. Thanks [@phuccvx12](https://github.com/phuccvx12) for this fix!
