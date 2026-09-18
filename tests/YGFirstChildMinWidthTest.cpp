/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Regression test for https://github.com/react/yoga/issues/2006
// A row of three growable children (flexGrow/flexShrink 1, maxWidth 180) inside
// a 540px container. When the *first* child has a larger minWidth than the
// rest, the first free-space pass used to freeze every item at its max while
// also draining the remaining free space to exactly zero, leaving the second
// pass with nothing to distribute. The children then collapsed to their
// minWidths (60/30/30) instead of growing to fill the row (180/180/180).
//
// The fix is gated on clearing YGErrataFlexFirstPassUsesRunningTotals, which
// new configs set by default, so both behaviors are pinned here.

#include <array>

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

namespace {

// A config opted into the spec-correct single distribution.
YGConfigRef makeFixedConfig() {
  YGConfigRef config = YGConfigNew();
  const YGErrata errata = YGConfigGetErrata(config);
  YGConfigSetErrata(
      config,
      static_cast<YGErrata>(errata & ~YGErrataFlexFirstPassUsesRunningTotals));
  return config;
}

// Lay out a 540px row containing three children, each with flexGrow/
// flexShrink of 1, maxWidth of 180 and the given minWidths, and return the
// resolved width of each child.
std::array<float, 3> layoutRow(
    YGConfigRef config,
    float minWidth0,
    float minWidth1,
    float minWidth2) {
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 540.0f);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);

  const float minWidths[] = {minWidth0, minWidth1, minWidth2};
  for (size_t i = 0; i < 3; i++) {
    YGNodeRef child = YGNodeNewWithConfig(config);
    YGNodeStyleSetFlexGrow(child, 1.0f);
    YGNodeStyleSetFlexShrink(child, 1.0f);
    YGNodeStyleSetMaxWidth(child, 180.0f);
    YGNodeStyleSetHeight(child, 30.0f);
    YGNodeStyleSetMinWidth(child, minWidths[i]);
    YGNodeInsertChild(root, child, i);
  }

  YGNodeCalculateLayout(root, 540.0f, 30.0f, YGDirectionLTR);

  std::array<float, 3> widths{};
  for (size_t i = 0; i < 3; i++) {
    widths[i] = YGNodeLayoutGetWidth(YGNodeGetChild(root, i));
  }

  YGNodeFreeRecursive(root);
  return widths;
}

// Assert every child grows to maxWidth (the row exactly fills the container).
void expectAllGrowToMax(float minWidth0, float minWidth1, float minWidth2) {
  YGConfigRef config = makeFixedConfig();
  const auto widths = layoutRow(config, minWidth0, minWidth1, minWidth2);

  for (size_t i = 0; i < 3; i++) {
    EXPECT_NEAR(180.0f, widths[i], 1e-3f)
        << "child[" << i
        << "] should grow to maxWidth, not collapse to its minWidth";
  }

  YGConfigFree(config);
}

} // namespace

TEST(YGFirstChildMinWidth, first_child_larger_minwidth_row) {
  // The originally reported case: the first child has a larger minWidth than
  // the other two, which used to break the whole row.
  expectAllGrowToMax(60.0f, 30.0f, 30.0f);
}

TEST(YGFirstChildMinWidth, other_positions_still_work) {
  // Sanity: placing the outlier minWidth on the second or third child already
  // worked and must keep working.
  expectAllGrowToMax(30.0f, 60.0f, 30.0f);
  expectAllGrowToMax(30.0f, 30.0f, 60.0f);
}

TEST(YGFirstChildMinWidth, uniform_minwidth_row) {
  // All children share the same minWidth: no issue either way.
  expectAllGrowToMax(30.0f, 30.0f, 30.0f);
}

TEST(YGFirstChildMinWidth, errata_preserves_prefix_geometry_by_default) {
  // A default config keeps the pre-fix collapse, so existing layouts do not
  // shift when this change lands. Removing the errata bit from the defaults is
  // what makes this test fail.
  YGConfigRef config = YGConfigNew();
  ASSERT_NE(
      YGConfigGetErrata(config) & YGErrataFlexFirstPassUsesRunningTotals, 0);

  const auto widths = layoutRow(config, 60.0f, 30.0f, 30.0f);
  EXPECT_NEAR(60.0f, widths[0], 1e-3f);
  EXPECT_NEAR(30.0f, widths[1], 1e-3f);
  EXPECT_NEAR(30.0f, widths[2], 1e-3f);

  YGConfigFree(config);
}

TEST(YGFirstChildMinWidth, errata_bit_round_trips) {
  YGConfigRef config = YGConfigNew();
  EXPECT_NE(
      YGConfigGetErrata(config) & YGErrataFlexFirstPassUsesRunningTotals, 0);

  YGConfigSetErrata(
      config,
      static_cast<YGErrata>(
          YGConfigGetErrata(config) & ~YGErrataFlexFirstPassUsesRunningTotals));
  EXPECT_EQ(
      YGConfigGetErrata(config) & YGErrataFlexFirstPassUsesRunningTotals, 0);

  YGConfigFree(config);
}
