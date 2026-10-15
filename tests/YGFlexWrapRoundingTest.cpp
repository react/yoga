/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

namespace {

// Reports the fractional width supplied through the node's context, standing in
// for a text measurement whose result is not representable on the pixel grid.
YGSize measureFractionalWidth(
    YGNodeConstRef node,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  const float* measuredWidth =
      static_cast<const float*>(YGNodeGetContext(const_cast<YGNodeRef>(node)));
  return YGSize{*measuredWidth, 20.0f};
}

} // namespace

// A content-sized wrapping row sizes itself to the max-content sum of its
// items, then recovers the space available to those items by subtracting its
// padding and border back off. When a measure function contributes a fractional
// width, that float32 round-trip can land a fraction of an ulp below the sum
// it came from, while the measurement cache still returns the max-content
// measurement unchanged because it treats the difference as equal
// (oldSizeIsMaxContentAndStillFits). The line must not break in that case: the
// container has already been sized for a single line, so breaking leaves its
// items laid out on two lines inside a box only tall enough for one.
//
// The widths swept here put the row's content sum just under 128 and its outer
// width just over it, so adding padding and border crosses a float32 binade and
// the subtraction cannot always recover the original value.
TEST(YogaTest, content_sized_wrap_row_does_not_break_line_on_rounding) {
  for (int step = 0; step < 100; step++) {
    float measuredWidth = 84.0f + static_cast<float>(step) / 100.0f;
    SCOPED_TRACE(
        "measured width " + std::to_string(measuredWidth) + " (step " +
        std::to_string(step) + ")");

    YGNodeRef root = YGNodeNew();
    YGNodeStyleSetWidth(root, 400.0f);
    YGNodeStyleSetHeight(root, 400.0f);

    // Shrink-to-fit in the cross axis, so its width comes from its content.
    YGNodeRef wrapper = YGNodeNew();
    YGNodeStyleSetAlignSelf(wrapper, YGAlignFlexStart);
    YGNodeStyleSetBorder(wrapper, YGEdgeAll, 1.0f);
    YGNodeInsertChild(root, wrapper, 0);

    YGNodeRef row = YGNodeNew();
    YGNodeStyleSetFlexDirection(row, YGFlexDirectionRow);
    YGNodeStyleSetFlexWrap(row, YGWrapWrap);
    YGNodeStyleSetPadding(row, YGEdgeAll, 16.0f);
    YGNodeInsertChild(wrapper, row, 0);

    YGNodeRef measured = YGNodeNew();
    YGNodeSetContext(measured, &measuredWidth);
    YGNodeSetMeasureFunc(measured, measureFractionalWidth);
    YGNodeInsertChild(row, measured, 0);

    YGNodeRef rigid = YGNodeNew();
    YGNodeStyleSetWidth(rigid, 20.0f);
    YGNodeStyleSetHeight(rigid, 20.0f);
    YGNodeInsertChild(row, rigid, 1);

    YGNodeCalculateLayout(root, 400.0f, 400.0f, YGDirectionLTR);

    // Both items fit on one line by construction: the row is content-sized and
    // the root is far wider than the content needs.
    EXPECT_EQ(YGNodeLayoutGetTop(rigid), YGNodeLayoutGetTop(measured));

    // A single line of 20pt content plus 16pt of padding on each side. If the
    // line broke, the items occupy two lines while the box keeps this height.
    EXPECT_EQ(52.0f, YGNodeLayoutGetHeight(row));

    YGNodeFreeRecursive(root);
  }
}
