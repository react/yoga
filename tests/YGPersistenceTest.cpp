/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>
#include <yoga/node/Node.h>

#include "util/TestUtil.h"

using namespace facebook;
using facebook::yoga::test::TestUtil;

TEST(YogaTest, cloning_shared_root) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexGrow(root_child0, 1);
  YGNodeStyleSetFlexBasis(root_child0, 50);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexGrow(root_child1, 1);
  YGNodeInsertChild(root, root_child1, 1);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(75, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(75, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetHeight(root_child1));

  YGNodeRef root2 = YGNodeClone(root);
  YGNodeStyleSetWidth(root2, 100);

  ASSERT_EQ(2u, YGNodeGetChildCount(root2));
  // The children should have referential equality at this point.
  ASSERT_EQ(root_child0, YGNodeGetChild(root2, 0));
  ASSERT_EQ(root_child1, YGNodeGetChild(root2, 1));

  YGNodeCalculateLayout(root2, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(2u, YGNodeGetChildCount(root2));
  // Relayout with no changed input should result in referential equality.
  ASSERT_EQ(root_child0, YGNodeGetChild(root2, 0));
  ASSERT_EQ(root_child1, YGNodeGetChild(root2, 1));

  YGNodeStyleSetWidth(root2, 150);
  YGNodeStyleSetHeight(root2, 200);
  YGNodeCalculateLayout(root2, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(2u, YGNodeGetChildCount(root2));
  // Relayout with changed input should result in cloned children.
  YGNodeRef root2_child0 = YGNodeGetChild(root2, 0);
  YGNodeRef root2_child1 = YGNodeGetChild(root2, 1);
  ASSERT_NE(root_child0, root2_child0);
  ASSERT_NE(root_child1, root2_child1);

  // Everything in the root should remain unchanged.
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetHeight(root));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root_child0));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child0));
  ASSERT_FLOAT_EQ(75, YGNodeLayoutGetHeight(root_child0));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root_child1));
  ASSERT_FLOAT_EQ(75, YGNodeLayoutGetTop(root_child1));
  ASSERT_FLOAT_EQ(100, YGNodeLayoutGetWidth(root_child1));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetHeight(root_child1));

  // The new root now has new layout.
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root2));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root2));
  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetWidth(root2));
  ASSERT_FLOAT_EQ(200, YGNodeLayoutGetHeight(root2));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root2_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetTop(root2_child0));
  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetWidth(root2_child0));
  ASSERT_FLOAT_EQ(125, YGNodeLayoutGetHeight(root2_child0));

  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetLeft(root2_child1));
  ASSERT_FLOAT_EQ(125, YGNodeLayoutGetTop(root2_child1));
  ASSERT_FLOAT_EQ(150, YGNodeLayoutGetWidth(root2_child1));
  ASSERT_FLOAT_EQ(75, YGNodeLayoutGetHeight(root2_child1));

  YGNodeFreeRecursive(root2);

  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, mutating_children_of_a_clone_clones_only_after_layout) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  ASSERT_EQ(0u, YGNodeGetChildCount(root));

  YGNodeRef root2 = YGNodeClone(root);
  ASSERT_EQ(0u, YGNodeGetChildCount(root2));

  YGNodeRef root2_child0 = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root2, root2_child0, 0);

  ASSERT_EQ(0u, YGNodeGetChildCount(root));
  ASSERT_EQ(1u, YGNodeGetChildCount(root2));

  YGNodeRef root3 = YGNodeClone(root2);
  ASSERT_EQ(1u, YGNodeGetChildCount(root2));
  ASSERT_EQ(1u, YGNodeGetChildCount(root3));
  ASSERT_EQ(YGNodeGetChild(root2, 0), YGNodeGetChild(root3, 0));

  YGNodeRef root3_child1 = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root3, root3_child1, 1);
  ASSERT_EQ(1u, YGNodeGetChildCount(root2));
  ASSERT_EQ(2u, YGNodeGetChildCount(root3));
  ASSERT_EQ(root3_child1, YGNodeGetChild(root3, 1));
  ASSERT_EQ(YGNodeGetChild(root2, 0), YGNodeGetChild(root3, 0));

  YGNodeRef root4 = YGNodeClone(root3);
  ASSERT_EQ(root3_child1, YGNodeGetChild(root4, 1));

  YGNodeRemoveChild(root4, root3_child1);
  ASSERT_EQ(2u, YGNodeGetChildCount(root3));
  ASSERT_EQ(1u, YGNodeGetChildCount(root4));
  ASSERT_EQ(YGNodeGetChild(root3, 0), YGNodeGetChild(root4, 0));

  YGNodeCalculateLayout(root4, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_NE(YGNodeGetChild(root3, 0), YGNodeGetChild(root4, 0));
  YGNodeCalculateLayout(root3, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_NE(YGNodeGetChild(root2, 0), YGNodeGetChild(root3, 0));

  YGNodeFreeRecursive(root4);
  YGNodeFreeRecursive(root3);
  YGNodeFreeRecursive(root2);
  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, cloning_two_levels) {
  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexGrow(root_child0, 1);
  YGNodeStyleSetFlexBasis(root_child0, 15);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexGrow(root_child1, 1);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeRef root_child1_0 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(root_child1_0, 10);
  YGNodeStyleSetFlexGrow(root_child1_0, 1);
  YGNodeInsertChild(root_child1, root_child1_0, 0);

  YGNodeRef root_child1_1 = YGNodeNewWithConfig(config);
  YGNodeStyleSetFlexBasis(root_child1_1, 25);
  YGNodeInsertChild(root_child1, root_child1_1, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_FLOAT_EQ(40, YGNodeLayoutGetHeight(root_child0));
  ASSERT_FLOAT_EQ(60, YGNodeLayoutGetHeight(root_child1));
  ASSERT_FLOAT_EQ(35, YGNodeLayoutGetHeight(root_child1_0));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetHeight(root_child1_1));

  YGNodeRef root2_child0 = YGNodeClone(root_child0);
  YGNodeRef root2_child1 = YGNodeClone(root_child1);
  YGNodeRef root2 = YGNodeClone(root);

  YGNodeStyleSetFlexGrow(root2_child0, 0);
  YGNodeStyleSetFlexBasis(root2_child0, 40);

  YGNodeRemoveAllChildren(root2);
  YGNodeInsertChild(root2, root2_child0, 0);
  YGNodeInsertChild(root2, root2_child1, 1);
  ASSERT_EQ(2u, YGNodeGetChildCount(root2));

  YGNodeCalculateLayout(root2, YGUndefined, YGUndefined, YGDirectionLTR);

  // Original root is unchanged
  ASSERT_FLOAT_EQ(40, YGNodeLayoutGetHeight(root_child0));
  ASSERT_FLOAT_EQ(60, YGNodeLayoutGetHeight(root_child1));
  ASSERT_FLOAT_EQ(35, YGNodeLayoutGetHeight(root_child1_0));
  ASSERT_FLOAT_EQ(25, YGNodeLayoutGetHeight(root_child1_1));

  // New root has new layout at the top
  ASSERT_FLOAT_EQ(40, YGNodeLayoutGetHeight(root2_child0));
  ASSERT_FLOAT_EQ(60, YGNodeLayoutGetHeight(root2_child1));

  // The deeper children are untouched.
  ASSERT_EQ(YGNodeGetChild(root2_child1, 0), root_child1_0);
  ASSERT_EQ(YGNodeGetChild(root2_child1, 1), root_child1_1);

  YGNodeFreeRecursive(root2);
  YGNodeFreeRecursive(root);

  YGConfigFree(config);
}

TEST(YogaTest, cloning_and_freeing) {
  TestUtil::startCountingNodes();

  YGConfigRef config = YGConfigNew();

  YGNodeRef root = YGNodeNewWithConfig(config);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);
  YGNodeRef root_child0 = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root, root_child0, 0);
  YGNodeRef root_child1 = YGNodeNewWithConfig(config);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeRef root2 = YGNodeClone(root);

  // Freeing the original root should be safe as long as we don't free its
  // children.
  YGNodeFree(root);

  YGNodeCalculateLayout(root2, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeFreeRecursive(root2);

  YGNodeFree(root_child0);
  YGNodeFree(root_child1);

  YGConfigFree(config);

  ASSERT_EQ(0, TestUtil::stopCountingNodes());
}

TEST(YogaTest, mixed_shared_and_owned_children) {
  // Don't try this at home!

  YGNodeRef root0 = YGNodeNew();
  YGNodeRef root1 = YGNodeNew();

  YGNodeRef root0_child0 = YGNodeNew();
  YGNodeRef root0_child0_0 = YGNodeNew();
  YGNodeInsertChild(root0, root0_child0, 0);
  YGNodeInsertChild(root0_child0, root0_child0_0, 0);

  YGNodeRef root1_child0 = YGNodeNew();
  YGNodeRef root1_child2 = YGNodeNew();
  YGNodeInsertChild(root1, root1_child0, 0);
  YGNodeInsertChild(root1, root1_child2, 1);

  // Build the child list without vector::insert. GCC 16 at -O2/-O3 reports a
  // false array-bounds warning on insert(begin()+1, ...) and the test is
  // compiled with -Werror.
  auto* node1 = static_cast<yoga::Node*>(root1);
  std::vector<yoga::Node*> children{
      node1->getChild(0),
      static_cast<yoga::Node*>(root0_child0),
      node1->getChild(1),
  };
  node1->setChildren(children);

  auto secondChild = YGNodeGetChild(root1, 1);
  ASSERT_EQ(secondChild, YGNodeGetChild(root0, 0));
  ASSERT_EQ(YGNodeGetChild(secondChild, 0), YGNodeGetChild(root0_child0, 0));

  YGNodeCalculateLayout(root1, YGUndefined, YGUndefined, YGDirectionLTR);
  secondChild = YGNodeGetChild(root1, 1);
  ASSERT_NE(secondChild, YGNodeGetChild(root0, 0));
  ASSERT_EQ(YGNodeGetOwner(secondChild), root1);
  ASSERT_NE(YGNodeGetChild(secondChild, 0), YGNodeGetChild(root0_child0, 0));
  ASSERT_EQ(YGNodeGetOwner(YGNodeGetChild(secondChild, 0)), secondChild);
}
