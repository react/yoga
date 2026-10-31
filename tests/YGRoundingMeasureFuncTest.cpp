/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

#include <vector>

static YGSize _measureFloor(
    YGNodeConstRef /*node*/,
    float width,
    YGMeasureMode /*widthMode*/,
    float height,
    YGMeasureMode /*heightMode*/) {
  return YGSize{
      width = 10.2f,
      height = 10.2f,
  };
}

static YGSize _measureCeil(
    YGNodeConstRef /*node*/,
    float width,
    YGMeasureMode /*widthMode*/,
    float height,
    YGMeasureMode /*heightMode*/) {
  return YGSize{
      width = 10.5f,
      height = 10.5f,
  };
}

static YGSize _measureFractional(
    YGNodeConstRef /*node*/,
    float width,
    YGMeasureMode /*widthMode*/,
    float height,
    YGMeasureMode /*heightMode*/) {
  return YGSize{
      width = 0.5f,
      height = 0.5f,
  };
}

TEST(YogaTest, rounding_feature_with_custom_measure_func_floor) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = YGNodeNewWithConfig(config);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _measureFloor);
  YGNodeInsertChild(root, root_child0, 0);

  YGConfigSetPointScaleFactor(config, 0.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionRTL);

  ASSERT_FLOAT_EQ(10.2f, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(10.2f, YGNodeLayoutGetHeight(root_child0));

  YGConfigSetPointScaleFactor(config, 1.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(11, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(11, YGNodeLayoutGetHeight(root_child0));

  YGConfigSetPointScaleFactor(config, 2.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionRTL);

  ASSERT_FLOAT_EQ(10.5, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(10.5, YGNodeLayoutGetHeight(root_child0));

  YGConfigSetPointScaleFactor(config, 4.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(10.25, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(10.25, YGNodeLayoutGetHeight(root_child0));

  YGConfigSetPointScaleFactor(config, 1.0f / 3.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionRTL);

  ASSERT_FLOAT_EQ(12.0, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(12.0, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, rounding_feature_with_custom_measure_func_ceil) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = YGNodeNewWithConfig(config);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(root_child0, _measureCeil);
  YGNodeInsertChild(root, root_child0, 0);

  YGConfigSetPointScaleFactor(config, 1.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(11, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(11, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(
    YogaTest,
    rounding_feature_with_custom_measure_and_fractional_matching_scale) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root, YGPositionTypeAbsolute);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPosition(root_child0, YGEdgeLeft, 73.625);
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeRelative);
  YGNodeSetMeasureFunc(root_child0, _measureFractional);
  YGNodeInsertChild(root, root_child0, 0);

  YGConfigSetPointScaleFactor(config, 2.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0.5, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(0.5, YGNodeLayoutGetHeight(root_child0));
  ASSERT_FLOAT_EQ(73.5, YGNodeLayoutGetLeft(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

// Regression test for https://github.com/react/yoga/issues/749
TEST(YogaTest, rounding_feature_with_custom_measure_keeps_whole_pixel_size) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 3.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetPadding(root, YGEdgeLeft, 163.666641f);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(
      root_child0,
      [](YGNodeConstRef, float, YGMeasureMode, float, YGMeasureMode) {
        return YGSize{54.666656f, 20};
      });
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(164.0f / 3.0f, YGNodeLayoutGetWidth(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, rounding_feature_with_custom_measure_keeps_whole_pixel_height) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 3.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetPadding(root, YGEdgeTop, 163.666641f);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(
      root_child0,
      [](YGNodeConstRef, float, YGMeasureMode, float, YGMeasureMode) {
        return YGSize{20, 54.666656f};
      });
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(164.0f / 3.0f, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

// Regression test for https://github.com/react/yoga/issues/2053
// The container is served from the layout cache when the sibling changes, so
// the measured node inside it is rounded again on every pass. That has to
// keep its rounded height, or the height grows a pixel each time.
TEST(YogaTest, rounding_feature_with_custom_measure_keeps_height_when_cached) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 100.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 200);

  // Puts the container at y = 527.37, which a float holds as 527.3699951.
  YGNodeRef sibling = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(sibling, 10);
  YGNodeStyleSetHeight(sibling, 527.37f);
  YGNodeInsertChild(root, sibling, 0);

  YGNodeRef container = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root, container, 1);

  YGNodeRef text = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(
      text, [](YGNodeConstRef, float, YGMeasureMode, float, YGMeasureMode) {
        return YGSize{50, 128.03f};
      });
  YGNodeInsertChild(container, text, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  const float height = YGNodeLayoutGetHeight(text);

  for (const float width : {11.0f, 12.0f, 13.0f, 14.0f, 15.0f}) {
    YGNodeStyleSetWidth(sibling, width);
    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
    ASSERT_FLOAT_EQ(height, YGNodeLayoutGetHeight(text)) << "width " << width;
  }

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

static int _measureLabelCount = 0;

static YGSize _measureLabel(
    YGNodeConstRef node,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  _measureLabelCount++;
  return *static_cast<const YGSize*>(YGNodeGetContext(node));
}

// The tree from https://github.com/react/yoga/issues/2053:
// root
// +-- bar     (its width changes, so root is laid out again)
// +-- spacer
// +-- anchor  (served from the layout cache when only the bar changes)
//     +-- list (absolute, bottom: 110%, far above the origin)
//         +-- 30 x row (padding 10) +-- measured label
static YGNodeRef _buildLabelList(
    YGConfigRef config,
    YGSize* labelSize,
    YGNodeRef* bar,
    std::vector<YGNodeRef>* labels) {
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 620);
  YGNodeStyleSetPadding(root, YGEdgeAll, 24.3f);

  *bar = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(*bar, 10);
  YGNodeStyleSetHeight(*bar, 8);
  YGNodeInsertChild(root, *bar, 0);

  YGNodeRef spacer = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(spacer, 517.37f);
  YGNodeInsertChild(root, spacer, 1);

  YGNodeRef anchor = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(anchor, 50.8f);
  YGNodeInsertChild(root, anchor, 2);

  YGNodeRef list = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(list, YGPositionTypeAbsolute);
  YGNodeStyleSetPositionPercent(list, YGEdgeBottom, 110);
  YGNodeInsertChild(anchor, list, 0);

  for (size_t i = 0; i < 30; i++) {
    YGNodeRef row = YGNodeNewWithConfig(config);
    YGNodeStyleSetFlexDirection(row, YGFlexDirectionRow);
    YGNodeStyleSetPadding(row, YGEdgeVertical, 10);
    YGNodeRef label = YGNodeNewWithConfig(config);
    YGNodeSetContext(label, labelSize);
    YGNodeSetMeasureFunc(label, _measureLabel);
    YGNodeInsertChild(row, label, 0);
    YGNodeInsertChild(list, row, i);
    labels->push_back(label);
  }
  return root;
}

// Lays the tree out, then changes the bar's width for 20 passes and checks
// that the list came from the cache and every label kept its first height.
static void _expectCachedLabelsKeepHeight(
    float pointScaleFactor,
    float labelHeight) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, pointScaleFactor);
  YGSize labelSize{70.37f, labelHeight};
  YGNodeRef bar = nullptr;
  std::vector<YGNodeRef> labels;
  YGNodeRef root = _buildLabelList(config, &labelSize, &bar, &labels);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  const int measureCount = _measureLabelCount;
  std::vector<float> heights;
  for (YGNodeRef label : labels) {
    heights.push_back(YGNodeLayoutGetHeight(label));
  }

  for (int pass = 1; pass <= 20; pass++) {
    YGNodeStyleSetWidth(bar, 10.0f + static_cast<float>(pass));
    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
    ASSERT_EQ(measureCount, _measureLabelCount) << "list was laid out again";
    for (size_t i = 0; i < labels.size(); i++) {
      ASSERT_EQ(heights[i], YGNodeLayoutGetHeight(labels[i]))
          << "label " << i << " on pass " << pass;
    }
  }

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(
    YogaTest,
    rounding_feature_with_custom_measure_keeps_height_in_cached_list) {
  _expectCachedLabelsKeepHeight(100.0f, 128.03f);
}

// At a scale of 2 most heights aren't whole pixels, so the first layout ceils
// them from the label's edges. The cached labels have to keep those heights.
TEST(
    YogaTest,
    rounding_feature_with_custom_measure_keeps_height_in_cached_list_at_scale_2) {
  _expectCachedLabelsKeepHeight(2.0f, 10.331f);
}

// 128.03 is a whole number of pixels at a scale of 100, but the nearest float
// is further than the 0.0001 snap tolerance from 12803 once scaled.
TEST(
    YogaTest,
    rounding_feature_with_custom_measure_keeps_whole_pixel_height_far_from_origin) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 100.0f);
  YGSize labelSize{70.37f, 128.03f};
  YGNodeRef bar = nullptr;
  std::vector<YGNodeRef> labels;
  YGNodeRef root = _buildLabelList(config, &labelSize, &bar, &labels);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  for (size_t i = 0; i < labels.size(); i++) {
    ASSERT_EQ(128.03f, YGNodeLayoutGetHeight(labels[i])) << "label " << i;
  }

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}
