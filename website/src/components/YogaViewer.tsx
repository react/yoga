/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import {useMemo} from 'react';
import Yoga, {
  Direction,
  Display,
  Overflow,
  Node as YogaNode,
} from 'yoga-layout';
import {FlexStyle, applyStyle} from './FlexStyle';
import LayoutBox from './LayoutBox';

import type {LayoutMetrics} from './LayoutBox';

export type Props = Readonly<{
  rootNode: StyleNode;
  width?: number;
  height?: number;
  className?: string;
  useWebDefaults?: boolean;
}>;

export type StyleNode = {
  style?: FlexStyle;
  children?: StyleNode[];
};

export default function YogaViewer({
  rootNode,
  width,
  height,
  className,
  useWebDefaults,
}: Props) {
  const layout = useMemo(
    () => layoutStyleTree(rootNode, width, height, {useWebDefaults}),
    [rootNode, width, height, useWebDefaults],
  );
  return <LayoutBox metrics={layout} depth={0} className={className} />;
}

type LayoutConfig = Readonly<{
  useWebDefaults?: boolean;
}>;

// This is not efficient and not a good real-world-example for the best way to use Yoga, but sufficient for a playground
function layoutStyleTree(
  node: StyleNode,
  rootWidth: number | undefined,
  rootHeight: number | undefined,
  layoutConfig: LayoutConfig,
): LayoutMetrics {
  const root = yogaNodeFromStyleNode(node, layoutConfig);
  root.calculateLayout(rootWidth, rootHeight, Direction.LTR);

  const layoutMetrics = metricsFromYogaNode(root);
  layoutMetrics.overflow = node.style?.overflow;

  return layoutMetrics;
}

function yogaNodeFromStyleNode(
  styleNode: StyleNode,
  layoutConfig: LayoutConfig,
): YogaNode {
  const node = Yoga.Node.create(
    layoutConfig.useWebDefaults ? webDefaultsConfig : undefined,
  );
  applyStyle(node, styleNode.style);

  for (const child of styleNode.children ?? []) {
    node.insertChild(
      yogaNodeFromStyleNode(child, layoutConfig),
      node.getChildCount(),
    );
  }

  return node;
}

const webDefaultsConfig = Yoga.Config.create();
webDefaultsConfig.setUseWebDefaults(true);

function metricsFromYogaNode(node: YogaNode): LayoutMetrics {
  const children: LayoutMetrics[] = [];
  for (let i = 0; i < node.getChildCount(); i++) {
    children.push(metricsFromYogaNode(node.getChild(i)));
  }

  // Offset is relative to parent padding box, so we need to subtract the extra
  // border we show as part of the box.
  const parentBorderThickness = 1;

  return {
    top: node.getComputedTop() - parentBorderThickness,
    left: node.getComputedLeft() - parentBorderThickness,
    width: node.getComputedWidth(),
    height: node.getComputedHeight(),
    clipPath: node.getClipPath(),
    overflow: (() => {
      switch (node.getOverflow()) {
        case Overflow.Hidden:
          return 'hidden';
        case Overflow.Scroll:
          return 'scroll';
        case Overflow.Visible:
          return 'visible';
      }
    })(),
    display: (() => {
      switch (node.getDisplay()) {
        case Display.Flex:
          return 'flex';
        case Display.None:
          return 'none';
        case Display.Contents:
          return 'contents';
      }
    })(),
    children,
  };
}
