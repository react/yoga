/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

TEST(YogaTest, grid_template_column_index_in_range) {
  YGNodeRef const node = YGNodeNew();
  YGNodeStyleSetGridTemplateColumnsCount(node, 2);

  YGNodeStyleSetGridTemplateColumn(node, 1, YGGridTrackTypePoints, 50.0f);
  YGNodeStyleSetGridTemplateColumnMinMax(
      node, 0, YGGridTrackTypePoints, 10.0f, YGGridTrackTypeFr, 1.0f);

  YGNodeFree(node);
}

TEST(YogaTest, grid_template_column_index_out_of_range_throws) {
  YGNodeRef const node = YGNodeNew();
  YGNodeStyleSetGridTemplateColumnsCount(node, 1);

  ASSERT_THROW(
      YGNodeStyleSetGridTemplateColumn(node, 1, YGGridTrackTypePoints, 50.0f),
      std::logic_error);
  ASSERT_THROW(
      YGNodeStyleSetGridTemplateColumnMinMax(
          node, 4, YGGridTrackTypePoints, 10.0f, YGGridTrackTypeFr, 1.0f),
      std::logic_error);

  YGNodeFree(node);
}

TEST(YogaTest, grid_template_row_index_out_of_range_throws) {
  YGNodeRef const node = YGNodeNew();
  YGNodeStyleSetGridTemplateRowsCount(node, 1);

  ASSERT_THROW(
      YGNodeStyleSetGridTemplateRow(node, 2, YGGridTrackTypePoints, 50.0f),
      std::logic_error);
  ASSERT_THROW(
      YGNodeStyleSetGridTemplateRowMinMax(
          node, 9, YGGridTrackTypePoints, 10.0f, YGGridTrackTypeFr, 1.0f),
      std::logic_error);

  YGNodeFree(node);
}

TEST(YogaTest, grid_auto_column_index_out_of_range_throws) {
  YGNodeRef const node = YGNodeNew();
  YGNodeStyleSetGridAutoColumnsCount(node, 1);

  ASSERT_THROW(
      YGNodeStyleSetGridAutoColumn(node, 3, YGGridTrackTypePoints, 50.0f),
      std::logic_error);
  ASSERT_THROW(
      YGNodeStyleSetGridAutoColumnMinMax(
          node, 7, YGGridTrackTypePoints, 10.0f, YGGridTrackTypeFr, 1.0f),
      std::logic_error);

  YGNodeFree(node);
}

TEST(YogaTest, grid_auto_row_index_out_of_range_throws) {
  YGNodeRef const node = YGNodeNew();
  YGNodeStyleSetGridAutoRowsCount(node, 1);

  ASSERT_THROW(
      YGNodeStyleSetGridAutoRow(node, 5, YGGridTrackTypePoints, 50.0f),
      std::logic_error);
  ASSERT_THROW(
      YGNodeStyleSetGridAutoRowMinMax(
          node, 6, YGGridTrackTypePoints, 10.0f, YGGridTrackTypeFr, 1.0f),
      std::logic_error);

  YGNodeFree(node);
}
