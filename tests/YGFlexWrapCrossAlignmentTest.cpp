/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

#include <array>
#include <initializer_list>

namespace {

struct Box {
  float left;
  float top;
  float width;
  float height;
};

struct Expected {
  YGAlign align;
  Box ltr;
  Box rtl;
};

// A new config with or without the erratum that keeps the cross positions the
// multi-line alignment pass gave before it honored margins and relative insets.
// New configs already carry it; setting it explicitly keeps these tests on the
// intended behavior even if that default changes.
YGConfigRef newConfig(bool withErratum) {
  YGConfigRef config = YGConfigNew();
  const int errata = YGConfigGetErrata(config);
  const int bit = YGErrataWrapCrossAlignmentIgnoresMarginAndInset;
  YGConfigSetErrata(
      config,
      static_cast<YGErrata>(withErratum ? errata | bit : errata & ~bit));
  return config;
}

// Lays out a wrapping container with two flex lines and returns the box of
// the item on the second line. That item has a margin of 5 on every edge and a
// relative inset of 10 on its left and top edges. Its sibling makes the second
// line 60 thick in the cross axis, and the first line is 30 thick. With
// align-items: stretch the item has no cross size of its own.
Box layoutItemOnSecondLine(
    YGFlexDirection flexDirection,
    YGWrap wrap,
    YGAlign alignItems,
    YGDirection direction,
    bool withErratum) {
  const bool isRow = flexDirection == YGFlexDirectionRow;
  const bool stretch = alignItems == YGAlignStretch;

  YGConfigRef config = newConfig(withErratum);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, flexDirection);
  YGNodeStyleSetFlexWrap(root, wrap);
  YGNodeStyleSetAlignContent(root, YGAlignFlexStart);
  YGNodeStyleSetAlignItems(root, alignItems);
  YGNodeStyleSetPadding(root, YGEdgeAll, 10);
  YGNodeStyleSetWidth(root, isRow ? 150 : 200);
  YGNodeStyleSetHeight(root, isRow ? 200 : 150);

  YGNodeRef first = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(first, isRow ? 100 : 30);
  YGNodeStyleSetHeight(first, isRow ? 30 : 100);
  YGNodeInsertChild(root, first, 0);

  YGNodeRef item = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(item, YGEdgeAll, 5);
  YGNodeStyleSetPosition(item, YGEdgeLeft, 10);
  YGNodeStyleSetPosition(item, YGEdgeTop, 10);
  if (isRow) {
    YGNodeStyleSetWidth(item, 50);
    if (!stretch) {
      YGNodeStyleSetHeight(item, 20);
    }
  } else {
    YGNodeStyleSetHeight(item, 50);
    if (!stretch) {
      YGNodeStyleSetWidth(item, 20);
    }
  }
  YGNodeInsertChild(root, item, 1);

  YGNodeRef sibling = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(sibling, isRow ? 40 : 60);
  YGNodeStyleSetHeight(sibling, isRow ? 60 : 40);
  YGNodeInsertChild(root, sibling, 2);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, direction);

  const Box box{
      YGNodeLayoutGetLeft(item),
      YGNodeLayoutGetTop(item),
      YGNodeLayoutGetWidth(item),
      YGNodeLayoutGetHeight(item)};

  YGNodeFreeRecursive(root);
  YGConfigFree(config);

  return box;
}

void expectBox(const Box& actual, const Box& expected) {
  EXPECT_FLOAT_EQ(expected.left, actual.left);
  EXPECT_FLOAT_EQ(expected.top, actual.top);
  EXPECT_FLOAT_EQ(expected.width, actual.width);
  EXPECT_FLOAT_EQ(expected.height, actual.height);
}

void checkAllAlignments(
    YGFlexDirection flexDirection,
    YGWrap wrap,
    const std::initializer_list<Expected>& cases,
    bool withErratum) {
  for (const auto& expected : cases) {
    SCOPED_TRACE(YGAlignToString(expected.align));
    {
      SCOPED_TRACE("LTR");
      expectBox(
          layoutItemOnSecondLine(
              flexDirection, wrap, expected.align, YGDirectionLTR, withErratum),
          expected.ltr);
    }
    {
      SCOPED_TRACE("RTL");
      expectBox(
          layoutItemOnSecondLine(
              flexDirection, wrap, expected.align, YGDirectionRTL, withErratum),
          expected.rtl);
    }
  }
}

// Lays out a wrap-reverse container with one line, 40 thick in the cross
// axis and packed against the cross start, which is the bottom of a row and
// the right of an LTR column. The item comes after a sibling and has a margin
// of 10 on one edge only.
Box layoutWrapReverseItemWithOneSidedMargin(
    YGFlexDirection flexDirection,
    YGEdge marginEdge,
    YGAlign alignItems,
    YGDirection direction,
    bool withErratum) {
  const bool isRow = flexDirection == YGFlexDirectionRow;
  const bool stretch = alignItems == YGAlignStretch;

  YGConfigRef config = newConfig(withErratum);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, flexDirection);
  YGNodeStyleSetFlexWrap(root, YGWrapWrapReverse);
  YGNodeStyleSetAlignContent(root, YGAlignFlexStart);
  YGNodeStyleSetAlignItems(root, alignItems);
  YGNodeStyleSetWidth(root, isRow ? 200 : 100);
  YGNodeStyleSetHeight(root, isRow ? 100 : 200);

  YGNodeRef sibling = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(sibling, isRow ? 50 : 40);
  YGNodeStyleSetHeight(sibling, isRow ? 40 : 50);
  YGNodeInsertChild(root, sibling, 0);

  YGNodeRef item = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(item, marginEdge, 10);
  if (isRow) {
    YGNodeStyleSetWidth(item, 50);
    if (!stretch) {
      YGNodeStyleSetHeight(item, 20);
    }
  } else {
    YGNodeStyleSetHeight(item, 50);
    if (!stretch) {
      YGNodeStyleSetWidth(item, 20);
    }
  }
  YGNodeInsertChild(root, item, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, direction);

  const Box box{
      YGNodeLayoutGetLeft(item),
      YGNodeLayoutGetTop(item),
      YGNodeLayoutGetWidth(item),
      YGNodeLayoutGetHeight(item)};

  YGNodeFreeRecursive(root);
  YGConfigFree(config);

  return box;
}

struct AlignedBox {
  YGAlign align;
  Box box;
};

void checkOneSidedMargin(
    YGFlexDirection flexDirection,
    YGEdge marginEdge,
    YGDirection direction,
    const std::initializer_list<AlignedBox>& cases,
    bool withErratum) {
  for (const auto& expected : cases) {
    SCOPED_TRACE(YGAlignToString(expected.align));
    expectBox(
        layoutWrapReverseItemWithOneSidedMargin(
            flexDirection, marginEdge, expected.align, direction, withErratum),
        expected.box);
  }
}

// A row 100 wide with three 40x30 items that have a margin of 4: two items on
// the first line and one on the second. Returns the top of each item.
std::array<float, 3> layoutWrappingRowWithMargins(YGErrata errata) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetErrata(config, errata);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetFlexWrap(root, YGWrapWrap);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);

  for (size_t i = 0; i < 3; i++) {
    YGNodeRef child = YGNodeNewWithConfig(config);
    YGNodeStyleSetMargin(child, YGEdgeAll, 4);
    YGNodeStyleSetWidth(child, 40);
    YGNodeStyleSetHeight(child, 30);
    YGNodeInsertChild(root, child, i);
  }

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  const std::array<float, 3> tops{
      YGNodeLayoutGetTop(YGNodeGetChild(root, 0)),
      YGNodeLayoutGetTop(YGNodeGetChild(root, 1)),
      YGNodeLayoutGetTop(YGNodeGetChild(root, 2))};

  YGNodeFreeRecursive(root);
  YGConfigFree(config);

  return tops;
}

void expectTops(
    const std::array<float, 3>& actual,
    const std::array<float, 3>& expected) {
  for (size_t i = 0; i < 3; i++) {
    SCOPED_TRACE(i);
    EXPECT_FLOAT_EQ(expected[i], actual[i]);
  }
}

// https://github.com/react/yoga/issues/1067: a centered item in a wrapping
// row with a margin of 100 on its left and top edges only.
void checkCenteredItemWithOneSidedMargins(bool withErratum, float expectedTop) {
  YGConfigRef config = newConfig(withErratum);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetFlexWrap(root, YGWrapWrap);
  YGNodeStyleSetAlignItems(root, YGAlignCenter);
  YGNodeStyleSetWidth(root, 300);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetMargin(root_child0, YGEdgeLeft, 100);
  YGNodeStyleSetMargin(root_child0, YGEdgeTop, 100);
  YGNodeStyleSetWidth(root_child0, 100);
  YGNodeStyleSetHeight(root_child0, 100);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root));
  EXPECT_FLOAT_EQ(100, YGNodeLayoutGetLeft(root_child0));
  EXPECT_FLOAT_EQ(expectedTop, YGNodeLayoutGetTop(root_child0));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// A row aligned to the baseline, which runs the multi-line pass without wrap:
// a 60 tall item that sets the baseline, a flex-start item with a top margin
// of 5 and a top inset of 10, and a baseline item with a bottom inset of 10.
void checkBaselineRowWithInsets(
    bool withErratum,
    const std::array<float, 3>& expectedTops) {
  YGConfigRef config = newConfig(withErratum);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetAlignItems(root, YGAlignBaseline);
  YGNodeStyleSetPadding(root, YGEdgeAll, 10);
  YGNodeStyleSetWidth(root, 300);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 60);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetAlignSelf(root_child1, YGAlignFlexStart);
  YGNodeStyleSetMargin(root_child1, YGEdgeTop, 5);
  YGNodeStyleSetPosition(root_child1, YGEdgeTop, 10);
  YGNodeStyleSetWidth(root_child1, 50);
  YGNodeStyleSetHeight(root_child1, 20);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeRef root_child2 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPosition(root_child2, YGEdgeBottom, 10);
  YGNodeStyleSetWidth(root_child2, 50);
  YGNodeStyleSetHeight(root_child2, 20);
  YGNodeInsertChild(root, root_child2, 2);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  expectTops(
      {YGNodeLayoutGetTop(root_child0),
       YGNodeLayoutGetTop(root_child1),
       YGNodeLayoutGetTop(root_child2)},
      expectedTops);

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// A statically positioned flex-start item in a wrapping row, with a top margin
// of 5 and a top inset of 10.
void checkStaticItemWithInset(bool withErratum, float expectedTop) {
  YGConfigRef config = newConfig(withErratum);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetFlexWrap(root, YGWrapWrap);
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetPositionType(root_child0, YGPositionTypeStatic);
  YGNodeStyleSetPosition(root_child0, YGEdgeTop, 10);
  YGNodeStyleSetMargin(root_child0, YGEdgeTop, 5);
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 20);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FLOAT_EQ(expectedTop, YGNodeLayoutGetTop(root_child0));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

} // namespace

// YGErrataWrapCrossAlignmentIgnoresMarginAndInset keeps the positions this
// pass gave before the fix. New configs carry it, and so do YGErrataAll and
// YGErrataClassic. Clearing it gives the spec positions.
TEST(YogaTest, wrap_errata_keeps_cross_positions_without_margin) {
  EXPECT_NE(YGErrataAll & YGErrataWrapCrossAlignmentIgnoresMarginAndInset, 0);
  EXPECT_NE(
      YGErrataClassic & YGErrataWrapCrossAlignmentIgnoresMarginAndInset, 0);

  YGConfigRef config = YGConfigNew();
  const YGErrata defaults = YGConfigGetErrata(config);
  YGConfigFree(config);
  EXPECT_NE(defaults & YGErrataWrapCrossAlignmentIgnoresMarginAndInset, 0);

  // The second line starts at 38. The erratum leaves the top margin of 4 out
  // of each item's position.
  expectTops(layoutWrappingRowWithMargins(defaults), {0, 0, 38});
  expectTops(layoutWrappingRowWithMargins(YGErrataAll), {0, 0, 38});
  expectTops(layoutWrappingRowWithMargins(YGErrataClassic), {0, 0, 38});
  expectTops(
      layoutWrappingRowWithMargins(
          YGErrataWrapCrossAlignmentIgnoresMarginAndInset),
      {0, 0, 38});
  expectTops(
      layoutWrappingRowWithMargins(
          static_cast<YGErrata>(
              defaults &
              (YGErrataAll ^ YGErrataWrapCrossAlignmentIgnoresMarginAndInset))),
      {4, 4, 42});
  expectTops(layoutWrappingRowWithMargins(YGErrataNone), {4, 4, 42});
}

// An item with a margin and a relative inset on the second line of a wrapping
// container. With the erratum, flex-start applies the inset but not the
// margin, center drops both, flex-end and stretch drop the inset, and
// wrap-reverse mirrors the whole position.
TEST(YogaTest, wrap_errata_keeps_previous_positions_for_every_alignment) {
  {
    SCOPED_TRACE("row, wrap");
    checkAllAlignments(
        YGFlexDirectionRow,
        YGWrapWrap,
        {
            {YGAlignFlexStart, {25, 50, 50, 20}, {95, 50, 50, 20}},
            {YGAlignCenter, {25, 60, 50, 20}, {95, 60, 50, 20}},
            {YGAlignFlexEnd, {25, 75, 50, 20}, {95, 75, 50, 20}},
            {YGAlignStretch, {25, 45, 50, 50}, {95, 45, 50, 50}},
        },
        true);
  }
  {
    SCOPED_TRACE("row, wrap-reverse");
    checkAllAlignments(
        YGFlexDirectionRow,
        YGWrapWrapReverse,
        {
            {YGAlignFlexStart, {25, 130, 50, 20}, {95, 130, 50, 20}},
            {YGAlignCenter, {25, 120, 50, 20}, {95, 120, 50, 20}},
            {YGAlignFlexEnd, {25, 105, 50, 20}, {95, 105, 50, 20}},
            {YGAlignStretch, {25, 105, 50, 50}, {95, 105, 50, 50}},
        },
        true);
  }
  {
    SCOPED_TRACE("column, wrap");
    checkAllAlignments(
        YGFlexDirectionColumn,
        YGWrapWrap,
        {
            {YGAlignFlexStart, {50, 25, 20, 50}, {140, 25, 20, 50}},
            {YGAlignCenter, {60, 25, 20, 50}, {120, 25, 20, 50}},
            {YGAlignFlexEnd, {75, 25, 20, 50}, {105, 25, 20, 50}},
            {YGAlignStretch, {45, 25, 50, 50}, {105, 25, 50, 50}},
        },
        true);
  }
  {
    SCOPED_TRACE("column, wrap-reverse");
    checkAllAlignments(
        YGFlexDirectionColumn,
        YGWrapWrapReverse,
        {
            {YGAlignFlexStart, {130, 25, 20, 50}, {40, 25, 20, 50}},
            {YGAlignCenter, {120, 25, 20, 50}, {60, 25, 20, 50}},
            {YGAlignFlexEnd, {105, 25, 20, 50}, {75, 25, 20, 50}},
            {YGAlignStretch, {105, 25, 50, 50}, {45, 25, 50, 50}},
        },
        true);
  }
}

// With the erratum a one sided margin is ignored under flex-start and center,
// and under flex-end and stretch it ends up on the opposite edge.
TEST(YogaTest, wrap_reverse_errata_keeps_previous_one_sided_margin_positions) {
  checkOneSidedMargin(
      YGFlexDirectionRow,
      YGEdgeTop,
      YGDirectionLTR,
      {
          {YGAlignFlexStart, {50, 80, 50, 20}},
          {YGAlignCenter, {50, 70, 50, 20}},
          {YGAlignFlexEnd, {50, 60, 50, 20}},
          {YGAlignStretch, {50, 60, 50, 30}},
      },
      true);
  checkOneSidedMargin(
      YGFlexDirectionRow,
      YGEdgeBottom,
      YGDirectionLTR,
      {
          {YGAlignFlexStart, {50, 80, 50, 20}},
          {YGAlignCenter, {50, 70, 50, 20}},
          {YGAlignFlexEnd, {50, 70, 50, 20}},
          {YGAlignStretch, {50, 70, 50, 30}},
      },
      true);
  checkOneSidedMargin(
      YGFlexDirectionColumn,
      YGEdgeLeft,
      YGDirectionLTR,
      {
          {YGAlignFlexStart, {80, 50, 20, 50}},
          {YGAlignCenter, {70, 50, 20, 50}},
          {YGAlignFlexEnd, {60, 50, 20, 50}},
      },
      true);
  checkOneSidedMargin(
      YGFlexDirectionColumn,
      YGEdgeLeft,
      YGDirectionRTL,
      {
          {YGAlignFlexStart, {0, 50, 20, 50}},
          {YGAlignCenter, {10, 50, 20, 50}},
          {YGAlignFlexEnd, {10, 50, 20, 50}},
      },
      true);
}

// With the erratum the #1067 item is centered on its border box, the baseline
// row ignores the margin of the flex-start item and the bottom inset, and the
// static item takes its inset in place of its margin.
TEST(YogaTest, errata_keeps_previous_center_baseline_and_static_positions) {
  {
    SCOPED_TRACE("center with one sided margins");
    checkCenteredItemWithOneSidedMargins(true, 50);
  }
  {
    SCOPED_TRACE("baseline");
    checkBaselineRowWithInsets(true, {10, 20, 50});
  }
  {
    SCOPED_TRACE("static");
    checkStaticItemWithInset(true, 10);
  }
}
