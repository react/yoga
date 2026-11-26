/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

TEST(YogaTest, dont_cache_computed_flex_basis_between_layouts) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureWebFlexBasis, true);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeightPercent(root, 100);
  YGNodeStyleSetWidthPercent(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasisPercent(root_child0, 100);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, YGUndefined, YGDirectionLTR);
  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);

  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, recalculate_resolvedDimension_onchange) {
  YGNodeRef root = YGNodeNew();

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetMinHeight(root_child0, 10);
  YGNodeStyleSetMaxHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetHeight(root_child0));

  YGNodeStyleSetMinHeight(root_child0, YGUndefined);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, relayout_containing_block_size_changes) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root, YGPositionTypeAbsolute);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeRelative);
  YGNodeStyleSetMargin(root_child0, YGEdgeLeft, 4);
  YGNodeStyleSetMargin(root_child0, YGEdgeTop, 5);
  YGNodeStyleSetMargin(root_child0, YGEdgeRight, 9);
  YGNodeStyleSetMargin(root_child0, YGEdgeBottom, 1);
  YGNodeStyleSetPadding(root_child0, YGEdgeLeft, 2);
  YGNodeStyleSetPadding(root_child0, YGEdgeTop, 9);
  YGNodeStyleSetPadding(root_child0, YGEdgeRight, 11);
  YGNodeStyleSetPadding(root_child0, YGEdgeBottom, 13);
  YGNodeStyleSetBorder(root_child0, YGEdgeLeft, 5);
  YGNodeStyleSetBorder(root_child0, YGEdgeTop, 6);
  YGNodeStyleSetBorder(root_child0, YGEdgeRight, 7);
  YGNodeStyleSetBorder(root_child0, YGEdgeBottom, 8);
  YGNodeStyleSetWidth(root_child0, 500);
  YGNodeStyleSetHeight(root_child0, 500);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root_child0_child0, YGPositionTypeStatic);
  YGNodeStyleSetMargin(root_child0_child0, YGEdgeLeft, 8);
  YGNodeStyleSetMargin(root_child0_child0, YGEdgeTop, 6);
  YGNodeStyleSetMargin(root_child0_child0, YGEdgeRight, 3);
  YGNodeStyleSetMargin(root_child0_child0, YGEdgeBottom, 9);
  YGNodeStyleSetPadding(root_child0_child0, YGEdgeLeft, 1);
  YGNodeStyleSetPadding(root_child0_child0, YGEdgeTop, 7);
  YGNodeStyleSetPadding(root_child0_child0, YGEdgeRight, 9);
  YGNodeStyleSetPadding(root_child0_child0, YGEdgeBottom, 4);
  YGNodeStyleSetBorder(root_child0_child0, YGEdgeLeft, 8);
  YGNodeStyleSetBorder(root_child0_child0, YGEdgeTop, 10);
  YGNodeStyleSetBorder(root_child0_child0, YGEdgeRight, 2);
  YGNodeStyleSetBorder(root_child0_child0, YGEdgeBottom, 1);
  YGNodeStyleSetWidth(root_child0_child0, 200);
  YGNodeStyleSetHeight(root_child0_child0, 200);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeRef root_child0_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root_child0_child0_child0, YGPositionTypeAbsolute);
  YGNodeStyleSetPosition(root_child0_child0_child0, YGEdgeLeft, 2);
  YGNodeStyleSetPosition(root_child0_child0_child0, YGEdgeRight, 12);
  YGNodeStyleSetMargin(root_child0_child0_child0, YGEdgeLeft, 9);
  YGNodeStyleSetMargin(root_child0_child0_child0, YGEdgeTop, 12);
  YGNodeStyleSetMargin(root_child0_child0_child0, YGEdgeRight, 4);
  YGNodeStyleSetMargin(root_child0_child0_child0, YGEdgeBottom, 7);
  YGNodeStyleSetPadding(root_child0_child0_child0, YGEdgeLeft, 5);
  YGNodeStyleSetPadding(root_child0_child0_child0, YGEdgeTop, 3);
  YGNodeStyleSetPadding(root_child0_child0_child0, YGEdgeRight, 8);
  YGNodeStyleSetPadding(root_child0_child0_child0, YGEdgeBottom, 10);
  YGNodeStyleSetBorder(root_child0_child0_child0, YGEdgeLeft, 2);
  YGNodeStyleSetBorder(root_child0_child0_child0, YGEdgeTop, 1);
  YGNodeStyleSetBorder(root_child0_child0_child0, YGEdgeRight, 5);
  YGNodeStyleSetBorder(root_child0_child0_child0, YGEdgeBottom, 9);
  YGNodeStyleSetWidthPercent(root_child0_child0_child0, 41);
  YGNodeStyleSetHeightPercent(root_child0_child0_child0, 63);
  YGNodeInsertChild(root_child0_child0, root_child0_child0_child0, 0);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(513, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(506, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(4, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(15, YGNodeLayoutGetLeft(root_child0_child0));
  ASSERT_FLOAT_EQ(21, YGNodeLayoutGetTop(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child0_child0));

  ASSERT_FLOAT_EQ(1, YGNodeLayoutGetLeft(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(29, YGNodeLayoutGetTop(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(306, YGNodeLayoutGetHeight(root_child0_child0_child0));

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionRTL);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(513, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(506, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(4, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(279, YGNodeLayoutGetLeft(root_child0_child0));
  ASSERT_FLOAT_EQ(21, YGNodeLayoutGetTop(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child0_child0));

  ASSERT_FLOAT_EQ(-2, YGNodeLayoutGetLeft(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(29, YGNodeLayoutGetTop(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(306, YGNodeLayoutGetHeight(root_child0_child0_child0));

  // Relayout starts here
  YGNodeStyleSetWidth(root_child0, 456);
  YGNodeStyleSetHeight(root_child0, 432);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(469, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(438, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(4, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(456, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(432, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(15, YGNodeLayoutGetLeft(root_child0_child0));
  ASSERT_FLOAT_EQ(21, YGNodeLayoutGetTop(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child0_child0));

  ASSERT_FLOAT_EQ(1, YGNodeLayoutGetLeft(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(29, YGNodeLayoutGetTop(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(182, YGNodeLayoutGetWidth(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(263, YGNodeLayoutGetHeight(root_child0_child0_child0));

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionRTL);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(469, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(438, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(4, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(456, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(432, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(235, YGNodeLayoutGetLeft(root_child0_child0));
  ASSERT_FLOAT_EQ(21, YGNodeLayoutGetTop(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child0_child0));

  ASSERT_FLOAT_EQ(16, YGNodeLayoutGetLeft(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(29, YGNodeLayoutGetTop(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(182, YGNodeLayoutGetWidth(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(263, YGNodeLayoutGetHeight(root_child0_child0_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, has_new_layout_flag_set_static) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeStatic);
  YGNodeStyleSetWidth(root_child0, 10);
  YGNodeStyleSetHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child1 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child1, YGPositionTypeAbsolute);
  YGNodeStyleSetWidth(root_child0_child1, 5);
  YGNodeStyleSetHeight(root_child0_child1, 5);
  YGNodeInsertChild(root_child0, root_child0_child1, 0);

  YGNodeRef root_child0_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child0, YGPositionTypeStatic);
  YGNodeStyleSetWidth(root_child0_child0, 5);
  YGNodeStyleSetHeight(root_child0_child0, 5);
  YGNodeInsertChild(root_child0, root_child0_child0, 1);

  YGNodeRef root_child0_child0_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child0_child0, YGPositionTypeAbsolute);
  YGNodeStyleSetWidthPercent(root_child0_child0_child0, 1);
  YGNodeStyleSetHeight(root_child0_child0_child0, 1);
  YGNodeInsertChild(root_child0_child0, root_child0_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  YGNodeSetHasNewLayout(root, false);
  YGNodeSetHasNewLayout(root_child0, false);
  YGNodeSetHasNewLayout(root_child0_child0, false);
  YGNodeSetHasNewLayout(root_child0_child0_child0, false);

  YGNodeStyleSetWidth(root, 110);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_TRUE(YGNodeGetHasNewLayout(root));
  ASSERT_TRUE(YGNodeGetHasNewLayout(root_child0));
  ASSERT_TRUE(YGNodeGetHasNewLayout(root_child0_child0));
  ASSERT_TRUE(YGNodeGetHasNewLayout(root_child0_child0_child0));

  YGNodeFreeRecursive(root);
}

TEST(
    YogaTest,
    percent_height_in_auto_container_relayout_on_root_height_change) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 400);
  YGNodeStyleSetHeight(root, 1000);

  YGNodeRef container = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root, container, 0);

  YGNodeRef child = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeightPercent(child, 50);
  YGNodeInsertChild(container, child, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(container));
  ASSERT_FLOAT_EQ(250, YGNodeLayoutGetHeight(child));

  YGNodeStyleSetHeight(root, 1200);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(600, YGNodeLayoutGetHeight(container));
  ASSERT_FLOAT_EQ(300, YGNodeLayoutGetHeight(child));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(
    YogaTest,
    percent_row_gap_in_auto_container_relayout_on_root_height_change) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 400);
  YGNodeStyleSetHeight(root, 1000);

  YGNodeRef container = YGNodeNewWithConfig(config);
  YGNodeStyleSetGapPercent(container, YGGutterRow, 10);
  YGNodeInsertChild(root, container, 0);

  for (uint32_t i = 0; i < 2; i++) {
    YGNodeRef child = YGNodeNewWithConfig(config);
    YGNodeStyleSetHeight(child, 50);
    YGNodeInsertChild(container, child, i);
  }

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(120, YGNodeLayoutGetHeight(container));

  YGNodeStyleSetHeight(root, 1200);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(122, YGNodeLayoutGetHeight(container));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(
    YogaTest,
    percent_height_under_display_contents_relayout_on_root_height_change) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 400);
  YGNodeStyleSetHeight(root, 1000);

  YGNodeRef container = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root, container, 0);

  // The height of a `display: contents` node doesn't apply; its child takes
  // part in the container's layout.
  YGNodeRef contents = YGNodeNewWithConfig(config);
  YGNodeStyleSetDisplay(contents, YGDisplayContents);
  YGNodeStyleSetHeight(contents, 10);
  YGNodeInsertChild(container, contents, 0);

  YGNodeRef child = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeightPercent(child, 50);
  YGNodeInsertChild(contents, child, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(container));

  YGNodeStyleSetHeight(root, 1200);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(600, YGNodeLayoutGetHeight(container));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(
    YogaTest,
    partial_flex_grow_in_auto_column_relayout_on_root_height_change) {
  // Under the StretchFlexBasis errata a column measured with a FitContent
  // height hands its free space to children with flex grow, so a child with
  // flex grow 0.5 takes half of it and the column depends on the available
  // height without filling it.
  YGConfigRef config = YGConfigNew();
  YGConfigSetErrata(config, YGErrataStretchFlexBasis);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 400);
  YGNodeStyleSetHeight(root, 1000);

  YGNodeRef container = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(container, 100);
  YGNodeInsertChild(root, container, 0);

  YGNodeRef child = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(child, 100);
  YGNodeStyleSetFlexGrow(child, 0.5f);
  YGNodeInsertChild(container, child, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  const float before = YGNodeLayoutGetHeight(container);

  YGNodeStyleSetHeight(root, 1200);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  const float after = YGNodeLayoutGetHeight(container);

  YGNodeRef freshRoot = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(freshRoot, YGFlexDirectionRow);
  YGNodeStyleSetAlignItems(freshRoot, YGAlignFlexStart);
  YGNodeStyleSetWidth(freshRoot, 400);
  YGNodeStyleSetHeight(freshRoot, 1200);
  YGNodeRef freshContainer = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(freshContainer, 100);
  YGNodeInsertChild(freshRoot, freshContainer, 0);
  YGNodeRef freshChild = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(freshChild, 100);
  YGNodeStyleSetFlexGrow(freshChild, 0.5f);
  YGNodeInsertChild(freshContainer, freshChild, 0);
  YGNodeCalculateLayout(freshRoot, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_NE(before, after);
  ASSERT_FLOAT_EQ(YGNodeLayoutGetHeight(freshContainer), after);

  YGNodeFreeRecursive(root);
  YGNodeFreeRecursive(freshRoot);

  YGConfigFree(config);
}

TEST(YogaTest, percent_padding_follows_owner_size_on_relayout) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetPaddingPercent(root_child0, YGEdgeLeft, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetPadding(root_child0, YGEdgeLeft));

  YGNodeStyleSetWidth(root, 60);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(30, YGNodeLayoutGetPadding(root_child0, YGEdgeLeft));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, percent_min_width_follows_owner_size_on_relayout) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetMinWidthPercent(root_child0, 100);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(root_child0_child0, 10);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0_child0));

  YGNodeStyleSetWidth(root, 200);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, percent_min_height_follows_owner_size_on_relayout) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(root_child0, 50);
  YGNodeStyleSetMinHeightPercent(root_child0, 100);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  YGNodeStyleSetHeight(root, 200);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(
    YogaTest,
    percent_padding_follows_owner_size_on_relayout_with_taller_fit_content) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetPaddingPercent(root_child0, YGEdgeTop, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(root_child0_child0, 10);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetPadding(root_child0, YGEdgeTop));
  ASSERT_FLOAT_EQ(60, YGNodeLayoutGetHeight(root_child0));

  YGNodeStyleSetWidth(root, 60);
  YGNodeStyleSetHeight(root, 120);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(30, YGNodeLayoutGetPadding(root_child0, YGEdgeTop));
  ASSERT_FLOAT_EQ(40, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}
