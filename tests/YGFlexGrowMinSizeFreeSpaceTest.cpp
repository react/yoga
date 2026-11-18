/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

class YGFlexGrowMinSizeFreeSpace : public testing::Test {
 protected:
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = YGNodeNewWithConfig(config);

  void SetUp() override {
    YGConfigSetErrata(config, YGErrataNone);
    YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
    YGNodeStyleSetHeight(root, 10);
  }

  void TearDown() override {
    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }

  YGNodeRef addChild(float grow, float basis = 0) {
    YGNodeRef child = YGNodeNewWithConfig(config);
    YGNodeStyleSetFlexGrow(child, grow);
    YGNodeStyleSetFlexBasis(child, basis);
    YGNodeInsertChild(root, child, YGNodeGetChildCount(root));
    return child;
  }
};

TEST_F(YGFlexGrowMinSizeFreeSpace, min_width_below_proportional_share) {
  YGNodeStyleSetWidth(root, 120);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinWidth(first, 30);
  YGNodeRef second = addChild(2);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(40, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(80, YGNodeLayoutGetWidth(second));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, min_width_above_proportional_share) {
  YGNodeStyleSetWidth(root, 300);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinWidth(first, 200);
  YGNodeRef second = addChild(1);
  YGNodeRef third = addChild(1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(200, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(50, YGNodeLayoutGetWidth(second));
  EXPECT_FLOAT_EQ(50, YGNodeLayoutGetWidth(third));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, errata_preserves_original_sizes) {
  YGConfigSetErrata(config, YGErrataFlexGrowFreeSpaceUsesClampedBasis);
  YGNodeStyleSetWidth(root, 300);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinWidth(first, 200);
  YGNodeRef second = addChild(1);
  YGNodeRef third = addChild(1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(233, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(34, YGNodeLayoutGetWidth(second));
  EXPECT_FLOAT_EQ(33, YGNodeLayoutGetWidth(third));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, errata_enabled_by_default) {
  YGConfigRef defaults = YGConfigNew();
  const YGErrata errata = YGConfigGetErrata(defaults);
  YGConfigFree(defaults);
  EXPECT_NE(errata & YGErrataFlexGrowFreeSpaceUsesClampedBasis, 0);
  EXPECT_NE(YGErrataAll & YGErrataFlexGrowFreeSpaceUsesClampedBasis, 0);
  EXPECT_NE(YGErrataClassic & YGErrataFlexGrowFreeSpaceUsesClampedBasis, 0);
  YGConfigSetErrata(config, errata);
  YGNodeStyleSetWidth(root, 120);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinWidth(first, 30);
  YGNodeRef second = addChild(2);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(60, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(60, YGNodeLayoutGetWidth(second));

  YGConfigSetErrata(
      config,
      static_cast<YGErrata>(
          errata & ~YGErrataFlexGrowFreeSpaceUsesClampedBasis));
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(40, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(80, YGNodeLayoutGetWidth(second));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, max_width_freezes_before_distribution) {
  YGNodeStyleSetWidth(root, 300);
  YGNodeRef first = addChild(1, 150);
  YGNodeStyleSetMaxWidth(first, 50);
  YGNodeRef second = addChild(1);
  YGNodeStyleSetMinWidth(second, 100);
  YGNodeRef third = addChild(1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(50, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(125, YGNodeLayoutGetWidth(second));
  EXPECT_FLOAT_EQ(125, YGNodeLayoutGetWidth(third));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, gap_is_removed_from_free_space) {
  YGNodeStyleSetWidth(root, 140);
  YGNodeStyleSetGap(root, YGGutterColumn, 20);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinWidth(first, 30);
  YGNodeRef second = addChild(2);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(40, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(80, YGNodeLayoutGetWidth(second));
  EXPECT_FLOAT_EQ(60, YGNodeLayoutGetLeft(second));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, column_uses_unclamped_height) {
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);
  YGNodeStyleSetWidth(root, 10);
  YGNodeStyleSetHeight(root, 120);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinHeight(first, 30);
  YGNodeRef second = addChild(2);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(40, YGNodeLayoutGetHeight(first));
  EXPECT_FLOAT_EQ(80, YGNodeLayoutGetHeight(second));
  EXPECT_FLOAT_EQ(40, YGNodeLayoutGetTop(second));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, wrapping_uses_hypothetical_sizes) {
  YGNodeStyleSetWidth(root, 120);
  YGNodeStyleSetHeight(root, 20);
  YGNodeStyleSetFlexWrap(root, YGWrapWrap);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinWidth(first, 30);
  YGNodeRef second = addChild(2);
  YGNodeStyleSetMinWidth(second, 30);
  YGNodeRef third = addChild(1);
  YGNodeStyleSetMinWidth(third, 100);
  for (YGNodeRef child : {first, second, third}) {
    YGNodeStyleSetHeight(child, 10);
  }

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(40, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(80, YGNodeLayoutGetWidth(second));
  EXPECT_FLOAT_EQ(120, YGNodeLayoutGetWidth(third));
  EXPECT_FLOAT_EQ(0, YGNodeLayoutGetTop(first));
  EXPECT_FLOAT_EQ(0, YGNodeLayoutGetTop(second));
  EXPECT_FLOAT_EQ(10, YGNodeLayoutGetTop(third));

  YGConfigSetErrata(config, YGErrataFlexGrowFreeSpaceUsesClampedBasis);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(50, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(70, YGNodeLayoutGetWidth(second));
  EXPECT_FLOAT_EQ(120, YGNodeLayoutGetWidth(third));
  EXPECT_FLOAT_EQ(0, YGNodeLayoutGetTop(first));
  EXPECT_FLOAT_EQ(0, YGNodeLayoutGetTop(second));
  EXPECT_FLOAT_EQ(10, YGNodeLayoutGetTop(third));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, zero_grow_uses_hypothetical_size) {
  YGNodeStyleSetWidth(root, 200);
  YGNodeRef first = addChild(0);
  YGNodeStyleSetMinWidth(first, 50);
  YGNodeRef second = addChild(1);
  YGNodeRef third = addChild(2);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(50, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(50, YGNodeLayoutGetWidth(second));
  EXPECT_FLOAT_EQ(100, YGNodeLayoutGetWidth(third));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, nonzero_basis_and_margins) {
  YGNodeStyleSetWidth(root, 200);
  YGNodeRef first = addChild(1, 20);
  YGNodeStyleSetMinWidth(first, 60);
  YGNodeStyleSetMargin(first, YGEdgeRight, 20);
  YGNodeRef second = addChild(2, 40);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(60, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(120, YGNodeLayoutGetWidth(second));
  EXPECT_FLOAT_EQ(80, YGNodeLayoutGetLeft(second));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, mixed_min_max_freezes_only_max_violation) {
  YGNodeStyleSetWidth(root, 300);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinWidth(first, 160);
  YGNodeRef second = addChild(1);
  YGNodeStyleSetMaxWidth(second, 100);

  for (YGErrata errata :
       {YGErrataNone, YGErrataFlexFirstPassUsesRunningTotals}) {
    YGConfigSetErrata(config, errata);
    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

    EXPECT_FLOAT_EQ(200, YGNodeLayoutGetWidth(first));
    EXPECT_FLOAT_EQ(100, YGNodeLayoutGetWidth(second));
  }
}

TEST_F(YGFlexGrowMinSizeFreeSpace, mixed_violations_cancel) {
  YGNodeStyleSetWidth(root, 300);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinWidth(first, 180);
  YGNodeRef second = addChild(1);
  YGNodeStyleSetMaxWidth(second, 120);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(180, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(120, YGNodeLayoutGetWidth(second));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, fractional_grow_after_min_freezes) {
  YGConfigSetPointScaleFactor(config, 0);
  YGNodeStyleSetWidth(root, 200);
  YGNodeRef first = addChild(0.25f);
  YGNodeStyleSetMinWidth(first, 100);
  YGNodeRef second = addChild(0.25f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(100, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(50, YGNodeLayoutGetWidth(second));
}

TEST_F(YGFlexGrowMinSizeFreeSpace, successive_min_violations) {
  YGNodeStyleSetWidth(root, 600);
  YGNodeRef first = addChild(1);
  YGNodeStyleSetMinWidth(first, 200);
  YGNodeRef second = addChild(1);
  YGNodeStyleSetMinWidth(second, 180);
  YGNodeRef third = addChild(1);
  YGNodeStyleSetMinWidth(third, 120);
  YGNodeRef fourth = addChild(1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(200, YGNodeLayoutGetWidth(first));
  EXPECT_FLOAT_EQ(180, YGNodeLayoutGetWidth(second));
  EXPECT_FLOAT_EQ(120, YGNodeLayoutGetWidth(third));
  EXPECT_FLOAT_EQ(100, YGNodeLayoutGetWidth(fourth));
}
