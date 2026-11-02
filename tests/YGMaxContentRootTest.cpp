/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

// TODO: move these to fixture based tests once gentest supports passing an
// owner size to YGNodeCalculateLayout. With an undefined owner size,
// max-content on the root is indistinguishable from auto, so only these
// hand-written tests can pin the root keyword branches.

// Equivalent fixture, laid out with an owner size of 500x500 (the part
// gentest cannot express):
//
// <div style="flex-direction: row; width: max-content;">
//   <div style="width: 50px; height: 50px;"></div>
//   <div style="width: 100px; height: 50px;"></div>
//   <div style="width: 25px; height: 50px;"></div>
// </div>
TEST(MaxContentRoot, width_max_content_with_definite_owner_size) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidthMaxContent(root);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 100);
  YGNodeStyleSetHeight(root_child1, 50);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeRef root_child2 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child2, 25);
  YGNodeStyleSetHeight(root_child2, 50);
  YGNodeInsertChild(root, root_child2, 2);

  YGNodeCalculateLayout(root, 500, 500, YGDirectionLTR);

  // The max-content axis measures content instead of filling the owner.
  ASSERT_FLOAT_EQ(175, YGNodeLayoutGetWidth(root));
  // The auto axis still fills the owner.
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetLeft(root_child2));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// A numeric max-width clamps the root's max-content size, and the content
// must lay out (wrap) within the clamped size, not just report it.
//
// Equivalent fixture, laid out with an owner size of 500x500:
//
// <div style="flex-direction: row; flex-wrap: wrap; width: max-content; max-width: 100px;">
//   <div style="width: 80px; height: 10px;"></div>
//   <div style="width: 80px; height: 10px;"></div>
// </div>
TEST(MaxContentRoot, width_max_content_wraps_within_numeric_max_width) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetFlexWrap(root, YGWrapWrap);
  YGNodeStyleSetWidthMaxContent(root);
  YGNodeStyleSetMaxWidth(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 80);
  YGNodeStyleSetHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 80);
  YGNodeStyleSetHeight(root_child1, 10);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeCalculateLayout(root, 500, 500, YGDirectionLTR);

  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetTop(root_child1));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Percentages inside a max-content root are cyclic: they contribute nothing
// to the measurement, then resolve against the resolved size — the same
// two-pass contract the flex-item path guarantees.
//
// Equivalent fixture, laid out with an owner size of 500x500:
//
// <div style="width: max-content;">
//   <div style="width: 100px; height: 10px;"></div>
//   <div style="width: 50%; height: 10px;"></div>
// </div>
TEST(MaxContentRoot, width_max_content_resolves_percent_child) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidthMaxContent(root);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 100);
  YGNodeStyleSetHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidthPercent(root_child1, 50);
  YGNodeStyleSetHeight(root_child1, 10);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeCalculateLayout(root, 500, 500, YGDirectionLTR);

  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root_child1));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Equivalent fixture, laid out with an owner size of 500x500 (the part
// gentest cannot express):
//
// <div style="height: max-content;">
//   <div style="width: 50px; height: 50px;"></div>
//   <div style="width: 50px; height: 100px;"></div>
//   <div style="width: 50px; height: 25px;"></div>
// </div>
TEST(MaxContentRoot, height_max_content_with_definite_owner_size) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeightMaxContent(root);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 50);
  YGNodeStyleSetHeight(root_child1, 100);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeRef root_child2 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child2, 50);
  YGNodeStyleSetHeight(root_child2, 25);
  YGNodeInsertChild(root, root_child2, 2);

  YGNodeCalculateLayout(root, 500, 500, YGDirectionLTR);

  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(175, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetTop(root_child2));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
