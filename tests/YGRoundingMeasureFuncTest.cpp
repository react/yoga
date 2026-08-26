/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

#include <iomanip>
#include <limits>

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

static YGSize _measureFractial(
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
    rounding_feature_with_custom_measure_and_fractial_matching_scale) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root, YGPositionTypeAbsolute);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPosition(root_child0, YGEdgeLeft, 73.625);
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeRelative);
  YGNodeSetMeasureFunc(root_child0, _measureFractial);
  YGNodeInsertChild(root, root_child0, 0);

  YGConfigSetPointScaleFactor(config, 2.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0.5, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(0.5, YGNodeLayoutGetHeight(root_child0));
  ASSERT_FLOAT_EQ(73.5, YGNodeLayoutGetLeft(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}
static YGSize _measureExactMultipleOfLineHeight(
    YGNodeConstRef /*node*/,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  // 12 lines of 24pt: a height that is exactly representable and exactly on the pixel grid at any
  // scale factor, so any shortfall in the committed height comes from rounding, not from the input.
  return YGSize{
      300.0f,
      288.0f,
  };
}

// A node with a measure function must never be committed a size smaller than it measured — the
// rounding code calls this out explicitly ("we never want to round down its size as this could lead
// to unwanted text truncation"), and forces ceil/floor on the node's edges to guarantee it.
//
// That guarantee used to be defeated by how the dimension was derived. Rounding each edge back to
// points narrows it to float, and the dimension is the difference of two such edges, so each
// operand's representation error leaked into the result. It only surfaced when pointScaleFactor made
// the conversion inexact (n/2 is dyadic and always exact, n/3 almost never is) and when the absolute
// coordinates were large enough for one float ULP to matter. A node measured at exactly 288.0 could
// then be committed 287.999755859375 — enough for a platform text engine applying a strict "does this
// line still fit" test to silently drop an entire trailing line.
//
// Note the exact comparison: ASSERT_FLOAT_EQ tolerates 4 ULPs and would not catch a 1-ULP shortfall.
TEST(YogaTest, rounding_measured_size_is_never_rounded_down_at_large_offsets) {
  const float pointScaleFactor = 3.0f;
  const float measuredHeight = 288.0f;

  // Sweep pixel-grid-aligned offsets through a range where one float ULP is significant.
  for (int scaledOffset = 11000; scaledOffset <= 12000; scaledOffset++) {
    const float offset = static_cast<float>(scaledOffset) / pointScaleFactor;

    YGConfigRef config = YGConfigNew();
    YGConfigSetPointScaleFactor(config, pointScaleFactor);

    YGNodeRef root = YGNodeNewWithConfig(config);
    YGNodeStyleSetWidth(root, 400.0f);
    YGNodeStyleSetHeight(root, offset + 1000.0f);

    YGNodeRef root_child0 = YGNodeNewWithConfig(config);
    YGNodeStyleSetWidth(root_child0, 400.0f);
    YGNodeStyleSetHeight(root_child0, offset);
    YGNodeInsertChild(root, root_child0, 0);

    YGNodeRef root_child1 = YGNodeNewWithConfig(config);
    YGNodeSetMeasureFunc(root_child1, _measureExactMultipleOfLineHeight);
    YGNodeInsertChild(root, root_child1, 1);

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

    // Printed at full float precision: the shortfall is a single ULP, so the default formatting
    // would render both values as "288" and hide the difference.
    const float committedHeight = YGNodeLayoutGetHeight(root_child1);
    ASSERT_EQ(measuredHeight, committedHeight)
        << std::setprecision(std::numeric_limits<float>::max_digits10)
        << "measured height " << measuredHeight << " was committed as " << committedHeight
        << " at offset " << offset;

    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }
}
