/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import Yoga from 'yoga-layout';

test('clip_path_defaults_to_none', () => {
  const node = Yoga.Node.create();

  expect(node.getClipPath()).toBe('none');
});

test.each([
  'circle(40% at 50% 50%)',
  'inset(10% 20% round 12px) padding-box',
  'polygon(evenodd, 0 0, 100% 0, 50% 100%)',
  'path("M 0 0 L 100 0 L 50 100 Z") border-box',
  'url("https://example.com/clip.svg#剪裁")',
])('clip_path_round_trips_%s', clipPath => {
  const node = Yoga.Node.create();

  node.setClipPath(clipPath);

  expect(node.getClipPath()).toBe(clipPath);
});

test('clip_path_copies_and_resets_independently', () => {
  const source = Yoga.Node.create();
  const target = Yoga.Node.create();
  const clipPath = 'polygon(0 0, 100% 0, 50% 100%)';

  source.setClipPath(clipPath);
  target.copyStyle(source);
  expect(target.getClipPath()).toBe(clipPath);

  source.setClipPath(undefined);
  expect(source.getClipPath()).toBe('none');
  expect(target.getClipPath()).toBe(clipPath);

  target.reset();
  expect(target.getClipPath()).toBe('none');
});

test('clip_path_dirties_only_when_the_value_changes', () => {
  const node = Yoga.Node.create();
  const clipPath = 'circle(50%)';

  node.calculateLayout(undefined, undefined);
  expect(node.isDirty()).toBe(false);

  node.setClipPath(clipPath);
  expect(node.isDirty()).toBe(true);

  node.calculateLayout(undefined, undefined);
  node.setClipPath(clipPath);
  expect(node.isDirty()).toBe(false);

  node.setClipPath('inset(10px)');
  expect(node.isDirty()).toBe(true);
});

test('clip_path_does_not_change_layout_geometry', () => {
  const root = Yoga.Node.create();
  const child = Yoga.Node.create();
  root.setWidth(200);
  root.setHeight(100);
  child.setWidth(80);
  child.setHeight(40);
  root.insertChild(child, 0);

  root.calculateLayout(undefined, undefined);
  const before = child.getComputedLayout();

  child.setClipPath('circle(25% at 75% 50%)');
  root.calculateLayout(undefined, undefined);

  expect(child.getComputedLayout()).toEqual(before);
});
