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

TEST(YogaTest, roundLayoutResultsToPixelGrid_height_rounding_up) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 3);

  YGNodeRef node = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(node, YGPositionTypeAbsolute);

  // These are values extracted from a debugging session in a real iOS app
  YGNodeStyleSetPosition(node, YGEdgeLeft, 38.333333969116211);
  YGNodeStyleSetPosition(node, YGEdgeTop, 1970.3333333432674);
  YGNodeStyleSetWidth(node, 339.66665649414063);
  YGNodeStyleSetHeight(node, 96);
  YGNodeSetNodeType(node, YGNodeTypeText);

  YGNodeCalculateLayout(node, YGUndefined, YGUndefined, YGDirectionLTR);

  // If this value is anything less than 96, iOS will not wrap the text to a 4th line
  ASSERT_FLOAT_EQ(YGNodeLayoutGetHeight(node), 96.0f);

  YGNodeFreeRecursive(node);

  YGConfigFree(config);
}
