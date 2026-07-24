/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/algorithm/grid/LinePlacement.h>

namespace facebook::yoga {

namespace {
// explicitLineCount is the number of grid lines in the explicit grid, i.e. the
// track count + 1. The examples below assume a 3-column grid (4 lines).
constexpr int32_t kThreeColumnLineCount = 4;

void expectPlacement(
    const LinePlacement& placement,
    int32_t start,
    int32_t end,
    int32_t span) {
  EXPECT_EQ(placement.start, start);
  EXPECT_EQ(placement.end, end);
  EXPECT_EQ(placement.span, span);
}
} // namespace

// grid-column: 1 / 3 -> tracks [0, 2)
TEST(LinePlacement, two_definite_lines) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::fromInteger(1),
          GridLine::fromInteger(3),
          kThreeColumnLineCount),
      0,
      2,
      2);
}

// grid-column: 3 / 1 -> reversed lines are swapped
TEST(LinePlacement, reversed_lines_are_swapped) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::fromInteger(3),
          GridLine::fromInteger(1),
          kThreeColumnLineCount),
      0,
      2,
      2);
}

// grid-column: 2 / 2 -> equal lines, end line is dropped, spans one track
TEST(LinePlacement, equal_lines_span_one_track) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::fromInteger(2),
          GridLine::fromInteger(2),
          kThreeColumnLineCount),
      1,
      2,
      1);
}

// grid-column: 2 / span 2
TEST(LinePlacement, integer_start_with_span_end) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::fromInteger(2), GridLine::span(2), kThreeColumnLineCount),
      1,
      3,
      2);
}

// grid-column: span 2 / 3
TEST(LinePlacement, span_start_with_integer_end) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::span(2), GridLine::fromInteger(3), kThreeColumnLineCount),
      0,
      2,
      2);
}

// grid-column: -1 -> the last line of a 3-column grid (line 4 -> index 3)
TEST(LinePlacement, negative_line_counts_from_end) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::fromInteger(-1),
          GridLine::auto_(),
          kThreeColumnLineCount),
      3,
      4,
      1);
}

// grid-column: -5 / -4 -> resolves to the implicit track [-1, 0)
TEST(LinePlacement, negative_range_before_explicit_grid) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::fromInteger(-5),
          GridLine::fromInteger(-4),
          kThreeColumnLineCount),
      -1,
      0,
      1);
}

// grid-column: auto -> placeholder, resolved later by auto-placement
TEST(LinePlacement, auto_is_a_placeholder) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::auto_(), GridLine::auto_(), kThreeColumnLineCount),
      -1,
      -1,
      1);
}

// grid-column: span 0 is invalid; a span must be at least 1. The -1/-1 result
// is the auto placeholder, so the item is auto-placed downstream (span 1).
TEST(LinePlacement, zero_span_clamps_to_one) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::span(0), GridLine::auto_(), kThreeColumnLineCount),
      -1,
      -1,
      1);
}

// grid-column: span -2 is invalid; a span must be at least 1. The -1/-1 result
// is the auto placeholder, so the item is auto-placed downstream (span 1).
TEST(LinePlacement, negative_span_clamps_to_one) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::span(-2), GridLine::auto_(), kThreeColumnLineCount),
      -1,
      -1,
      1);
}

// grid-column: 0 is an invalid line; it resolves to the auto placeholder
// (-1/-1), so the item is auto-placed downstream.
TEST(LinePlacement, line_zero_is_treated_as_auto) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::fromInteger(0),
          GridLine::auto_(),
          kThreeColumnLineCount),
      -1,
      -1,
      1);
}

// grid-column: 0 / 2 -> the invalid 0 start becomes auto, leaving a definite end
// at line 2, so the item resolves to track [0, 1).
TEST(LinePlacement, line_zero_start_with_definite_end) {
  expectPlacement(
      resolveLinePlacement(
          GridLine::fromInteger(0),
          GridLine::fromInteger(2),
          kThreeColumnLineCount),
      0,
      1,
      1);
}

} // namespace facebook::yoga
