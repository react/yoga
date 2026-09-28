/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

// Pins Yoga's documented behavior, not the CSS spec: Yoga does not transfer
// the aspect ratio from a resolved max-content axis, so the item takes its
// content width (10). Browsers transfer (height 40 x ratio 2 = width 80);
// that target is recorded in the disabled fixture case
// max_content_with_aspect_ratio in gentest/fixtures/YGMaxContentsTest.html.
// This test also guards the stretch-override guards in
// computeFlexBasisForChild: without them the aspect ratio transfers a
// stretched height that the keyword then discards, producing width 300.
// If aspect-ratio transfer from intrinsic keywords is implemented, update
// this test to 80x40 and enable the fixture case.
//
// Equivalent fixture:
//
// <div style="flex-direction: row; width: 300px; height: 150px;">
//   <div style="height: max-content; aspect-ratio: 2;">
//     <div style="width: 10px; height: 40px;"></div>
//   </div>
// </div>
TEST(MaxContentAspectRatio, no_transfer_from_max_content_axis) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetWidth(root, 300);
  YGNodeStyleSetHeight(root, 150);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetHeightMaxContent(root_child0);
  YGNodeStyleSetAspectRatio(root_child0, 2.0f);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child0_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root_child0_child0, 10);
  YGNodeStyleSetHeight(root_child0_child0, 40);
  YGNodeInsertChild(root_child0, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(10, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(40, YGNodeLayoutGetHeight(root_child0));

  YGNodeFreeRecursive(root);
  YGConfigFree(config);
}
