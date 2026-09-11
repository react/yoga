---
slug: announcing-yoga-3.1
title: Announcing Yoga 3.1
authors:
  - NickGerleman
---

import Playground from '@site/src/components/Playground';

Yoga 3.1 is a new minor version of Yoga, used by React Native 0.75.

## Highlights

1. Percentage support for `gap` properties
2. Layout conformance and regression fixes
3. Additional JavaScript bindings

## Percentage support for `gap` properties

Yoga [now supports](https://github.com/facebook/yoga/commit/cd4a1b8cf63cf09a62d6aefcd16843cc8869488c) gap values specified as percentages instead of points.

:::warning

Percentages in Yoga do not always act consistently with browsers when a definite container size is not provided. This may be improved in a future version of Yoga.

:::

<Playground code={`<Layout config={{useWebDefaults: true}}>
  <Node
    style={{
      columnGap: "10%",
      width: 200,
      height: 100,
    }}>
    <Node style={{flexGrow: 1}} />
    <Node style={{flexGrow: 1}} />
    <Node style={{flexGrow: 1}} />
  </Node>
</Layout>`} />

## Alignment changes to overflowed containers

Yoga has made [several](https://github.com/facebook/yoga/commit/932361cdbf5864a2e6929b68154b4ada2ac586d9) [fixes](https://github.com/facebook/yoga/commit/6f10656868c2eca9b2eae55ad754e01e70565f69) to how flex-children of overflowed containers are aligned when using `justify-content`, `align-content`, or `margin: "auto"`. This includes some cases where adding an alignment keyword could cause items to overlap, or padding to be removed.

<Playground code={`<Layout config={{useWebDefaults: false}}>
<Node
style={{
      width: 100,
      height: 100,
      padding: 10,
      justifyContent: 'space-evenly',
    }}

>

    <Node style={{height: 100, width: 100}} />

  </Node>
</Layout>`} />

## Fixes for regressions in Yoga 3.0

We fixed a couple of regressions added in Yoga 3.0, around percentage insets applied to some absolute containers ([#1657](https://github.com/facebook/yoga/issues/1657)), and start/end resolution when physical edge styles are also present ([#1658](https://github.com/facebook/yoga/issues/1658)).

## Additional JavaScript bindings

We've added JavaScript bindings for setting layout direction to Yoga 3.1, along with JavaScript additions made in patch releases to Yoga 3.0, including bindings for `hasNewLayout` flag manipulation, and an entrypoint for users who are unable to use top-level await.
