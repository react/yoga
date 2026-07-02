/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

static void recursivelyAssertProperNodeOwnership(YGNodeRef node) {
  for (size_t i = 0; i < YGNodeGetChildCount(node); ++i) {
    const auto child = YGNodeGetChild(node, i);
    ASSERT_EQ(node, YGNodeGetOwner(child));
    recursivelyAssertProperNodeOwnership(child);
  }
}

TEST(YogaTest, absolute_node_cloned_with_static_parent) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeStatic);
  YGNodeStyleSetWidth(root_child0, 10);
  YGNodeStyleSetHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child0, YGPositionTypeAbsolute);
  YGNodeStyleSetWidthPercent(root_child0_child0, 1);
  YGNodeStyleSetHeight(root_child0_child0, 1);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeRef clonedRoot = YGNodeClone(root);
  YGNodeStyleSetWidth(clonedRoot, 110);
  YGNodeCalculateLayout(clonedRoot, YGUndefined, YGUndefined, YGDirectionLTR);

  recursivelyAssertProperNodeOwnership(clonedRoot);

  YGNodeFreeRecursive(root);
  YGNodeFreeRecursive(clonedRoot);
}

TEST(YogaTest, absolute_node_cloned_through_nested_display_contents) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef wrapper = YGNodeNew();
  YGNodeStyleSetPositionType(wrapper, YGPositionTypeStatic);
  YGNodeStyleSetWidth(wrapper, 50);
  YGNodeStyleSetHeight(wrapper, 50);
  YGNodeInsertChild(root, wrapper, 0);

  YGNodeRef static1 = YGNodeNew();
  YGNodeStyleSetPositionType(static1, YGPositionTypeStatic);
  YGNodeStyleSetFlexGrow(static1, 1);
  YGNodeInsertChild(wrapper, static1, 0);

  YGNodeRef contents1 = YGNodeNew();
  YGNodeStyleSetDisplay(contents1, YGDisplayContents);
  YGNodeInsertChild(static1, contents1, 0);

  YGNodeRef contents2 = YGNodeNew();
  YGNodeStyleSetDisplay(contents2, YGDisplayContents);
  YGNodeInsertChild(contents1, contents2, 0);

  YGNodeRef absolute = YGNodeNew();
  YGNodeStyleSetPositionType(absolute, YGPositionTypeAbsolute);
  YGNodeStyleSetWidthPercent(absolute, 50);
  YGNodeStyleSetHeight(absolute, 1);
  YGNodeInsertChild(contents2, absolute, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeRef clonedRoot = YGNodeClone(root);
  YGNodeStyleSetWidth(clonedRoot, 200);
  YGNodeCalculateLayout(clonedRoot, YGUndefined, YGUndefined, YGDirectionLTR);

  recursivelyAssertProperNodeOwnership(clonedRoot);

  YGNodeFreeRecursive(root);
  YGNodeFreeRecursive(clonedRoot);
}

TEST(YogaTest, absolute_node_cloned_with_static_ancestors) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeStatic);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child0, YGPositionTypeStatic);
  YGNodeStyleSetWidth(root_child0_child0, 40);
  YGNodeStyleSetHeight(root_child0_child0, 40);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeRef root_child0_child0_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child0_child0, YGPositionTypeStatic);
  YGNodeStyleSetWidth(root_child0_child0_child0, 30);
  YGNodeStyleSetHeight(root_child0_child0_child0, 30);
  YGNodeInsertChild(root_child0_child0, root_child0_child0_child0, 0);

  YGNodeRef root_child0_child0_child0_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(
      root_child0_child0_child0_child0, YGPositionTypeAbsolute);
  YGNodeStyleSetWidthPercent(root_child0_child0_child0_child0, 1);
  YGNodeStyleSetHeight(root_child0_child0_child0_child0, 1);
  YGNodeInsertChild(
      root_child0_child0_child0, root_child0_child0_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeRef clonedRoot = YGNodeClone(root);
  YGNodeStyleSetWidth(clonedRoot, 110);
  YGNodeCalculateLayout(clonedRoot, YGUndefined, YGUndefined, YGDirectionLTR);

  recursivelyAssertProperNodeOwnership(clonedRoot);

  YGNodeFreeRecursive(root);
  YGNodeFreeRecursive(clonedRoot);
}
