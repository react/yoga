/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import Yoga from 'yoga-layout';
import {Direction, FlexDirection, Wrap} from 'yoga-layout';

test('children_overflow_no_wrap_and_no_flex_children', () => {
  const root = Yoga.Node.create();
  root.setWidth(200);
  root.setHeight(100);
  root.setFlexDirection(FlexDirection.Column);
  root.setFlexWrap(Wrap.NoWrap);

  const child0 = Yoga.Node.create();
  child0.setWidth(80);
  child0.setHeight(40);
  child0.setMargin(Yoga.EDGE_TOP, 10);
  child0.setMargin(Yoga.EDGE_BOTTOM, 15);
  root.insertChild(child0, 0);

  const child1 = Yoga.Node.create();
  child1.setWidth(80);
  child1.setHeight(40);
  child1.setMargin(Yoga.EDGE_BOTTOM, 5);
  root.insertChild(child1, 1);

  root.calculateLayout(200, 100, Direction.LTR);

  expect(root.getComputedHadOverflow()).toBe(true);
});

test('no_overflow_no_wrap_and_flex_children', () => {
  const root = Yoga.Node.create();
  root.setWidth(200);
  root.setHeight(100);
  root.setFlexDirection(FlexDirection.Column);
  root.setFlexWrap(Wrap.NoWrap);

  const child0 = Yoga.Node.create();
  child0.setWidth(80);
  child0.setHeight(40);
  child0.setMargin(Yoga.EDGE_TOP, 10);
  child0.setMargin(Yoga.EDGE_BOTTOM, 10);
  root.insertChild(child0, 0);

  const child1 = Yoga.Node.create();
  child1.setWidth(80);
  child1.setHeight(40);
  child1.setMargin(Yoga.EDGE_BOTTOM, 5);
  child1.setFlexShrink(1);
  root.insertChild(child1, 1);

  root.calculateLayout(200, 100, Direction.LTR);

  expect(root.getComputedHadOverflow()).toBe(false);
});

test('hadOverflow_gets_reset_if_no_longer_valid', () => {
  const root = Yoga.Node.create();
  root.setWidth(200);
  root.setHeight(100);
  root.setFlexDirection(FlexDirection.Column);
  root.setFlexWrap(Wrap.NoWrap);

  const child0 = Yoga.Node.create();
  child0.setWidth(80);
  child0.setHeight(40);
  child0.setMargin(Yoga.EDGE_TOP, 10);
  child0.setMargin(Yoga.EDGE_BOTTOM, 10);
  root.insertChild(child0, 0);

  const child1 = Yoga.Node.create();
  child1.setWidth(80);
  child1.setHeight(40);
  child1.setMargin(Yoga.EDGE_BOTTOM, 5);
  root.insertChild(child1, 1);

  root.calculateLayout(200, 100, Direction.LTR);
  expect(root.getComputedHadOverflow()).toBe(true);

  child1.setFlexShrink(1);
  root.calculateLayout(200, 100, Direction.LTR);
  expect(root.getComputedHadOverflow()).toBe(false);
});

test('hadOverflow_included_in_getComputedLayout', () => {
  const root = Yoga.Node.create();
  root.setWidth(200);
  root.setHeight(100);
  root.setFlexDirection(FlexDirection.Column);

  const child0 = Yoga.Node.create();
  child0.setWidth(80);
  child0.setHeight(80);
  root.insertChild(child0, 0);

  const child1 = Yoga.Node.create();
  child1.setWidth(80);
  child1.setHeight(80);
  root.insertChild(child1, 1);

  root.calculateLayout(200, 100, Direction.LTR);

  const layout = root.getComputedLayout();
  expect(layout.hadOverflow).toBe(true);
});
