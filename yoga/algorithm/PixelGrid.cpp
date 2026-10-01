/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <yoga/Yoga.h>

#include <yoga/algorithm/PixelGrid.h>
#include <yoga/numeric/Comparison.h>

namespace facebook::yoga {

// Rounds `value` to the pixel grid and returns the result *in pixel space* (i.e. still multiplied by
// `pointScaleFactor`), where a grid-aligned value is always an exact integer.
//
// Callers that need a difference of two rounded values must subtract in this space rather than convert each
// operand back to points first: `scaledValue / pointScaleFactor` is generally not representable (for a 3x
// screen it almost never is), and narrowing each operand to `float` before subtracting leaks that
// representation error into the result. The error grows with the magnitude of the operands, so for a node far
// down a long scrolling list it becomes large enough to matter — a height of exactly 288.0 points can come
// back as 287.999755859375, which is enough for a text node to lose an entire trailing line when the platform
// text engine checks whether the last line still fits. Subtracting two exact integers first, and narrowing
// once at the end, keeps the returned dimension exactly grid-aligned.
double roundValueToPixelGridScaled(
    const double value,
    const double pointScaleFactor,
    const bool forceCeil,
    const bool forceFloor) {
  double scaledValue = value * pointScaleFactor;
  // We want to calculate `fractional` such that `floor(scaledValue) =
  // scaledValue
  // - fractional`.
  double fractional = fmod(scaledValue, 1.0);
  if (fractional < 0) {
    // This branch is for handling negative numbers for `value`.
    //
    // Regarding `floor` and `ceil`. Note that for a number x, `floor(x) <= x <=
    // ceil(x)` even for negative numbers. Here are a couple of examples:
    //   - x =  2.2: floor( 2.2) =  2, ceil( 2.2) =  3
    //   - x = -2.2: floor(-2.2) = -3, ceil(-2.2) = -2
    //
    // Regarding `fmodf`. For fractional negative numbers, `fmodf` returns a
    // negative number. For example, `fmodf(-2.2) = -0.2`. However, we want
    // `fractional` to be the number such that subtracting it from `value` will
    // give us `floor(value)`. In the case of negative numbers, adding 1 to
    // `fmodf(value)` gives us this. Let's continue the example from above:
    //   - fractional = fmodf(-2.2) = -0.2
    //   - Add 1 to the fraction: fractional2 = fractional + 1 = -0.2 + 1 = 0.8
    //   - Finding the `floor`: -2.2 - fractional2 = -2.2 - 0.8 = -3
    ++fractional;
  }
  if (yoga::inexactEquals(fractional, 0)) {
    // First we check if the value is already rounded
    scaledValue = scaledValue - fractional;
  } else if (yoga::inexactEquals(fractional, 1.0)) {
    scaledValue = scaledValue - fractional + 1.0;
  } else if (forceCeil) {
    // Next we check if we need to use forced rounding
    scaledValue = scaledValue - fractional + 1.0;
  } else if (forceFloor) {
    scaledValue = scaledValue - fractional;
  } else {
    // Finally we just round the value
    scaledValue = scaledValue - fractional +
        (!std::isnan(fractional) &&
                 (fractional > 0.5 || yoga::inexactEquals(fractional, 0.5))
             ? 1.0
             : 0.0);
  }
  return scaledValue;
}

float roundValueToPixelGrid(
    const double value,
    const double pointScaleFactor,
    const bool forceCeil,
    const bool forceFloor) {
  const double scaledValue =
      roundValueToPixelGridScaled(value, pointScaleFactor, forceCeil, forceFloor);
  return (std::isnan(scaledValue) || std::isnan(pointScaleFactor))
      ? YGUndefined
      : (float)(scaledValue / pointScaleFactor);
}

// Converts a pixel-space value produced by `roundValueToPixelGridScaled()` back to points.
static float pixelGridValueToPoints(
    const double scaledValue,
    const double pointScaleFactor) {
  return (std::isnan(scaledValue) || std::isnan(pointScaleFactor))
      ? YGUndefined
      : (float)(scaledValue / pointScaleFactor);
}

void roundLayoutResultsToPixelGrid(
    yoga::Node* const node,
    const double absoluteLeft,
    const double absoluteTop) {
  const auto pointScaleFactor =
      static_cast<double>(node->getConfig()->getPointScaleFactor());

  const double nodeLeft = node->getLayout().position(PhysicalEdge::Left);
  const double nodeTop = node->getLayout().position(PhysicalEdge::Top);

  const double nodeWidth = node->getLayout().dimension(Dimension::Width);
  const double nodeHeight = node->getLayout().dimension(Dimension::Height);

  const double absoluteNodeLeft = absoluteLeft + nodeLeft;
  const double absoluteNodeTop = absoluteTop + nodeTop;

  const double absoluteNodeRight = absoluteNodeLeft + nodeWidth;
  const double absoluteNodeBottom = absoluteNodeTop + nodeHeight;

  if (pointScaleFactor != 0.0) {
    // If a node has a custom measure function we never want to round down its
    // size as this could lead to unwanted text truncation.
    const bool textRounding = node->getNodeType() == NodeType::Text;

    const double scaledLeft =
        roundValueToPixelGridScaled(nodeLeft, pointScaleFactor, false, textRounding);
    const double scaledTop =
        roundValueToPixelGridScaled(nodeTop, pointScaleFactor, false, textRounding);

    node->setLayoutPosition(
        pixelGridValueToPoints(scaledLeft, pointScaleFactor), PhysicalEdge::Left);
    node->setLayoutPosition(
        pixelGridValueToPoints(scaledTop, pointScaleFactor), PhysicalEdge::Top);

    // We multiply dimension by scale factor and if the result is close to the
    // whole number, we don't have any fraction To verify if the result is close
    // to whole number we want to check both floor and ceil numbers

    const double scaledNodeWith = nodeWidth * pointScaleFactor;
    const bool hasFractionalWidth =
        !yoga::inexactEquals(round(scaledNodeWith), scaledNodeWith);

    const double scaledNodeHeight = nodeHeight * pointScaleFactor;
    const bool hasFractionalHeight =
        !yoga::inexactEquals(round(scaledNodeHeight), scaledNodeHeight);

    // The dimensions are derived as the difference of two rounded absolute edges. Both operands are exact
    // integers in pixel space, so subtracting there and narrowing once yields an exactly grid-aligned
    // dimension; converting each edge back to points first and subtracting in `float` would not.
    const double scaledAbsoluteLeft =
        roundValueToPixelGridScaled(absoluteNodeLeft, pointScaleFactor, false, textRounding);
    const double scaledAbsoluteRight = roundValueToPixelGridScaled(
        absoluteNodeRight,
        pointScaleFactor,
        (textRounding && hasFractionalWidth),
        (textRounding && !hasFractionalWidth));

    const double scaledAbsoluteTop =
        roundValueToPixelGridScaled(absoluteNodeTop, pointScaleFactor, false, textRounding);
    const double scaledAbsoluteBottom = roundValueToPixelGridScaled(
        absoluteNodeBottom,
        pointScaleFactor,
        (textRounding && hasFractionalHeight),
        (textRounding && !hasFractionalHeight));

    node->getLayout().setDimension(
        Dimension::Width,
        pixelGridValueToPoints(scaledAbsoluteRight - scaledAbsoluteLeft, pointScaleFactor));

    node->getLayout().setDimension(
        Dimension::Height,
        pixelGridValueToPoints(scaledAbsoluteBottom - scaledAbsoluteTop, pointScaleFactor));
  }

  for (yoga::Node* child : node->getChildren()) {
    if (child->getOwner() != node) {
      continue;
    }
    roundLayoutResultsToPixelGrid(child, absoluteNodeLeft, absoluteNodeTop);
  }
}

} // namespace facebook::yoga
