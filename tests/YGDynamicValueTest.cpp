/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

static YGValue _resolveToFixedValue(
    YGNodeConstRef /*node*/,
    YGValueDynamicID /*id*/,
    YGValueDynamicContext /*context*/) {
  return YGValue{50, YGUnitPoint};
}

static YGValue _resolveToHalfReference(
    YGNodeConstRef /*node*/,
    YGValueDynamicID /*id*/,
    YGValueDynamicContext context) {
  return YGValue{context.referenceLength * 0.5f, YGUnitPoint};
}

static YGValue _resolveByID(
    YGNodeConstRef /*node*/,
    YGValueDynamicID id,
    YGValueDynamicContext /*context*/) {
  switch (id) {
    case 1:
      return YGValue{10, YGUnitPoint};
    case 2:
      return YGValue{20, YGUnitPoint};
    default:
      return YGValue{0, YGUnitPoint};
  }
}

TEST(YogaTest, dynamic_width) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidthDynamic(root, _resolveToFixedValue, 0);
  YGNodeStyleSetHeight(root, 100);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_height) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeightDynamic(root, _resolveToFixedValue, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_flex_basis) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetFlexBasisDynamic(root_child0, _resolveToFixedValue, 0);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_margin) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 50);
  YGNodeStyleSetMarginDynamic(root_child0, YGEdgeLeft, _resolveToFixedValue, 0);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetLeft(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_padding) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);
  YGNodeStyleSetPaddingDynamic(root, YGEdgeAll, _resolveToFixedValue, 0);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetFlexGrow(root_child0, 1);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_border) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);
  YGNodeStyleSetBorderDynamic(root, YGEdgeAll, _resolveToFixedValue, 0);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetFlexGrow(root_child0, 1);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_position) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeAbsolute);
  YGNodeStyleSetPositionDynamic(
      root_child0, YGEdgeLeft, _resolveToFixedValue, 0);
  YGNodeStyleSetPositionDynamic(
      root_child0, YGEdgeTop, _resolveToFixedValue, 0);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetTop(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_gap) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 100);
  YGNodeStyleSetGapDynamic(root, YGGutterColumn, _resolveToFixedValue, 0);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNew();
  YGNodeStyleSetWidth(root_child1, 50);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetLeft(root_child1));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_min_width) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0, 10);
  YGNodeStyleSetMinWidthDynamic(root_child0, _resolveToFixedValue, 0);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_max_height) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetFlexGrow(root_child0, 1);
  YGNodeStyleSetMaxHeightDynamic(root_child0, _resolveToFixedValue, 0);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_value_receives_reference_length) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetWidthDynamic(root_child0, _resolveToHalfReference, 0);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Child width resolves to half of parent width (200 * 0.5 = 100)
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_value_id_disambiguates_properties) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeAbsolute);
  // ID 1 resolves to 10, ID 2 resolves to 20
  YGNodeStyleSetPositionDynamic(root_child0, YGEdgeLeft, _resolveByID, 1);
  YGNodeStyleSetPositionDynamic(root_child0, YGEdgeTop, _resolveByID, 2);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetTop(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_value_overwritten_by_point_value) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidthDynamic(root, _resolveToFixedValue, 0);
  YGNodeStyleSetHeight(root, 100);

  // Overwrite dynamic with a concrete value
  YGNodeStyleSetWidth(root, 80);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(80, YGNodeLayoutGetWidth(root));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, point_value_overwritten_by_dynamic_value) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 80);
  YGNodeStyleSetHeight(root, 100);

  // Overwrite concrete value with dynamic
  YGNodeStyleSetWidthDynamic(root, _resolveToFixedValue, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(root));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dynamic_value_style_get_returns_dynamic_unit) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidthDynamic(root, _resolveToFixedValue, 0);

  YGValue width = YGNodeStyleGetWidth(root);
  ASSERT_EQ(width.unit, YGUnitDynamic);

  YGNodeFreeRecursive(root);
}
