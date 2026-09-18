/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import {
  Align,
  BoxSizing,
  Direction,
  Display,
  Edge,
  FlexDirection,
  Gutter,
  Justify,
  Overflow,
  PositionType,
  Wrap,
  Node as YogaNode,
} from 'yoga-layout';

export type AlignContent =
  | 'flex-start'
  | 'flex-end'
  | 'center'
  | 'stretch'
  | 'space-between'
  | 'space-around'
  | 'space-evenly';

export type AlignItems =
  'flex-start' | 'flex-end' | 'center' | 'stretch' | 'baseline';

export type JustifyContent =
  | 'flex-start'
  | 'flex-end'
  | 'center'
  | 'space-between'
  | 'space-around'
  | 'space-evenly';

export type FlexStyle = {
  alignContent?: AlignContent;
  alignItems?: AlignItems;
  alignSelf?: AlignItems;
  aspectRatio?: number;
  borderBottomWidth?: number;
  borderEndWidth?: number;
  borderLeftWidth?: number;
  borderRightWidth?: number;
  borderStartWidth?: number;
  borderTopWidth?: number;
  borderWidth?: number;
  borderInlineWidth?: number;
  borderBlockWidth?: number;
  bottom?: number | `${number}%`;
  boxSizing?: 'border-box' | 'content-box';
  direction?: 'ltr' | 'rtl';
  display?: 'none' | 'flex' | 'contents';
  end?: number | `${number}%`;
  flex?: number;
  flexBasis?: number | 'auto' | `${number}%`;
  flexDirection?: 'row' | 'column' | 'row-reverse' | 'column-reverse';
  rowGap?: number;
  gap?: number;
  columnGap?: number;
  flexGrow?: number;
  flexShrink?: number;
  flexWrap?: 'wrap' | 'nowrap' | 'wrap-reverse';
  height?: number | 'auto' | `${number}%`;
  justifyContent?: JustifyContent;
  left?: number | `${number}%`;
  margin?: number | 'auto' | `${number}%`;
  marginBottom?: number | 'auto' | `${number}%`;
  marginEnd?: number | 'auto' | `${number}%`;
  marginLeft?: number | 'auto' | `${number}%`;
  marginRight?: number | 'auto' | `${number}%`;
  marginStart?: number | 'auto' | `${number}%`;
  marginTop?: number | 'auto' | `${number}%`;
  marginInline?: number | 'auto' | `${number}%`;
  marginBlock?: number | 'auto' | `${number}%`;
  maxHeight?: number | `${number}%`;
  maxWidth?: number | `${number}%`;
  minHeight?: number | `${number}%`;
  minWidth?: number | `${number}%`;
  overflow?: 'visible' | 'hidden' | 'scroll';
  padding?: number | `${number}%`;
  paddingBottom?: number | `${number}%`;
  paddingEnd?: number | `${number}%`;
  paddingLeft?: number | `${number}%`;
  paddingRight?: number | `${number}%`;
  paddingStart?: number | `${number}%`;
  paddingTop?: number | `${number}%`;
  paddingInline?: number | `${number}%`;
  paddingBlock?: number | `${number}%`;
  position?: 'absolute' | 'relative' | 'static';
  right?: number | `${number}%`;
  start?: number | `${number}%`;
  top?: number | `${number}%`;
  insetInline?: number | `${number}%`;
  insetBlock?: number | `${number}%`;
  inset?: number | `${number}%`;
  width?: number | 'auto' | `${number}%`;
};

export function applyStyle(node: YogaNode, style: FlexStyle = {}): void {
  for (const key of Object.keys(style)) {
    try {
      switch (key) {
        case 'alignContent':
          node.setAlignContent(alignContent(style.alignContent));
          break;
        case 'alignItems':
          node.setAlignItems(alignItems(style.alignItems));
          break;
        case 'alignSelf':
          node.setAlignSelf(alignItems(style.alignSelf));
          break;
        case 'aspectRatio':
          node.setAspectRatio(style.aspectRatio);
          break;
        case 'borderBottomWidth':
          node.setBorder(Edge.Bottom, style.borderBottomWidth);
          break;
        case 'borderEndWidth':
          node.setBorder(Edge.End, style.borderEndWidth);
          break;
        case 'borderLeftWidth':
          node.setBorder(Edge.Left, style.borderLeftWidth);
          break;
        case 'borderRightWidth':
          node.setBorder(Edge.Right, style.borderRightWidth);
          break;
        case 'borderStartWidth':
          node.setBorder(Edge.Start, style.borderStartWidth);
          break;
        case 'borderTopWidth':
          node.setBorder(Edge.Top, style.borderTopWidth);
          break;
        case 'borderWidth':
          node.setBorder(Edge.All, style.borderWidth);
          break;
        case 'borderInlineWidth':
          node.setBorder(Edge.Horizontal, style.borderInlineWidth);
          break;
        case 'borderBlockWidth':
          node.setBorder(Edge.Vertical, style.borderBlockWidth);
          break;
        case 'bottom':
          node.setPosition(Edge.Bottom, style.bottom);
          break;
        case 'boxSizing':
          node.setBoxSizing(boxSizing(style.boxSizing));
          break;
        case 'direction':
          node.setDirection(direction(style.direction));
          break;
        case 'display':
          node.setDisplay(display(style.display));
          break;
        case 'end':
          node.setPosition(Edge.End, style.end);
          break;
        case 'flex':
          node.setFlex(style.flex);
          break;
        case 'flexBasis':
          node.setFlexBasis(style.flexBasis);
          break;
        case 'flexDirection':
          node.setFlexDirection(flexDirection(style.flexDirection));
          break;
        case 'rowGap':
          node.setGap(Gutter.Row, style.rowGap);
          break;
        case 'gap':
          node.setGap(Gutter.All, style.gap);
          break;
        case 'columnGap':
          node.setGap(Gutter.Column, style.columnGap);
          break;
        case 'flexGrow':
          node.setFlexGrow(style.flexGrow);
          break;
        case 'flexShrink':
          node.setFlexShrink(style.flexShrink);
          break;
        case 'flexWrap':
          node.setFlexWrap(flexWrap(style.flexWrap));
          break;
        case 'height':
          node.setHeight(style.height);
          break;
        case 'justifyContent':
          node.setJustifyContent(justifyContent(style.justifyContent));
          break;
        case 'left':
          node.setPosition(Edge.Left, style.left);
          break;
        case 'margin':
          node.setMargin(Edge.All, style.margin);
          break;
        case 'marginBottom':
          node.setMargin(Edge.Bottom, style.marginBottom);
          break;
        case 'marginEnd':
          node.setMargin(Edge.End, style.marginEnd);
          break;
        case 'marginLeft':
          node.setMargin(Edge.Left, style.marginLeft);
          break;
        case 'marginRight':
          node.setMargin(Edge.Right, style.marginRight);
          break;
        case 'marginStart':
          node.setMargin(Edge.Start, style.marginStart);
          break;
        case 'marginTop':
          node.setMargin(Edge.Top, style.marginTop);
          break;
        case 'marginInline':
          node.setMargin(Edge.Horizontal, style.marginInline);
          break;
        case 'marginBlock':
          node.setMargin(Edge.Vertical, style.marginBlock);
          break;
        case 'maxHeight':
          node.setMaxHeight(style.maxHeight);
          break;
        case 'maxWidth':
          node.setMaxWidth(style.maxWidth);
          break;
        case 'minHeight':
          node.setMinHeight(style.minHeight);
          break;
        case 'minWidth':
          node.setMinWidth(style.minWidth);
          break;
        case 'overflow':
          node.setOverflow(overflow(style.overflow));
          break;
        case 'padding':
          node.setPadding(Edge.All, style.padding);
          break;
        case 'paddingBottom':
          node.setPadding(Edge.Bottom, style.paddingBottom);
          break;
        case 'paddingEnd':
          node.setPadding(Edge.End, style.paddingEnd);
          break;
        case 'paddingLeft':
          node.setPadding(Edge.Left, style.paddingLeft);
          break;
        case 'paddingRight':
          node.setPadding(Edge.Right, style.paddingRight);
          break;
        case 'paddingStart':
          node.setPadding(Edge.Start, style.paddingStart);
          break;
        case 'paddingTop':
          node.setPadding(Edge.Top, style.paddingTop);
          break;
        case 'paddingInline':
          node.setPadding(Edge.Horizontal, style.paddingInline);
          break;
        case 'paddingBlock':
          node.setPadding(Edge.Vertical, style.paddingBlock);
          break;
        case 'position':
          node.setPositionType(position(style.position));
          break;
        case 'right':
          node.setPosition(Edge.Right, style.right);
          break;
        case 'start':
          node.setPosition(Edge.Start, style.start);
          break;
        case 'top':
          node.setPosition(Edge.Top, style.top);
          break;
        case 'insetInline':
          node.setPosition(Edge.Horizontal, style.insetInline);
          break;
        case 'insetBlock':
          node.setPosition(Edge.Vertical, style.insetBlock);
          break;
        case 'inset':
          node.setPosition(Edge.All, style.inset);
          break;
        case 'width':
          node.setWidth(style.width);
          break;
      }
    } catch (e) {
      // Fail gracefully
    }
  }
}

function alignContent(str?: AlignContent): Align {
  switch (str) {
    case 'flex-start':
      return Align.FlexStart;
    case 'flex-end':
      return Align.FlexEnd;
    case 'center':
      return Align.Center;
    case 'stretch':
      return Align.Stretch;
    case 'space-between':
      return Align.SpaceBetween;
    case 'space-around':
      return Align.SpaceAround;
    case 'space-evenly':
      return Align.SpaceEvenly;
  }
  throw new Error(`"${str}" is not a valid value for alignContent`);
}

function alignItems(str?: AlignItems): Align {
  switch (str) {
    case 'flex-start':
      return Align.FlexStart;
    case 'flex-end':
      return Align.FlexEnd;
    case 'center':
      return Align.Center;
    case 'stretch':
      return Align.Stretch;
    case 'baseline':
      return Align.Baseline;
  }
  throw new Error(`"${str}" is not a valid value for alignItems`);
}

function boxSizing(str?: 'border-box' | 'content-box'): BoxSizing {
  switch (str) {
    case 'border-box':
      return BoxSizing.BorderBox;
    case 'content-box':
      return BoxSizing.ContentBox;
  }
  throw new Error(`"${str}" is not a valid value for boxSizing`);
}

function direction(str?: 'ltr' | 'rtl'): Direction {
  switch (str) {
    case 'ltr':
      return Direction.LTR;
    case 'rtl':
      return Direction.RTL;
  }
  throw new Error(`"${str}" is not a valid value for direction`);
}

function display(str?: 'none' | 'flex' | 'contents'): Display {
  switch (str) {
    case 'none':
      return Display.None;
    case 'flex':
      return Display.Flex;
    case 'contents':
      return Display.Contents;
  }
  throw new Error(`"${str}" is not a valid value for display`);
}

function flexDirection(
  str?: 'row' | 'column' | 'row-reverse' | 'column-reverse',
): FlexDirection {
  switch (str) {
    case 'row':
      return FlexDirection.Row;
    case 'column':
      return FlexDirection.Column;
    case 'row-reverse':
      return FlexDirection.RowReverse;
    case 'column-reverse':
      return FlexDirection.ColumnReverse;
  }
  throw new Error(`"${str}" is not a valid value for flexDirection`);
}

function flexWrap(str?: 'wrap' | 'nowrap' | 'wrap-reverse'): Wrap {
  switch (str) {
    case 'wrap':
      return Wrap.Wrap;
    case 'nowrap':
      return Wrap.NoWrap;
    case 'wrap-reverse':
      return Wrap.WrapReverse;
  }
  throw new Error(`"${str}" is not a valid value for flexWrap`);
}

function justifyContent(str?: JustifyContent): Justify {
  switch (str) {
    case 'flex-start':
      return Justify.FlexStart;
    case 'flex-end':
      return Justify.FlexEnd;
    case 'center':
      return Justify.Center;
    case 'space-between':
      return Justify.SpaceBetween;
    case 'space-around':
      return Justify.SpaceAround;
    case 'space-evenly':
      return Justify.SpaceEvenly;
  }
  throw new Error(`"${str}" is not a valid value for justifyContent`);
}

function overflow(str?: 'visible' | 'hidden' | 'scroll'): Overflow {
  switch (str) {
    case 'visible':
      return Overflow.Visible;
    case 'hidden':
      return Overflow.Hidden;
    case 'scroll':
      return Overflow.Scroll;
  }
  throw new Error(`"${str}" is not a valid value for overflow`);
}

function position(str?: 'absolute' | 'relative' | 'static'): PositionType {
  switch (str) {
    case 'absolute':
      return PositionType.Absolute;
    case 'relative':
      return PositionType.Relative;
    case 'static':
      return PositionType.Static;
  }
  throw new Error(`"${str}" is not a valid value for position`);
}
