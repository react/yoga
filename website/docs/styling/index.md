---
sidebar_position: 2
---

# Styling

Each Yoga node has an associated style. Nodes may be styled using similar properties to CSS, with some caveats.

## Default styles

The default Yoga node style is similar to a box on web set to `display: flex`, with a few notable exceptions:

1. `flex-direction` defaults to `column` instead of `row`
2. `align-content` defaults to `flex-start` instead of `stretch`
3. `flex-shrink` defaults to `0` instead of `1`
4. `position` defaults to `relative` instead of `static`

Yoga may be [configured](../getting-started/configuring-yoga.mdx) to align to the defaults on web for `flex-direction`, `align-content`, and `flex-shrink` by setting the `UseWebDefaults` flag.

:::warning

Because `UseWebDefaults` was established before the introduction of `position: 'static'`, it does not change the default `position`, in order to preserve compatibility.

:::

## Box Sizing

Yoga acts as if [`box-sizing: 'border-box'`](https://developer.mozilla.org/en-US/docs/Web/CSS/box-sizing) is set. This means that specified dimensions for a box will include the space taken for padding and border.

## Edges

Margin, padding, position, and border, are set against an Edge, which may be:

1. Relative to writing direction (start/end)
2. A physical edge (top/right/left/bottom)
3. A collection of edges (vertical/horizontal/all)

A style value may be set against multiple Edge values at once, with precedence given in the above order.

## Units

Yoga does not operate on CSS units like `px` or `em`. Yoga instead works on "points", representing an arbitrary, canonical absolute unit (usually mapped to display independent pixels), along with percentage. Other units should be absolutized before being given to Yoga.

## Non-standard properties

Yoga's `aspect-ratio` property was added before the same property was added to CSS, and may act subtlety different. These differences may be reconciled in a future version of Yoga.

Yoga's `flex` shorthand will act as `flex-grow` if positive, or `flex-shrink` if negative.
