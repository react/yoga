/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

namespace facebook::yoga {

// Regression test for `cleanupContentsNodesRecursively` stamping
// `hasNewLayout=true` on a display:contents child during a measurement-only
// visit.
//
// Setup: Root (overflow=visible, flex column) -> Parent (flex-grow=1)
//        -> Contents (display:contents) -> Leaf.
// Flipping root's overflow between the two passes invalidates Parent's
// measurement cache (computeFlexBasisForChild's `applyHeightFitContent`
// branch flips) but leaves Parent's layout cache intact (its allotment is
// unchanged). So in pass 2, Parent's calculateLayoutImpl runs only with
// performLayout=false - the layout-phase visit is served from cache and
// `cleanupContentsNodesRecursively` never runs at performLayout=true.
TEST(YogaTest, contents_child_hasNewLayout_not_stamped_on_measure_only_visit) {
  YGNodeRef leaf = YGNodeNew();
  YGNodeStyleSetWidth(leaf, 20);
  YGNodeStyleSetHeight(leaf, 20);

  YGNodeRef contents = YGNodeNew();
  YGNodeStyleSetDisplay(contents, YGDisplayContents);
  YGNodeInsertChild(contents, leaf, 0);

  YGNodeRef parent = YGNodeNew();
  YGNodeStyleSetFlexGrow(parent, 1);
  YGNodeInsertChild(parent, contents, 0);

  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);
  YGNodeStyleSetOverflow(root, YGOverflowVisible);
  YGNodeInsertChild(root, parent, 0);

  YGNodeCalculateLayout(root, 200, 200, YGDirectionLTR);

  // Simulate a consumer (e.g. React Native's layout pass) reading and
  // clearing the hasNewLayout flags.
  YGNodeSetHasNewLayout(root, false);
  YGNodeSetHasNewLayout(parent, false);
  YGNodeSetHasNewLayout(contents, false);
  YGNodeSetHasNewLayout(leaf, false);

  YGNodeStyleSetOverflow(root, YGOverflowScroll);
  YGNodeCalculateLayout(root, 200, 200, YGDirectionLTR);

  EXPECT_FALSE(YGNodeGetHasNewLayout(contents))
      << "contents.hasNewLayout was stamped during a measure-only visit "
         "(cleanupContentsNodesRecursively ran with performLayout=false but "
         "no matching performLayout=true visit occurred this pass)";

  YGNodeFreeRecursive(root);
}

// Regression test for `cleanupContentsNodesRecursively` invoked from
// `layoutAbsoluteDescendants`: it must stamp `hasNewLayout=true` on
// display:contents children on the path to an absolute descendant whose
// position changed this pass. Otherwise consumers traversing the tree via
// hasNewLayout would skip the contents subtree and miss the update.
//
// Setup: root (containing block) -> staticChild (fixed 50x50)
//        -> contents (display:contents) -> absoluteChild (right/bottom-
//        anchored so its position depends on the containing block).
// Growing root in pass 2 dirties only root. staticChild's fixed dimensions
// make its layout cache hit, so its main-path cleanup never runs.
// absoluteChild depends on the containing block and is repositioned by
// `layoutAbsoluteDescendants`, which is the only path that can stamp
// contents along the way.
TEST(
    YogaTest,
    absolute_descendant_through_contents_is_reachable_via_hasNewLayout) {
  YGNodeRef absoluteChild = YGNodeNew();
  YGNodeStyleSetPositionType(absoluteChild, YGPositionTypeAbsolute);
  YGNodeStyleSetPosition(absoluteChild, YGEdgeRight, 0);
  YGNodeStyleSetPosition(absoluteChild, YGEdgeBottom, 0);
  YGNodeStyleSetWidth(absoluteChild, 10);
  YGNodeStyleSetHeight(absoluteChild, 10);

  YGNodeRef contents = YGNodeNew();
  YGNodeStyleSetDisplay(contents, YGDisplayContents);
  YGNodeInsertChild(contents, absoluteChild, 0);

  YGNodeRef staticChild = YGNodeNew();
  YGNodeStyleSetPositionType(staticChild, YGPositionTypeStatic);
  YGNodeStyleSetWidth(staticChild, 50);
  YGNodeStyleSetHeight(staticChild, 50);
  YGNodeInsertChild(staticChild, contents, 0);

  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);
  YGNodeInsertChild(root, staticChild, 0);

  YGNodeCalculateLayout(root, 100, 100, YGDirectionLTR);

  // Simulate a consumer (e.g. React Native's layout pass) reading and
  // clearing the hasNewLayout flags.
  YGNodeSetHasNewLayout(root, false);
  YGNodeSetHasNewLayout(staticChild, false);
  YGNodeSetHasNewLayout(contents, false);
  YGNodeSetHasNewLayout(absoluteChild, false);

  YGNodeStyleSetWidth(root, 150);
  YGNodeCalculateLayout(root, 150, 100, YGDirectionLTR);

  ASSERT_TRUE(YGNodeGetHasNewLayout(absoluteChild));
  EXPECT_TRUE(YGNodeGetHasNewLayout(staticChild));
  EXPECT_TRUE(YGNodeGetHasNewLayout(contents))
      << "contents node on the path to a freshly-positioned absolute "
         "descendant must have hasNewLayout=true so consumers can traverse "
         "to it";

  YGNodeFreeRecursive(root);
}

// Regression test for `cleanupContentsNodesRecursively` invoked from
// `layoutAbsoluteDescendants`: it must not stamp `hasNewLayout=true` on
// display:contents children when no new layout was produced for their
// parent this pass. Otherwise the stale flag survives across passes and
// can be observed by a later cache-hit on the parent.
//
// Setup: root -> a (fixed 50x50) -> b (fixed 30x30)
//        -> contents (display:contents) -> leaf.
// Flipping root's overflow in pass 2 dirties only root. a and b have fixed
// sizes so their layout caches hit; a.calculateLayoutImpl is skipped, so
// b.calculateLayoutInternal is never invoked. `layoutAbsoluteDescendants`
// still walks down through a and b looking for absolute descendants, but
// there are none beneath b - so b.hasNewLayout stays false and the
// cleanup along that walk must leave contents unflagged.
TEST(
    YogaTest,
    absolute_phase_cleanup_does_not_stamp_when_parent_layout_skipped) {
  YGNodeRef leaf = YGNodeNew();
  YGNodeStyleSetWidth(leaf, 10);
  YGNodeStyleSetHeight(leaf, 10);

  YGNodeRef contents = YGNodeNew();
  YGNodeStyleSetDisplay(contents, YGDisplayContents);
  YGNodeInsertChild(contents, leaf, 0);

  YGNodeRef b = YGNodeNew();
  YGNodeStyleSetPositionType(b, YGPositionTypeStatic);
  YGNodeStyleSetWidth(b, 30);
  YGNodeStyleSetHeight(b, 30);
  YGNodeInsertChild(b, contents, 0);

  YGNodeRef a = YGNodeNew();
  YGNodeStyleSetPositionType(a, YGPositionTypeStatic);
  YGNodeStyleSetWidth(a, 50);
  YGNodeStyleSetHeight(a, 50);
  YGNodeInsertChild(a, b, 0);

  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 200);
  YGNodeStyleSetHeight(root, 200);
  YGNodeStyleSetOverflow(root, YGOverflowVisible);
  YGNodeInsertChild(root, a, 0);

  YGNodeCalculateLayout(root, 200, 200, YGDirectionLTR);

  // Simulate a consumer (e.g. React Native's layout pass) reading and
  // clearing the hasNewLayout flags.
  YGNodeSetHasNewLayout(root, false);
  YGNodeSetHasNewLayout(a, false);
  YGNodeSetHasNewLayout(b, false);
  YGNodeSetHasNewLayout(contents, false);
  YGNodeSetHasNewLayout(leaf, false);

  YGNodeStyleSetOverflow(root, YGOverflowScroll);
  YGNodeCalculateLayout(root, 200, 200, YGDirectionLTR);

  EXPECT_FALSE(YGNodeGetHasNewLayout(b));
  EXPECT_FALSE(YGNodeGetHasNewLayout(contents))
      << "contents.hasNewLayout was stamped during a walk where its "
         "parent's hasNewLayout remained false this pass";

  YGNodeFreeRecursive(root);
}

} // namespace facebook::yoga
