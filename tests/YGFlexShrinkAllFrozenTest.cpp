/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Regression tests for flex shrink after the first pass freezes items:
// https://github.com/react/yoga/issues/2038 and #1651.

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

namespace {

const bool kWithoutErrata[] = {false, true};

YGConfigRef newConfig(bool withoutErrata) {
  YGConfigRef config = YGConfigNew();
  if (withoutErrata) {
    YGConfigSetErrata(config, YGErrataNone);
  }
  return config;
}

// root (350 x 350) > row > [box, sibling (600 x 23)]. The box is the only
// shrinkable item, and its basis comes from its aspectRatio and the stretched
// height: 23 * 22/24 = 21.083334.
YGNodeRef newAspectRatioRow(
    YGConfigRef config,
    float shrink,
    YGNodeRef* box,
    YGNodeRef* sibling) {
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 350.0f);
  YGNodeStyleSetHeight(root, 350.0f);

  YGNodeRef row = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(row, YGFlexDirectionRow);
  YGNodeInsertChild(root, row, 0);

  *box = YGNodeNewWithConfig(config);
  YGNodeStyleSetAspectRatio(*box, 22.0f / 24.0f);
  YGNodeStyleSetFlexShrink(*box, shrink);
  YGNodeInsertChild(row, *box, 0);

  *sibling = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(*sibling, 600.0f);
  YGNodeStyleSetHeight(*sibling, 23.0f);
  YGNodeInsertChild(row, *sibling, 1);

  return root;
}

YGSize measureHeightFromContext(
    YGNodeConstRef node,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  return YGSize{1.0f, *static_cast<float*>(YGNodeGetContext(node))};
}

} // namespace

// https://github.com/react/yoga/issues/2038
// All items freeze at their minWidth. Removing them from the float total left a
// residual instead of 0.
TEST(YGFlexShrinkAllFrozen, all_items_frozen_at_min_width) {
  for (bool withoutErrata : kWithoutErrata) {
    YGConfigRef config = newConfig(withoutErrata);
    YGNodeRef root = YGNodeNewWithConfig(config);
    YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
    YGNodeStyleSetWidth(root, 350.0f);
    YGNodeStyleSetHeight(root, 50.0f);

    const float widths[] = {100.1f, 200.3f, 300.7f};
    const float minWidths[] = {90.0f, 190.0f, 290.0f};
    for (size_t i = 0; i < 3; i++) {
      YGNodeRef child = YGNodeNewWithConfig(config);
      YGNodeStyleSetWidth(child, widths[i]);
      YGNodeStyleSetMinWidth(child, minWidths[i]);
      YGNodeStyleSetFlexShrink(child, 1.0f);
      YGNodeInsertChild(root, child, i);
    }

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

    for (size_t i = 0; i < 3; i++) {
      EXPECT_EQ(minWidths[i], YGNodeLayoutGetWidth(YGNodeGetChild(root, i)))
          << "withoutErrata " << withoutErrata << ", child[" << i << "]";
    }

    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }
}

// https://github.com/react/yoga/issues/2038
// The box freezes at 0, and freezing it left a 2^-19 residual. Shrink factors
// from #2039.
TEST(YGFlexShrinkAllFrozen, aspect_ratio_sole_shrinkable_item) {
  const float shrinkFactors[] = {
      1.0f, 2.0f, 3.0f, 6.0f, 9.0f, 11.0f, 12.0f, 19.0f, 22.0f, 999.0f};

  for (bool withoutErrata : kWithoutErrata) {
    for (float shrink : shrinkFactors) {
      YGConfigRef config = newConfig(withoutErrata);
      YGNodeRef box = nullptr;
      YGNodeRef sibling = nullptr;
      YGNodeRef root = newAspectRatioRow(config, shrink, &box, &sibling);

      YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

      EXPECT_EQ(0.0f, YGNodeLayoutGetWidth(box))
          << "withoutErrata " << withoutErrata << ", flexShrink " << shrink;
      EXPECT_EQ(0.0f, YGNodeLayoutGetHeight(box))
          << "withoutErrata " << withoutErrata << ", flexShrink " << shrink;
      EXPECT_EQ(0.0f, YGNodeLayoutGetLeft(sibling))
          << "withoutErrata " << withoutErrata << ", flexShrink " << shrink;

      YGNodeFreeRecursive(root);
      YGConfigFree(config);
    }
  }
}

// https://github.com/react/yoga/issues/2038
// With flexShrink 0 the box keeps its aspectRatio basis (21.08 x 23, rounded to
// 21 x 23). From #2039.
TEST(YGFlexShrinkAllFrozen, unshrinkable_item_keeps_aspect_ratio_basis) {
  for (bool withoutErrata : kWithoutErrata) {
    YGConfigRef config = newConfig(withoutErrata);
    YGNodeRef box = nullptr;
    YGNodeRef sibling = nullptr;
    YGNodeRef root = newAspectRatioRow(config, 0.0f, &box, &sibling);

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

    EXPECT_EQ(21.0f, YGNodeLayoutGetWidth(box))
        << "withoutErrata " << withoutErrata;
    EXPECT_EQ(23.0f, YGNodeLayoutGetHeight(box))
        << "withoutErrata " << withoutErrata;
    EXPECT_EQ(21.0f, YGNodeLayoutGetLeft(sibling))
        << "withoutErrata " << withoutErrata;

    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }
}

// https://github.com/react/yoga/issues/2038
// The residual grows with the size of the total: here it is about 0.0078, far
// above any fixed near-zero threshold, but it is still positive. Pixel rounding
// is off so that each item can be compared with its exact minWidth.
TEST(YGFlexShrinkAllFrozen, large_total_residual_is_not_divided_by) {
  for (bool withoutErrata : kWithoutErrata) {
    YGConfigRef config = newConfig(withoutErrata);
    YGConfigSetPointScaleFactor(config, 0.0f);
    YGNodeRef root = YGNodeNewWithConfig(config);
    YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
    YGNodeStyleSetWidth(root, 1000.0f);
    YGNodeStyleSetHeight(root, 50.0f);

    const float widths[] = {10000.1f, 30000.7f, 34567.8f};
    for (size_t i = 0; i < 3; i++) {
      YGNodeRef child = YGNodeNewWithConfig(config);
      YGNodeStyleSetWidth(child, widths[i]);
      YGNodeStyleSetMinWidth(child, widths[i] - 50.0f);
      YGNodeStyleSetFlexShrink(child, 1.0f);
      YGNodeInsertChild(root, child, i);
    }

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

    for (size_t i = 0; i < 3; i++) {
      EXPECT_EQ(
          widths[i] - 50.0f, YGNodeLayoutGetWidth(YGNodeGetChild(root, i)))
          << "withoutErrata " << withoutErrata << ", child[" << i << "]";
    }

    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }
}

// https://github.com/react/yoga/issues/2038
// A tiny item stays unfrozen while the others freeze. Its factor (-1e-5) is
// below the float precision of the total, so the total after removing the
// frozen items was a positive residual instead of the tiny item's factor.
TEST(YGFlexShrinkAllFrozen, tiny_item_remains_after_others_freeze) {
  for (bool withoutErrata : kWithoutErrata) {
    YGConfigRef config = newConfig(withoutErrata);
    YGNodeRef root = YGNodeNewWithConfig(config);
    YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
    YGNodeStyleSetWidth(root, 350.0f);
    YGNodeStyleSetHeight(root, 50.0f);

    const float widths[] = {100.1f, 200.3f, 300.7f, 0.00001f};
    const float minWidths[] = {90.0f, 190.0f, 290.0f, 0.0f};
    for (size_t i = 0; i < 4; i++) {
      YGNodeRef child = YGNodeNewWithConfig(config);
      YGNodeStyleSetWidth(child, widths[i]);
      YGNodeStyleSetMinWidth(child, minWidths[i]);
      YGNodeStyleSetFlexShrink(child, 1.0f);
      YGNodeInsertChild(root, child, i);
    }

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

    for (size_t i = 0; i < 4; i++) {
      EXPECT_EQ(minWidths[i], YGNodeLayoutGetWidth(YGNodeGetChild(root, i)))
          << "withoutErrata " << withoutErrata << ", child[" << i << "]";
    }

    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }
}

// https://github.com/react/yoga/issues/1651
// Measured items freeze at their lower bound, which is 0 with the default
// errata and the content height without (automatic minimum size). The residual
// made them millions of points tall.
TEST(YGFlexShrinkAllFrozen, issue_1651_measured_items_freeze_at_lower_bound) {
  for (bool withoutErrata : kWithoutErrata) {
    YGConfigRef config = newConfig(withoutErrata);
    YGConfigSetUseWebDefaults(config, true);
    YGConfigSetPointScaleFactor(config, 100.0f);
    YGNodeRef root = YGNodeNewWithConfig(config);
    YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);
    YGNodeStyleSetHeight(root, 99.0f);

    float heights[] = {10.01f, 10.01f, 12.01f};
    for (size_t i = 0; i < 3; i++) {
      YGNodeRef child = YGNodeNewWithConfig(config);
      YGNodeSetContext(child, &heights[i]);
      YGNodeSetMeasureFunc(child, measureHeightFromContext);
      YGNodeInsertChild(root, child, i);
    }
    YGNodeStyleSetMargin(YGNodeGetChild(root, 0), YGEdgeBottom, 100.0f);

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

    float top = 0.0f;
    for (size_t i = 0; i < 3; i++) {
      YGNodeRef child = YGNodeGetChild(root, i);
      const float height = withoutErrata ? heights[i] : 0.0f;
      EXPECT_FLOAT_EQ(height, YGNodeLayoutGetHeight(child))
          << "withoutErrata " << withoutErrata << ", child[" << i << "]";
      EXPECT_FLOAT_EQ(top, YGNodeLayoutGetTop(child))
          << "withoutErrata " << withoutErrata << ", child[" << i << "]";
      top += height + (i == 0 ? 100.0f : 0.0f);
    }

    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }
}
