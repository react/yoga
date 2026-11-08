/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Laying out a tree again at another root size must give the layout a fresh
// tree of the same styles gets at that size. The measurements an auto-height
// child cached under a FitContent height that didn't constrain it are reused
// when that height changes, so this builds random trees and compares both
// layouts at several root heights and widths. The generator is seeded, so a
// failure names the tree that produced it.
//
// Two vocabularies are used. Together they cover the values the reuse looks
// at; apart, each stays within what the layout caches already kept consistent
// before the reuse existed (flex bases, min/max heights, percent margins and
// positions combine into stale layouts that are not the subject of this
// test).

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <random>
#include <string>
#include <vector>

namespace {

struct Vocabulary {
  bool rows = true;
  bool percentSizes = true;
  bool keywordSizes = true;
  bool minWidth = true;
  bool flexBasis = true;
  bool minMaxHeight = true;
  bool maxWidth = true;
  bool padding = true;
  bool margin = true;
  bool grow = true;
  bool flex = true;
  bool aspectRatio = true;
  bool position = true;
  bool text = true;
  bool alignItems = true;
  bool alignSelf = true;
  bool overflow = true;
};

// Columns with percent and keyword sizes, flex grow and shrink, padding,
// positions and text.
constexpr Vocabulary kColumnsWithPercentSizes = {
    .rows = false,
    .minWidth = false,
    .flexBasis = false,
    .minMaxHeight = false,
    .margin = false,
    .flex = false,
    .aspectRatio = false,
    .alignSelf = false,
    .overflow = false,
};

// Rows and columns with percent and keyword sizes, `flex`, aspect ratios,
// text and overflow.
constexpr Vocabulary kRowsWithPercentSizes = {
    .minWidth = false,
    .flexBasis = false,
    .minMaxHeight = false,
    .maxWidth = false,
    .padding = false,
    .margin = false,
    .grow = false,
    .position = false,
    .alignItems = false,
    .alignSelf = false,
};

class Random {
 public:
  explicit Random(uint32_t seed) : engine_(seed) {}

  // In [0, n).
  uint32_t below(uint32_t n) {
    return std::uniform_int_distribution<uint32_t>(0, n - 1)(engine_);
  }

  bool chance(uint32_t percent) {
    return below(100) < percent;
  }

  float points(float min, float max) {
    // Whole points keep the comparison free of accumulated rounding noise
    // that has nothing to do with the cache.
    return static_cast<float>(std::uniform_int_distribution<int>(
        static_cast<int>(min), static_cast<int>(max))(engine_));
  }

  template <typename T>
  T pick(std::initializer_list<T> values) {
    return *(values.begin() + below(static_cast<uint32_t>(values.size())));
  }

 private:
  std::mt19937 engine_;
};

YGSize measureText(
    YGNodeConstRef node,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode) {
  // A text-like leaf: a fixed amount of content wraps into lines of the
  // available width.
  const auto chars =
      static_cast<float>(reinterpret_cast<intptr_t>(YGNodeGetContext(node)));
  const float charWidth = 7.0f;
  const float lineHeight = 18.0f;
  float measuredWidth = chars * charWidth;
  float lines = 1.0f;
  if (widthMode != YGMeasureModeUndefined && !std::isnan(width) &&
      (widthMode == YGMeasureModeExactly || measuredWidth > width)) {
    measuredWidth = std::fmax(width, 0.0f);
    lines = std::ceil(chars * charWidth / std::fmax(width, charWidth));
  }
  float measuredHeight = lines * lineHeight;
  if (heightMode != YGMeasureModeUndefined && !std::isnan(height) &&
      (heightMode == YGMeasureModeExactly || measuredHeight > height)) {
    measuredHeight = std::fmax(height, 0.0f);
  }
  return YGSize{measuredWidth, measuredHeight};
}

float baselineAtTwoThirds(YGNodeConstRef, float, float height) {
  return std::isnan(height) ? 0.0f : height * 2.0f / 3.0f;
}

void setLength(
    Random& random,
    YGNodeRef node,
    void (*setPoints)(YGNodeRef, float),
    void (*setPercent)(YGNodeRef, float),
    void (*setAuto)(YGNodeRef),
    void (*setStretch)(YGNodeRef),
    void (*setFitContent)(YGNodeRef),
    void (*setMaxContent)(YGNodeRef),
    float maxPoints) {
  switch (random.below(12)) {
    case 0:
    case 1:
    case 2:
      setPoints(node, random.points(0, maxPoints));
      break;
    case 3:
    case 4:
      if (setPercent != nullptr) {
        setPercent(node, random.points(10, 120));
      } else {
        setPoints(node, random.points(10, maxPoints));
      }
      break;
    case 5:
      if (setAuto != nullptr) {
        setAuto(node);
      }
      break;
    case 6:
      if (setStretch != nullptr) {
        setStretch(node);
      }
      break;
    case 7:
      if (setFitContent != nullptr) {
        setFitContent(node);
      }
      break;
    case 8:
      if (setMaxContent != nullptr) {
        setMaxContent(node);
      }
      break;
    default:
      break; // undefined
  }
}

void setEdges(
    Random& random,
    YGNodeRef node,
    void (*setPoints)(YGNodeRef, YGEdge, float),
    void (*setPercent)(YGNodeRef, YGEdge, float),
    void (*setAuto)(YGNodeRef, YGEdge),
    float minPoints,
    float maxPoints) {
  const auto edges = {
      YGEdgeLeft,
      YGEdgeTop,
      YGEdgeRight,
      YGEdgeBottom,
      YGEdgeStart,
      YGEdgeEnd,
      YGEdgeHorizontal,
      YGEdgeVertical,
      YGEdgeAll};
  for (const auto edge : edges) {
    switch (random.below(10)) {
      case 0:
      case 1:
        setPoints(node, edge, random.points(minPoints, maxPoints));
        break;
      case 2:
        setPercent(node, edge, random.points(0, 20));
        break;
      case 3:
        if (setAuto != nullptr) {
          setAuto(node, edge);
        }
        break;
      default:
        break;
    }
  }
}

void styleNode(
    Random& random,
    const Vocabulary& vocabulary,
    YGNodeRef node,
    bool isRoot) {
  const auto direction = random.pick(
      {YGFlexDirectionColumn,
       YGFlexDirectionColumn,
       YGFlexDirectionRow,
       YGFlexDirectionColumnReverse,
       YGFlexDirectionRowReverse});
  if (vocabulary.rows || direction == YGFlexDirectionColumnReverse) {
    YGNodeStyleSetFlexDirection(node, direction);
  }
  if (random.chance(15)) {
    YGNodeStyleSetFlexWrap(node, random.pick({YGWrapWrap, YGWrapWrapReverse}));
  }
  YGNodeStyleSetJustifyContent(
      node,
      random.pick(
          {YGJustifyFlexStart,
           YGJustifyFlexStart,
           YGJustifyCenter,
           YGJustifyFlexEnd,
           YGJustifySpaceBetween,
           YGJustifySpaceAround,
           YGJustifySpaceEvenly}));
  const auto alignItems = random.pick(
      {YGAlignStretch,
       YGAlignStretch,
       YGAlignFlexStart,
       YGAlignCenter,
       YGAlignFlexEnd,
       YGAlignBaseline});
  if (vocabulary.alignItems) {
    YGNodeStyleSetAlignItems(node, alignItems);
  }
  YGNodeStyleSetAlignContent(
      node,
      random.pick(
          {YGAlignFlexStart,
           YGAlignStretch,
           YGAlignCenter,
           YGAlignFlexEnd,
           YGAlignSpaceBetween,
           YGAlignSpaceAround,
           YGAlignSpaceEvenly}));
  if (random.chance(30) && vocabulary.alignSelf) {
    YGNodeStyleSetAlignSelf(
        node,
        random.pick(
            {YGAlignAuto,
             YGAlignStretch,
             YGAlignFlexStart,
             YGAlignCenter,
             YGAlignFlexEnd,
             YGAlignBaseline}));
  }
  if (random.chance(20) && vocabulary.overflow) {
    YGNodeStyleSetOverflow(
        node, random.pick({YGOverflowHidden, YGOverflowScroll}));
  }
  if (random.chance(10)) {
    YGNodeStyleSetBoxSizing(node, YGBoxSizingContentBox);
  }

  if (!isRoot) {
    setLength(
        random,
        node,
        YGNodeStyleSetWidth,
        vocabulary.percentSizes ? YGNodeStyleSetWidthPercent : nullptr,
        YGNodeStyleSetWidthAuto,
        vocabulary.keywordSizes ? YGNodeStyleSetWidthStretch : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetWidthFitContent : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetWidthMaxContent : nullptr,
        300);
    setLength(
        random,
        node,
        YGNodeStyleSetHeight,
        vocabulary.percentSizes ? YGNodeStyleSetHeightPercent : nullptr,
        YGNodeStyleSetHeightAuto,
        vocabulary.keywordSizes ? YGNodeStyleSetHeightStretch : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetHeightFitContent : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetHeightMaxContent : nullptr,
        300);
    if (random.chance(25) && vocabulary.flexBasis) {
      setLength(
          random,
          node,
          YGNodeStyleSetFlexBasis,
          YGNodeStyleSetFlexBasisPercent,
          YGNodeStyleSetFlexBasisAuto,
          vocabulary.keywordSizes ? YGNodeStyleSetFlexBasisStretch : nullptr,
          vocabulary.keywordSizes ? YGNodeStyleSetFlexBasisFitContent : nullptr,
          vocabulary.keywordSizes ? YGNodeStyleSetFlexBasisMaxContent : nullptr,
          200);
    }
    if (random.chance(30) && vocabulary.grow) {
      YGNodeStyleSetFlexGrow(node, random.pick({0.0f, 0.5f, 1.0f, 2.0f}));
    }
    if (random.chance(30) && vocabulary.grow) {
      YGNodeStyleSetFlexShrink(node, random.pick({0.0f, 1.0f}));
    }
    if (random.chance(10) && vocabulary.flex) {
      YGNodeStyleSetFlex(node, random.pick({-1.0f, 0.0f, 1.0f}));
    }
    if (random.chance(15) && vocabulary.aspectRatio) {
      YGNodeStyleSetAspectRatio(node, random.pick({0.5f, 1.0f, 2.0f}));
    }
    if (random.chance(15) && vocabulary.position) {
      YGNodeStyleSetPositionType(
          node,
          random.pick(
              {YGPositionTypeAbsolute,
               YGPositionTypeStatic,
               YGPositionTypeRelative}));
      for (const auto edge : {YGEdgeTop, YGEdgeBottom, YGEdgeLeft}) {
        if (random.chance(40)) {
          if (random.chance(50)) {
            YGNodeStyleSetPosition(node, edge, random.points(-20, 40));
          } else {
            YGNodeStyleSetPositionPercent(node, edge, random.points(0, 50));
          }
        }
      }
    }
    if (random.chance(8)) {
      YGNodeStyleSetDisplay(
          node, random.pick({YGDisplayNone, YGDisplayContents}));
    }
  }
  if (random.chance(30) && vocabulary.minMaxHeight) {
    setLength(
        random,
        node,
        YGNodeStyleSetMinHeight,
        YGNodeStyleSetMinHeightPercent,
        nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMinHeightStretch : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMinHeightFitContent : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMinHeightMaxContent : nullptr,
        150);
  }
  if (random.chance(30) && vocabulary.minMaxHeight) {
    setLength(
        random,
        node,
        YGNodeStyleSetMaxHeight,
        YGNodeStyleSetMaxHeightPercent,
        nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMaxHeightStretch : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMaxHeightFitContent : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMaxHeightMaxContent : nullptr,
        400);
  }
  if (random.chance(20) && vocabulary.minWidth) {
    setLength(
        random,
        node,
        YGNodeStyleSetMinWidth,
        YGNodeStyleSetMinWidthPercent,
        nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMinWidthStretch : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMinWidthFitContent : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMinWidthMaxContent : nullptr,
        150);
  }
  if (random.chance(20) && vocabulary.maxWidth) {
    setLength(
        random,
        node,
        YGNodeStyleSetMaxWidth,
        YGNodeStyleSetMaxWidthPercent,
        nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMaxWidthStretch : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMaxWidthFitContent : nullptr,
        vocabulary.keywordSizes ? YGNodeStyleSetMaxWidthMaxContent : nullptr,
        400);
  }
  if (vocabulary.margin) {
    setEdges(
        random,
        node,
        YGNodeStyleSetMargin,
        YGNodeStyleSetMarginPercent,
        YGNodeStyleSetMarginAuto,
        -20,
        30);
  }
  if (vocabulary.padding) {
    setEdges(
        random,
        node,
        YGNodeStyleSetPadding,
        YGNodeStyleSetPaddingPercent,
        nullptr,
        0,
        30);
  }
  if (random.chance(20)) {
    YGNodeStyleSetBorder(node, YGEdgeAll, random.points(1, 5));
  }
  for (const auto gutter : {YGGutterRow, YGGutterColumn, YGGutterAll}) {
    switch (random.below(8)) {
      case 0:
      case 1:
        YGNodeStyleSetGap(node, gutter, random.points(0, 20));
        break;
      case 2:
        YGNodeStyleSetGapPercent(node, gutter, random.points(0, 15));
        break;
      default:
        break;
    }
  }
}

// Builds a tree whose shape and styles are fully determined by the seed.
YGNodeRef buildTree(
    Random& random,
    const Vocabulary& vocabulary,
    YGConfigRef config,
    uint32_t depth,
    std::vector<YGNodeRef>& nodes) {
  YGNodeRef node = YGNodeNewWithConfig(config);
  nodes.push_back(node);
  styleNode(random, vocabulary, node, depth == 0);
  if (depth > 0 && random.chance(25) && vocabulary.text) {
    // A text-like leaf.
    YGNodeSetContext(
        node,
        reinterpret_cast<void*>(static_cast<intptr_t>(1 + random.below(60))));
    YGNodeSetMeasureFunc(node, measureText);
    if (random.chance(30)) {
      YGNodeSetBaselineFunc(node, baselineAtTwoThirds);
    }
    return node;
  }
  const uint32_t maxChildren = depth >= 4 ? 0 : (depth == 0 ? 4 : 3);
  const uint32_t childCount =
      maxChildren == 0 ? 0 : random.below(maxChildren + 1);
  for (uint32_t i = 0; i < childCount; i++) {
    YGNodeInsertChild(
        node, buildTree(random, vocabulary, config, depth + 1, nodes), i);
  }
  return node;
}

struct Tree {
  YGConfigRef config;
  YGNodeRef root;
  std::vector<YGNodeRef> nodes;
};

Tree makeTree(const Vocabulary& vocabulary, uint32_t seed) {
  Random random(seed);
  Tree tree;
  tree.config = YGConfigNew();
  YGConfigSetErrata(
      tree.config,
      random.pick({YGErrataNone, YGErrataAll, YGErrataStretchFlexBasis}));
  // Rounding to the pixel grid depends on a node's absolute position, so a
  // node that didn't move relative to an owner that did can round to a size a
  // pixel off the fresh layout's. That has nothing to do with the caches
  // under test, so the layouts are compared unrounded.
  YGConfigSetPointScaleFactor(tree.config, 0.0f);
  tree.root = buildTree(random, vocabulary, tree.config, 0, tree.nodes);
  return tree;
}

void freeTree(Tree& tree) {
  YGNodeFreeRecursive(tree.root);
  YGConfigFree(tree.config);
}

void layoutAt(const Tree& tree, float width, float height) {
  YGNodeStyleSetWidth(tree.root, width);
  YGNodeStyleSetHeight(tree.root, height);
  YGNodeCalculateLayout(tree.root, YGUndefined, YGUndefined, YGDirectionLTR);
}

::testing::AssertionResult sameLayout(
    const Tree& relaid,
    const Tree& fresh,
    uint32_t seed,
    const std::string& step) {
  for (size_t i = 0; i < relaid.nodes.size(); i++) {
    const auto a = relaid.nodes[i];
    const auto b = fresh.nodes[i];
    const float got[] = {
        YGNodeLayoutGetLeft(a),
        YGNodeLayoutGetTop(a),
        YGNodeLayoutGetWidth(a),
        YGNodeLayoutGetHeight(a)};
    const float want[] = {
        YGNodeLayoutGetLeft(b),
        YGNodeLayoutGetTop(b),
        YGNodeLayoutGetWidth(b),
        YGNodeLayoutGetHeight(b)};
    for (int k = 0; k < 4; k++) {
      const bool bothNaN = std::isnan(got[k]) && std::isnan(want[k]);
      if (!bothNaN && std::fabs(got[k] - want[k]) > 0.01f) {
        return ::testing::AssertionFailure()
            << "seed " << seed << ", " << step << ": node " << i
            << " has left/top/width/height " << got[0] << "/" << got[1] << "/"
            << got[2] << "/" << got[3] << ", a fresh tree gets " << want[0]
            << "/" << want[1] << "/" << want[2] << "/" << want[3];
      }
    }
  }
  return ::testing::AssertionSuccess();
}

// Each tree is laid out at a first size, then again at the others, in order,
// and compared after each step with a fresh tree laid out at that size only.
void relayoutMatchesFreshLayout(const Vocabulary& vocabulary) {
  const float widths[] = {400, 400, 400, 400, 400, 300, 300, 500};
  const float heights[] = {1000, 1200, 1500, 900, 2000, 2000, 1100, 1100};
  const uint32_t trees = 1000;
  for (uint32_t seed = 1; seed <= trees; seed++) {
    Tree relaid = makeTree(vocabulary, seed);
    layoutAt(relaid, widths[0], heights[0]);
    for (size_t step = 1; step < std::size(widths); step++) {
      layoutAt(relaid, widths[step], heights[step]);
      Tree fresh = makeTree(vocabulary, seed);
      layoutAt(fresh, widths[step], heights[step]);
      const auto result = sameLayout(
          relaid,
          fresh,
          seed,
          "step " + std::to_string(step) + " (" +
              std::to_string(static_cast<int>(widths[step])) + "x" +
              std::to_string(static_cast<int>(heights[step])) + ")");
      freeTree(fresh);
      ASSERT_TRUE(result);
    }
    freeTree(relaid);
  }
}

} // namespace

TEST(YogaTest, relayout_of_columns_with_percent_sizes_matches_fresh_layout) {
  relayoutMatchesFreshLayout(kColumnsWithPercentSizes);
}

TEST(YogaTest, relayout_of_rows_with_percent_sizes_matches_fresh_layout) {
  relayoutMatchesFreshLayout(kRowsWithPercentSizes);
}
