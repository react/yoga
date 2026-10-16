/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

// Verifies that percentage min/max on flex items resolves against the parent's
// inner size (not the grandparent's owner size). When the errata flag
// FlexItemPercentMinMaxAgainstOwner is set, the old (buggy) behavior is
// preserved for backward compatibility.
//
// See: https://github.com/facebook/yoga/issues/872

TEST(YogaTest, percent_min_width_resolves_against_parent) {
  // Layout: root (40x20, row) > child (10px wide, row) > grandchild (min-width: 50%)
  // Correct: grandchild min-width = 50% of 10 = 5
  // Bug: grandchild min-width = 50% of 40 = 20
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 40);
  YGNodeStyleSetHeight(root, 20);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root_child0, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root_child0_child0, YGFlexDirectionRow);
  YGNodeStyleSetMinWidthPercent(root_child0_child0, 50);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  // Default (no errata): correct behavior
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child0_child0));

  // With errata: old buggy behavior (resolves against grandparent)
  YGConfigSetErrata(config, YGErrataFlexItemPercentMinMaxAgainstOwner);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetWidth(root_child0_child0));

  // Back to no errata: correct behavior restored
  YGConfigSetErrata(config, YGErrataNone);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child0_child0));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, percent_max_width_resolves_against_parent) {
  // Layout: root (40x20, row) > child (10px wide, row) > grandchild (width: 20, max-width: 50%)
  // Correct: grandchild max-width = 50% of 10 = 5
  // Note: For max-width, boundAxis() always re-applies the correct constraint
  // at the end of layout, so the errata flag does not change the final rendered
  // size (it only affects intermediate flex basis clamping and line-breaking).
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 40);
  YGNodeStyleSetHeight(root, 20);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root_child0, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root_child0_child0, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root_child0_child0, 20);
  YGNodeStyleSetMaxWidthPercent(root_child0_child0, 50);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  // Default (no errata): correct behavior
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child0_child0));

  // With errata: max-width still correctly resolved because boundAxis()
  // re-applies the constraint using the parent's inner size
  YGConfigSetErrata(config, YGErrataFlexItemPercentMinMaxAgainstOwner);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetWidth(root_child0_child0));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, percent_min_height_resolves_against_parent) {
  // Layout: root (20x40, column) > child (10px tall) > grandchild (min-height: 50%)
  // Correct: grandchild min-height = 50% of 10 = 5
  // Bug: grandchild min-height = 50% of 40 = 20
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);
  YGNodeStyleSetWidth(root, 20);
  YGNodeStyleSetHeight(root, 40);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetMinHeightPercent(root_child0_child0, 50);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  // Default (no errata): correct behavior
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetHeight(root_child0_child0));

  // With errata: old buggy behavior (resolves against grandparent)
  YGConfigSetErrata(config, YGErrataFlexItemPercentMinMaxAgainstOwner);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetHeight(root_child0_child0));

  // Back to no errata: correct behavior restored
  YGConfigSetErrata(config, YGErrataNone);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetHeight(root_child0_child0));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, classic_errata_includes_percent_min_max_against_owner) {
  // Classic errata should include FlexItemPercentMinMaxAgainstOwner,
  // preserving the old behavior for backward-compatible consumers
  YGConfigRef config = YGConfigNew();
  YGConfigSetErrata(config, YGErrataClassic);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 40);
  YGNodeStyleSetHeight(root, 20);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root_child0, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root_child0, 10);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root_child0_child0, YGFlexDirectionRow);
  YGNodeStyleSetMinWidthPercent(root_child0_child0, 50);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Classic errata should produce the old buggy behavior (50% of 40 = 20)
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetWidth(root_child0_child0));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
