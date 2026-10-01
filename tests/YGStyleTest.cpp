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

TEST(YogaTest, clip_path_defaults_and_preserves_owned_serialization) {
  YGNodeRef node = YGNodeNew();

  EXPECT_STREQ("none", YGNodeStyleGetClipPath(node));

  char clipPath[] = "polygon(0 0, 100% 0, 50% 100%) border-box";
  YGNodeStyleSetClipPath(node, clipPath);
  clipPath[0] = 'x';

  EXPECT_STREQ(
      "polygon(0 0, 100% 0, 50% 100%) border-box",
      YGNodeStyleGetClipPath(node));

  YGNodeFree(node);
}

TEST(YogaTest, clip_path_is_copied_and_can_be_reset) {
  YGNodeRef source = YGNodeNew();
  YGNodeRef destination = YGNodeNew();

  YGNodeStyleSetClipPath(source, "circle(40% at 50% 50%)");
  YGNodeCopyStyle(destination, source);
  YGNodeStyleSetClipPath(source, "inset(10px 20px)");

  EXPECT_STREQ("circle(40% at 50% 50%)", YGNodeStyleGetClipPath(destination));

  YGNodeStyleSetClipPath(destination, nullptr);
  EXPECT_STREQ("none", YGNodeStyleGetClipPath(destination));

  YGNodeStyleSetClipPath(destination, "");
  EXPECT_STREQ("", YGNodeStyleGetClipPath(destination));

  YGNodeStyleSetClipPath(destination, "none");
  EXPECT_STREQ("none", YGNodeStyleGetClipPath(destination));

  YGNodeStyleSetClipPath(destination, "circle(25%)");
  YGNodeReset(destination);
  EXPECT_STREQ("none", YGNodeStyleGetClipPath(destination));

  YGNodeFree(source);
  YGNodeFree(destination);
}

TEST(YogaTest, clip_path_dirties_only_when_its_value_changes) {
  YGNodeRef node = YGNodeNew();
  YGNodeStyleSetWidth(node, 100);
  YGNodeStyleSetHeight(node, 100);
  YGNodeCalculateLayout(node, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FALSE(YGNodeIsDirty(node));

  YGNodeStyleSetClipPath(node, "ellipse(25% 40% at 50% 50%)");
  EXPECT_TRUE(YGNodeIsDirty(node));

  YGNodeCalculateLayout(node, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FALSE(YGNodeIsDirty(node));

  YGNodeStyleSetClipPath(node, "ellipse(25% 40% at 50% 50%)");
  EXPECT_FALSE(YGNodeIsDirty(node));

  YGNodeFree(node);
}

TEST(YogaTest, clip_path_does_not_change_layout) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 120);
  YGNodeStyleSetHeight(root, 80);

  YGNodeRef child = YGNodeNew();
  YGNodeStyleSetWidth(child, 30);
  YGNodeStyleSetHeight(child, 20);
  YGNodeStyleSetMargin(child, YGEdgeLeft, 7);
  YGNodeInsertChild(root, child, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  const float rootWidth = YGNodeLayoutGetWidth(root);
  const float rootHeight = YGNodeLayoutGetHeight(root);
  const float childLeft = YGNodeLayoutGetLeft(child);
  const float childTop = YGNodeLayoutGetTop(child);
  const float childWidth = YGNodeLayoutGetWidth(child);
  const float childHeight = YGNodeLayoutGetHeight(child);

  YGNodeStyleSetClipPath(child, "polygon(0 0, 100% 0, 100% 100%, 0 100%)");
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(rootWidth, YGNodeLayoutGetWidth(root));
  EXPECT_FLOAT_EQ(rootHeight, YGNodeLayoutGetHeight(root));
  EXPECT_FLOAT_EQ(childLeft, YGNodeLayoutGetLeft(child));
  EXPECT_FLOAT_EQ(childTop, YGNodeLayoutGetTop(child));
  EXPECT_FLOAT_EQ(childWidth, YGNodeLayoutGetWidth(child));
  EXPECT_FLOAT_EQ(childHeight, YGNodeLayoutGetHeight(child));

  YGNodeFreeRecursive(root);
}
