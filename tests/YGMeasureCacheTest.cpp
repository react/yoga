/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>
#include <yoga/node/Node.h>

static YGSize _measureMax(
    YGNodeConstRef node,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode) {
  int* measureCount = (int*)YGNodeGetContext(node);
  (*measureCount)++;

  return YGSize{
      widthMode == YGMeasureModeUndefined ? 10 : width,
      heightMode == YGMeasureModeUndefined ? 10 : height,
  };
}

static YGSize _measureMin(
    YGNodeConstRef node,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode) {
  int* measureCount = (int*)YGNodeGetContext(node);
  *measureCount = *measureCount + 1;
  return YGSize{
      widthMode == YGMeasureModeUndefined ||
              (widthMode == YGMeasureModeAtMost && width > 10)
          ? 10
          : width,
      heightMode == YGMeasureModeUndefined ||
              (heightMode == YGMeasureModeAtMost && height > 10)
          ? 10
          : height,
  };
}

static YGSize _measure_84_49(
    YGNodeConstRef node,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  int* measureCount = (int*)YGNodeGetContext(node);
  if (measureCount != nullptr) {
    (*measureCount)++;
  }

  return YGSize{84.f, 49.f};
}

TEST(YogaTest, measure_once_single_flexible_child) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMax);
  YGNodeStyleSetFlexGrow(root_child0, 1);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(1, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, remeasure_with_same_exact_width_larger_than_needed_height) {
  YGNodeRef root = YGNodeNew();

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);
  YGNodeCalculateLayout(root, 100, 50, YGDirectionLTR);

  ASSERT_EQ(1, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, remeasure_with_same_atmost_width_larger_than_needed_height) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);
  YGNodeCalculateLayout(root, 100, 50, YGDirectionLTR);

  ASSERT_EQ(1, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, remeasure_with_computed_width_larger_than_needed_height) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);
  YGNodeStyleSetAlignItems(root, YGAlignStretch);
  YGNodeCalculateLayout(root, 10, 50, YGDirectionLTR);

  ASSERT_EQ(1, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, remeasure_with_atmost_computed_width_undefined_height) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, YGUndefined, YGDirectionLTR);
  YGNodeCalculateLayout(root, 10, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(1, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(
    YogaTest,
    remeasure_with_already_measured_value_smaller_but_still_float_equal) {
  int measureCount = 0;

  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 288.f);
  YGNodeStyleSetHeight(root, 288.f);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetPadding(root_child0, YGEdgeAll, 2.88f);
  YGNodeStyleSetFlexDirection(root_child0, YGFlexDirectionRow);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNew();
  YGNodeSetContext(root_child0_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0_child0, _measure_84_49);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeFreeRecursive(root);

  ASSERT_EQ(1, measureCount);
}

TEST(YogaTest, keep_cached_measurement_when_owner_resizes_without_percentages) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMax);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  const int measuresBeforeResize = measureCount;

  YGNodeStyleSetWidth(root, 200);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_EQ(measuresBeforeResize, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, remeasure_when_owner_resizes_with_percent_padding) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMax);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetPaddingPercent(root_child0, YGEdgeTop, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetPadding(root_child0, YGEdgeTop));
  const int measuresBeforeResize = measureCount;

  YGNodeStyleSetWidth(root, 200);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetPadding(root_child0, YGEdgeTop));
  ASSERT_GT(measureCount, measuresBeforeResize);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, measure_func_follows_measure_modes_by_default) {
  YGNodeRef node = YGNodeNew();
  ASSERT_TRUE(YGNodeGetMeasureFuncFollowsMeasureModes(node));
  YGNodeFree(node);
}

TEST(
    YogaTest,
    remeasure_tighter_atmost_width_when_not_following_measure_modes) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeSetMeasureFuncFollowsMeasureModes(root_child0, false);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, YGUndefined, YGDirectionLTR);
  ASSERT_EQ(2, measureCount);
  YGNodeCalculateLayout(root, 80, YGUndefined, YGDirectionLTR);
  ASSERT_EQ(4, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, keep_tighter_atmost_width_measurement_by_default) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, YGUndefined, YGDirectionLTR);
  YGNodeCalculateLayout(root, 80, YGUndefined, YGDirectionLTR);
  ASSERT_EQ(1, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(
    YogaTest,
    remeasure_atmost_after_max_content_when_not_following_measure_modes) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeSetMeasureFuncFollowsMeasureModes(root_child0, false);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_EQ(2, measureCount);
  YGNodeCalculateLayout(root, 80, YGUndefined, YGDirectionLTR);
  ASSERT_EQ(4, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, keep_max_content_measurement_for_atmost_by_default) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  YGNodeCalculateLayout(root, 80, YGUndefined, YGDirectionLTR);
  ASSERT_EQ(1, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(
    YogaTest,
    remeasure_exact_measured_width_when_not_following_measure_modes) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeSetMeasureFuncFollowsMeasureModes(root_child0, false);
  YGNodeInsertChild(root, root_child0, 0);

  // At most 100 wide it measures 10 wide. Stretched to exactly 10 wide, under
  // the same height spec, its width is exact and equal to what it measured:
  // only a measure function that follows the measure modes reuses its
  // measurement there.
  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);
  ASSERT_EQ(2, measureCount);
  YGNodeStyleSetAlignItems(root, YGAlignStretch);
  YGNodeCalculateLayout(root, 10, 100, YGDirectionLTR);
  ASSERT_EQ(3, measureCount);
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetWidth(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, keep_measurement_for_exact_measured_width_by_default) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);
  YGNodeStyleSetAlignItems(root, YGAlignStretch);
  YGNodeCalculateLayout(root, 10, 100, YGDirectionLTR);
  ASSERT_EQ(1, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, remeasure_exact_measured_size_when_not_following_measure_modes) {
  YGNodeRef root = YGNodeNew();
  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeSetMeasureFuncFollowsMeasureModes(root_child0, false);
  YGNodeInsertChild(root, root_child0, 0);

  // Unbounded it measures 10 x 10; then exactly 10 wide and at most 100 high:
  // asked again.
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_EQ(2, measureCount);
  YGNodeStyleSetAlignItems(root, YGAlignStretch);
  YGNodeCalculateLayout(root, 10, 100, YGDirectionLTR);
  ASSERT_EQ(3, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(
    YogaTest,
    keep_measurement_for_same_spec_when_not_following_measure_modes) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeSetMeasureFuncFollowsMeasureModes(root_child0, false);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNew();
  YGNodeStyleSetWidth(root_child1, 10);
  YGNodeStyleSetHeight(root_child1, 10);
  YGNodeInsertChild(root, root_child1, 1);

  // Its sibling grows, which lays the root out again, and asks it under the
  // same specs: its measurements are kept.
  YGNodeCalculateLayout(root, 100, YGUndefined, YGDirectionLTR);
  const int firstLayoutCount = measureCount;
  YGNodeStyleSetHeight(root_child1, 20);
  YGNodeCalculateLayout(root, 100, YGUndefined, YGDirectionLTR);
  ASSERT_EQ(firstLayoutCount, measureCount);
  ASSERT_FLOAT_EQ(30, YGNodeLayoutGetHeight(root));

  YGNodeFreeRecursive(root);
}

TEST(
    YogaTest,
    remeasure_exact_width_tighter_atmost_height_when_not_following_measure_modes) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeSetMeasureFuncFollowsMeasureModes(root_child0, false);
  YGNodeInsertChild(root, root_child0, 0);

  // As remeasure_with_computed_width_larger_than_needed_height: exactly 10
  // wide matches the width it measured, but at most 50 high is a tighter bound
  // than the 100 it measured under: only a measure function that follows the
  // measure modes reuses its measurement there.
  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);
  ASSERT_EQ(2, measureCount);
  YGNodeStyleSetAlignItems(root, YGAlignStretch);
  YGNodeCalculateLayout(root, 10, 50, YGDirectionLTR);
  ASSERT_EQ(3, measureCount);

  YGNodeFreeRecursive(root);
}

TEST(
    YogaTest,
    measure_again_at_exactly_its_measured_height_when_not_following_measure_modes) {
  // In a column, its height is measured with no bound, then laid out at
  // exactly the height it measured: measured once when its measure function
  // follows the measure modes, and twice when it doesn't.
  for (const bool measureFuncFollowsMeasureModes : {true, false}) {
    YGNodeRef root = YGNodeNew();
    YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

    YGNodeRef root_child0 = YGNodeNew();
    int measureCount = 0;
    YGNodeSetContext(root_child0, &measureCount);
    YGNodeSetMeasureFunc(root_child0, _measureMin);
    YGNodeSetMeasureFuncFollowsMeasureModes(
        root_child0, measureFuncFollowsMeasureModes);
    YGNodeInsertChild(root, root_child0, 0);

    YGNodeCalculateLayout(root, 100, YGUndefined, YGDirectionLTR);
    ASSERT_EQ(measureFuncFollowsMeasureModes ? 1 : 2, measureCount);

    YGNodeFreeRecursive(root);
  }
}

TEST(YogaTest, setting_measure_func_follows_measure_modes_dirties_on_change) {
  YGNodeRef root = YGNodeNew();

  YGNodeRef root_child0 = YGNodeNew();
  int measureCount = 0;
  YGNodeSetContext(root_child0, &measureCount);
  YGNodeSetMeasureFunc(root_child0, _measureMin);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);
  ASSERT_FALSE(YGNodeIsDirty(root));

  YGNodeSetMeasureFuncFollowsMeasureModes(root_child0, true);
  ASSERT_FALSE(YGNodeIsDirty(root_child0));
  ASSERT_FALSE(YGNodeIsDirty(root));

  YGNodeSetMeasureFuncFollowsMeasureModes(root_child0, false);
  ASSERT_TRUE(YGNodeIsDirty(root_child0));
  ASSERT_TRUE(YGNodeIsDirty(root));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, cloned_node_keeps_measure_func_follows_measure_modes) {
  YGNodeRef node = YGNodeNew();
  YGNodeSetMeasureFunc(node, _measureMin);
  YGNodeSetMeasureFuncFollowsMeasureModes(node, false);

  YGNodeRef clone = YGNodeClone(node);
  ASSERT_FALSE(YGNodeGetMeasureFuncFollowsMeasureModes(clone));

  YGNodeFree(clone);
  YGNodeFree(node);
}

TEST(YogaTest, moved_node_keeps_measure_func_follows_measure_modes) {
  facebook::yoga::Node node;
  node.setMeasureFuncFollowsMeasureModes(false);

  facebook::yoga::Node moved{std::move(node)};
  ASSERT_FALSE(moved.measureFuncFollowsMeasureModes());
}

TEST(YogaTest, reset_restores_measure_func_follows_measure_modes) {
  YGNodeRef node = YGNodeNew();
  YGNodeSetMeasureFuncFollowsMeasureModes(node, false);

  YGNodeReset(node);
  ASSERT_TRUE(YGNodeGetMeasureFuncFollowsMeasureModes(node));

  YGNodeFree(node);
}

TEST(YogaTest, measure_func_follows_measure_modes_has_no_effect_on_container) {
  // A container without a measure function reuses its layout only under the
  // same constraints either way: its measured child is asked as often, and it
  // is laid out the same.
  int measureCounts[2] = {0, 0};
  float containerWidths[2] = {0, 0};
  for (const bool measureFuncFollowsMeasureModes : {true, false}) {
    int& measureCount = measureCounts[measureFuncFollowsMeasureModes ? 0 : 1];
    YGNodeRef root = YGNodeNew();
    YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

    YGNodeRef container = YGNodeNew();
    YGNodeSetMeasureFuncFollowsMeasureModes(
        container, measureFuncFollowsMeasureModes);
    YGNodeInsertChild(root, container, 0);

    YGNodeRef leaf = YGNodeNew();
    YGNodeSetContext(leaf, &measureCount);
    YGNodeSetMeasureFunc(leaf, _measureMin);
    YGNodeInsertChild(container, leaf, 0);

    YGNodeCalculateLayout(root, 100, YGUndefined, YGDirectionLTR);
    YGNodeStyleSetAlignItems(root, YGAlignStretch);
    YGNodeCalculateLayout(root, 10, YGUndefined, YGDirectionLTR);
    YGNodeCalculateLayout(root, 80, YGUndefined, YGDirectionLTR);
    containerWidths[measureFuncFollowsMeasureModes ? 0 : 1] =
        YGNodeLayoutGetWidth(container);

    YGNodeFreeRecursive(root);
  }
  ASSERT_EQ(measureCounts[0], measureCounts[1]);
  ASSERT_FLOAT_EQ(containerWidths[0], containerWidths[1]);
}
