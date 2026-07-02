/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

static YGSize _measure(
    YGNodeConstRef node,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  int* measureCount = (int*)YGNodeGetContext(node);
  if (measureCount != nullptr) {
    (*measureCount)++;
  }

  return YGSize{10, 10};
}

static YGSize _simulate_wrapping_text(
    YGNodeConstRef /*node*/,
    float width,
    YGMeasureMode widthMode,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  if (widthMode == YGMeasureModeUndefined || width >= 68) {
    return YGSize{68, 16};
  }

  return YGSize{50, 32};
}

static YGSize _measure_assert_negative(
    YGNodeConstRef /*node*/,
    float width,
    YGMeasureMode /*widthMode*/,
    float height,
    YGMeasureMode /*heightMode*/) {
  EXPECT_GE(width, 0);
  EXPECT_GE(height, 0);

  return YGSize{0, 0};
}

TEST(YogaTest, dont_measure_single_grow_shrink_child) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  int measureCount = 0;

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measure);
  YGNodeStyleSetFlexGrow(root_child0, 1);
  YGNodeStyleSetFlexShrink(root_child0, 1);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(0, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, measure_absolute_child_with_no_constraints) {
  YGNodeRef root = YGNodeNew();

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeInsertChild(root, root_child0, 0);

  int measureCount = 0;

  YGNodeRef root_child0_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child0, YGPositionTypeAbsolute);
  YGNodeSetContext(root_child0_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0_child0, _measure);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(1, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dont_measure_when_min_equals_max) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  int measureCount = 0;

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measure);
  YGNodeStyleSetMinWidth(root_child0, 10);
  YGNodeStyleSetMaxWidth(root_child0, 10);
  YGNodeStyleSetMinHeight(root_child0, 10);
  YGNodeStyleSetMaxHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(0, measureCount);
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dont_measure_when_min_equals_max_percentages) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  int measureCount = 0;

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measure);
  YGNodeStyleSetMinWidthPercent(root_child0, 10);
  YGNodeStyleSetMaxWidthPercent(root_child0, 10);
  YGNodeStyleSetMinHeightPercent(root_child0, 10);
  YGNodeStyleSetMaxHeightPercent(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(0, measureCount);
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, measure_nodes_with_margin_auto_and_stretch) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 500);
  YGNodeStyleSetHeight(root, 500);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeSetMeasureFunc(root_child0, _measure);
  YGNodeStyleSetMarginAuto(root_child0, YGEdgeLeft);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_EQ(490, YGNodeLayoutGetLeft(root_child0));
  EXPECT_EQ(0, YGNodeLayoutGetTop(root_child0));
  EXPECT_EQ(10, YGNodeLayoutGetWidth(root_child0));
  EXPECT_EQ(10, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dont_measure_when_min_equals_max_mixed_width_percent) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  int measureCount = 0;

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measure);
  YGNodeStyleSetMinWidthPercent(root_child0, 10);
  YGNodeStyleSetMaxWidthPercent(root_child0, 10);
  YGNodeStyleSetMinHeight(root_child0, 10);
  YGNodeStyleSetMaxHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(0, measureCount);
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dont_measure_when_min_equals_max_mixed_height_percent) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  int measureCount = 0;

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measure);
  YGNodeStyleSetMinWidth(root_child0, 10);
  YGNodeStyleSetMaxWidth(root_child0, 10);
  YGNodeStyleSetMinHeightPercent(root_child0, 10);
  YGNodeStyleSetMaxHeightPercent(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(0, measureCount);
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, measure_enough_size_should_be_in_single_line) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetAlignSelf(root_child0, YGAlignFlexStart);
  YGNodeSetMeasureFunc(root_child0, _simulate_wrapping_text);

  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(68, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(16, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, measure_not_enough_size_should_wrap) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 55);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetAlignSelf(root_child0, YGAlignFlexStart);
  YGNodeSetMeasureFunc(root_child0, _simulate_wrapping_text);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(32, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, measure_zero_space_should_grow) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetHeight(root, 200);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);
  YGNodeStyleSetFlexGrow(root, 0);

  int measureCount = 0;

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetFlexDirection(root_child0, YGFlexDirectionColumn);
  YGNodeStyleSetPadding(root_child0, YGEdgeAll, 100);
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measure);

  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 282, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(282, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, measure_flex_direction_row_and_padding) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetPadding(root, YGEdgeLeft, 25);
  YGNodeStyleSetPadding(root, YGEdgeTop, 25);
  YGNodeStyleSetPadding(root, YGEdgeRight, 25);
  YGNodeStyleSetPadding(root, YGEdgeBottom, 25);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _simulate_wrapping_text);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 5);
  YGNodeStyleSetHeight(root_child1, 5);
  YGNodeInsertChild(root, root_child1, 1);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(75, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetHeight(root_child1));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, measure_flex_direction_column_and_padding) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(root, YGEdgeTop, 20);
  YGNodeStyleSetPadding(root, YGEdgeAll, 25);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _simulate_wrapping_text);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 5);
  YGNodeStyleSetHeight(root_child1, 5);
  YGNodeInsertChild(root, root_child1, 1);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(32, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(57, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetHeight(root_child1));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, measure_flex_direction_row_no_padding) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetMargin(root, YGEdgeTop, 20);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _simulate_wrapping_text);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 5);
  YGNodeStyleSetHeight(root_child1, 5);
  YGNodeInsertChild(root, root_child1, 1);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetHeight(root_child1));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, measure_flex_direction_row_no_padding_align_items_flexstart) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetMargin(root, YGEdgeTop, 20);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _simulate_wrapping_text);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 5);
  YGNodeStyleSetHeight(root_child1, 5);
  YGNodeInsertChild(root, root_child1, 1);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(32, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetHeight(root_child1));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, measure_with_fixed_size) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(root, YGEdgeTop, 20);
  YGNodeStyleSetPadding(root, YGEdgeAll, 25);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _simulate_wrapping_text);
  YGNodeStyleSetWidth(root_child0, 10);
  YGNodeStyleSetHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 5);
  YGNodeStyleSetHeight(root_child1, 5);
  YGNodeInsertChild(root, root_child1, 1);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(35, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetHeight(root_child1));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, measure_with_flex_shrink) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(root, YGEdgeTop, 20);
  YGNodeStyleSetPadding(root, YGEdgeAll, 25);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _simulate_wrapping_text);
  YGNodeStyleSetFlexShrink(root_child0, 1);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 5);
  YGNodeStyleSetHeight(root_child1, 5);
  YGNodeInsertChild(root, root_child1, 1);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetHeight(root_child1));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, measure_no_padding) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(root, YGEdgeTop, 20);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _simulate_wrapping_text);
  YGNodeStyleSetFlexShrink(root_child0, 1);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 5);
  YGNodeStyleSetHeight(root_child1, 5);
  YGNodeInsertChild(root, root_child1, 1);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(32, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(32, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetHeight(root_child1));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

#if GTEST_HAS_DEATH_TEST
TEST(YogaDeathTest, cannot_add_child_to_node_with_measure_func) {
  YGNodeRef root = YGNodeNew();
  YGNodeSetMeasureFunc(root, _measure);

  YGNodeRef root_child0 = YGNodeNew();
#if defined(__cpp_exceptions)
  ASSERT_THROW(YGNodeInsertChild(root, root_child0, 0), std::logic_error);
#else // !defined(__cpp_exceptions)
  ASSERT_DEATH(YGNodeInsertChild(root, root_child0, 0), "Cannot add child.*");
#endif // defined(__cpp_exceptions)
  YGNodeFree(root_child0);
  YGNodeFreeRecursive(root);
}

TEST(YogaDeathTest, cannot_add_nonnull_measure_func_to_non_leaf_node) {
  YGNodeRef root = YGNodeNew();
  YGNodeRef root_child0 = YGNodeNew();
  YGNodeInsertChild(root, root_child0, 0);
#if defined(__cpp_exceptions)
  ASSERT_THROW(YGNodeSetMeasureFunc(root, _measure), std::logic_error);
#else // !defined(__cpp_exceptions)
  ASSERT_DEATH(
      YGNodeSetMeasureFunc(root, _measure), "Cannot set measure function.*");
#endif // defined(__cpp_exceptions)
  YGNodeFreeRecursive(root);
}

#endif

TEST(YogaTest, can_nullify_measure_func_on_any_node) {
  YGNodeRef root = YGNodeNew();
  YGNodeInsertChild(root, YGNodeNew(), 0);
  YGNodeSetMeasureFunc(root, nullptr);
  ASSERT_TRUE(!YGNodeHasMeasureFunc(root));
  YGNodeFreeRecursive(root);
}

TEST(YogaTest, cant_call_negative_measure) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 10);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _measure_assert_negative);
  YGNodeStyleSetMargin(root_child0, YGEdgeTop, 20);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, cant_call_negative_measure_horizontal) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 10);
  YGNodeStyleSetHeight(root, 20);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _measure_assert_negative);
  YGNodeStyleSetMargin(root_child0, YGEdgeStart, 20);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

static YGSize _measure_90_10(
    YGNodeConstRef /*node*/,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  return YGSize{90, 10};
}

static YGSize _measure_100_100(
    YGNodeConstRef /*node*/,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  return YGSize{100, 100};
}

TEST(YogaTest, percent_with_text_node) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetJustifyContent(root, YGJustifySpaceBetween);
  YGNodeStyleSetAlignItems(root, YGAlignCenter);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 80);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child1, _measure_90_10);
  YGNodeStyleSetMaxWidthPercent(root_child1, 50);
  YGNodeStyleSetPaddingPercent(root_child1, YGEdgeTop, 50);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(80, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(40, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(60, YGNodeLayoutGetHeight(root_child1));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, percent_margin_with_measure_func) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 500);
  YGNodeStyleSetHeight(root, 500);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 100);
  YGNodeStyleSetHeight(root_child0, 100);
  YGNodeStyleSetMargin(root_child0, YGEdgeTop, 0);
  YGNodeSetMeasureFunc(root_child0, _measure_100_100);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 100);
  YGNodeStyleSetHeight(root_child1, 100);
  YGNodeStyleSetMargin(root_child1, YGEdgeTop, 100);
  YGNodeSetMeasureFunc(root_child1, _measure_100_100);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeRef root_child2 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child2, 100);
  YGNodeStyleSetHeight(root_child2, 100);
  YGNodeStyleSetMarginPercent(root_child2, YGEdgeTop, 10);
  YGNodeSetMeasureFunc(root_child2, _measure_100_100);
  YGNodeInsertChild(root, root_child2, 2);

  YGNodeRef root_child3 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child3, 100);
  YGNodeStyleSetHeight(root_child3, 100);
  YGNodeStyleSetMarginPercent(root_child3, YGEdgeTop, 20);
  YGNodeSetMeasureFunc(root_child3, _measure_100_100);
  YGNodeInsertChild(root, root_child3, 3);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child1));

  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetLeft(root_child2));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetTop(root_child2));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child2));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child2));

  ASSERT_FLOAT_EQ(300, YGNodeLayoutGetLeft(root_child3));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetTop(root_child3));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child3));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child3));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, percent_padding_with_measure_func) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetAlignContent(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 500);
  YGNodeStyleSetHeight(root, 500);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 100);
  YGNodeStyleSetHeight(root_child0, 100);
  YGNodeStyleSetPadding(root_child0, YGEdgeTop, 0);
  YGNodeSetMeasureFunc(root_child0, _measure_100_100);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 100);
  YGNodeStyleSetHeight(root_child1, 100);
  YGNodeStyleSetPadding(root_child1, YGEdgeTop, 100);
  YGNodeSetMeasureFunc(root_child1, _measure_100_100);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeRef root_child2 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPaddingPercent(root_child2, YGEdgeTop, 10);
  YGNodeSetMeasureFunc(root_child2, _measure_100_100);
  YGNodeInsertChild(root, root_child2, 2);

  YGNodeRef root_child3 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPaddingPercent(root_child3, YGEdgeTop, 20);
  YGNodeSetMeasureFunc(root_child3, _measure_100_100);
  YGNodeInsertChild(root, root_child3, 3);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child1));

  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetLeft(root_child2));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child2));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child2));
  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetHeight(root_child2));

  ASSERT_FLOAT_EQ(300, YGNodeLayoutGetLeft(root_child3));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child3));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child3));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child3));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, percent_padding_and_percent_margin_with_measure_func) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetAlignContent(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 500);
  YGNodeStyleSetHeight(root, 500);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 100);
  YGNodeStyleSetHeight(root_child0, 100);
  YGNodeStyleSetPadding(root_child0, YGEdgeTop, 0);
  YGNodeSetMeasureFunc(root_child0, _measure_100_100);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child1, 100);
  YGNodeStyleSetHeight(root_child1, 100);
  YGNodeStyleSetPadding(root_child1, YGEdgeTop, 100);
  YGNodeSetMeasureFunc(root_child1, _measure_100_100);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeRef root_child2 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPaddingPercent(root_child2, YGEdgeTop, 10);
  YGNodeStyleSetMarginPercent(root_child2, YGEdgeTop, 10);
  YGNodeSetMeasureFunc(root_child2, _measure_100_100);
  YGNodeInsertChild(root, root_child2, 2);

  YGNodeRef root_child3 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPaddingPercent(root_child3, YGEdgeTop, 20);
  YGNodeStyleSetMarginPercent(root_child3, YGEdgeTop, 20);
  YGNodeSetMeasureFunc(root_child3, _measure_100_100);
  YGNodeInsertChild(root, root_child3, 3);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child1));

  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetLeft(root_child2));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetTop(root_child2));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child2));
  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetHeight(root_child2));

  ASSERT_FLOAT_EQ(300, YGNodeLayoutGetLeft(root_child3));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetTop(root_child3));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child3));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child3));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

static YGSize _measure_half_width_height(
    YGNodeConstRef node,
    float width,
    YGMeasureMode /*widthMode*/,
    float height,
    YGMeasureMode /*heightMode*/) {
  int* measureCount = (int*)YGNodeGetContext(node);
  if (measureCount != nullptr) {
    (*measureCount)++;
  }

  return YGSize{0.5f * width, 0.5f * height};
}

TEST(YogaTest, measure_content_box) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 200);
  YGNodeStyleSetBoxSizing(root, YGBoxSizingContentBox);
  YGNodeStyleSetPadding(root, YGEdgeAll, 5);
  YGNodeStyleSetBorder(root, YGEdgeAll, 10);

  int measureCount = 0;

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measure_half_width_height);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(1, measureCount);

  ASSERT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_EQ(130, YGNodeLayoutGetWidth(root));
  ASSERT_EQ(230, YGNodeLayoutGetHeight(root));

  ASSERT_EQ(15, YGNodeLayoutGetLeft(root_child0));
  ASSERT_EQ(15, YGNodeLayoutGetTop(root_child0));
  ASSERT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, measure_border_box) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 200);
  YGNodeStyleSetBoxSizing(root, YGBoxSizingBorderBox);
  YGNodeStyleSetPadding(root, YGEdgeAll, 5);
  YGNodeStyleSetBorder(root, YGEdgeAll, 10);

  int measureCount = 0;

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measure_half_width_height);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(1, measureCount);

  ASSERT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_EQ(100, YGNodeLayoutGetWidth(root));
  ASSERT_EQ(200, YGNodeLayoutGetHeight(root));

  ASSERT_EQ(15, YGNodeLayoutGetLeft(root_child0));
  ASSERT_EQ(15, YGNodeLayoutGetTop(root_child0));
  ASSERT_EQ(70, YGNodeLayoutGetWidth(root_child0));
  ASSERT_EQ(85, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, min_width_larger_than_width_propagates_to_auto_parent) {
  YGNodeRef root = YGNodeNew();

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetFlexDirection(root_child0, YGFlexDirectionRow);
  YGNodeStyleSetHeight(root_child0, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0_child0, 50);
  YGNodeStyleSetMinWidth(root_child0_child0, 100);
  YGNodeStyleSetHeight(root_child0_child0, 50);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_EQ(100, YGNodeLayoutGetWidth(root));
  ASSERT_EQ(50, YGNodeLayoutGetHeight(root));

  ASSERT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_EQ(50, YGNodeLayoutGetHeight(root_child0));

  ASSERT_EQ(0, YGNodeLayoutGetLeft(root_child0_child0));
  ASSERT_EQ(0, YGNodeLayoutGetTop(root_child0_child0));
  ASSERT_EQ(100, YGNodeLayoutGetWidth(root_child0_child0));
  ASSERT_EQ(50, YGNodeLayoutGetHeight(root_child0_child0));

  YGNodeFreeRecursive(root);
}
