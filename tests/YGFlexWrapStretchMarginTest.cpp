/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

// When a wrapping container stretches its items to the cross size of their
// line, it lays each item out again with its outer size. On the main axis that
// outer size is the item's measured main size plus its main-axis margins, which
// the item then subtracts again. In a column the main axis is vertical, so the
// height handed to the item must include its vertical margins, not its
// horizontal ones. Expected values match Chrome.

namespace {

YGNodeRef createWrappingColumn(YGConfigRef config, YGAlign alignContent) {
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root, YGPositionTypeAbsolute);
  YGNodeStyleSetFlexWrap(root, YGWrapWrap);
  YGNodeStyleSetAlignContent(root, alignContent);
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 300);
  return root;
}

YGNodeRef insertContentSizedItem(YGConfigRef config, YGNodeRef root) {
  YGNodeRef item = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root, item, YGNodeGetChildCount(root));

  YGNodeRef content = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(content, 50);
  YGNodeStyleSetHeight(content, 50);
  YGNodeInsertChild(item, content, 0);
  return item;
}

} // namespace

TEST(YogaTest, wrap_column_stretch_item_with_horizontal_margin_keeps_height) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = createWrappingColumn(config, YGAlignFlexStart);
  YGNodeRef item = insertContentSizedItem(config, root);
  YGNodeStyleSetMargin(item, YGEdgeLeft, 10);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetLeft(item));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(item));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(item));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(item));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, wrap_column_stretch_item_with_vertical_margin_keeps_height) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = createWrappingColumn(config, YGAlignFlexStart);
  YGNodeRef item = insertContentSizedItem(config, root);
  YGNodeStyleSetMargin(item, YGEdgeLeft, 5);
  YGNodeStyleSetMargin(item, YGEdgeTop, 20);
  YGNodeStyleSetMargin(item, YGEdgeBottom, 10);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(5, YGNodeLayoutGetLeft(item));
  ASSERT_FLOAT_EQ(20, YGNodeLayoutGetTop(item));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(item));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(item));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, wrap_column_align_content_stretch_item_keeps_height) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = createWrappingColumn(config, YGAlignStretch);
  YGNodeRef item = insertContentSizedItem(config, root);
  YGNodeStyleSetMargin(item, YGEdgeHorizontal, 10);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetLeft(item));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(item));
  ASSERT_FLOAT_EQ(180, YGNodeLayoutGetWidth(item));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(item));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaTest, wrap_column_stretch_empty_item_with_horizontal_margin) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = createWrappingColumn(config, YGAlignFlexStart);
  YGNodeRef item = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(item, YGEdgeRight, 9);
  YGNodeInsertChild(root, item, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(item));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(item));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetWidth(item));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetHeight(item));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
