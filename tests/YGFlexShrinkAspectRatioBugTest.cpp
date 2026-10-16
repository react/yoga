/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Regression test for https://github.com/react/yoga/issues/2038
// Sibling case to YGFlexShrinkBorderBugTest (issue #1665): an item whose flex
// basis comes from an aspectRatio, and which is the only shrinkable item on an
// overflowing line, is measured at ~8.3e9 instead of being clamped to 0.
//
// Mechanism: distributeFreeSpaceFirstPass freezes the item and subtracts its
// contribution from totalFlexShrinkScaledFactors. The compiler contracts that
// multiply-subtract into a fused multiply-add using the unrounded product, so
// it does not cancel against the float accumulator built in calculateFlexLine
// and leaves a residual of 2^-19. distributeFreeSpaceSecondPass then divides
// the remaining free space by that residual. The absolute 1e-6f epsilon guard
// added for #1665 does not catch it, because the residual's magnitude scales
// with the basis and shrink factor.

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

namespace {

// root(350x350) > row(flexDirection: row) > [ box(aspectRatio, flexShrink), sibling(600x23) ]
// Returns the box; the caller owns the tree via rootOut.
YGNodeRef buildOverflowingRow(YGConfigRef config, YGNodeRef* rootOut, float shrink) {
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 350.0f);
  YGNodeStyleSetHeight(root, 350.0f);

  YGNodeRef row = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(row, YGFlexDirectionRow);
  YGNodeInsertChild(root, row, 0);

  // The only shrinkable item on the line. Its flex basis is derived from the
  // aspect ratio and the stretched cross size (23 * 22/24 = 21.083...), which is
  // what makes the precision asymmetry observable.
  YGNodeRef box = YGNodeNewWithConfig(config);
  YGNodeStyleSetAspectRatio(box, 22.0f / 24.0f);
  YGNodeStyleSetFlexShrink(box, shrink);
  YGNodeInsertChild(row, box, 0);

  // Overflows the row so remainingFreeSpace is negative. flexShrink defaults to
  // 0, so this item cannot shrink and the box is the sole candidate.
  YGNodeRef sibling = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(sibling, 600.0f);
  YGNodeStyleSetHeight(sibling, 23.0f);
  YGNodeInsertChild(row, sibling, 1);

  *rootOut = root;
  return box;
}

} // namespace

TEST(YGFlexShrinkAspectRatioBug, aspect_ratio_sole_shrinkable_item_on_overflowing_line) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = nullptr;
  YGNodeRef box = buildOverflowingRow(config, &root, 3.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Unshrunk the box would be 21.083 wide; shrinking can only reduce it.
  EXPECT_GE(YGNodeLayoutGetWidth(box), 0.0f);
  EXPECT_LE(YGNodeLayoutGetWidth(box), 22.0f)
      << "shrinking item is astronomically large (bug)";
  // aspectRatio derives the cross axis from the main axis, so a runaway width
  // propagates to the height as well.
  EXPECT_LE(YGNodeLayoutGetHeight(box), 24.0f)
      << "runaway propagated to the cross axis via aspectRatio";

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// flexShrink 3 is not special. Whether a given factor diverges depends on where
// its product with the 55/3 basis lands in the float mantissa, so cover a
// spread. Values 1 and 2 already clamp correctly on the unfixed code; the rest
// do not.
TEST(YGFlexShrinkAspectRatioBug, bounded_across_shrink_factors) {
  const float shrinkFactors[] = {
      1.0f, 2.0f, 3.0f, 6.0f, 9.0f, 11.0f, 12.0f, 19.0f, 22.0f, 999.0f};

  for (float shrink : shrinkFactors) {
    YGConfigRef config = YGConfigNew();
    YGNodeRef root = nullptr;
    YGNodeRef box = buildOverflowingRow(config, &root, shrink);

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

    EXPECT_LE(YGNodeLayoutGetWidth(box), 22.0f)
        << "flexShrink " << shrink << " produced a runaway width";
    EXPECT_GE(YGNodeLayoutGetWidth(box), 0.0f)
        << "flexShrink " << shrink << " produced a negative width";

    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }
}

// Guards the basis itself: with no shrinking the box must keep its
// aspect-ratio-derived size, so a change here would mean the basis moved rather
// than the distribution misbehaving.
TEST(YGFlexShrinkAspectRatioBug, unshrinkable_item_keeps_aspect_ratio_basis) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = nullptr;
  YGNodeRef box = buildOverflowingRow(config, &root, 0.0f);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // 23 * 22/24 = 21.083..., rounded to the default pixel grid.
  EXPECT_FLOAT_EQ(21.0f, YGNodeLayoutGetWidth(box));
  EXPECT_FLOAT_EQ(23.0f, YGNodeLayoutGetHeight(box));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
