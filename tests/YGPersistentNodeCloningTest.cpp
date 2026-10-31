/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>
#include <yoga/config/Config.h>
#include <yoga/node/Node.h>

#include <functional>
#include <memory>
#include <vector>

namespace facebook::yoga {

struct YGPersistentNodeCloningTest : public ::testing::Test {
  struct NodeWrapper {
    explicit NodeWrapper(
        YGConfigRef config,
        std::vector<std::shared_ptr<NodeWrapper>> children = {})
        : node{YGNodeNewWithConfig(config)}, children{std::move(children)} {
      YGNodeSetContext(node, this);

      auto privateNode = resolveRef(node);
      for (const auto& child : this->children) {
        auto privateChild = resolveRef(child->node);
        // Claim first ownership of not yet owned nodes, to avoid immediately
        // cloning them
        if (YGNodeGetOwner(child->node) == nullptr) {
          privateChild->setOwner(privateNode);
        }
        privateNode->insertChild(privateChild, privateNode->getChildCount());
      }
    }

    // Clone, with current children, for mutation
    NodeWrapper(const NodeWrapper& other)
        : node{YGNodeClone(other.node)}, children{other.children} {
      YGNodeSetContext(node, this);

      auto privateNode = resolveRef(node);
      privateNode->setOwner(nullptr);
    }

    // Clone, with new children
    NodeWrapper(
        const NodeWrapper& other,
        std::vector<std::shared_ptr<NodeWrapper>> children)
        : node{YGNodeClone(other.node)}, children{std::move(children)} {
      YGNodeSetContext(node, this);

      auto privateNode = resolveRef(node);
      privateNode->setOwner(nullptr);
      privateNode->setChildren({});
      privateNode->setDirty(true);

      for (const auto& child : this->children) {
        auto privateChild = resolveRef(child->node);
        // Claim first ownership of not yet owned nodes, to avoid immediately
        // cloning them
        if (YGNodeGetOwner(child->node) == nullptr) {
          privateChild->setOwner(privateNode);
        }
        privateNode->insertChild(privateChild, privateNode->getChildCount());
      }
    }

    NodeWrapper(NodeWrapper&&) = delete;

    ~NodeWrapper() {
      YGNodeFree(node);
    }

    NodeWrapper& operator=(const NodeWrapper& other) = delete;
    NodeWrapper& operator=(NodeWrapper&& other) = delete;

    YGNodeRef node;
    std::vector<std::shared_ptr<NodeWrapper>> children;
  };

  struct ConfigWrapper {
    ConfigWrapper() {
      YGConfigSetCloneNodeFunc(
          config,
          [](YGNodeConstRef oldNode, YGNodeConstRef owner, size_t childIndex) {
            onClone(oldNode, owner, childIndex);
            auto wrapper = static_cast<NodeWrapper*>(YGNodeGetContext(owner));
            auto old = static_cast<NodeWrapper*>(YGNodeGetContext(oldNode));

            wrapper->children[childIndex] = std::make_shared<NodeWrapper>(*old);
            return wrapper->children[childIndex]->node;
          });
    }

    ConfigWrapper(const ConfigWrapper&) = delete;
    ConfigWrapper(ConfigWrapper&&) = delete;

    ~ConfigWrapper() {
      YGConfigFree(config);
    }

    ConfigWrapper& operator=(const ConfigWrapper&) = delete;
    ConfigWrapper& operator=(ConfigWrapper&&) = delete;

    YGConfigRef config{YGConfigNew()};
  };

  ConfigWrapper configWrapper;
  YGConfigRef config{configWrapper.config};

  void SetUp() override {
    onClone = [](...) {};
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static inline std::function<void(YGNodeConstRef, YGNodeConstRef, size_t)>
      onClone;
};

TEST_F(
    YGPersistentNodeCloningTest,
    changing_sibling_height_does_not_clone_neighbors) {
  // <ScrollView>
  //   <View id="Sibling" style={{ height: 1 }} />
  //   <View id="A" style={{ height: 1 }}>
  //     <View id="B">
  //       <View id="C">
  //         <View id="D"/>
  //       </View>
  //     </View>
  //   </View>
  // </ScrollView>

  auto sibling = std::make_shared<NodeWrapper>(config);
  YGNodeStyleSetHeight(sibling->node, 1);

  auto d = std::make_shared<NodeWrapper>(config);
  auto c = std::make_shared<NodeWrapper>(config, std::vector{d});
  auto b = std::make_shared<NodeWrapper>(config, std::vector{c});
  auto a = std::make_shared<NodeWrapper>(config, std::vector{b});
  YGNodeStyleSetHeight(a->node, 1);

  auto scrollContentView =
      std::make_shared<NodeWrapper>(config, std::vector{sibling, a});
  YGNodeStyleSetPositionType(scrollContentView->node, YGPositionTypeAbsolute);

  auto scrollView =
      std::make_shared<NodeWrapper>(config, std::vector{scrollContentView});
  YGNodeStyleSetWidth(scrollView->node, 100);
  YGNodeStyleSetHeight(scrollView->node, 100);

  // We don't expect any cloning during the first layout
  onClone = [](...) { FAIL(); };

  YGNodeCalculateLayout(
      scrollView->node, YGUndefined, YGUndefined, YGDirectionLTR);

  auto siblingPrime = std::make_shared<NodeWrapper>(config);
  YGNodeStyleSetHeight(siblingPrime->node, 2);

  auto scrollContentViewPrime = std::make_shared<NodeWrapper>(
      *scrollContentView, std::vector{siblingPrime, a});
  auto scrollViewPrime = std::make_shared<NodeWrapper>(
      *scrollView, std::vector{scrollContentViewPrime});

  std::vector<NodeWrapper*> nodesCloned;
  // We should only need to clone "A"
  onClone = [&](YGNodeConstRef oldNode,
                YGNodeConstRef /*owner*/,
                size_t /*childIndex*/) {
    nodesCloned.push_back(static_cast<NodeWrapper*>(YGNodeGetContext(oldNode)));
  };

  YGNodeCalculateLayout(
      scrollViewPrime->node, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_EQ(nodesCloned.size(), 1);
  EXPECT_EQ(nodesCloned[0], a.get());
}

TEST_F(YGPersistentNodeCloningTest, clone_leaf_display_contents_node) {
  // <View id="A">
  //   <View id="B" style={{ display: 'contents' }} />
  // </View>

  auto b = std::make_shared<NodeWrapper>(config);
  auto a = std::make_shared<NodeWrapper>(config, std::vector{b});
  YGNodeStyleSetDisplay(b->node, YGDisplayContents);

  // We don't expect any cloning during the first layout
  onClone = [](...) { FAIL(); };

  YGNodeCalculateLayout(a->node, YGUndefined, YGUndefined, YGDirectionLTR);

  auto aPrime = std::make_shared<NodeWrapper>(config, std::vector{b});

  std::vector<NodeWrapper*> nodesCloned;
  // We should clone "C"
  onClone = [&](YGNodeConstRef oldNode,
                YGNodeConstRef /*owner*/,
                size_t /*childIndex*/) {
    nodesCloned.push_back(static_cast<NodeWrapper*>(YGNodeGetContext(oldNode)));
  };

  YGNodeCalculateLayout(aPrime->node, 100, 100, YGDirectionLTR);

  EXPECT_EQ(nodesCloned.size(), 1);
  EXPECT_EQ(nodesCloned[0], b.get());
}

// Clears `hasNewLayout` like an embedder applying the layout would. Like
// React Native, it doesn't descend into display: none nodes, which have no
// layout to apply.
static void consumeNewLayout(YGNodeRef node) {
  YGNodeSetHasNewLayout(node, false);
  if (YGNodeStyleGetDisplay(node) == YGDisplayNone) {
    return;
  }
  for (size_t i = 0; i < YGNodeGetChildCount(node); i++) {
    consumeNewLayout(YGNodeGetChild(node, i));
  }
}

static void expectSameLayout(YGNodeRef actual, YGNodeRef expected) {
  EXPECT_EQ(YGNodeLayoutGetLeft(actual), YGNodeLayoutGetLeft(expected));
  EXPECT_EQ(YGNodeLayoutGetTop(actual), YGNodeLayoutGetTop(expected));
  EXPECT_EQ(YGNodeLayoutGetWidth(actual), YGNodeLayoutGetWidth(expected));
  EXPECT_EQ(YGNodeLayoutGetHeight(actual), YGNodeLayoutGetHeight(expected));
  ASSERT_EQ(YGNodeGetChildCount(actual), YGNodeGetChildCount(expected));
  for (size_t i = 0; i < YGNodeGetChildCount(actual); i++) {
    expectSameLayout(YGNodeGetChild(actual, i), YGNodeGetChild(expected, i));
  }
}

static void expectZeroedOut(YGNodeRef node) {
  EXPECT_EQ(YGNodeLayoutGetWidth(node), 0);
  EXPECT_EQ(YGNodeLayoutGetHeight(node), 0);
  EXPECT_TRUE(YGNodeGetHasNewLayout(node));
}

TEST_F(
    YGPersistentNodeCloningTest,
    relayout_does_not_clone_zeroed_out_display_none_subtree) {
  // <View id="Root">
  //   <View id="Sibling" style={{ height: 1 }} />
  //   <View id="Hidden" style={{ display: 'none' }}>
  //     <View id="A">
  //       <View id="B" />
  //       <View id="C">
  //         <View id="D" />
  //       </View>
  //     </View>
  //   </View>
  // </View>

  auto sibling = std::make_shared<NodeWrapper>(config);
  YGNodeStyleSetHeight(sibling->node, 1);

  auto d = std::make_shared<NodeWrapper>(config);
  auto c = std::make_shared<NodeWrapper>(config, std::vector{d});
  auto b = std::make_shared<NodeWrapper>(config);
  auto a = std::make_shared<NodeWrapper>(config, std::vector{b, c});
  auto hidden = std::make_shared<NodeWrapper>(config, std::vector{a});
  YGNodeStyleSetDisplay(hidden->node, YGDisplayNone);

  auto root =
      std::make_shared<NodeWrapper>(config, std::vector{sibling, hidden});
  YGNodeStyleSetWidth(root->node, 100);
  YGNodeStyleSetHeight(root->node, 100);

  // We don't expect any cloning during the first layout
  onClone = [](...) { FAIL(); };

  YGNodeCalculateLayout(root->node, YGUndefined, YGUndefined, YGDirectionLTR);
  consumeNewLayout(root->node);

  std::vector<NodeWrapper*> nodesCloned;
  onClone = [&](YGNodeConstRef oldNode,
                YGNodeConstRef /*owner*/,
                size_t /*childIndex*/) {
    nodesCloned.push_back(static_cast<NodeWrapper*>(YGNodeGetContext(oldNode)));
  };

  // Resizing "Sibling" lays out "Root" again on every pass. "Hidden" is shared
  // with the earlier tree, so it is cloned, but its zeroed out descendants
  // should not be.
  auto previousRoot = root;
  for (int pass = 0; pass < 2; pass++) {
    auto previousHidden = previousRoot->children[1];

    auto siblingPrime = std::make_shared<NodeWrapper>(config);
    YGNodeStyleSetHeight(siblingPrime->node, 2 + static_cast<float>(pass));
    auto rootPrime = std::make_shared<NodeWrapper>(
        *previousRoot, std::vector{siblingPrime, previousHidden});

    nodesCloned.clear();
    YGNodeCalculateLayout(
        rootPrime->node, YGUndefined, YGUndefined, YGDirectionLTR);
    consumeNewLayout(rootPrime->node);

    ASSERT_EQ(nodesCloned.size(), 1);
    EXPECT_EQ(nodesCloned[0], previousHidden.get());
    previousRoot = rootPrime;
  }

  expectZeroedOut(a->node);
  expectZeroedOut(b->node);
  expectZeroedOut(c->node);
  expectZeroedOut(d->node);
}

TEST_F(
    YGPersistentNodeCloningTest,
    adding_node_to_display_none_subtree_does_not_clone_zeroed_out_subtrees) {
  // <View id="Root">
  //   <View id="Hidden" style={{ display: 'none' }}>
  //     <View id="A">
  //       <View id="B">
  //         <View id="C" />
  //       </View>
  //     </View>
  //     <View id="D">
  //       <View id="E" />
  //     </View>
  //   </View>
  // </View>

  auto e = std::make_shared<NodeWrapper>(config);
  auto d = std::make_shared<NodeWrapper>(config, std::vector{e});
  auto c = std::make_shared<NodeWrapper>(config);
  auto b = std::make_shared<NodeWrapper>(config, std::vector{c});
  auto a = std::make_shared<NodeWrapper>(config, std::vector{b});
  auto hidden = std::make_shared<NodeWrapper>(config, std::vector{a, d});
  YGNodeStyleSetDisplay(hidden->node, YGDisplayNone);

  auto root = std::make_shared<NodeWrapper>(config, std::vector{hidden});
  YGNodeStyleSetWidth(root->node, 100);
  YGNodeStyleSetHeight(root->node, 100);

  // We don't expect any cloning during the first layout
  onClone = [](...) { FAIL(); };

  YGNodeCalculateLayout(root->node, YGUndefined, YGUndefined, YGDirectionLTR);
  consumeNewLayout(root->node);

  // Add "New" next to "C", cloning its ancestors
  auto newNode = std::make_shared<NodeWrapper>(config);
  YGNodeStyleSetWidth(newNode->node, 50);
  YGNodeStyleSetHeight(newNode->node, 50);
  auto bPrime = std::make_shared<NodeWrapper>(*b, std::vector{c, newNode});
  auto aPrime = std::make_shared<NodeWrapper>(*a, std::vector{bPrime});
  auto hiddenPrime =
      std::make_shared<NodeWrapper>(*hidden, std::vector{aPrime, d});
  auto rootPrime =
      std::make_shared<NodeWrapper>(*root, std::vector{hiddenPrime});

  std::vector<NodeWrapper*> nodesCloned;
  // We should only need to clone the shared siblings on the path to "New",
  // "C" and "D", but not the zeroed out subtree of "D"
  onClone = [&](YGNodeConstRef oldNode,
                YGNodeConstRef /*owner*/,
                size_t /*childIndex*/) {
    nodesCloned.push_back(static_cast<NodeWrapper*>(YGNodeGetContext(oldNode)));
  };

  YGNodeCalculateLayout(
      rootPrime->node, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_EQ(nodesCloned, (std::vector<NodeWrapper*>{d.get(), c.get()}));
  expectZeroedOut(newNode->node);
  expectZeroedOut(e->node);
}

TEST_F(
    YGPersistentNodeCloningTest,
    showing_display_none_subtree_matches_never_hidden_layout) {
  // <View id="Root">
  //   <View id="Sibling" style={{ height: 1 }} />
  //   <View id="Toggled" style={{ display: 'none' }}>
  //     <View id="A" style={{ flexDirection: 'row', padding: 5 }}>
  //       <View id="B" style={{ width: 10, height: 20, margin: 3 }} />
  //       <View id="C" style={{ flexGrow: 1, height: 15 }}>
  //         <View id="D" style={{ width: 7, height: 7 }} />
  //       </View>
  //     </View>
  //   </View>
  // </View>

  auto makeRoot = [&](YGDisplay toggledDisplay) {
    auto sibling = std::make_shared<NodeWrapper>(config);
    YGNodeStyleSetHeight(sibling->node, 1);

    auto d = std::make_shared<NodeWrapper>(config);
    YGNodeStyleSetWidth(d->node, 7);
    YGNodeStyleSetHeight(d->node, 7);
    auto c = std::make_shared<NodeWrapper>(config, std::vector{d});
    YGNodeStyleSetFlexGrow(c->node, 1);
    YGNodeStyleSetHeight(c->node, 15);
    auto b = std::make_shared<NodeWrapper>(config);
    YGNodeStyleSetWidth(b->node, 10);
    YGNodeStyleSetHeight(b->node, 20);
    YGNodeStyleSetMargin(b->node, YGEdgeAll, 3);
    auto a = std::make_shared<NodeWrapper>(config, std::vector{b, c});
    YGNodeStyleSetFlexDirection(a->node, YGFlexDirectionRow);
    YGNodeStyleSetPadding(a->node, YGEdgeAll, 5);
    auto toggled = std::make_shared<NodeWrapper>(config, std::vector{a});
    YGNodeStyleSetDisplay(toggled->node, toggledDisplay);

    auto root =
        std::make_shared<NodeWrapper>(config, std::vector{sibling, toggled});
    YGNodeStyleSetWidth(root->node, 100);
    YGNodeStyleSetHeight(root->node, 100);
    return root;
  };

  auto root = makeRoot(YGDisplayNone);
  YGNodeCalculateLayout(root->node, YGUndefined, YGUndefined, YGDirectionLTR);
  consumeNewLayout(root->node);

  // Lay out "Root" again while "Toggled" is still hidden
  auto siblingPrime = std::make_shared<NodeWrapper>(config);
  YGNodeStyleSetHeight(siblingPrime->node, 2);
  auto rootPrime = std::make_shared<NodeWrapper>(
      *root, std::vector{siblingPrime, root->children[1]});
  YGNodeCalculateLayout(
      rootPrime->node, YGUndefined, YGUndefined, YGDirectionLTR);
  consumeNewLayout(rootPrime->node);

  // Show "Toggled"
  auto toggledPrime = std::make_shared<NodeWrapper>(*rootPrime->children[1]);
  YGNodeStyleSetDisplay(toggledPrime->node, YGDisplayFlex);
  auto rootPrimePrime = std::make_shared<NodeWrapper>(
      *rootPrime, std::vector{siblingPrime, toggledPrime});
  YGNodeCalculateLayout(
      rootPrimePrime->node, YGUndefined, YGUndefined, YGDirectionLTR);

  auto expectedRoot = makeRoot(YGDisplayFlex);
  YGNodeStyleSetHeight(expectedRoot->children[0]->node, 2);
  YGNodeCalculateLayout(
      expectedRoot->node, YGUndefined, YGUndefined, YGDirectionLTR);

  ASSERT_EQ(YGNodeLayoutGetWidth(expectedRoot->children[1]->node), 100);
  expectSameLayout(rootPrimePrime->node, expectedRoot->node);
}

TEST_F(
    YGPersistentNodeCloningTest,
    relayout_does_not_clone_display_contents_in_display_none_subtree) {
  // <View id="Root">
  //   <View id="Hidden" style={{ display: 'none' }}>
  //     <View id="A" style={{ display: 'contents' }}>
  //       <View id="B">
  //         <View id="C" />
  //       </View>
  //       <View id="D" />
  //     </View>
  //   </View>
  // </View>

  auto d = std::make_shared<NodeWrapper>(config);
  auto c = std::make_shared<NodeWrapper>(config);
  auto b = std::make_shared<NodeWrapper>(config, std::vector{c});
  auto a = std::make_shared<NodeWrapper>(config, std::vector{b, d});
  YGNodeStyleSetDisplay(a->node, YGDisplayContents);
  auto hidden = std::make_shared<NodeWrapper>(config, std::vector{a});
  YGNodeStyleSetDisplay(hidden->node, YGDisplayNone);

  auto root = std::make_shared<NodeWrapper>(config, std::vector{hidden});
  YGNodeStyleSetWidth(root->node, 100);
  YGNodeStyleSetHeight(root->node, 100);

  // We don't expect any cloning during the first layout
  onClone = [](...) { FAIL(); };

  YGNodeCalculateLayout(root->node, YGUndefined, YGUndefined, YGDirectionLTR);
  consumeNewLayout(root->node);

  auto rootPrime = std::make_shared<NodeWrapper>(*root, std::vector{hidden});

  std::vector<NodeWrapper*> nodesCloned;
  // Cloning "Hidden" also clones its display: contents child "A" and the
  // children of "A". We should not need to clone the zeroed out subtree of
  // "B"
  onClone = [&](YGNodeConstRef oldNode,
                YGNodeConstRef /*owner*/,
                size_t /*childIndex*/) {
    nodesCloned.push_back(static_cast<NodeWrapper*>(YGNodeGetContext(oldNode)));
  };

  YGNodeCalculateLayout(
      rootPrime->node, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_EQ(
      nodesCloned,
      (std::vector<NodeWrapper*>{hidden.get(), a.get(), b.get(), d.get()}));
  expectZeroedOut(a->node);
  expectZeroedOut(b->node);
  expectZeroedOut(c->node);
  expectZeroedOut(d->node);
}

TEST_F(
    YGPersistentNodeCloningTest,
    relayout_does_not_clone_display_none_subtree_after_config_is_reapplied) {
  // <View id="Root">
  //   <View id="Hidden" style={{ display: 'none' }}>
  //     <View id="A">
  //       <View id="B" />
  //       <View id="C" />
  //     </View>
  //   </View>
  // </View>

  // Give the config a non-zero version
  YGConfigSetPointScaleFactor(config, 2);

  auto c = std::make_shared<NodeWrapper>(config);
  auto b = std::make_shared<NodeWrapper>(config);
  auto a = std::make_shared<NodeWrapper>(config, std::vector{b, c});
  auto hidden = std::make_shared<NodeWrapper>(config, std::vector{a});
  YGNodeStyleSetDisplay(hidden->node, YGDisplayNone);

  auto root = std::make_shared<NodeWrapper>(config, std::vector{hidden});
  YGNodeStyleSetWidth(root->node, 100);
  YGNodeStyleSetHeight(root->node, 100);

  // We don't expect any cloning during the first layout
  onClone = [](...) { FAIL(); };

  YGNodeCalculateLayout(root->node, YGUndefined, YGUndefined, YGDirectionLTR);
  consumeNewLayout(root->node);

  // Clone "B" for a change that doesn't affect layout, setting the config on
  // each clone like React Native does. This aligns their `configVersion` with
  // the config, while the rest of the zeroed out subtree has none.
  auto setConfig = [&](const std::shared_ptr<NodeWrapper>& wrapper) {
    YGNodeSetConfig(wrapper->node, config);
    return wrapper;
  };
  auto bPrime = setConfig(std::make_shared<NodeWrapper>(*b));
  auto aPrime =
      setConfig(std::make_shared<NodeWrapper>(*a, std::vector{bPrime, c}));
  auto hiddenPrime =
      setConfig(std::make_shared<NodeWrapper>(*hidden, std::vector{aPrime}));
  auto rootPrime =
      setConfig(std::make_shared<NodeWrapper>(*root, std::vector{hiddenPrime}));

  std::vector<NodeWrapper*> nodesCloned;
  // The subtree is still zeroed out, so "C" should not be cloned
  onClone = [&](YGNodeConstRef oldNode,
                YGNodeConstRef /*owner*/,
                size_t /*childIndex*/) {
    nodesCloned.push_back(static_cast<NodeWrapper*>(YGNodeGetContext(oldNode)));
  };

  YGNodeCalculateLayout(
      rootPrime->node, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_TRUE(nodesCloned.empty());
}

TEST_F(
    YGPersistentNodeCloningTest,
    relayout_clones_display_none_subtree_without_new_layout) {
  // <View id="Root">
  //   <View id="Hidden" style={{ display: 'none' }}>
  //     <View id="A">
  //       <View id="B" />
  //     </View>
  //   </View>
  // </View>

  auto b = std::make_shared<NodeWrapper>(config);
  auto a = std::make_shared<NodeWrapper>(config, std::vector{b});
  auto hidden = std::make_shared<NodeWrapper>(config, std::vector{a});
  YGNodeStyleSetDisplay(hidden->node, YGDisplayNone);

  auto root = std::make_shared<NodeWrapper>(config, std::vector{hidden});
  YGNodeStyleSetWidth(root->node, 100);
  YGNodeStyleSetHeight(root->node, 100);

  YGNodeCalculateLayout(root->node, YGUndefined, YGUndefined, YGDirectionLTR);

  // An embedder that clears `hasNewLayout` under display: none nodes too
  YGNodeSetHasNewLayout(root->node, false);
  YGNodeSetHasNewLayout(hidden->node, false);
  YGNodeSetHasNewLayout(a->node, false);
  YGNodeSetHasNewLayout(b->node, false);

  auto rootPrime = std::make_shared<NodeWrapper>(*root, std::vector{hidden});

  std::vector<NodeWrapper*> nodesCloned;
  // The subtree is cloned and zeroed out again, like before
  onClone = [&](YGNodeConstRef oldNode,
                YGNodeConstRef /*owner*/,
                size_t /*childIndex*/) {
    nodesCloned.push_back(static_cast<NodeWrapper*>(YGNodeGetContext(oldNode)));
  };

  YGNodeCalculateLayout(
      rootPrime->node, YGUndefined, YGUndefined, YGDirectionLTR);

  EXPECT_EQ(
      nodesCloned, (std::vector<NodeWrapper*>{hidden.get(), a.get(), b.get()}));
  auto hiddenPrime = rootPrime->children[0];
  auto aPrime = hiddenPrime->children[0];
  expectZeroedOut(aPrime->node);
  expectZeroedOut(aPrime->children[0]->node);
}

} // namespace facebook::yoga
