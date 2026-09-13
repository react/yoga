/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

namespace {

// Simulates a min-content-aware text measure: the longest "word" is
// `kWordWidth`. Asked to be smaller than that on the main axis, the measure
// function returns the longest-word width — that's how a real text engine
// reports its min-content width.
constexpr float kWordWidth = 30.0f;
constexpr float kNaturalWidth = 90.0f;
constexpr float kLineHeight = 16.0f;

YGSize measureWordWrappingText(
    YGNodeConstRef /*node*/,
    float width,
    YGMeasureMode widthMode,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  if (widthMode == YGMeasureModeAtMost) {
    if (width < kWordWidth) {
      return YGSize{kWordWidth, kLineHeight * 3};
    }
    if (width < kNaturalWidth) {
      return YGSize{width, kLineHeight * 2};
    }
    return YGSize{kNaturalWidth, kLineHeight};
  }
  if (widthMode == YGMeasureModeExactly) {
    return YGSize{width, kLineHeight};
  }
  return YGSize{kNaturalWidth, kLineHeight};
}

YGSize measureFixedSize(
    YGNodeConstRef node,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  const auto* dims = static_cast<const YGSize*>(YGNodeGetContext(node));
  return dims != nullptr ? *dims : YGSize{};
}

YGConfigRef makeWebConfig(bool useAutoMinSize) {
  YGConfigRef config = YGConfigNew();
  YGConfigSetUseWebDefaults(config, true);
  // Default config has YGErrataMinSizeUndefinedInsteadOfAuto set (preserves
  // legacy "no auto-min" behavior). Clear the bit to opt into CSS §4.5
  // automatic minimum sizing.
  if (useAutoMinSize) {
    const YGErrata errata = YGConfigGetErrata(config);
    YGConfigSetErrata(
        config,
        static_cast<YGErrata>(errata & ~YGErrataMinSizeUndefinedInsteadOfAuto));
  }
  return config;
}

// Builds a 2-child row where the first child is shrinkable text and the
// second is a fixed-size spacer that doesn't shrink. This forces the text
// to absorb all the shrink when free space is negative.
struct ShrinkRow {
  YGConfigRef config;
  YGNodeRef root;
  YGNodeRef text;
  YGNodeRef spacer;

  ShrinkRow(bool useAutoMinSize, float containerWidth)
      : config(makeWebConfig(useAutoMinSize)),
        root(YGNodeNewWithConfig(config)),
        text(YGNodeNewWithConfig(config)),
        spacer(YGNodeNewWithConfig(config)) {
    YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
    YGNodeStyleSetWidth(root, containerWidth);
    YGNodeStyleSetHeight(root, 50);

    YGNodeSetMeasureFunc(text, measureWordWrappingText);
    YGNodeStyleSetFlexBasis(text, kNaturalWidth);
    YGNodeStyleSetFlexGrow(text, 0);
    YGNodeStyleSetFlexShrink(text, 1);
    YGNodeInsertChild(root, text, 0);

    YGNodeStyleSetWidth(spacer, 10);
    YGNodeStyleSetFlexShrink(spacer, 0);
    YGNodeInsertChild(root, spacer, 1);
  }

  ShrinkRow(const ShrinkRow&) = delete;
  ShrinkRow& operator=(const ShrinkRow&) = delete;

  ~ShrinkRow() {
    YGNodeFreeRecursive(root);
    YGConfigFree(config);
  }

  void layout() {
    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  }
};

} // namespace

// Default config (auto-min off): shrink path takes the text below its
// content size — legacy Yoga behavior preserved.
TEST(YogaAutoMinSize, default_config_preserves_existing_shrink) {
  ShrinkRow row(/*useAutoMinSize=*/false, /*containerWidth=*/20);
  row.layout();
  // Container 20 - spacer 10 = 10 for text. Without auto-min, text shrinks
  // freely below kWordWidth (30).
  EXPECT_FLOAT_EQ(10.0f, YGNodeLayoutGetWidth(row.text));
}

// Auto-min on: text floored at min-content (kWordWidth). Container
// overflows rather than violate the floor.
TEST(YogaAutoMinSize, auto_min_floors_text_at_min_content_width) {
  ShrinkRow row(/*useAutoMinSize=*/true, /*containerWidth=*/20);
  row.layout();
  // Floor = min(content=30, specified=NaN) = 30. Text stuck at 30; the
  // 10-px spacer takes its space; container of 20 overflows.
  EXPECT_FLOAT_EQ(kWordWidth, YGNodeLayoutGetWidth(row.text));
  EXPECT_FLOAT_EQ(10.0f, YGNodeLayoutGetWidth(row.spacer));
}

// A measure-func leaf's auto-min floor must include the leaf's own padding and
// border on the main axis, just like the container branch and the normal
// measure pass. Regression test: the original probe omitted them, flooring a
// padded text at its bare longest-word width.
TEST(YogaAutoMinSize, auto_min_includes_leaf_padding_and_border_width) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 20);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef text = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(text, measureWordWrappingText);
  YGNodeStyleSetFlexBasis(text, kNaturalWidth);
  YGNodeStyleSetFlexGrow(text, 0);
  YGNodeStyleSetFlexShrink(text, 1);
  YGNodeStyleSetPadding(text, YGEdgeLeft, 4);
  YGNodeStyleSetPadding(text, YGEdgeRight, 4);
  YGNodeStyleSetBorder(text, YGEdgeLeft, 1);
  YGNodeStyleSetBorder(text, YGEdgeRight, 1);
  YGNodeInsertChild(root, text, 0);

  YGNodeRef spacer = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(spacer, 10);
  YGNodeStyleSetFlexShrink(spacer, 0);
  YGNodeInsertChild(root, spacer, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Floor = content kWordWidth(30) + padding(4+4) + border(1+1) = 40. Without
  // the padding/border contribution the leaf would be wrongly floored at 30.
  EXPECT_FLOAT_EQ(40.0f, YGNodeLayoutGetWidth(text));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Same fix on the column (cross) axis: vertical padding must be included in the
// height min-content.
TEST(YogaAutoMinSize, auto_min_includes_leaf_padding_height) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 20);

  YGNodeRef text = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(text, measureWordWrappingText);
  YGNodeStyleSetFlexBasis(text, kNaturalWidth); // tall basis forces shrink
  YGNodeStyleSetFlexGrow(text, 0);
  YGNodeStyleSetFlexShrink(text, 1);
  YGNodeStyleSetPadding(text, YGEdgeTop, 4);
  YGNodeStyleSetPadding(text, YGEdgeBottom, 4);
  YGNodeInsertChild(root, text, 0);

  YGNodeRef spacer = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(spacer, 10);
  YGNodeStyleSetFlexShrink(spacer, 0);
  YGNodeInsertChild(root, spacer, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Column probe height = natural kLineHeight(16) + padding(4+4) = 24.
  EXPECT_FLOAT_EQ(24.0f, YGNodeLayoutGetHeight(text));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// flex-basis: 0 with intrinsic content (the under-protection case from the
// critique). With auto-min on, an item with `flex: 1` is still floored at
// its min-content even though basis is 0.
TEST(YogaAutoMinSize, flex_basis_zero_floors_at_min_content) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef a = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(a, measureWordWrappingText);
  YGNodeStyleSetFlex(a, 1);
  YGNodeInsertChild(root, a, 0);

  YGNodeRef b = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(b, measureWordWrappingText);
  YGNodeStyleSetFlex(b, 1);
  YGNodeInsertChild(root, b, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Each auto-min = kWordWidth. Container 50, total floor 60, overflows.
  EXPECT_FLOAT_EQ(kWordWidth, YGNodeLayoutGetWidth(a));
  EXPECT_FLOAT_EQ(kWordWidth, YGNodeLayoutGetWidth(b));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Explicit width (basis) > content: floor = min(content, specified) =
// content. So text can shrink from basis-90 down to content-30.
TEST(YogaAutoMinSize, content_smaller_than_specified_shrinks_to_content) {
  ShrinkRow row(/*useAutoMinSize=*/true, /*containerWidth=*/20);
  row.layout();
  // Auto-min = min(content=30, specified=NaN) = 30. (No flex-basis set as
  // a "specified main size" — Yoga's basis is set via setFlexBasis but the
  // CSS spec checks `width`/`height`, which here are undefined.) So the
  // floor is 30, and text shrinks from natural-90 down to 30.
  EXPECT_FLOAT_EQ(kWordWidth, YGNodeLayoutGetWidth(row.text));
}

// max-width caps the auto-min.
TEST(YogaAutoMinSize, auto_min_capped_by_max_size) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 10);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef text = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(text, measureWordWrappingText);
  YGNodeStyleSetFlexBasis(text, kNaturalWidth);
  YGNodeStyleSetFlexGrow(text, 0);
  YGNodeStyleSetFlexShrink(text, 1);
  YGNodeStyleSetMaxWidth(text, 20);
  YGNodeInsertChild(root, text, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Auto-min = min(content=30) capped by max=20 → 20. Text floored at 20.
  EXPECT_FLOAT_EQ(20.0f, YGNodeLayoutGetWidth(text));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Explicit min-width: 0 opts out (CSS escape hatch).
TEST(YogaAutoMinSize, explicit_min_width_zero_opts_out) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 20);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef text = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(text, measureWordWrappingText);
  YGNodeStyleSetFlexBasis(text, kNaturalWidth);
  YGNodeStyleSetFlexGrow(text, 0);
  YGNodeStyleSetFlexShrink(text, 1);
  YGNodeStyleSetMinWidth(text, 0);
  YGNodeInsertChild(root, text, 0);

  YGNodeRef spacer = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(spacer, 10);
  YGNodeStyleSetFlexShrink(spacer, 0);
  YGNodeInsertChild(root, spacer, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // min-width:0 → no auto-min. Text shrinks to 10 (container - spacer).
  EXPECT_FLOAT_EQ(10.0f, YGNodeLayoutGetWidth(text));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Aspect-ratio item with definite cross-size and no specified main:
// transferred-size = cross × ratio acts as the floor.
TEST(YogaAutoMinSize, aspect_ratio_transferred_size_floors_main) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 30);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef img = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeight(img, 40);
  YGNodeStyleSetAspectRatio(img, 2.0f);
  YGNodeStyleSetFlexBasis(img, kNaturalWidth); // basis 90, container 30
  YGNodeStyleSetFlexShrink(img, 1);
  YGNodeInsertChild(root, img, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // No specified main width, aspect-ratio defined, cross definite:
  // transferred-size = 40 * 2 = 80. Auto-min = min(content=0,
  // transferred=80) = 0 — wait, per §4.5: when specified is undefined and
  // aspect-ratio applies, floor = min(content, transferred). content=0
  // (no measure func, no children) so floor = 0. img can shrink to 30.
  // The transferred-size is the LOWER bound on the *content suggestion*
  // when there's no measure func: per §4.5, when content suggestion
  // would be 0 and transferred applies, transferred replaces it.
  // Yoga implements the spec as min(content, transferred), preferring the
  // smaller — pragmatic but slightly under-protective for replaced
  // elements without intrinsic size.
  EXPECT_FLOAT_EQ(30.0f, YGNodeLayoutGetWidth(img));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Multi-level: outer column has limited height; inner wrapper has a
// fixed-size leaf (height 50) — auto-min protects the wrapper at 50.
TEST(YogaAutoMinSize, nested_flexbox_recurses_into_min_content) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 30);

  YGNodeRef wrapper = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(wrapper, YGFlexDirectionColumn);
  YGNodeStyleSetFlexGrow(wrapper, 0);
  YGNodeStyleSetFlexShrink(wrapper, 1);
  YGNodeInsertChild(root, wrapper, 0);

  YGSize dims{200.0f, 50.0f};
  YGNodeRef leaf = YGNodeNewWithConfig(config);
  YGNodeSetContext(leaf, &dims);
  YGNodeSetMeasureFunc(leaf, measureFixedSize);
  YGNodeInsertChild(wrapper, leaf, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Wrapper's recursive min-content = leaf's intrinsic 50. Floor 50,
  // container 30 → wrapper protected at 50, container overflows.
  EXPECT_FLOAT_EQ(50.0f, YGNodeLayoutGetHeight(wrapper));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// overflow != visible disables auto-min on that item (CSS spec).
TEST(YogaAutoMinSize, overflow_hidden_disables_auto_min) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 20);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef text = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(text, measureWordWrappingText);
  YGNodeStyleSetFlexBasis(text, kNaturalWidth);
  YGNodeStyleSetFlexGrow(text, 0);
  YGNodeStyleSetFlexShrink(text, 1);
  YGNodeStyleSetOverflow(text, YGOverflowHidden);
  YGNodeInsertChild(root, text, 0);

  YGNodeRef spacer = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(spacer, 10);
  YGNodeStyleSetFlexShrink(spacer, 0);
  YGNodeInsertChild(root, spacer, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // overflow:hidden → auto-min = 0 → text shrinks to 10 (container -
  // spacer), well below kWordWidth.
  EXPECT_FLOAT_EQ(10.0f, YGNodeLayoutGetWidth(text));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Counter for the min-content callback invocations, used by the next test.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static int gMinContentCalls = 0;

static YGSize measureMinContentZero(
    YGNodeConstRef /*node*/,
    float /*width*/,
    YGMeasureMode /*widthMode*/,
    float /*height*/,
    YGMeasureMode /*heightMode*/) {
  ++gMinContentCalls;
  return YGSize{0.0f, 0.0f};
}

// When a `YGMinContentMeasureFunc` is set, it's used for the §4.5 probe
// instead of the regular measure function. Models the Image/Collection
// Primitive case: the regular measure returns intrinsic content, but
// min-content is 0.
TEST(YogaAutoMinSize, min_content_measure_func_preferred_during_probe) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 20);
  YGNodeStyleSetHeight(root, 50);

  // Item with a regular measure that would return 90 (the natural width
  // of the longest word) under `AtMost 0`, but a min-content measure that
  // returns 0 — like an Image whose intrinsic width is 90 but whose
  // min-content contribution is 0.
  YGNodeRef item = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(item, measureWordWrappingText);
  gMinContentCalls = 0;
  YGNodeSetMinContentMeasureFunc(item, measureMinContentZero);
  YGNodeStyleSetFlexBasis(item, kNaturalWidth);
  YGNodeStyleSetFlexGrow(item, 0);
  YGNodeStyleSetFlexShrink(item, 1);
  YGNodeInsertChild(root, item, 0);

  YGNodeRef spacer = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(spacer, 10);
  YGNodeStyleSetFlexShrink(spacer, 0);
  YGNodeInsertChild(root, spacer, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // The probe used the min-content callback (returning 0), so the auto-min
  // floor for the item is 0 — it can shrink all the way to 10 (container
  // 20 - spacer 10), unlike the same test without the callback where it
  // would be floored at kWordWidth (30).
  EXPECT_FLOAT_EQ(10.0f, YGNodeLayoutGetWidth(item));
  EXPECT_GT(gMinContentCalls, 0);

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

TEST(YogaAutoMinSize, has_min_content_measure_func_tracks_setter) {
  YGNodeRef node = YGNodeNew();
  EXPECT_FALSE(YGNodeHasMinContentMeasureFunc(node));
  YGNodeSetMinContentMeasureFunc(node, measureMinContentZero);
  EXPECT_TRUE(YGNodeHasMinContentMeasureFunc(node));
  YGNodeSetMinContentMeasureFunc(node, nullptr);
  EXPECT_FALSE(YGNodeHasMinContentMeasureFunc(node));
  YGNodeFree(node);
}

// Static min-content takes precedence over the dynamic callback AND over
// the regular measure. Models the Image case: regular measure would return
// intrinsic size; the static `0` says "no min-content contribution per
// CSS-Images" and short-circuits the probe.
TEST(YogaAutoMinSize, static_min_content_width_short_circuits_probe) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 20);
  YGNodeStyleSetHeight(root, 50);

  // Item whose regular measure would return kNaturalWidth (90) under
  // `AtMost 0` (text measurers naturally do that), but the static
  // declaration overrides — min-content is 0.
  YGNodeRef item = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(item, measureWordWrappingText);
  YGNodeSetMinContentWidth(item, 0.0f);
  gMinContentCalls = 0;
  // Set the dynamic callback too — the static value should win and the
  // callback should not be invoked.
  YGNodeSetMinContentMeasureFunc(item, measureMinContentZero);
  YGNodeStyleSetFlexBasis(item, kNaturalWidth);
  YGNodeStyleSetFlexGrow(item, 0);
  YGNodeStyleSetFlexShrink(item, 1);
  YGNodeInsertChild(root, item, 0);

  YGNodeRef spacer = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(spacer, 10);
  YGNodeStyleSetFlexShrink(spacer, 0);
  YGNodeInsertChild(root, spacer, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Static floor = 0; item shrinks to 10 (container 20 - spacer 10).
  EXPECT_FLOAT_EQ(10.0f, YGNodeLayoutGetWidth(item));
  // Dynamic callback should not have been invoked — the static value wins.
  EXPECT_EQ(gMinContentCalls, 0);

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Static min-content on a CONTAINER short-circuits subtree recursion. The
// outer Row probes its child container; the child's static `0` means we
// skip its grandchildren entirely.
TEST(YogaAutoMinSize, static_min_content_short_circuits_container_recursion) {
  YGConfigRef config = makeWebConfig(/*useAutoMinSize=*/true);
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 20);
  YGNodeStyleSetHeight(root, 50);

  // Container item: would normally recurse into its child (kWordWidth = 30)
  // and floor at 30, but static `0` short-circuits before recursion.
  YGNodeRef item = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(item, kNaturalWidth);
  YGNodeStyleSetFlexGrow(item, 0);
  YGNodeStyleSetFlexShrink(item, 1);
  YGNodeSetMinContentWidth(item, 0.0f);
  YGNodeInsertChild(root, item, 0);

  YGNodeRef inner = YGNodeNewWithConfig(config);
  YGNodeSetMeasureFunc(inner, measureWordWrappingText);
  YGNodeInsertChild(item, inner, 0);

  YGNodeRef spacer = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(spacer, 10);
  YGNodeStyleSetFlexShrink(spacer, 0);
  YGNodeInsertChild(root, spacer, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // Container shrinks to 10 (container 20 - spacer 10) instead of being
  // floored at the inner text's kWordWidth = 30.
  EXPECT_FLOAT_EQ(10.0f, YGNodeLayoutGetWidth(item));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}

// Static min-content getter / setter round-trip smoke test.
TEST(YogaAutoMinSize, static_min_content_getter_setter_round_trip) {
  YGNodeRef node = YGNodeNew();
  EXPECT_TRUE(YGFloatIsUndefined(YGNodeGetMinContentWidth(node)));
  EXPECT_TRUE(YGFloatIsUndefined(YGNodeGetMinContentHeight(node)));

  YGNodeSetMinContentWidth(node, 0.0f);
  YGNodeSetMinContentHeight(node, 42.0f);
  EXPECT_FLOAT_EQ(0.0f, YGNodeGetMinContentWidth(node));
  EXPECT_FLOAT_EQ(42.0f, YGNodeGetMinContentHeight(node));

  YGNodeSetMinContentWidth(node, YGUndefined);
  EXPECT_TRUE(YGFloatIsUndefined(YGNodeGetMinContentWidth(node)));
  EXPECT_FLOAT_EQ(42.0f, YGNodeGetMinContentHeight(node));

  YGNodeFree(node);
}

// Errata smoke test: default config carries the legacy bit; clearing it
// enables auto-min, setting it back disables.
TEST(YogaAutoMinSize, errata_bit_round_trips) {
  YGConfigRef config = YGConfigNew();
  EXPECT_NE(
      YGConfigGetErrata(config) & YGErrataMinSizeUndefinedInsteadOfAuto, 0);

  YGConfigSetErrata(
      config,
      static_cast<YGErrata>(
          YGConfigGetErrata(config) & ~YGErrataMinSizeUndefinedInsteadOfAuto));
  EXPECT_EQ(
      YGConfigGetErrata(config) & YGErrataMinSizeUndefinedInsteadOfAuto, 0);

  YGConfigSetErrata(
      config,
      static_cast<YGErrata>(
          YGConfigGetErrata(config) | YGErrataMinSizeUndefinedInsteadOfAuto));
  EXPECT_NE(
      YGConfigGetErrata(config) & YGErrataMinSizeUndefinedInsteadOfAuto, 0);

  YGConfigFree(config);
}
