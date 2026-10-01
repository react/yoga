/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cmath>

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

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

// 23 physical pixels on a 2.75 density screen, as a float. Measurement on
// Android reports whole physical pixels converted to points, so the product
// with the scale factor lands a hair under 23 rather than exactly on it.
static YGSize _measureTwentyThreePhysicalPixels(
    YGNodeConstRef /*node*/,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  return YGSize{
      .width = 8.363636016845703f,
      .height = 8.363636016845703f,
  };
}

static long widthInPhysicalPixels(YGNodeConstRef node, float pointScaleFactor) {
  return std::lround(
      static_cast<double>(YGNodeLayoutGetWidth(node)) * pointScaleFactor);
}

// A node with a measure function must never be rounded below the size it
// measured. Here the left edge lands on 103.9999008 scaled units, inside the
// tolerance used to decide a value already sits on the pixel grid, and is
// snapped up to 104; the right edge lands on 126.9998999, just outside it, and
// is floored to 126. Rounding the edges independently yields 22 pixels for 23
// pixels of content.
TEST(
    YogaTest,
    rounding_feature_with_custom_measure_never_rounds_below_measure) {
  const float pointScaleFactor = 2.75f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, pointScaleFactor);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root, YGPositionTypeAbsolute);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeRelative);
  YGNodeStyleSetPosition(root_child0, YGEdgeLeft, 37.818145751953125f);
  YGNodeSetMeasureFunc(root_child0, _measureTwentyThreePhysicalPixels);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(23, widthInPhysicalPixels(root_child0, pointScaleFactor));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// The counterpart: a node that already rounds to the size it measured must not
// gain a pixel. The left edge lands on 28.875 scaled units, well clear of the
// tolerance, and both edges floor consistently. Forcing the right edge to ceil
// unconditionally would widen this node to 24 pixels.
TEST(
    YogaTest,
    rounding_feature_with_custom_measure_does_not_widen_when_it_fits) {
  const float pointScaleFactor = 2.75f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, pointScaleFactor);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root, YGPositionTypeAbsolute);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeRelative);
  YGNodeStyleSetPosition(root_child0, YGEdgeLeft, 10.5f);
  YGNodeSetMeasureFunc(root_child0, _measureTwentyThreePhysicalPixels);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(23, widthInPhysicalPixels(root_child0, pointScaleFactor));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
