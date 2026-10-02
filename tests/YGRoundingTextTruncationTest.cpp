/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

// This test reproduces a text truncation bug that occurs when:
// 1. A text node is positioned at an offset where absoluteNodeLeft * scale
//    has a fractional part very close to 1.0 (within 0.0001)
// 2. The text node width * scale also has fractional part close to 1.0
//    (hasFractionalWidth = false)
// 3. But absoluteNodeRight * scale has fractional part that is close to
//    but NOT within 0.0001 of 1.0
//
// In the buggy code, when hasFractionalWidth = false and textRounding = true,
// forceFloor was set to true. This caused absoluteNodeRight to be floored,
// while absoluteNodeLeft was correctly rounded up (due to inexactEquals check).
// The mismatch caused the final width to be smaller than the measured width.
//
// Example from real-world bug:
// - absoluteNodeLeft = 163.666641, * 3 = 490.999923 (fmod = 0.999923)
//   |1.0 - 0.999923| = 0.000077 < 0.0001 → rounds UP to 491
// - absoluteNodeRight = 218.333298, * 3 = 654.999894 (fmod = 0.999894)
//   |1.0 - 0.999894| = 0.000106 > 0.0001 → floors DOWN to 654 (BUG!)
// - Final width = 654/3 - 491/3 = 163/3 = 54.333... < measured 54.666... (TRUNCATED!)

// Measure function that returns width causing the truncation bug
// The exact value from the real-world bug case
static YGSize measureTextForTruncationBug(
    YGNodeConstRef /*node*/,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  // From the real bug: nodeWidth = 54.666656
  // 54.666656 * 3 = 163.999968
  // fmod = 0.999968, |1.0 - 0.999968| = 0.000032 < 0.0001
  // This means hasFractionalWidth = false (close to integer)
  return YGSize{
      .width = 54.666656f,
      .height = 50.0f,
  };
}

// Test that text nodes positioned at specific offsets don't get truncated
// due to pixel rounding edge cases.
//
// This test FAILS with the buggy code where forceFloor was used when
// hasFractionalWidth = false, and PASSES with the fix that always uses
// forceCeil for text nodes.
TEST(YogaTest, text_node_rounding_with_offset_should_not_truncate) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 3.0f);  // @3x display (iPhone Plus/Pro)

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 393);  // iPhone 15 Pro width
  YGNodeStyleSetHeight(root, 100);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);  // Prevent stretching
  // Padding creates offset matching real bug case:
  // - absoluteNodeLeft = 163.666641
  // - absoluteNodeLeft * 3 = 490.999923
  // - fmod = 0.999923, |1.0 - 0.999923| = 0.000077 < 0.0001 → rounds UP to 491/3
  YGNodeStyleSetPadding(root, YGEdgeLeft, 163.666641f);

  YGNodeRef textNode = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(textNode, measureTextForTruncationBug);
  YGNodeInsertChild(root, textNode, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  float measuredWidth = 54.666656f;
  float layoutWidth = YGNodeLayoutGetWidth(textNode);

  // With the bug (forceFloor when hasFractionalWidth = false):
  // - absoluteNodeRight = 163.666641 + 54.666656 = 218.333297
  // - absoluteNodeRight * 3 = 654.999891, fmod = 0.999891
  // - |1.0 - 0.999891| = 0.000109 > 0.0001 → inexactEquals returns false
  // - forceFloor = true → scaledValue floored to 654
  // - roundedRight = 654/3 = 218.0
  // - roundedLeft = 491/3 = 163.666...
  // - finalWidth = 218.0 - 163.666... = 54.333... < 54.666656 (TRUNCATED!)
  //
  // With the fix (forceCeil for text nodes, never forceFloor):
  // - forceCeil = true → scaledValue ceiled to 655
  // - roundedRight = 655/3 = 218.333...
  // - finalWidth = 218.333... - 163.666... = 54.666... >= 54.666656 ✓

  EXPECT_GE(layoutWidth, measuredWidth)
      << "Text node width should not be truncated. "
      << "Measured: " << measuredWidth << ", Layout: " << layoutWidth
      << ". This indicates the pixel rounding bug where text nodes get "
      << "incorrectly floored instead of ceiled.";

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Additional test with different offset values that also trigger the bug
TEST(YogaTest, text_node_rounding_alternate_offset_should_not_truncate) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 3.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 400);
  YGNodeStyleSetHeight(root, 100);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);  // Prevent stretching
  // Different offset that creates similar boundary condition
  // 290.33333 * 3 = 870.99999, very close to 871
  YGNodeStyleSetPadding(root, YGEdgeLeft, 290.33333f);

  YGNodeRef textNode = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(textNode, measureTextForTruncationBug);
  YGNodeInsertChild(root, textNode, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  float measuredWidth = 54.66665f;
  float layoutWidth = YGNodeLayoutGetWidth(textNode);

  EXPECT_GE(layoutWidth, measuredWidth)
      << "Text node width should not be truncated. "
      << "Measured: " << measuredWidth << ", Layout: " << layoutWidth;

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Test that the fix doesn't break normal text rounding behavior
// (text with clearly fractional width should still ceil)
TEST(YogaTest, text_node_with_fractional_width_still_ceils) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 3.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 400);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef textNode = YGNodeNewWithConfig(config);
  // 54.5 * 3 = 163.5, fmod = 0.5
  // This is clearly fractional (hasFractionalWidth = true)
  // Should still ceil to 164/3 = 54.666...
  YGNodeSetMeasureFunc(textNode, [](YGNodeConstRef, float, YGMeasureMode,
                                    float, YGMeasureMode) -> YGSize {
    return YGSize{.width = 54.5f, .height = 50.0f};
  });
  YGNodeInsertChild(root, textNode, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  float layoutWidth = YGNodeLayoutGetWidth(textNode);

  // Should be ceiled: 54.5 * 3 = 163.5 → ceil → 164 / 3 = 54.666...
  EXPECT_GE(layoutWidth, 54.5f);
  EXPECT_FLOAT_EQ(54.666666f, layoutWidth);

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Test text at zero offset (simple case, should always work)
TEST(YogaTest, text_node_at_zero_offset_should_not_truncate) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetPointScaleFactor(config, 3.0f);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 400);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);

  YGNodeRef textNode = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(textNode, measureTextForTruncationBug);
  YGNodeInsertChild(root, textNode, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  float measuredWidth = 54.66665f;
  float layoutWidth = YGNodeLayoutGetWidth(textNode);

  EXPECT_GE(layoutWidth, measuredWidth)
      << "Text node width should not be truncated even at zero offset.";

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
