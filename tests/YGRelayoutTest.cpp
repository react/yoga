/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

#include <cmath>
#include <limits>

/*
 * Behaves like wrapping text: 2000pt of content laid out at the available
 * width with a 20pt line height, so a narrower box measures taller.
 */
static YGSize _simulateWrappingText(
    YGNodeConstRef /*node*/,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode) {
  const float maxWidth = widthMode == YGMeasureModeUndefined
      ? std::numeric_limits<float>::infinity()
      : width;
  const float lines = std::isfinite(maxWidth) && maxWidth > 0
      ? std::ceil(2000.0f / maxWidth)
      : 1.0f;
  const float naturalHeight = lines * 20.0f;

  YGSize size{std::isfinite(maxWidth) ? maxWidth : 2000.0f, naturalHeight};
  if (heightMode == YGMeasureModeExactly) {
    size.height = height;
  } else if (heightMode == YGMeasureModeAtMost && naturalHeight > height) {
    size.height = height;
  }
  if (widthMode == YGMeasureModeExactly) {
    size.width = width;
  }

  return size;
}

/*
 * Pins the root to an exact size, the way a host platform does when the
 * available space changes (e.g. a device rotation), then lays out.
 */
static void _layoutAtSize(YGNodeRef root, float width, float height) {
  YGNodeStyleSetMinWidth(root, width);
  YGNodeStyleSetMaxWidth(root, width);
  YGNodeStyleSetMinHeight(root, height);
  YGNodeStyleSetMaxHeight(root, height);
  YGNodeCalculateLayout(root, width, height, YGDirectionLTR);
}

TEST(YogaTest, dont_cache_computed_flex_basis_between_layouts) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureWebFlexBasis, true);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeightPercent(root, 100);
  YGNodeStyleSetWidthPercent(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasisPercent(root_child0, 100);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, 100, YGUndefined, YGDirectionLTR);
  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);

  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, recalculate_resolvedDimonsion_onchange) {
  YGNodeRef root = YGNodeNew();

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetMinHeight(root_child0, 10);
  YGNodeStyleSetMaxHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetHeight(root_child0));

  YGNodeStyleSetMinHeight(root_child0, YGUndefined);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, relayout_containing_block_size_changes) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root, YGPositionTypeAbsolute);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeRelative);
  YGNodeStyleSetMargin(root_child0, YGEdgeLeft, 4);
  YGNodeStyleSetMargin(root_child0, YGEdgeTop, 5);
  YGNodeStyleSetMargin(root_child0, YGEdgeRight, 9);
  YGNodeStyleSetMargin(root_child0, YGEdgeBottom, 1);
  YGNodeStyleSetPadding(root_child0, YGEdgeLeft, 2);
  YGNodeStyleSetPadding(root_child0, YGEdgeTop, 9);
  YGNodeStyleSetPadding(root_child0, YGEdgeRight, 11);
  YGNodeStyleSetPadding(root_child0, YGEdgeBottom, 13);
  YGNodeStyleSetBorder(root_child0, YGEdgeLeft, 5);
  YGNodeStyleSetBorder(root_child0, YGEdgeTop, 6);
  YGNodeStyleSetBorder(root_child0, YGEdgeRight, 7);
  YGNodeStyleSetBorder(root_child0, YGEdgeBottom, 8);
  YGNodeStyleSetWidth(root_child0, 500);
  YGNodeStyleSetHeight(root_child0, 500);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root_child0_child0, YGPositionTypeStatic);
  YGNodeStyleSetMargin(root_child0_child0, YGEdgeLeft, 8);
  YGNodeStyleSetMargin(root_child0_child0, YGEdgeTop, 6);
  YGNodeStyleSetMargin(root_child0_child0, YGEdgeRight, 3);
  YGNodeStyleSetMargin(root_child0_child0, YGEdgeBottom, 9);
  YGNodeStyleSetPadding(root_child0_child0, YGEdgeLeft, 1);
  YGNodeStyleSetPadding(root_child0_child0, YGEdgeTop, 7);
  YGNodeStyleSetPadding(root_child0_child0, YGEdgeRight, 9);
  YGNodeStyleSetPadding(root_child0_child0, YGEdgeBottom, 4);
  YGNodeStyleSetBorder(root_child0_child0, YGEdgeLeft, 8);
  YGNodeStyleSetBorder(root_child0_child0, YGEdgeTop, 10);
  YGNodeStyleSetBorder(root_child0_child0, YGEdgeRight, 2);
  YGNodeStyleSetBorder(root_child0_child0, YGEdgeBottom, 1);
  YGNodeStyleSetWidth(root_child0_child0, 200);
  YGNodeStyleSetHeight(root_child0_child0, 200);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeRef root_child0_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root_child0_child0_child0, YGPositionTypeAbsolute);
  YGNodeStyleSetPosition(root_child0_child0_child0, YGEdgeLeft, 2);
  YGNodeStyleSetPosition(root_child0_child0_child0, YGEdgeRight, 12);
  YGNodeStyleSetMargin(root_child0_child0_child0, YGEdgeLeft, 9);
  YGNodeStyleSetMargin(root_child0_child0_child0, YGEdgeTop, 12);
  YGNodeStyleSetMargin(root_child0_child0_child0, YGEdgeRight, 4);
  YGNodeStyleSetMargin(root_child0_child0_child0, YGEdgeBottom, 7);
  YGNodeStyleSetPadding(root_child0_child0_child0, YGEdgeLeft, 5);
  YGNodeStyleSetPadding(root_child0_child0_child0, YGEdgeTop, 3);
  YGNodeStyleSetPadding(root_child0_child0_child0, YGEdgeRight, 8);
  YGNodeStyleSetPadding(root_child0_child0_child0, YGEdgeBottom, 10);
  YGNodeStyleSetBorder(root_child0_child0_child0, YGEdgeLeft, 2);
  YGNodeStyleSetBorder(root_child0_child0_child0, YGEdgeTop, 1);
  YGNodeStyleSetBorder(root_child0_child0_child0, YGEdgeRight, 5);
  YGNodeStyleSetBorder(root_child0_child0_child0, YGEdgeBottom, 9);
  YGNodeStyleSetWidthPercent(root_child0_child0_child0, 41);
  YGNodeStyleSetHeightPercent(root_child0_child0_child0, 63);
  YGNodeInsertChild(root_child0_child0, root_child0_child0_child0, 0);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(513, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(506, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(4, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(15, YGNodeLayoutGetLeft(root_child0_child0));
  ASSERT_FLOAT_EQ(21, YGNodeLayoutGetTop(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child0_child0));

  ASSERT_FLOAT_EQ(1, YGNodeLayoutGetLeft(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(29, YGNodeLayoutGetTop(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(306, YGNodeLayoutGetHeight(root_child0_child0_child0));

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionRTL);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(513, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(506, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(4, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(500, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(279, YGNodeLayoutGetLeft(root_child0_child0));
  ASSERT_FLOAT_EQ(21, YGNodeLayoutGetTop(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child0_child0));

  ASSERT_FLOAT_EQ(-2, YGNodeLayoutGetLeft(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(29, YGNodeLayoutGetTop(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(306, YGNodeLayoutGetHeight(root_child0_child0_child0));

  // Relayout starts here
  YGNodeStyleSetWidth(root_child0, 456);
  YGNodeStyleSetHeight(root_child0, 432);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(469, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(438, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(4, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(456, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(432, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(15, YGNodeLayoutGetLeft(root_child0_child0));
  ASSERT_FLOAT_EQ(21, YGNodeLayoutGetTop(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child0_child0));

  ASSERT_FLOAT_EQ(1, YGNodeLayoutGetLeft(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(29, YGNodeLayoutGetTop(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(182, YGNodeLayoutGetWidth(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(263, YGNodeLayoutGetHeight(root_child0_child0_child0));

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionRTL);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(469, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(438, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(4, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(456, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(432, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(235, YGNodeLayoutGetLeft(root_child0_child0));
  ASSERT_FLOAT_EQ(21, YGNodeLayoutGetTop(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(root_child0_child0));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root_child0_child0));

  ASSERT_FLOAT_EQ(16, YGNodeLayoutGetLeft(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(29, YGNodeLayoutGetTop(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(182, YGNodeLayoutGetWidth(root_child0_child0_child0));
  ASSERT_FLOAT_EQ(263, YGNodeLayoutGetHeight(root_child0_child0_child0));

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, has_new_layout_flag_set_static) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeStatic);
  YGNodeStyleSetWidth(root_child0, 10);
  YGNodeStyleSetHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child1 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child1, YGPositionTypeAbsolute);
  YGNodeStyleSetWidth(root_child0_child1, 5);
  YGNodeStyleSetHeight(root_child0_child1, 5);
  YGNodeInsertChild(root_child0, root_child0_child1, 0);

  YGNodeRef root_child0_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child0, YGPositionTypeStatic);
  YGNodeStyleSetWidth(root_child0_child0, 5);
  YGNodeStyleSetHeight(root_child0_child0, 5);
  YGNodeInsertChild(root_child0, root_child0_child0, 1);

  YGNodeRef root_child0_child0_child0 = YGNodeNew();
  YGNodeStyleSetPositionType(root_child0_child0_child0, YGPositionTypeAbsolute);
  YGNodeStyleSetWidthPercent(root_child0_child0_child0, 1);
  YGNodeStyleSetHeight(root_child0_child0_child0, 1);
  YGNodeInsertChild(root_child0_child0, root_child0_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  YGNodeSetHasNewLayout(root, false);
  YGNodeSetHasNewLayout(root_child0, false);
  YGNodeSetHasNewLayout(root_child0_child0, false);
  YGNodeSetHasNewLayout(root_child0_child0_child0, false);

  YGNodeStyleSetWidth(root, 110);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_TRUE(YGNodeGetHasNewLayout(root));
  ASSERT_TRUE(YGNodeGetHasNewLayout(root_child0));
  ASSERT_TRUE(YGNodeGetHasNewLayout(root_child0_child0));
  ASSERT_TRUE(YGNodeGetHasNewLayout(root_child0_child0_child0));

  YGNodeFreeRecursive(root);
}

/*
 * A flex basis measured from a child's content during an earlier layout must
 * not be reused once the available size changes. The wrapper levels let an
 * ancestor's measurement cache answer for the whole subtree during the
 * max-content pass, skipping the measurement that would otherwise refresh the
 * stored basis.
 */
TEST(YogaTest, measured_flex_basis_is_not_reused_after_relayout_at_new_size) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);

  YGNodeRef scrollView = YGNodeNewWithConfig(config);
  YGNodeStyleSetOverflow(scrollView, YGOverflowScroll);
  YGNodeStyleSetFlexGrow(scrollView, 1);
  YGNodeStyleSetFlexShrink(scrollView, 1);
  YGNodeInsertChild(root, scrollView, 0);

  YGNodeRef contentContainer = YGNodeNewWithConfig(config);
  YGNodeInsertChild(scrollView, contentContainer, 0);

  YGNodeRef autoHeightColumn = YGNodeNewWithConfig(config);
  YGNodeInsertChild(contentContainer, autoHeightColumn, 0);

  YGNodeRef flexColumn = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlex(flexColumn, 1);
  YGNodeInsertChild(autoHeightColumn, flexColumn, 0);

  YGNodeRef fixedSibling = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(fixedSibling, 24);
  YGNodeInsertChild(flexColumn, fixedSibling, 0);

  YGNodeRef wrapper0 = YGNodeNewWithConfig(config);
  YGNodeInsertChild(flexColumn, wrapper0, 1);

  YGNodeRef wrapper1 = YGNodeNewWithConfig(config);
  YGNodeInsertChild(wrapper0, wrapper1, 0);

  YGNodeRef text = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlex(text, 1);
  YGNodeSetMeasureFunc(text, _simulateWrappingText);
  YGNodeInsertChild(wrapper1, text, 0);

  _layoutAtSize(root, 400, 800);
  const float portraitHeight = YGNodeLayoutGetHeight(text);

  _layoutAtSize(root, 800, 400);
  _layoutAtSize(root, 400, 800);

  ASSERT_FLOAT_EQ(portraitHeight, YGNodeLayoutGetHeight(text));
  ASSERT_FLOAT_EQ(
      YGNodeLayoutGetHeight(wrapper0), YGNodeLayoutGetHeight(wrapper1));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
