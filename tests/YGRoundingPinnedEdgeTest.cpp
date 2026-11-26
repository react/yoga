/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

static YGSize _measureText(
    YGNodeConstRef /*node*/,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  return YGSize{50.0f, 10.0f};
}

TEST(YogaTest, rounding_text_position_is_stable_across_relayouts) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 1.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 100);

  YGNodeRef spacer = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root, spacer, 0);

  YGNodeRef box = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(box, 80);
  YGNodeStyleSetHeight(box, 40);
  YGNodeInsertChild(root, box, 1);

  YGNodeRef view = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(view, 60);
  YGNodeStyleSetHeight(view, 20);
  YGNodeInsertChild(box, view, 0);

  YGNodeRef text = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(text, _measureText);
  YGNodeInsertChild(box, text, 1);

  for (const float height : {0.0f, 0.6f, 0.7f, 0.8f, 0.9f, 0.55f, 0.65f}) {
    YGNodeStyleSetHeight(spacer, height);
    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
    ASSERT_FLOAT_EQ(20.0f, YGNodeLayoutGetTop(text)) << "spacer " << height;
  }

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, rounding_text_at_leading_edge_stays_inside_parent) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 1.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 100);

  YGNodeRef parent = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(parent, YGEdgeLeft, 0.5f);
  YGNodeStyleSetMargin(parent, YGEdgeTop, 0.5f);
  YGNodeStyleSetOverflow(parent, YGOverflowHidden);
  YGNodeInsertChild(root, parent, 0);

  YGNodeRef text = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(text, _measureText);
  YGNodeInsertChild(parent, text, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0.0f, YGNodeLayoutGetLeft(text));
  ASSERT_FLOAT_EQ(0.0f, YGNodeLayoutGetTop(text));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, rounding_child_pinned_to_bottom_keeps_parent_bottom_edge) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 1.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 100);

  YGNodeRef parent = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(parent, YGPositionTypeAbsolute);
  YGNodeStyleSetPosition(parent, YGEdgeTop, 0.25f);
  YGNodeStyleSetWidth(parent, 100);
  YGNodeStyleSetHeight(parent, 10.5f);
  YGNodeInsertChild(root, parent, 0);

  YGNodeRef child = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(child, YGPositionTypeAbsolute);
  YGNodeStyleSetPosition(child, YGEdgeBottom, 0);
  YGNodeStyleSetWidth(child, 100);
  YGNodeStyleSetHeight(child, 10.25f);
  YGNodeInsertChild(parent, child, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(11.0f, YGNodeLayoutGetHeight(parent));
  ASSERT_FLOAT_EQ(1.0f, YGNodeLayoutGetTop(child));
  ASSERT_FLOAT_EQ(10.0f, YGNodeLayoutGetHeight(child));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, rounding_child_pinned_to_right_keeps_parent_right_edge) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 1.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef parent = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(parent, YGPositionTypeAbsolute);
  YGNodeStyleSetPosition(parent, YGEdgeLeft, 0.25f);
  YGNodeStyleSetWidth(parent, 10.5f);
  YGNodeStyleSetHeight(parent, 100);
  YGNodeInsertChild(root, parent, 0);

  YGNodeRef child = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(child, YGPositionTypeAbsolute);
  YGNodeStyleSetPosition(child, YGEdgeRight, 0);
  YGNodeStyleSetWidth(child, 10.25f);
  YGNodeStyleSetHeight(child, 100);
  YGNodeInsertChild(parent, child, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(11.0f, YGNodeLayoutGetWidth(parent));
  ASSERT_FLOAT_EQ(1.0f, YGNodeLayoutGetLeft(child));
  ASSERT_FLOAT_EQ(10.0f, YGNodeLayoutGetWidth(child));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, rounding_child_pinned_to_bottom_never_leaves_a_gap) {
  const float scale = 2.25f;
  for (float offset = 0.0f; offset < 4.0f; offset += 0.1111f) {
    for (float height = 150.0f; height < 260.0f; height += 0.4444f) {
      YGConfigRef config = YGConfigNew();
      YGConfigSetPointScaleFactor(config, scale);

      YGNodeRef root = YGNodeNewWithConfig(config);
      YGNodeStyleSetWidth(root, 448);

      YGNodeRef spacer = YGNodeNewWithConfig(config);
      YGNodeStyleSetHeight(spacer, offset);
      YGNodeInsertChild(root, spacer, 0);

      YGNodeRef parent = YGNodeNewWithConfig(config);
      YGNodeStyleSetWidth(parent, 284);
      YGNodeStyleSetHeight(parent, 506);
      YGNodeInsertChild(root, parent, 1);

      YGNodeRef child = YGNodeNewWithConfig(config);
      YGNodeStyleSetPositionType(child, YGPositionTypeAbsolute);
      YGNodeStyleSetPosition(child, YGEdgeLeft, 0);
      YGNodeStyleSetPosition(child, YGEdgeRight, 0);
      YGNodeStyleSetPosition(child, YGEdgeBottom, 0);
      YGNodeStyleSetHeight(child, height);
      YGNodeInsertChild(parent, child, 0);

      YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

      ASSERT_FLOAT_EQ(
          YGNodeLayoutGetHeight(parent),
          YGNodeLayoutGetTop(child) + YGNodeLayoutGetHeight(child))
          << "offset " << offset << " height " << height;

      YGNodeFreeRecursive(root);
      YGConfigFree(config);
    }
  }
}
