/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

TEST(YogaTest, copy_style_same) {
  YGNodeRef node0 = YGNodeNew();
  YGNodeRef node1 = YGNodeNew();

  YGNodeCopyStyle(node0, node1);

  YGNodeFree(node0);
  YGNodeFree(node1);
}

TEST(YogaTest, copy_style_modified) {
  YGNodeRef node0 = YGNodeNew();
  ASSERT_EQ(YGFlexDirectionColumn, YGNodeStyleGetFlexDirection(node0));
  ASSERT_FALSE(YGNodeStyleGetMaxHeight(node0).unit != YGUnitUndefined);

  YGNodeRef node1 = YGNodeNew();
  YGNodeStyleSetFlexDirection(node1, YGFlexDirectionRow);
  YGNodeStyleSetMaxHeight(node1, 10);

  YGNodeCopyStyle(node0, node1);
  ASSERT_EQ(YGFlexDirectionRow, YGNodeStyleGetFlexDirection(node0));
  ASSERT_FLOAT_EQ(10, YGNodeStyleGetMaxHeight(node0).value);

  YGNodeFree(node0);
  YGNodeFree(node1);
}

TEST(YogaTest, copy_style_modified_same) {
  YGNodeRef node0 = YGNodeNew();
  YGNodeStyleSetFlexDirection(node0, YGFlexDirectionRow);
  YGNodeStyleSetMaxHeight(node0, 10);
  YGNodeCalculateLayout(node0, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeRef node1 = YGNodeNew();
  YGNodeStyleSetFlexDirection(node1, YGFlexDirectionRow);
  YGNodeStyleSetMaxHeight(node1, 10);

  YGNodeCopyStyle(node0, node1);

  YGNodeFree(node0);
  YGNodeFree(node1);
}

TEST(YogaTest, copy_style_box_sizing) {
  YGNodeRef node0 = YGNodeNew();
  YGNodeRef node1 = YGNodeNew();
  YGNodeStyleSetBoxSizing(node1, YGBoxSizingContentBox);

  YGNodeCopyStyle(node0, node1);
  ASSERT_EQ(YGBoxSizingContentBox, YGNodeStyleGetBoxSizing(node0));

  YGNodeFree(node0);
  YGNodeFree(node1);
}

TEST(YogaTest, initialise_flexShrink_flexGrow) {
  YGNodeRef node0 = YGNodeNew();
  YGNodeStyleSetFlexShrink(node0, 1);
  ASSERT_EQ(1, YGNodeStyleGetFlexShrink(node0));

  YGNodeStyleSetFlexShrink(node0, YGUndefined);
  YGNodeStyleSetFlexGrow(node0, 3);
  ASSERT_EQ(
      0,
      YGNodeStyleGetFlexShrink(
          node0)); // Default value is Zero, if flex shrink is not defined
  ASSERT_EQ(3, YGNodeStyleGetFlexGrow(node0));

  YGNodeStyleSetFlexGrow(node0, YGUndefined);
  YGNodeStyleSetFlexShrink(node0, 3);
  ASSERT_EQ(
      0,
      YGNodeStyleGetFlexGrow(
          node0)); // Default value is Zero, if flex grow is not defined
  ASSERT_EQ(3, YGNodeStyleGetFlexShrink(node0));
  YGNodeFree(node0);
}
