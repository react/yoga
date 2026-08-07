/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Regression test for https://github.com/facebook/yoga/issues/1665
// flexBasis:0 + flexShrink:1 + borderWidth + minWidth produces an astronomically
// large width (~1.65e11) instead of being clamped to minWidth.

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

TEST(YGFlexShrinkBorderBug, flex_basis_0_border_minwidth_row) {
  YGConfigRef config = YGConfigNew();
  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 393.0f);
  YGNodeStyleSetHeight(root, 100.0f);

  // Four row children: flexBasis:0, flexGrow:1, flexShrink:1, minWidth:160
  // First child has borderWidth:0.594443, rest 0.594442.
  // Bug: first child (and all others) get width ~1.65e11 instead of 160.
  float borders[] = {0.594443f, 0.594442f, 0.594442f, 0.594442f};
  for (int i = 0; i < 4; i++) {
    YGNodeRef child = YGNodeNewWithConfig(config);
    YGNodeStyleSetFlexBasis(child, 0.0f);
    YGNodeStyleSetFlexGrow(child, 1.0f);
    YGNodeStyleSetFlexShrink(child, 1.0f);
    YGNodeStyleSetMinWidth(child, 160.0f);
    YGNodeStyleSetBorder(child, YGEdgeAll, borders[i]);
    YGNodeInsertChild(root, child, i);
  }

  YGNodeCalculateLayout(root, 393.0f, 100.0f, YGDirectionLTR);

  for (int i = 0; i < 4; i++) {
    YGNodeRef child = YGNodeGetChild(root, i);
    EXPECT_GE(YGNodeLayoutGetWidth(child), 160.0f)
        << "child[" << i << "] width below minWidth";
    EXPECT_LE(YGNodeLayoutGetWidth(child), 200.0f)
        << "child[" << i << "] width is astronomically large (bug)";
  }

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
