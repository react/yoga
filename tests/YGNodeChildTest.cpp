/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <stdexcept>

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

TEST(YogaTest, reset_layout_when_child_removed) {
  YGNodeRef root = YGNodeNew();

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0, 100);
  YGNodeStyleSetHeight(root_child0, 100);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  YGNodeRemoveChild(root, root_child0);

  ASSERT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_TRUE(YGFloatIsUndefined(YGNodeLayoutGetWidth(root_child0)));
  ASSERT_TRUE(YGFloatIsUndefined(YGNodeLayoutGetHeight(root_child0)));

  YGNodeFreeRecursive(root);
  YGNodeFreeRecursive(root_child0);
}

TEST(YogaTest, removed_child_can_be_reused_with_valid_layout) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);

  YGNodeRef child = YGNodeNew();
  YGNodeStyleSetWidth(child, 100);
  YGNodeStyleSetHeight(child, 100);
  YGNodeInsertChild(root, child, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(100, YGNodeLayoutGetWidth(child));
  ASSERT_EQ(100, YGNodeLayoutGetHeight(child));

  // Remove child - layout should be cleared and child marked dirty
  YGNodeRemoveChild(root, child);

  ASSERT_TRUE(YGFloatIsUndefined(YGNodeLayoutGetWidth(child)));
  ASSERT_TRUE(YGFloatIsUndefined(YGNodeLayoutGetHeight(child)));
  ASSERT_TRUE(YGNodeIsDirty(child));

  // Reinsert the child and recalculate - layout should be valid again
  YGNodeInsertChild(root, child, 0);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(100, YGNodeLayoutGetWidth(child));
  ASSERT_EQ(100, YGNodeLayoutGetHeight(child));
  ASSERT_FALSE(YGNodeIsDirty(child));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, swap_child_with_out_of_bounds_index_is_rejected) {
  YGNodeRef root = YGNodeNew();
  YGNodeRef child = YGNodeNew();
  YGNodeInsertChild(root, child, 0);

  // Replacing an existing index works.
  YGNodeRef replacement = YGNodeNew();
  YGNodeSwapChild(root, replacement, 0);
  ASSERT_EQ(replacement, YGNodeGetChild(root, 0));

  // An index past the last child is rejected rather than reading and writing
  // out of bounds on the underlying children vector.
  YGNodeRef stray = YGNodeNew();
  ASSERT_THROW(YGNodeSwapChild(root, stray, 1), std::logic_error);

  YGNodeFree(stray);
  YGNodeFree(child);
  YGNodeFreeRecursive(root);
}

TEST(YogaTest, insert_child_with_out_of_bounds_index_is_rejected) {
  YGNodeRef root = YGNodeNew();

  YGNodeRef first = YGNodeNew();
  YGNodeInsertChild(root, first, 0);

  // Appending at the end (index == child count) is allowed.
  YGNodeRef second = YGNodeNew();
  YGNodeInsertChild(root, second, 1);
  ASSERT_EQ(2u, YGNodeGetChildCount(root));

  // An index beyond the end is rejected rather than constructing an invalid
  // iterator into the children vector.
  YGNodeRef stray = YGNodeNew();
  ASSERT_THROW(YGNodeInsertChild(root, stray, 5), std::logic_error);

  YGNodeFree(stray);
  YGNodeFreeRecursive(root);
}
