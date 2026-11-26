/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>
#include <yoga/algorithm/grid/AutoPlacement.h>
#include <yoga/node/Node.h>
#include <yoga/style/GridTrackSize.h>
#include <yoga/style/StyleLength.h>

using namespace facebook::yoga;

class GridAutoplacementTest : public ::testing::Test {
 protected:
  void SetUp() override {
    gridContainer = new Node();
    gridContainer->style().setDisplay(Display::Grid);
  }

  void TearDown() override {
    delete gridContainer;
  }

  Node* gridContainer{nullptr};

  Node* createGridItem(
      GridLine columnStart = GridLine::auto_(),
      GridLine columnEnd = GridLine::auto_(),
      GridLine rowStart = GridLine::auto_(),
      GridLine rowEnd = GridLine::auto_()) {
    auto item = new Node();
    item->style().setGridColumnStart(columnStart);
    item->style().setGridColumnEnd(columnEnd);
    item->style().setGridRowStart(rowStart);
    item->style().setGridRowEnd(rowEnd);
    gridContainer->insertChild(item, gridContainer->getChildren().size());
    return item;
  }
};

TEST_F(GridAutoplacementTest, places_items_with_definite_positions) {
  createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(3),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));
  createGridItem(
      GridLine::fromInteger(2),
      GridLine::fromInteger(4),
      GridLine::fromInteger(2),
      GridLine::fromInteger(3));

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;
  ASSERT_EQ(placements.size(), 2);

  EXPECT_EQ(placements[0].columnStart, 0);
  EXPECT_EQ(placements[0].columnEnd, 2);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);

  EXPECT_EQ(placements[1].columnStart, 1);
  EXPECT_EQ(placements[1].columnEnd, 3);
  EXPECT_EQ(placements[1].rowStart, 1);
  EXPECT_EQ(placements[1].rowEnd, 2);
}

TEST_F(GridAutoplacementTest, places_items_with_definite_row_auto_column) {
  createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(3),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  createGridItem(
      GridLine::auto_(),
      GridLine::auto_(),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;
  ASSERT_EQ(placements.size(), 2);

  EXPECT_EQ(placements[0].columnStart, 0);
  EXPECT_EQ(placements[0].columnEnd, 2);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);
  EXPECT_EQ(placements[1].columnStart, 2);
  EXPECT_EQ(placements[1].columnEnd, 3);
  EXPECT_EQ(placements[1].rowStart, 0);
  EXPECT_EQ(placements[1].rowEnd, 1);
}

TEST_F(GridAutoplacementTest, handles_overlapping_definite_row_items) {
  createGridItem(
      GridLine::auto_(),
      GridLine::span(2),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));
  createGridItem(
      GridLine::auto_(),
      GridLine::span(2),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 2);

  EXPECT_EQ(placements[0].columnStart, 0);
  EXPECT_EQ(placements[0].columnEnd, 2);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);

  EXPECT_EQ(placements[1].columnStart, 2);
  EXPECT_EQ(placements[1].columnEnd, 4);
  EXPECT_EQ(placements[1].rowStart, 0);
  EXPECT_EQ(placements[1].rowEnd, 1);
}

TEST_F(GridAutoplacementTest, places_auto_positioned_items) {
  createGridItem();
  createGridItem();

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 2);

  EXPECT_EQ(placements[0].columnStart, 0);
  EXPECT_EQ(placements[0].columnEnd, 1);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);

  EXPECT_EQ(placements[1].columnStart, 0);
  EXPECT_EQ(placements[1].columnEnd, 1);
  EXPECT_EQ(placements[1].rowStart, 1);
  EXPECT_EQ(placements[1].rowEnd, 2);
}

TEST_F(GridAutoplacementTest, handles_large_spans) {
  createGridItem(GridLine::auto_(), GridLine::span(5));

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 1);

  EXPECT_EQ(placements[0].columnStart, 0);
  EXPECT_EQ(placements[0].columnEnd, 5);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);
}

TEST_F(GridAutoplacementTest, places_items_with_definite_column_auto_row) {
  createGridItem(
      GridLine::fromInteger(2),
      GridLine::fromInteger(4),
      GridLine::auto_(),
      GridLine::auto_());

  createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(2),
      GridLine::auto_(),
      GridLine::auto_());

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 2);

  EXPECT_EQ(placements[0].columnStart, 1);
  EXPECT_EQ(placements[0].columnEnd, 3);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);

  EXPECT_EQ(placements[1].columnStart, 0);
  EXPECT_EQ(placements[1].columnEnd, 1);
  EXPECT_EQ(placements[1].rowStart, 1);
  EXPECT_EQ(placements[1].rowEnd, 2);
}

TEST_F(GridAutoplacementTest, avoids_overlaps_with_definite_column_items) {
  createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(3),
      GridLine::fromInteger(1),
      GridLine::fromInteger(3));

  createGridItem(
      GridLine::fromInteger(2),
      GridLine::fromInteger(3),
      GridLine::auto_(),
      GridLine::auto_());

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 2);

  EXPECT_EQ(placements[0].columnStart, 0);
  EXPECT_EQ(placements[0].columnEnd, 2);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 2);

  EXPECT_EQ(placements[1].columnStart, 1);
  EXPECT_EQ(placements[1].columnEnd, 2);
  EXPECT_EQ(placements[1].rowStart, 2);
  EXPECT_EQ(placements[1].rowEnd, 3);
}

TEST_F(GridAutoplacementTest, handles_mixed_positioning_strategies) {
  createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(2),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  createGridItem(
      GridLine::auto_(),
      GridLine::span(2),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  createGridItem();

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 3);

  EXPECT_EQ(placements[0].columnStart, 0);
  EXPECT_EQ(placements[0].columnEnd, 1);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);

  EXPECT_EQ(placements[1].columnStart, 1);
  EXPECT_EQ(placements[1].columnEnd, 3);
  EXPECT_EQ(placements[1].rowStart, 0);
  EXPECT_EQ(placements[1].rowEnd, 1);

  EXPECT_EQ(placements[2].columnStart, 0);
  EXPECT_EQ(placements[2].columnEnd, 1);
  EXPECT_EQ(placements[2].rowStart, 1);
  EXPECT_EQ(placements[2].rowEnd, 2);
}

TEST_F(GridAutoplacementTest, handles_negative_grid_line_references_simple) {
  std::vector<GridTrackSize> columns = {
      GridTrackSize::length(20.0f),
      GridTrackSize::length(20.0f),
      GridTrackSize::length(20.0f)};
  gridContainer->style().setGridTemplateColumns(std::move(columns));
  createGridItem(GridLine::fromInteger(-1), GridLine::fromInteger(-2));

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 1);
  EXPECT_EQ(placements[0].columnStart, 2);
  EXPECT_EQ(placements[0].columnEnd, 3);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);
}

TEST_F(GridAutoplacementTest, handles_negative_grid_line_references) {
  std::vector<GridTrackSize> columns = {
      GridTrackSize::length(20.0f),
      GridTrackSize::length(20.0f),
      GridTrackSize::length(20.0f)};
  gridContainer->style().setGridTemplateColumns(std::move(columns));

  // .one { grid-column-start: -5; grid-column-end: -4; }
  auto node1 =
      createGridItem(GridLine::fromInteger(-5), GridLine::fromInteger(-4));

  // .two { grid-column-start: 1; grid-column-end: 2; grid-row-start: 1;
  // grid-row-end: 2; }
  auto node2 = createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(2),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  auto node3 = createGridItem();
  auto node4 = createGridItem();
  auto node5 = createGridItem();

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 5);

  for (auto placement : placements) {
    if (placement.node == node1) {
      EXPECT_EQ(placement.columnStart, -1);
      EXPECT_EQ(placement.columnEnd, 0);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node2) {
      EXPECT_EQ(placement.columnStart, 0);
      EXPECT_EQ(placement.columnEnd, 1);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node3) {
      EXPECT_EQ(placement.columnStart, 1);
      EXPECT_EQ(placement.columnEnd, 2);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node4) {
      EXPECT_EQ(placement.columnStart, 2);
      EXPECT_EQ(placement.columnEnd, 3);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node5) {
      EXPECT_EQ(placement.columnStart, -1);
      EXPECT_EQ(placement.columnEnd, 0);
      EXPECT_EQ(placement.rowStart, 1);
      EXPECT_EQ(placement.rowEnd, 2);
    }
  }
}

TEST_F(GridAutoplacementTest, same_start_and_end_line_spans_one_track) {
  createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(1),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 1);
  EXPECT_EQ(placements[0].columnStart, 0);
  EXPECT_EQ(placements[0].columnEnd, 1);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);
}

TEST_F(
    GridAutoplacementTest,
    handles_negative_grid_lines_with_row_positioning) {
  std::vector<GridTrackSize> columns = {
      GridTrackSize::length(20.0f),
      GridTrackSize::length(20.0f),
      GridTrackSize::length(20.0f)};
  gridContainer->style().setGridTemplateColumns(std::move(columns));

  // .one { grid-column-start: -5; grid-column-end: -4; grid-row-start: 2;
  // grid-row-end: 3; }
  auto node1 = createGridItem(
      GridLine::fromInteger(-5),
      GridLine::fromInteger(-4),
      GridLine::fromInteger(2),
      GridLine::fromInteger(3));

  // .two { grid-row-start: 1; grid-row-end: 2; }
  auto node2 = createGridItem(
      GridLine::auto_(),
      GridLine::auto_(),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  auto node3 = createGridItem();
  auto node4 = createGridItem();
  auto node5 = createGridItem();

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 5);

  for (auto placement : placements) {
    if (placement.node == node1) {
      EXPECT_EQ(placement.columnStart, -1);
      EXPECT_EQ(placement.columnEnd, 0);
      EXPECT_EQ(placement.rowStart, 1);
      EXPECT_EQ(placement.rowEnd, 2);
    } else if (placement.node == node2) {
      EXPECT_EQ(placement.columnStart, -1);
      EXPECT_EQ(placement.columnEnd, 0);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node3) {
      EXPECT_EQ(placement.columnStart, 0);
      EXPECT_EQ(placement.columnEnd, 1);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node4) {
      EXPECT_EQ(placement.columnStart, 1);
      EXPECT_EQ(placement.columnEnd, 2);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node5) {
      EXPECT_EQ(placement.columnStart, 2);
      EXPECT_EQ(placement.columnEnd, 3);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    }
  }
}

TEST_F(GridAutoplacementTest, skips_past_large_blocking_items) {
  auto largeItem = createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(6),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  auto autoItem1 = createGridItem(
      GridLine::auto_(),
      GridLine::auto_(),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));
  auto autoItem2 = createGridItem(
      GridLine::auto_(),
      GridLine::auto_(),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 3);

  for (auto placement : placements) {
    if (placement.node == largeItem) {
      EXPECT_EQ(placement.columnStart, 0);
      EXPECT_EQ(placement.columnEnd, 5);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == autoItem1) {
      EXPECT_EQ(placement.columnStart, 5);
      EXPECT_EQ(placement.columnEnd, 6);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == autoItem2) {
      EXPECT_EQ(placement.columnStart, 6);
      EXPECT_EQ(placement.columnEnd, 7);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    }
  }
}

TEST_F(GridAutoplacementTest, skips_past_large_blocking_rows) {
  createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(2),
      GridLine::fromInteger(1),
      GridLine::fromInteger(6));

  auto autoItem = createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(2),
      GridLine::auto_(),
      GridLine::auto_());

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 2);

  for (auto placement : placements) {
    if (placement.node == autoItem) {
      EXPECT_EQ(placement.columnStart, 0);
      EXPECT_EQ(placement.columnEnd, 1);
      EXPECT_EQ(placement.rowStart, 5);
      EXPECT_EQ(placement.rowEnd, 6);
    }
  }
}

TEST_F(GridAutoplacementTest, handles_nested_overlapping_items) {
  createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(8),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  createGridItem(
      GridLine::fromInteger(3),
      GridLine::fromInteger(5),
      GridLine::fromInteger(2),
      GridLine::fromInteger(3));

  auto autoItem = createGridItem(
      GridLine::auto_(),
      GridLine::auto_(),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 3);

  for (auto placement : placements) {
    if (placement.node == autoItem) {
      EXPECT_EQ(placement.columnStart, 7);
      EXPECT_EQ(placement.columnEnd, 8);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    }
  }
}

TEST_F(
    GridAutoplacementTest,
    handles_negative_and_positive_grid_lines_auto_row) {
  std::vector<GridTrackSize> columns = {
      GridTrackSize::length(20.0f),
      GridTrackSize::length(20.0f),
      GridTrackSize::length(20.0f)};
  gridContainer->style().setGridTemplateColumns(std::move(columns));

  // .one { grid-column-start: -5; grid-column-end: -4; grid-row-start: 1;
  // grid-row-end: 2; }
  auto node1 = createGridItem(
      GridLine::fromInteger(-5),
      GridLine::fromInteger(-4),
      GridLine::fromInteger(1),
      GridLine::fromInteger(2));

  // .two { grid-column-start: 1; grid-column-end: 2; } (auto rows)
  auto node2 = createGridItem(
      GridLine::fromInteger(1),
      GridLine::fromInteger(2),
      GridLine::auto_(),
      GridLine::auto_());

  auto node3 = createGridItem();
  auto node4 = createGridItem();
  auto node5 = createGridItem();

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;
  ASSERT_EQ(placements.size(), 5);

  for (auto placement : placements) {
    if (placement.node == node1) {
      EXPECT_EQ(placement.columnStart, -1);
      EXPECT_EQ(placement.columnEnd, 0);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node2) {
      EXPECT_EQ(placement.columnStart, 0);
      EXPECT_EQ(placement.columnEnd, 1);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node3) {
      EXPECT_EQ(placement.columnStart, 1);
      EXPECT_EQ(placement.columnEnd, 2);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node4) {
      EXPECT_EQ(placement.columnStart, 2);
      EXPECT_EQ(placement.columnEnd, 3);
      EXPECT_EQ(placement.rowStart, 0);
      EXPECT_EQ(placement.rowEnd, 1);
    } else if (placement.node == node5) {
      EXPECT_EQ(placement.columnStart, -1);
      EXPECT_EQ(placement.columnEnd, 0);
      EXPECT_EQ(placement.rowStart, 1);
      EXPECT_EQ(placement.rowEnd, 2);
    }
  }
}

// grid-column-start: 0 is an invalid line and is treated as auto, so the item
// auto-places at the first cell rather than at an implicit negative column.
TEST_F(GridAutoplacementTest, grid_line_zero_is_treated_as_auto) {
  createGridItem(GridLine::fromInteger(0), GridLine::auto_());

  auto autoPlacementResult = AutoPlacement::performAutoPlacement(gridContainer);
  auto& placements = autoPlacementResult.gridItems;

  ASSERT_EQ(placements.size(), 1);
  EXPECT_EQ(placements[0].columnStart, 0);
  EXPECT_EQ(placements[0].columnEnd, 1);
  EXPECT_EQ(placements[0].rowStart, 0);
  EXPECT_EQ(placements[0].rowEnd, 1);
}
