/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>

TEST(YogaTest, dirty_propagation) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 20);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNew();
  YGNodeStyleSetWidth(root_child1, 50);
  YGNodeStyleSetHeight(root_child1, 20);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeStyleSetWidth(root_child0, 20);

  EXPECT_TRUE(YGNodeIsDirty(root_child0));
  EXPECT_FALSE(YGNodeIsDirty(root_child1));
  EXPECT_TRUE(YGNodeIsDirty(root));

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FALSE(YGNodeIsDirty(root_child0));
  EXPECT_FALSE(YGNodeIsDirty(root_child1));
  EXPECT_FALSE(YGNodeIsDirty(root));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dirty_propagation_only_if_prop_changed) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 20);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNew();
  YGNodeStyleSetWidth(root_child1, 50);
  YGNodeStyleSetHeight(root_child1, 20);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeStyleSetWidth(root_child0, 50);

  EXPECT_FALSE(YGNodeIsDirty(root_child0));
  EXPECT_FALSE(YGNodeIsDirty(root_child1));
  EXPECT_FALSE(YGNodeIsDirty(root));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dirty_propagation_changing_layout_config) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 20);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNew();
  YGNodeStyleSetWidth(root_child1, 50);
  YGNodeStyleSetHeight(root_child1, 20);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeRef root_child0_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0_child0, 25);
  YGNodeStyleSetHeight(root_child0_child0, 20);
  YGNodeInsertChild(root, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FALSE(YGNodeIsDirty(root));
  EXPECT_FALSE(YGNodeIsDirty(root_child0));
  EXPECT_FALSE(YGNodeIsDirty(root_child1));
  EXPECT_FALSE(YGNodeIsDirty(root_child0_child0));

  YGConfigRef newConfig = YGConfigNew();
  YGConfigSetErrata(newConfig, YGErrataStretchFlexBasis);
  YGNodeSetConfig(root_child0, newConfig);

  EXPECT_TRUE(YGNodeIsDirty(root));
  EXPECT_TRUE(YGNodeIsDirty(root_child0));
  EXPECT_FALSE(YGNodeIsDirty(root_child1));
  EXPECT_FALSE(YGNodeIsDirty(root_child0_child0));

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FALSE(YGNodeIsDirty(root));
  EXPECT_FALSE(YGNodeIsDirty(root_child0));
  EXPECT_FALSE(YGNodeIsDirty(root_child1));
  EXPECT_FALSE(YGNodeIsDirty(root_child0_child0));

  YGConfigFree(newConfig);
  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dirty_propagation_changing_benign_config) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef root_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0, 50);
  YGNodeStyleSetHeight(root_child0, 20);
  YGNodeInsertChild(root, root_child0, 0);

  YGNodeRef root_child1 = YGNodeNew();
  YGNodeStyleSetWidth(root_child1, 50);
  YGNodeStyleSetHeight(root_child1, 20);
  YGNodeInsertChild(root, root_child1, 1);

  YGNodeRef root_child0_child0 = YGNodeNew();
  YGNodeStyleSetWidth(root_child0_child0, 25);
  YGNodeStyleSetHeight(root_child0_child0, 20);
  YGNodeInsertChild(root, root_child0_child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FALSE(YGNodeIsDirty(root));
  EXPECT_FALSE(YGNodeIsDirty(root_child0));
  EXPECT_FALSE(YGNodeIsDirty(root_child1));
  EXPECT_FALSE(YGNodeIsDirty(root_child0_child0));

  YGConfigRef newConfig = YGConfigNew();
  YGConfigSetLogger(
      newConfig,
      [](YGConfigConstRef, YGNodeConstRef, YGLogLevel, const char*, va_list) {
        return 0;
      });
  YGNodeSetConfig(root_child0, newConfig);

  EXPECT_FALSE(YGNodeIsDirty(root));
  EXPECT_FALSE(YGNodeIsDirty(root_child0));
  EXPECT_FALSE(YGNodeIsDirty(root_child1));
  EXPECT_FALSE(YGNodeIsDirty(root_child0_child0));

  YGConfigFree(newConfig);
  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dirty_mark_all_children_as_dirty_when_display_changes) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef child0 = YGNodeNew();
  YGNodeStyleSetFlexGrow(child0, 1);
  YGNodeRef child1 = YGNodeNew();
  YGNodeStyleSetFlexGrow(child1, 1);

  YGNodeRef child1_child0 = YGNodeNew();
  YGNodeRef child1_child0_child0 = YGNodeNew();
  YGNodeStyleSetWidth(child1_child0_child0, 8);
  YGNodeStyleSetHeight(child1_child0_child0, 16);

  YGNodeInsertChild(child1_child0, child1_child0_child0, 0);

  YGNodeInsertChild(child1, child1_child0, 0);
  YGNodeInsertChild(root, child0, 0);
  YGNodeInsertChild(root, child1, 0);

  YGNodeStyleSetDisplay(child0, YGDisplayFlex);
  YGNodeStyleSetDisplay(child1, YGDisplayNone);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetWidth(child1_child0_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetHeight(child1_child0_child0));

  YGNodeStyleSetDisplay(child0, YGDisplayNone);
  YGNodeStyleSetDisplay(child1, YGDisplayFlex);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(8, YGNodeLayoutGetWidth(child1_child0_child0));
  ASSERT_FLOAT_EQ(16, YGNodeLayoutGetHeight(child1_child0_child0));

  YGNodeStyleSetDisplay(child0, YGDisplayFlex);
  YGNodeStyleSetDisplay(child1, YGDisplayNone);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetWidth(child1_child0_child0));
  ASSERT_FLOAT_EQ(0, YGNodeLayoutGetHeight(child1_child0_child0));

  YGNodeStyleSetDisplay(child0, YGDisplayNone);
  YGNodeStyleSetDisplay(child1, YGDisplayFlex);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  ASSERT_FLOAT_EQ(8, YGNodeLayoutGetWidth(child1_child0_child0));
  ASSERT_FLOAT_EQ(16, YGNodeLayoutGetHeight(child1_child0_child0));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dirty_node_only_if_children_are_actually_removed) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetAlignItems(root, YGAlignFlexStart);
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);

  YGNodeRef child0 = YGNodeNew();
  YGNodeStyleSetWidth(child0, 50);
  YGNodeStyleSetHeight(child0, 25);
  YGNodeInsertChild(root, child0, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  YGNodeRef child1 = YGNodeNew();
  YGNodeRemoveChild(root, child1);
  EXPECT_FALSE(YGNodeIsDirty(root));
  YGNodeFree(child1);

  YGNodeRemoveChild(root, child0);
  EXPECT_TRUE(YGNodeIsDirty(root));
  YGNodeFree(child0);

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dirty_node_only_if_undefined_values_gets_set_to_undefined) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 50);
  YGNodeStyleSetHeight(root, 50);
  YGNodeStyleSetMinWidth(root, YGUndefined);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  EXPECT_FALSE(YGNodeIsDirty(root));

  YGNodeStyleSetMinWidth(root, YGUndefined);

  EXPECT_FALSE(YGNodeIsDirty(root));

  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dirty_removed_child_node) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef child = YGNodeNew();
  YGNodeStyleSetWidth(child, 50);
  YGNodeStyleSetHeight(child, 50);
  YGNodeInsertChild(root, child, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FALSE(YGNodeIsDirty(child));

  YGNodeRemoveChild(root, child);

  // Child should be marked dirty after removal so layout is recalculated
  // when the child is reused (e.g., in a recycling view system)
  EXPECT_TRUE(YGNodeIsDirty(child));

  YGNodeFree(child);
  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dirty_removed_child_nodes_when_removing_all) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef child0 = YGNodeNew();
  YGNodeStyleSetWidth(child0, 50);
  YGNodeStyleSetHeight(child0, 25);
  YGNodeInsertChild(root, child0, 0);

  YGNodeRef child1 = YGNodeNew();
  YGNodeStyleSetWidth(child1, 50);
  YGNodeStyleSetHeight(child1, 25);
  YGNodeInsertChild(root, child1, 1);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_FALSE(YGNodeIsDirty(child0));
  EXPECT_FALSE(YGNodeIsDirty(child1));

  YGNodeRemoveAllChildren(root);

  // All children should be marked dirty after removal
  EXPECT_TRUE(YGNodeIsDirty(child0));
  EXPECT_TRUE(YGNodeIsDirty(child1));

  YGNodeFree(child0);
  YGNodeFree(child1);
  YGNodeFreeRecursive(root);
}

TEST(YogaTest, dirty_parent_when_child_freed) {
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 100);
  YGNodeStyleSetHeight(root, 100);

  YGNodeRef child = YGNodeNew();
  YGNodeStyleSetWidth(child, 50);
  YGNodeStyleSetHeight(child, 50);
  YGNodeInsertChild(root, child, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  EXPECT_FALSE(YGNodeIsDirty(root));

  YGNodeFree(child);

  EXPECT_TRUE(YGNodeIsDirty(root));
  YGNodeFree(root);
}

TEST(YogaTest, dirty_parent_when_subtree_freed_recursive) {
  YGNodeRef root = YGNodeNew();
  YGNodeRef child = YGNodeNew();
  YGNodeRef grandchild = YGNodeNew();
  YGNodeInsertChild(root, child, 0);
  YGNodeInsertChild(child, grandchild, 0);

  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  EXPECT_FALSE(YGNodeIsDirty(root));

  YGNodeFreeRecursive(child);

  EXPECT_TRUE(YGNodeIsDirty(root));
  YGNodeFree(root);
}
