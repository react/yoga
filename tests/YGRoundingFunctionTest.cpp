/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

#include <cmath>

TEST(YogaTest, rounding_value) {
  // Test that whole numbers are rounded to whole despite ceil/floor flags
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(6.000001, 2.0, false, false));
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(6.000001, 2.0, true, false));
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(6.000001, 2.0, false, true));
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(5.999999, 2.0, false, false));
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(5.999999, 2.0, true, false));
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(5.999999, 2.0, false, true));
  // Same tests for negative numbers
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-6.000001, 2.0, false, false));
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-6.000001, 2.0, true, false));
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-6.000001, 2.0, false, true));
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-5.999999, 2.0, false, false));
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-5.999999, 2.0, true, false));
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-5.999999, 2.0, false, true));

  // Test that numbers with fraction are rounded correctly accounting for
  // ceil/floor flags
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(6.01, 2.0, false, false));
  ASSERT_FLOAT_EQ(6.5, YGRoundValueToPixelGrid(6.01, 2.0, true, false));
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(6.01, 2.0, false, true));
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(5.99, 2.0, false, false));
  ASSERT_FLOAT_EQ(6.0, YGRoundValueToPixelGrid(5.99, 2.0, true, false));
  ASSERT_FLOAT_EQ(5.5, YGRoundValueToPixelGrid(5.99, 2.0, false, true));
  // Same tests for negative numbers
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-6.01, 2.0, false, false));
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-6.01, 2.0, true, false));
  ASSERT_FLOAT_EQ(-6.5, YGRoundValueToPixelGrid(-6.01, 2.0, false, true));
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-5.99, 2.0, false, false));
  ASSERT_FLOAT_EQ(-5.5, YGRoundValueToPixelGrid(-5.99, 2.0, true, false));
  ASSERT_FLOAT_EQ(-6.0, YGRoundValueToPixelGrid(-5.99, 2.0, false, true));

  // Rounding up/down halfway values is as expected for both positive and
  // negative numbers
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.5, 1.0, false, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.4, 1.0, false, false));
  ASSERT_FLOAT_EQ(-4, YGRoundValueToPixelGrid(-3.6, 1.0, false, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.499999, 1.0, false, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.500001, 1.0, false, false));
  ASSERT_FLOAT_EQ(-4, YGRoundValueToPixelGrid(-3.5001, 1.0, false, false));

  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.5, 1.0, true, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.4, 1.0, true, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.6, 1.0, true, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.499999, 1.0, true, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.500001, 1.0, true, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.5001, 1.0, true, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.00001, 1.0, true, false));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3, 1.0, true, false));

  ASSERT_FLOAT_EQ(-4, YGRoundValueToPixelGrid(-3.5, 1.0, false, true));
  ASSERT_FLOAT_EQ(-4, YGRoundValueToPixelGrid(-3.4, 1.0, false, true));
  ASSERT_FLOAT_EQ(-4, YGRoundValueToPixelGrid(-3.6, 1.0, false, true));
  ASSERT_FLOAT_EQ(-4, YGRoundValueToPixelGrid(-3.499999, 1.0, false, true));
  ASSERT_FLOAT_EQ(-4, YGRoundValueToPixelGrid(-3.500001, 1.0, false, true));
  ASSERT_FLOAT_EQ(-4, YGRoundValueToPixelGrid(-3.5001, 1.0, false, true));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3.00001, 1.0, false, true));
  ASSERT_FLOAT_EQ(-3, YGRoundValueToPixelGrid(-3, 1.0, false, true));

  // NAN is treated as expected:
  ASSERT_TRUE(
      std::isnan(YGRoundValueToPixelGrid(
          std::numeric_limits<double>::quiet_NaN(), 1.5, false, false)));
  ASSERT_TRUE(
      std::isnan(YGRoundValueToPixelGrid(
          1.5, std::numeric_limits<double>::quiet_NaN(), false, false)));
  ASSERT_TRUE(
      std::isnan(YGRoundValueToPixelGrid(
          std::numeric_limits<double>::quiet_NaN(),
          std::numeric_limits<double>::quiet_NaN(),
          false,
          false)));
}

static YGSize measureText(
    YGNodeConstRef /*node*/,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  return YGSize{10, 10};
}

// Regression test for https://github.com/facebook/yoga/issues/824
TEST(YogaTest, consistent_rounding_during_repeated_layouts) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 2);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(root, YGEdgeTop, -1.49f);
  YGNodeStyleSetWidth(root, 500);
  YGNodeStyleSetHeight(root, 500);

  YGNodeRef node0 = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root, node0, 0);

  YGNodeRef node1 = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(node1, measureText);
  YGNodeInsertChild(node0, node1, 0);

  for (int i = 0; i < 5; i++) {
    // Dirty the tree so YGRoundToPixelGrid runs again
    YGNodeStyleSetMargin(root, YGEdgeLeft, (float)(i + 1));

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
    ASSERT_FLOAT_EQ(10, YGNodeLayoutGetHeight(node1));
  }

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, per_node_point_scale_factor) {
  YGConfigRef config1 = YGConfigNew();
  YGConfigSetPointScaleFactor(config1, 2);

  YGConfigRef config2 = YGConfigNew();
  YGConfigSetPointScaleFactor(config2, 1);

  YGConfigRef config3 = YGConfigNew();
  YGConfigSetPointScaleFactor(config3, 0.5f);

  YGNodeRef root = YGNodeNewWithConfig(config1);
  YGNodeStyleSetWidth(root, 11.5);
  YGNodeStyleSetHeight(root, 11.5);

  YGNodeRef node0 = YGNodeNewWithConfig(config2);
  YGNodeStyleSetWidth(node0, 9.5);
  YGNodeStyleSetHeight(node0, 9.5);
  YGNodeInsertChild(root, node0, 0);

  YGNodeRef node1 = YGNodeNewWithConfig(config3);
  YGNodeStyleSetWidth(node1, 7);
  YGNodeStyleSetHeight(node1, 7);
  YGNodeInsertChild(node0, node1, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(YGNodeLayoutGetWidth(root), 11.5);
  ASSERT_EQ(YGNodeLayoutGetHeight(root), 11.5);

  ASSERT_EQ(YGNodeLayoutGetWidth(node0), 10);
  ASSERT_EQ(YGNodeLayoutGetHeight(node0), 10);

  ASSERT_EQ(YGNodeLayoutGetWidth(node1), 8);
  ASSERT_EQ(YGNodeLayoutGetHeight(node1), 8);

  YGNodeFreeRecursive(root);

  YGConfigFree(config1);
  YGConfigFree(config2);
  YGConfigFree(config3);
}

TEST(YogaTest, raw_layout_dimensions) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 0.5f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 11.5f);
  YGNodeStyleSetHeight(root, 9.5f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(YGNodeLayoutGetWidth(root), 12.0f);
  ASSERT_EQ(YGNodeLayoutGetHeight(root), 10.0f);
  ASSERT_EQ(YGNodeLayoutGetRawWidth(root), 11.5f);
  ASSERT_EQ(YGNodeLayoutGetRawHeight(root), 9.5f);

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, cached_root_preserves_rounded_dimensions) {
  for (const float scale : {0.0f, 1.0f, 2.0f, 3.0f, 100.0f}) {
    YGConfigRef config = YGConfigNew();
    YGConfigSetPointScaleFactor(config, scale);
    YGNodeRef root = YGNodeNewWithConfig(config);
    YGNodeStyleSetWidth(root, 10.4f);
    YGNodeStyleSetHeight(root, 20.4f);
    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
    const float width = YGNodeLayoutGetWidth(root);
    const float height = YGNodeLayoutGetHeight(root);

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
    const bool unconsumedLayout = YGNodeGetHasNewLayout(root);
    EXPECT_TRUE(unconsumedLayout);

    for (int pass = 0; pass < 5; pass++) {
      YGNodeSetHasNewLayout(root, false);
      YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
      const float cachedWidth = YGNodeLayoutGetWidth(root);
      const float cachedHeight = YGNodeLayoutGetHeight(root);
      const float rawWidth = YGNodeLayoutGetRawWidth(root);
      const float rawHeight = YGNodeLayoutGetRawHeight(root);
      const bool hasNewLayout = YGNodeGetHasNewLayout(root);
      EXPECT_FLOAT_EQ(width, cachedWidth);
      EXPECT_FLOAT_EQ(height, cachedHeight);
      EXPECT_FLOAT_EQ(10.4f, rawWidth);
      EXPECT_FLOAT_EQ(20.4f, rawHeight);
      EXPECT_FALSE(hasNewLayout);
    }

    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }
}

struct FractionalMeasurement {
  float height = 40.666668f;
  int calls = 0;
};

static YGSize measureFractionalHeight(
    YGNodeConstRef node,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  void* context = YGNodeGetContext(node);
  auto* measurement = static_cast<FractionalMeasurement*>(context);
  measurement->calls += 1;
  return YGSize{10.0f, measurement->height};
}

TEST(YogaTest, cached_auto_height_root_preserves_measurement_rounding) {
  for (const bool measureRoot : {false, true}) {
    FractionalMeasurement measurement;
    YGNodeRef root = YGNodeNew();
    YGNodeStyleSetWidth(root, 402);
    YGNodeStyleSetPadding(root, YGEdgeVertical, 8);
    YGNodeRef text;
    if (measureRoot) {
      text = root;
    } else {
      text = YGNodeNew();
      YGNodeStyleSetMargin(text, YGEdgeBottom, 1);
      YGNodeInsertChild(root, text, 0);
    }
    YGNodeSetContext(text, &measurement);
    YGNodeSetMeasureFunc(text, measureFractionalHeight);
    const float initialHeight = measureRoot ? 57.0f : 58.0f;
    int expectedMeasureCalls = 0;

    for (const float measuredHeight : {40.666668f, 40.666668f, 41.666668f}) {
      measurement.height = measuredHeight;
      expectedMeasureCalls += 1;
      YGNodeMarkDirty(text);
      YGNodeCalculateLayout(root, 402, YGUndefined, YGDirectionLTR);
      const float height = YGNodeLayoutGetHeight(root);
      const float expectedHeight =
          measuredHeight == 40.666668f ? initialHeight : initialHeight + 1;
      const bool dirtyLayout = YGNodeGetHasNewLayout(root);
      EXPECT_FLOAT_EQ(expectedHeight, height);
      EXPECT_EQ(expectedMeasureCalls, measurement.calls);
      EXPECT_TRUE(dirtyLayout);

      for (int pass = 0; pass < 5; pass++) {
        YGNodeSetHasNewLayout(root, false);
        YGNodeSetHasNewLayout(text, false);
        YGNodeCalculateLayout(root, 402, YGUndefined, YGDirectionLTR);
        const float cachedHeight = YGNodeLayoutGetHeight(root);
        const bool rootHasNewLayout = YGNodeGetHasNewLayout(root);
        const bool textHasNewLayout = YGNodeGetHasNewLayout(text);
        EXPECT_FLOAT_EQ(height, cachedHeight);
        EXPECT_EQ(expectedMeasureCalls, measurement.calls);
        EXPECT_FALSE(rootHasNewLayout);
        EXPECT_FALSE(textHasNewLayout);
      }
    }

    YGNodeFreeRecursive(root);
  }
}

TEST(YogaTest, cached_child_rounding_tracks_parent_position) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeRef child = YGNodeNew();
  YGNodeStyleSetWidth(child, 10.4f);
  YGNodeStyleSetHeight(child, 10);
  YGNodeInsertChild(root, child, 0);

  YGNodeStyleSetMargin(root, YGEdgeLeft, 0.4f);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  const float initialWidth = YGNodeLayoutGetWidth(child);
  EXPECT_FLOAT_EQ(11, initialWidth);

  YGNodeSetHasNewLayout(root, false);
  YGNodeSetHasNewLayout(child, false);
  YGNodeStyleSetMargin(root, YGEdgeLeft, 0.6f);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  const float movedWidth = YGNodeLayoutGetWidth(child);
  const float rawWidth = YGNodeLayoutGetRawWidth(child);
  const bool hasNewLayout = YGNodeGetHasNewLayout(child);
  EXPECT_FLOAT_EQ(10, movedWidth);
  EXPECT_FLOAT_EQ(10.4f, rawWidth);
  EXPECT_TRUE(hasNewLayout);

  YGNodeFreeRecursive(root);
}
