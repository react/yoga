/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import Yoga from 'yoga-layout';

import {
  getMeasureCounterMax,
  getMeasureCounterMin,
} from './tools/MeasureCounter.ts';

test('measure_once_single_flexible_child', () => {
  const root = Yoga.Node.create();
  root.setFlexDirection(Yoga.FLEX_DIRECTION_ROW);
  root.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.setWidth(100);
  root.setHeight(100);

  const measureCounter = getMeasureCounterMax();

  const root_child0 = Yoga.Node.create();
  root_child0.setMeasureFunc(measureCounter.inc);
  root_child0.setFlexGrow(1);
  root.insertChild(root_child0, 0);

  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  expect(measureCounter.get()).toBe(1);
});

test('measure_func_follows_measure_modes_by_default', () => {
  const node = Yoga.Node.create();

  expect(node.getMeasureFuncFollowsMeasureModes()).toBe(true);
});

test('keep_tighter_atmost_width_measurement_by_default', () => {
  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);

  const measureCounter = getMeasureCounterMin();

  const root_child0 = Yoga.Node.create();
  root_child0.setMeasureFunc(measureCounter.inc);
  root.insertChild(root_child0, 0);

  root.calculateLayout(100, undefined, Yoga.DIRECTION_LTR);
  root.calculateLayout(80, undefined, Yoga.DIRECTION_LTR);

  expect(measureCounter.get()).toBe(1);
});

test('remeasure_tighter_atmost_width_when_not_following_measure_modes', () => {
  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);

  const measureCounter = getMeasureCounterMin();

  const root_child0 = Yoga.Node.create();
  root_child0.setMeasureFunc(measureCounter.inc);
  root_child0.setMeasureFuncFollowsMeasureModes(false);
  root.insertChild(root_child0, 0);

  root.calculateLayout(100, undefined, Yoga.DIRECTION_LTR);
  expect(measureCounter.get()).toBe(2);
  root.calculateLayout(80, undefined, Yoga.DIRECTION_LTR);

  expect(measureCounter.get()).toBe(4);
});

test('keep_measurement_for_same_spec_when_not_following_measure_modes', () => {
  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);

  const measureCounter = getMeasureCounterMin();

  const root_child0 = Yoga.Node.create();
  root_child0.setMeasureFunc(measureCounter.inc);
  root_child0.setMeasureFuncFollowsMeasureModes(false);
  root.insertChild(root_child0, 0);

  const root_child1 = Yoga.Node.create();
  root_child1.setWidth(10);
  root_child1.setHeight(10);
  root.insertChild(root_child1, 1);

  root.calculateLayout(100, undefined, Yoga.DIRECTION_LTR);
  const firstLayoutCount = measureCounter.get();
  root_child1.setHeight(20);
  root.calculateLayout(100, undefined, Yoga.DIRECTION_LTR);

  expect(measureCounter.get()).toBe(firstLayoutCount);
  expect(root.getComputedHeight()).toBe(30);
});

test('setting_measure_func_follows_measure_modes_dirties_on_change', () => {
  const root = Yoga.Node.create();

  const root_child0 = Yoga.Node.create();
  root_child0.setMeasureFunc(getMeasureCounterMin().inc);
  root.insertChild(root_child0, 0);

  root.calculateLayout(100, 100, Yoga.DIRECTION_LTR);
  expect(root.isDirty()).toBe(false);

  root_child0.setMeasureFuncFollowsMeasureModes(true);
  expect(root_child0.isDirty()).toBe(false);
  expect(root.isDirty()).toBe(false);

  root_child0.setMeasureFuncFollowsMeasureModes(false);
  expect(root_child0.isDirty()).toBe(true);
  expect(root.isDirty()).toBe(true);
  expect(root_child0.getMeasureFuncFollowsMeasureModes()).toBe(false);
});

test('reset_restores_measure_func_follows_measure_modes', () => {
  const node = Yoga.Node.create();
  node.setMeasureFuncFollowsMeasureModes(false);

  node.reset();

  expect(node.getMeasureFuncFollowsMeasureModes()).toBe(true);
});
