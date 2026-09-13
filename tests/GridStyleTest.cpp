/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/style/GridStyle.h>
#include <yoga/style/Style.h>

namespace facebook::yoga {

// GridStyleStorage must cost a node the same as a plain pointer when it is not
// a grid container or item.
static_assert(sizeof(GridStyleStorage) == sizeof(void*));

TEST(GridStyle, unset_grid_style_reads_as_the_defaults) {
  Style style;

  ASSERT_TRUE(style.gridTemplateColumns().empty());
  ASSERT_TRUE(style.gridAutoRows().empty());
  ASSERT_EQ(style.gridColumnStart(), GridLine::auto_());
  ASSERT_EQ(style.gridRowEnd(), GridLine::auto_());
}

TEST(GridStyle, unset_grid_style_equals_grid_style_set_to_the_defaults) {
  Style unset;

  Style setToDefaults;
  setToDefaults.setGridColumnStart(GridLine::auto_());

  ASSERT_TRUE(unset == setToDefaults);
  ASSERT_TRUE(setToDefaults == unset);
}

TEST(GridStyle, styles_with_different_grid_values_are_not_equal) {
  Style style;
  Style withColumnStart;
  withColumnStart.setGridColumnStart(GridLine::fromInteger(2));

  ASSERT_FALSE(style == withColumnStart);
  ASSERT_FALSE(withColumnStart == style);
}

TEST(GridStyle, copies_do_not_share_grid_style) {
  Style style;
  style.setGridTemplateColumns(GridTrackList{GridTrackSize::length(10.0f)});
  style.setGridRowEnd(GridLine::span(3));

  Style copy = style;

  ASSERT_TRUE(copy == style);
  ASSERT_EQ(copy.gridTemplateColumns()[0], GridTrackSize::length(10.0f));
  ASSERT_EQ(copy.gridRowEnd(), GridLine::span(3));

  copy.setGridTemplateColumnAt(0, GridTrackSize::length(20.0f));

  ASSERT_EQ(style.gridTemplateColumns()[0], GridTrackSize::length(10.0f));
  ASSERT_FALSE(copy == style);
}

TEST(GridStyle, moving_a_style_hands_over_the_grid_style) {
  Style style;
  style.setGridRowEnd(GridLine::span(3));

  Style moved = std::move(style);
  ASSERT_EQ(moved.gridRowEnd(), GridLine::span(3));
  // NOLINTNEXTLINE(bugprone-use-after-move): intentionally verifying the
  // moved-from style is reset to its default state.
  ASSERT_EQ(style.gridRowEnd(), GridLine::auto_());

  Style target;
  target = std::move(moved);
  ASSERT_EQ(target.gridRowEnd(), GridLine::span(3));
  // NOLINTNEXTLINE(bugprone-use-after-move): intentionally verifying the
  // moved-from style is reset to its default state.
  ASSERT_EQ(moved.gridRowEnd(), GridLine::auto_());
}

TEST(GridStyle, assigning_an_unset_style_clears_a_set_one) {
  Style style;
  style.setGridTemplateColumns(GridTrackList{GridTrackSize::length(10.0f)});
  style.setGridRowStart(GridLine::span(2));

  style = Style{};

  ASSERT_TRUE(style.gridTemplateColumns().empty());
  ASSERT_EQ(style.gridRowStart(), GridLine::auto_());
}

TEST(GridStyle, tracks_can_be_sized_then_filled) {
  Style style;
  style.resizeGridTemplateColumns(2);
  style.setGridTemplateColumnAt(0, GridTrackSize::fr(1.0f));
  style.setGridTemplateColumnAt(1, GridTrackSize::percent(50.0f));

  ASSERT_EQ(style.gridTemplateColumns().size(), 2u);
  ASSERT_EQ(style.gridTemplateColumns()[0], GridTrackSize::fr(1.0f));
  ASSERT_EQ(style.gridTemplateColumns()[1], GridTrackSize::percent(50.0f));
}

} // namespace facebook::yoga
