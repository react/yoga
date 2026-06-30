/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>
#include <algorithm>

static YGSize measureTextLike(
    YGNodeConstRef /*node*/,
    float width,
    YGMeasureMode widthMode,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  float measuredWidth = 200.0f;
  if (widthMode == YGMeasureModeAtMost) {
    measuredWidth = std::min(measuredWidth, width);
  }
  return YGSize{.width = measuredWidth, .height = 20.0f};
}

static YGSize measureSquare(
    YGNodeConstRef /*node*/,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  return YGSize{.width = 10.0f, .height = 10.0f};
}

class YGFlexBasisFitContentTest : public testing::TestWithParam<bool> {
 protected:
  void SetUp() override {
    config_ = YGConfigNew();
    YGConfigSetExperimentalFeatureEnabled(
        config_, YGExperimentalFeatureFixFlexBasisFitContent, GetParam());
  }

  void TearDown() override {
    if (root_ != nullptr) {
      YGNodeFreeRecursive(root_);
    }
    YGConfigFree(config_);
  }

  YGConfigRef config_ = nullptr;
  YGNodeRef root_ = nullptr;
};

// Auto-height container with a percentage-height child produces the same
// layout regardless of feature state, because Check 3 preserves percentage
// resolution when availableInnerHeight is NaN.
TEST_P(YGFlexBasisFitContentTest, percentage_height_converges) {
  root_ = YGNodeNewWithConfig(config_);
  YGNodeStyleSetHeight(root_, 300);
  YGNodeStyleSetWidth(root_, 100);

  YGNodeRef container = YGNodeNewWithConfig(config_);
  YGNodeInsertChild(root_, container, 0);

  YGNodeRef child = YGNodeNewWithConfig(config_);
  YGNodeStyleSetHeightPercent(child, 50);
  YGNodeInsertChild(container, child, 0);

  YGNodeCalculateLayout(root_, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(75, YGNodeLayoutGetHeight(child));
  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetHeight(container));
}

// Two auto-height containers with percentage children and flexGrow:1 produce
// the same layout regardless of feature state.
TEST_P(YGFlexBasisFitContentTest, percentage_with_flex_grow_converges) {
  root_ = YGNodeNewWithConfig(config_);
  YGNodeStyleSetHeight(root_, 400);
  YGNodeStyleSetWidth(root_, 100);

  YGNodeRef containerA = YGNodeNewWithConfig(config_);
  YGNodeStyleSetFlexGrow(containerA, 1);
  YGNodeInsertChild(root_, containerA, 0);

  YGNodeRef childA = YGNodeNewWithConfig(config_);
  YGNodeStyleSetHeightPercent(childA, 25);
  YGNodeInsertChild(containerA, childA, 0);

  YGNodeRef containerB = YGNodeNewWithConfig(config_);
  YGNodeStyleSetFlexGrow(containerB, 1);
  YGNodeInsertChild(root_, containerB, 1);

  YGNodeRef childB = YGNodeNewWithConfig(config_);
  YGNodeStyleSetHeightPercent(childB, 50);
  YGNodeInsertChild(containerB, childB, 0);

  YGNodeCalculateLayout(root_, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetHeight(containerA));
  ASSERT_FLOAT_EQ(250, YGNodeLayoutGetHeight(containerB));
}

// Auto-height container with flexShrink and a percentage child causing
// overflow produces the same layout regardless of feature state.
TEST_P(YGFlexBasisFitContentTest, flex_shrink_overflow_converges) {
  root_ = YGNodeNewWithConfig(config_);
  YGNodeStyleSetHeight(root_, 200);
  YGNodeStyleSetWidth(root_, 100);

  YGNodeRef container = YGNodeNewWithConfig(config_);
  YGNodeStyleSetFlexShrink(container, 1);
  YGNodeInsertChild(root_, container, 0);

  YGNodeRef child = YGNodeNewWithConfig(config_);
  YGNodeStyleSetHeightPercent(child, 100);
  YGNodeInsertChild(container, child, 0);

  YGNodeRef fixed = YGNodeNewWithConfig(config_);
  YGNodeStyleSetHeight(fixed, 150);
  YGNodeInsertChild(root_, fixed, 1);

  YGNodeCalculateLayout(root_, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(container));
  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetHeight(fixed));
}

// In a scroll container (column), changing a sibling's height does not cause
// re-measurement of unaffected subtrees when the feature is enabled.
TEST_P(YGFlexBasisFitContentTest, scroll_avoids_remeasure) {
  static uint32_t measureCount = 0;
  auto measureFunc = [](YGNodeConstRef /*node*/,
                        float /*width*/,
                        YGMeasureMode /*widthMode*/,
                        float /*height*/,
                        YGMeasureMode /*heightMode*/) {
    measureCount++;
    return YGSize{.width = 50.0f, .height = 50.0f};
  };

  measureCount = 0;

  root_ = YGNodeNewWithConfig(config_);
  YGNodeStyleSetOverflow(root_, YGOverflowScroll);
  YGNodeStyleSetWidth(root_, 100);
  YGNodeStyleSetHeight(root_, 500);

  YGNodeRef sibling = YGNodeNewWithConfig(config_);
  YGNodeStyleSetHeight(sibling, 100);
  YGNodeInsertChild(root_, sibling, 0);

  YGNodeRef wrapper = YGNodeNewWithConfig(config_);
  YGNodeInsertChild(root_, wrapper, 1);

  YGNodeRef inner = YGNodeNewWithConfig(config_);
  YGNodeInsertChild(wrapper, inner, 0);

  YGNodeRef leaf = YGNodeNewWithConfig(config_);
  YGNodeSetMeasureFunc(leaf, measureFunc);
  YGNodeInsertChild(inner, leaf, 0);

  YGNodeCalculateLayout(root_, YGUndefined, YGUndefined, YGDirectionLTR);
  uint32_t firstPassCount = measureCount;

  YGNodeStyleSetHeight(sibling, 200);
  YGNodeCalculateLayout(root_, YGUndefined, YGUndefined, YGDirectionLTR);
  uint32_t secondPassCount = measureCount - firstPassCount;

  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetHeight(leaf));

  EXPECT_EQ(0, secondPassCount);
}

// Row direction is unaffected by the optimization. Width FitContent is always
// preserved to support text wrapping through container nodes.
TEST_P(YGFlexBasisFitContentTest, row_direction_unchanged) {
  root_ = YGNodeNewWithConfig(config_);
  YGNodeStyleSetWidth(root_, 100);
  YGNodeStyleSetHeight(root_, 100);

  YGNodeRef container = YGNodeNewWithConfig(config_);
  YGNodeInsertChild(root_, container, 0);

  YGNodeRef text = YGNodeNewWithConfig(config_);
  YGNodeSetMeasureFunc(text, measureTextLike);
  YGNodeInsertChild(container, text, 0);

  YGNodeCalculateLayout(root_, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(text));
}

// Scroll container in row direction: width FitContent is skipped for the
// main axis (row) in scroll containers, matching legacy behavior.
TEST_P(YGFlexBasisFitContentTest, row_scroll_skips_width) {
  root_ = YGNodeNewWithConfig(config_);
  YGNodeStyleSetFlexDirection(root_, YGFlexDirectionRow);
  YGNodeStyleSetOverflow(root_, YGOverflowScroll);
  YGNodeStyleSetWidth(root_, 100);
  YGNodeStyleSetHeight(root_, 100);

  YGNodeRef text = YGNodeNewWithConfig(config_);
  YGNodeSetMeasureFunc(text, measureTextLike);
  YGNodeInsertChild(root_, text, 0);

  YGNodeCalculateLayout(root_, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetWidth(text));
}

TEST(YogaTest, flex_basis_zero_column_wrapper_stays_content_sized) {
  constexpr float cellWidth = 100.0f;
  constexpr float measuredSize = 10.0f;
  constexpr float secondPassHeight = 20.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(cell, cellWidth);

  YGNodeRef wrapper = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(wrapper, 0);
  YGNodeStyleSetFlexGrow(wrapper, 0);
  YGNodeStyleSetFlexShrink(wrapper, 0);
  YGNodeInsertChild(cell, wrapper, 0);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(wrapper, label, 0);

  YGNodeCalculateLayout(cell, cellWidth, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(wrapper));
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(label));

  YGNodeMarkDirty(label);
  YGNodeCalculateLayout(cell, cellWidth, secondPassHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(wrapper));
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(label));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, flex_basis_zero_row_wrapper_stays_content_sized) {
  constexpr float cellHeight = 20.0f;
  constexpr float measuredSize = 10.0f;
  constexpr float secondPassWidth = 100.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(cell, YGFlexDirectionRow);
  YGNodeStyleSetHeight(cell, cellHeight);

  YGNodeRef wrapper = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(wrapper, 0);
  YGNodeStyleSetFlexGrow(wrapper, 0);
  YGNodeStyleSetFlexShrink(wrapper, 0);
  YGNodeInsertChild(cell, wrapper, 0);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(wrapper, label, 0);

  YGNodeCalculateLayout(cell, YGUndefined, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetWidth(wrapper));
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetWidth(label));

  YGNodeMarkDirty(label);
  YGNodeCalculateLayout(cell, secondPassWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetWidth(wrapper));
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetWidth(label));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, flex_basis_zero_column_measured_child_stays_content_sized) {
  constexpr float cellWidth = 100.0f;
  constexpr float cellHeight = 20.0f;
  constexpr float measuredSize = 10.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(cell, YGFlexDirectionColumn);
  YGNodeStyleSetWidth(cell, cellWidth);
  YGNodeStyleSetHeight(cell, cellHeight);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(label, 0);
  YGNodeStyleSetFlexGrow(label, 0);
  YGNodeStyleSetFlexShrink(label, 0);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(cell, label, 0);

  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(label));

  YGNodeMarkDirty(label);
  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(label));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, flex_basis_zero_row_measured_child_stays_content_sized) {
  constexpr float cellWidth = 100.0f;
  constexpr float cellHeight = 20.0f;
  constexpr float measuredSize = 10.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(cell, YGFlexDirectionRow);
  YGNodeStyleSetWidth(cell, cellWidth);
  YGNodeStyleSetHeight(cell, cellHeight);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(label, 0);
  YGNodeStyleSetFlexGrow(label, 0);
  YGNodeStyleSetFlexShrink(label, 0);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(cell, label, 0);

  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetWidth(label));

  YGNodeMarkDirty(label);
  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetWidth(label));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, flex_basis_zero_wrapper_is_content_sized_on_first_definite_layout) {
  constexpr float cellWidth = 100.0f;
  constexpr float cellHeight = 20.0f;
  constexpr float measuredSize = 10.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(cell, YGFlexDirectionRow);
  YGNodeStyleSetWidth(cell, cellWidth);
  YGNodeStyleSetHeight(cell, cellHeight);

  YGNodeRef wrapper = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(wrapper, 0);
  YGNodeStyleSetFlexGrow(wrapper, 0);
  YGNodeStyleSetFlexShrink(wrapper, 0);
  YGNodeInsertChild(cell, wrapper, 0);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(wrapper, label, 0);

  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetWidth(wrapper));

  YGNodeMarkDirty(label);
  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetWidth(wrapper));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, flex_basis_zero_percent_wrapper_stays_content_sized) {
  constexpr float cellWidth = 100.0f;
  constexpr float measuredSize = 10.0f;
  constexpr float secondPassHeight = 20.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(cell, cellWidth);

  YGNodeRef wrapper = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasisPercent(wrapper, 0);
  YGNodeStyleSetFlexGrow(wrapper, 0);
  YGNodeStyleSetFlexShrink(wrapper, 0);
  YGNodeInsertChild(cell, wrapper, 0);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(wrapper, label, 0);

  YGNodeCalculateLayout(cell, cellWidth, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(wrapper));

  YGNodeMarkDirty(label);
  YGNodeCalculateLayout(cell, cellWidth, secondPassHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(wrapper));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, flex_basis_zero_padded_wrapper_stays_content_sized) {
  constexpr float cellWidth = 100.0f;
  constexpr float cellHeight = 20.0f;
  constexpr float measuredSize = 10.0f;
  constexpr float padding = 5.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(cell, YGFlexDirectionColumn);
  YGNodeStyleSetWidth(cell, cellWidth);
  YGNodeStyleSetHeight(cell, cellHeight);

  YGNodeRef wrapper = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(wrapper, 0);
  YGNodeStyleSetFlexGrow(wrapper, 0);
  YGNodeStyleSetFlexShrink(wrapper, 0);
  YGNodeStyleSetPadding(wrapper, YGEdgeTop, padding);
  YGNodeStyleSetPadding(wrapper, YGEdgeBottom, padding);
  YGNodeInsertChild(cell, wrapper, 0);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(wrapper, label, 0);

  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(
      measuredSize + padding + padding, YGNodeLayoutGetHeight(wrapper));

  YGNodeMarkDirty(label);
  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(
      measuredSize + padding + padding, YGNodeLayoutGetHeight(wrapper));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, flex_basis_zero_wrapper_with_default_shrink_stays_content_sized) {
  constexpr float cellWidth = 100.0f;
  constexpr float cellHeight = 20.0f;
  constexpr float measuredSize = 10.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetUseWebDefaults(config, true);
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(cell, YGFlexDirectionColumn);
  YGNodeStyleSetWidth(cell, cellWidth);
  YGNodeStyleSetHeight(cell, cellHeight);

  YGNodeRef wrapper = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(wrapper, 0);
  YGNodeStyleSetFlexGrow(wrapper, 0);
  YGNodeInsertChild(cell, wrapper, 0);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(wrapper, label, 0);

  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(wrapper));

  YGNodeMarkDirty(label);
  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(wrapper));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, nested_flex_basis_zero_wrappers_stay_content_sized) {
  constexpr float cellWidth = 100.0f;
  constexpr float cellHeight = 20.0f;
  constexpr float measuredSize = 10.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(cell, cellWidth);
  YGNodeStyleSetHeight(cell, cellHeight);

  YGNodeRef outer = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(outer, 0);
  YGNodeStyleSetFlexGrow(outer, 0);
  YGNodeStyleSetFlexShrink(outer, 0);
  YGNodeInsertChild(cell, outer, 0);

  YGNodeRef inner = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(inner, 0);
  YGNodeStyleSetFlexGrow(inner, 0);
  YGNodeStyleSetFlexShrink(inner, 0);
  YGNodeInsertChild(outer, inner, 0);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(inner, label, 0);

  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(outer));
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(inner));

  YGNodeMarkDirty(label);
  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(outer));
  ASSERT_FLOAT_EQ(measuredSize, YGNodeLayoutGetHeight(inner));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, flex_growing_zero_basis_wrapper_uses_zero_basis) {
  constexpr float cellWidth = 100.0f;
  constexpr float cellHeight = 20.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(cell, YGFlexDirectionRow);
  YGNodeStyleSetWidth(cell, cellWidth);
  YGNodeStyleSetHeight(cell, cellHeight);

  YGNodeRef first = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(first, 0);
  YGNodeStyleSetFlexGrow(first, 1);
  YGNodeInsertChild(cell, first, 0);

  YGNodeRef firstLabel = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(firstLabel, measureSquare);
  YGNodeInsertChild(first, firstLabel, 0);

  YGNodeRef second = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(second, 0);
  YGNodeStyleSetFlexGrow(second, 1);
  YGNodeInsertChild(cell, second, 1);

  YGNodeRef secondLabel = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(secondLabel, measureSquare);
  YGNodeInsertChild(second, secondLabel, 0);

  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(first));
  ASSERT_FLOAT_EQ(50, YGNodeLayoutGetWidth(second));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

TEST(YogaTest, flex_basis_zero_child_with_explicit_size_uses_zero_basis) {
  constexpr float cellWidth = 100.0f;
  constexpr float cellHeight = 20.0f;
  constexpr float childHeight = 5.0f;

  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);

  YGNodeRef cell = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(cell, cellWidth);
  YGNodeStyleSetHeight(cell, cellHeight);

  YGNodeRef wrapper = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(wrapper, 0);
  YGNodeStyleSetFlexGrow(wrapper, 0);
  YGNodeStyleSetFlexShrink(wrapper, 0);
  YGNodeStyleSetHeight(wrapper, childHeight);
  YGNodeInsertChild(cell, wrapper, 0);

  YGNodeRef label = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(label, measureSquare);
  YGNodeInsertChild(wrapper, label, 0);

  YGNodeCalculateLayout(cell, cellWidth, cellHeight, YGDirectionLTR);
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetHeight(wrapper));

  YGNodeFreeRecursive(cell);
  YGConfigFree(config);
}

INSTANTIATE_TEST_SUITE_P(
    YogaTest,
    YGFlexBasisFitContentTest,
    testing::Values(false, true));

// Feature toggle invalidates layout cache.
TEST(YogaTest, flex_basis_fit_content_feature_change_invalidates_cache) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, false);

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(root, 300);
  YGNodeStyleSetWidth(root, 100);

  YGNodeRef container = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexGrow(container, 1);
  YGNodeInsertChild(root, container, 0);

  YGNodeRef child = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeightPercent(child, 50);
  YGNodeInsertChild(container, child, 0);

  YGNodeRef fixed = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(fixed, 100);
  YGNodeInsertChild(root, fixed, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  float heightBefore = YGNodeLayoutGetHeight(container);

  YGConfigSetExperimentalFeatureEnabled(
      config, YGExperimentalFeatureFixFlexBasisFitContent, true);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  float heightAfter = YGNodeLayoutGetHeight(container);

  ASSERT_FLOAT_EQ(heightBefore, heightAfter);

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
