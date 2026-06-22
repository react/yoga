/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

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

static YGSize _real_text_measurement_example(
    YGNodeRef node,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode) {
  if (YGFloatsEqual(width, 1000.f) && widthMode == YGMeasureModeAtMost &&
      heightMode == YGMeasureModeUndefined) {
    return YGSize{
        .width = 502.f,
        .height = 20.f,
    };
  } else if (
      YGFloatsEqual(width, 501) && widthMode == YGMeasureModeAtMost &&
      heightMode == YGMeasureModeUndefined) {
    return YGSize{
        .width = 462.f,
        .height = 40.f,
    };
  } else if (
      YGFloatsEqual(width, 501) && widthMode == YGMeasureModeAtMost &&
      heightMode == YGMeasureModeAtMost) {
    return YGSize{
        .width = 462.f,
        .height = height,
    };
  } else {
    // Unexpected measurement constraint
    return YGSize{
        .width = 0.f,
        .height = 0.f,
    };
  }
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

TEST(YogaTest, avoid_stale_flex_basis) {
  const YGNodeRef scroll = YGNodeNew();
  YGNodeStyleSetOverflow(scroll, YGOverflowScroll);
  YGNodeStyleSetFlexGrow(scroll, 1);
  YGNodeStyleSetFlexShrink(scroll, 1);
  YGNodeStyleSetFlexDirection(scroll, YGFlexDirectionColumn);

  const YGNodeRef scrollContainer = YGNodeNew();
  YGNodeInsertChild(scroll, scrollContainer, 0);

  const YGNodeRef outerRow = YGNodeNew();
  YGNodeStyleSetFlexDirection(outerRow, YGFlexDirectionRow);
  YGNodeInsertChild(scrollContainer, outerRow, 0);

  const YGNodeRef flexGrow = YGNodeNew();
  YGNodeStyleSetFlexGrow(flexGrow, 1);
  YGNodeInsertChild(outerRow, flexGrow, 0);

  const YGNodeRef flex = YGNodeNew();
  YGNodeStyleSetFlex(flex, 1);
  YGNodeInsertChild(flexGrow, flex, 0);

  const YGNodeRef innerRow = YGNodeNew();
  YGNodeStyleSetFlexDirection(innerRow, YGFlexDirectionRow);
  YGNodeInsertChild(flex, innerRow, 0);

  const YGNodeRef view = YGNodeNew();
  YGNodeInsertChild(innerRow, view, 0);

  const YGNodeRef leaf = YGNodeNew();
  leaf->setMeasureFunc(_real_text_measurement_example);
  YGNodeInsertChild(view, leaf, 0);

  // First measure
  YGNodeCalculateLayout(scroll, 1000, 1000, YGDirectionLTR);
  ASSERT_FLOAT_EQ(502, YGNodeLayoutGetWidth(leaf));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetHeight(leaf));

  // Second measure
  YGNodeCalculateLayout(scroll, 501, 1000, YGDirectionLTR);
  ASSERT_FLOAT_EQ(462, YGNodeLayoutGetWidth(leaf));
  ASSERT_FLOAT_EQ(40, YGNodeLayoutGetHeight(leaf));

  // Third measure
  YGNodeCalculateLayout(scroll, 502, 1000, YGDirectionLTR);
  ASSERT_FLOAT_EQ(502, YGNodeLayoutGetWidth(leaf));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetHeight(leaf));

  // Fourth measure
  YGNodeCalculateLayout(scroll, 501, 1000, YGDirectionLTR);
  ASSERT_FLOAT_EQ(462, YGNodeLayoutGetWidth(leaf));
  ASSERT_FLOAT_EQ(40, YGNodeLayoutGetHeight(leaf));

  YGNodeFreeRecursive(scroll);
}
