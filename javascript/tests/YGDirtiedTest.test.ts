/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import Yoga from 'yoga-layout';

test('dirtied', () => {
  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.setWidth(100);
  root.setHeight(100);

  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  let dirtied = 0;
  root.setDirtiedFunc(() => {
    dirtied++;
  });

  // only nodes with a measure function can be marked dirty
  root.setMeasureFunc(() => ({width: 0, height: 0}));

  expect(dirtied).toBe(0);

  // dirtied func MUST be called in case of explicit dirtying.
  root.markDirty();
  expect(dirtied).toBe(1);

  // dirtied func MUST be called ONCE.
  root.markDirty();
  expect(dirtied).toBe(1);
});

test('dirtied_propagation', () => {
  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.setWidth(100);
  root.setHeight(100);

  const root_child0 = Yoga.Node.create();
  root_child0.setAlignItems(Yoga.ALIGN_FLEX_START);
  root_child0.setWidth(50);
  root_child0.setHeight(20);
  root_child0.setMeasureFunc(() => ({width: 0, height: 0}));
  root.insertChild(root_child0, 0);

  const root_child1 = Yoga.Node.create();
  root_child1.setAlignItems(Yoga.ALIGN_FLEX_START);
  root_child1.setWidth(50);
  root_child1.setHeight(20);
  root.insertChild(root_child1, 0);

  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  let dirtied = 0;
  root.setDirtiedFunc(() => {
    dirtied++;
  });

  expect(dirtied).toBe(0);

  // dirtied func MUST be called for the first time.
  root_child0.markDirty();
  expect(dirtied).toBe(1);

  // dirtied func must NOT be called for the second time.
  root_child0.markDirty();
  expect(dirtied).toBe(1);
});

test('dirtied_hierarchy', () => {
  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.setWidth(100);
  root.setHeight(100);

  const root_child0 = Yoga.Node.create();
  root_child0.setAlignItems(Yoga.ALIGN_FLEX_START);
  root_child0.setWidth(50);
  root_child0.setHeight(20);
  root_child0.setMeasureFunc(() => ({width: 0, height: 0}));
  root.insertChild(root_child0, 0);

  const root_child1 = Yoga.Node.create();
  root_child1.setAlignItems(Yoga.ALIGN_FLEX_START);
  root_child1.setWidth(50);
  root_child1.setHeight(20);
  root_child0.setMeasureFunc(() => ({width: 0, height: 0}));
  root.insertChild(root_child1, 0);

  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  let dirtied = 0;
  root_child0.setDirtiedFunc(() => {
    dirtied++;
  });

  expect(dirtied).toBe(0);

  // dirtied func must NOT be called for descendants.
  // NOTE: nodes without a measure function cannot be marked dirty manually,
  // but nodes with a measure function can not have children.
  // Update the width to dirty the node instead.
  root.setWidth(110);
  expect(dirtied).toBe(0);

  // dirtied func MUST be called in case of explicit dirtying.
  root_child0.markDirty();
  expect(dirtied).toBe(1);
});

test('dirtied_reset', () => {
  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.setWidth(100);
  root.setHeight(100);
  root.setMeasureFunc(() => ({width: 0, height: 0}));

  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  let dirtied = 0;
  root.setDirtiedFunc(() => {
    dirtied++;
  });

  expect(dirtied).toBe(0);

  // dirtied func MUST be called in case of explicit dirtying.
  root.markDirty();
  expect(dirtied).toBe(1);

  // recalculate so the root is no longer dirty
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  root.reset();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.setWidth(100);
  root.setHeight(100);
  root.setMeasureFunc(() => ({width: 0, height: 0}));

  root.markDirty();

  // dirtied func must NOT be called after reset.
  root.markDirty();
  expect(dirtied).toBe(1);
});

test('dirtied_func_exception_propagates_to_caller', () => {
  const root = Yoga.Node.create();
  root.setWidth(100);
  root.setHeight(100);
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  root.setDirtiedFunc(() => {
    throw new Error('dirtied failed');
  });

  expect(() => root.setWidth(50)).toThrow('dirtied failed');
});

test('layout_works_after_repeated_dirtied_func_exceptions', () => {
  // Without holding the error in the bridge, this breaks after about 4,000
  // throws.
  for (let i = 0; i < 5000; i++) {
    const root = Yoga.Node.create();
    const root_child0 = Yoga.Node.create();
    root_child0.setWidth(10);
    root_child0.setHeight(10);
    root.insertChild(root_child0, 0);
    root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

    root.setDirtiedFunc(() => {
      throw new Error('dirtied failed');
    });
    expect(() => root_child0.setWidth(20)).toThrow('dirtied failed');
  }

  const root = Yoga.Node.create();
  root.setWidth(100);
  root.setHeight(100);
  root.setAlignItems(Yoga.ALIGN_FLEX_START);

  const root_child0 = Yoga.Node.create();
  root_child0.setWidth(10);
  root_child0.setHeight(10);
  root.insertChild(root_child0, 0);
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  expect(root_child0.getComputedWidth()).toBe(10);
  expect(root_child0.getComputedHeight()).toBe(10);
});

test('dirtied_func_exception_still_marks_ancestors_dirty', () => {
  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.setWidth(100);
  root.setHeight(100);

  const root_child0 = Yoga.Node.create();
  root_child0.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.insertChild(root_child0, 0);

  const root_child0_child0 = Yoga.Node.create();
  root_child0_child0.setWidth(10);
  root_child0_child0.setHeight(10);
  root_child0.insertChild(root_child0_child0, 0);

  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);
  expect(root_child0.getComputedWidth()).toBe(10);

  root_child0.setDirtiedFunc(() => {
    throw new Error('dirtied failed');
  });

  expect(() => root_child0_child0.setWidth(30)).toThrow('dirtied failed');
  expect(root_child0_child0.isDirty()).toBe(true);
  expect(root_child0.isDirty()).toBe(true);
  expect(root.isDirty()).toBe(true);

  root_child0.setDirtiedFunc(null);
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  expect(root_child0_child0.getComputedWidth()).toBe(30);
  expect(root_child0.getComputedWidth()).toBe(30);
});

test('dirtied_func_exception_keeps_children_in_sync', () => {
  const root = Yoga.Node.create();
  root.setFlexDirection(Yoga.FLEX_DIRECTION_ROW);
  root.setWidth(100);
  root.setHeight(100);

  const root_child0 = Yoga.Node.create();
  root_child0.setWidth(10);
  root.insertChild(root_child0, 0);
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  root.setDirtiedFunc(() => {
    throw new Error('dirtied failed');
  });

  const root_child1 = Yoga.Node.create();
  root_child1.setWidth(20);
  expect(() => root.insertChild(root_child1, 1)).toThrow('dirtied failed');
  expect(root.getChildCount()).toBe(2);
  expect(root.getChild(1)).toBe(root_child1);
  expect(root_child1.getParent()).toBe(root);

  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);
  expect(root_child1.getComputedLeft()).toBe(10);

  expect(() => root.removeChild(root_child0)).toThrow('dirtied failed');
  expect(root.getChildCount()).toBe(1);
  expect(root.getChild(0)).toBe(root_child1);
  expect(root_child0.getParent()).toBe(null);
});

test('dirtied_func_exception_is_rethrown_by_the_outermost_call', () => {
  const other = Yoga.Node.create();

  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.setWidth(100);
  root.setHeight(100);

  const root_child0 = Yoga.Node.create();
  root_child0.setAlignItems(Yoga.ALIGN_FLEX_START);
  root.insertChild(root_child0, 0);

  const root_child0_child0 = Yoga.Node.create();
  root_child0_child0.setWidth(10);
  root_child0_child0.setHeight(10);
  root_child0.insertChild(root_child0_child0, 0);
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  // Runs first, while root_child0_child0 is marked dirty.
  root_child0_child0.setDirtiedFunc(() => {
    throw new Error('dirtied failed');
  });
  // Runs next, and calls into Yoga from inside the dirtied function.
  let innerError: unknown = null;
  root_child0.setDirtiedFunc(() => {
    try {
      other.setWidth(5);
    } catch (e) {
      innerError = e;
    }
  });

  expect(() => root_child0_child0.setWidth(30)).toThrow('dirtied failed');
  expect(innerError).toBe(null);
  expect(root.isDirty()).toBe(true);
});

test('dirtied_func_exception_is_thrown_by_the_call_that_caused_it', () => {
  const other = Yoga.Node.create();
  other.setWidth(10);
  other.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);
  other.setDirtiedFunc(() => {
    throw new Error('other failed');
  });

  const root = Yoga.Node.create();
  root.setWidth(100);
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  let innerError: unknown = null;
  root.setDirtiedFunc(() => {
    try {
      other.setWidth(20);
    } catch (e) {
      innerError = e;
    }
  });

  root.setWidth(50);
  expect(innerError).toEqual(new Error('other failed'));
});

// These throw a WebAssembly.RuntimeError from JS to stand in for a trap or
// abort; a real abort ends the Jest run.
test('dirtied_func_runtime_error_takes_precedence', () => {
  const root = Yoga.Node.create();
  root.setAlignItems(Yoga.ALIGN_FLEX_START);

  const root_child0 = Yoga.Node.create();
  root_child0.setWidth(10);
  root_child0.setHeight(10);
  root.insertChild(root_child0, 0);
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  root_child0.setDirtiedFunc(() => {
    throw new Error('dirtied failed');
  });
  root.setDirtiedFunc(() => {
    throw new WebAssembly.RuntimeError('aborted');
  });

  expect(() => root_child0.setWidth(20)).toThrow(WebAssembly.RuntimeError);
});

test('dirtied_func_runtime_error_keeps_children_in_sync', () => {
  const root = Yoga.Node.create();
  root.setFlexDirection(Yoga.FLEX_DIRECTION_ROW);

  const root_child0 = Yoga.Node.create();
  root_child0.setWidth(10);
  root.insertChild(root_child0, 0);
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  root.setDirtiedFunc(() => {
    throw new WebAssembly.RuntimeError('dirtied failed');
  });

  const root_child1 = Yoga.Node.create();
  expect(() => root.insertChild(root_child1, 1)).toThrow(
    WebAssembly.RuntimeError,
  );
  expect(root.getChildCount()).toBe(2);
  expect(root_child1.getParent()).toBe(root);
});
