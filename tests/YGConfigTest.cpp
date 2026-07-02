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
#include <stdexcept>

using namespace facebook;

struct ConfigCloningTest : public ::testing::Test {
  std::unique_ptr<yoga::Config, std::function<void(yoga::Config*)>> config;
  void SetUp() override;
  void TearDown() override;

  static yoga::Node clonedNode;
  static YGNodeRef cloneNode(
      YGNodeConstRef /*unused*/,
      YGNodeConstRef /*unused*/,
      size_t /*unused*/) {
    return &clonedNode;
  }
  static YGNodeRef doNotClone(
      YGNodeConstRef /*unused*/,
      YGNodeConstRef /*unused*/,
      size_t /*unused*/) {
    return nullptr;
  }
};

TEST_F(ConfigCloningTest, uses_values_provided_by_cloning_callback) {
  config->setCloneNodeCallback(cloneNode);

  yoga::Node node{};
  yoga::Node owner{};
  auto clone = config->cloneNode(&node, &owner, 0);

  ASSERT_EQ(clone, &clonedNode);
}

TEST_F(
    ConfigCloningTest,
    falls_back_to_regular_cloning_if_callback_returns_null) {
  config->setCloneNodeCallback(doNotClone);

  yoga::Node node{};
  yoga::Node owner{};
  auto clone = config->cloneNode(&node, &owner, 0);

  ASSERT_NE(clone, nullptr);
  YGNodeFree(clone);
}

void ConfigCloningTest::SetUp() {
  config = {static_cast<yoga::Config*>(YGConfigNew()), YGConfigFree};
}

void ConfigCloningTest::TearDown() {
  config.reset();
}

yoga::Node ConfigCloningTest::clonedNode = {};

TEST(YogaTest, config_point_scale_factor_negative_throws) {
  YGConfigRef config = YGConfigNew();

  // Zero is explicitly allowed per the API contract
  YGConfigSetPointScaleFactor(config, 0.0f);
  ASSERT_EQ(YGConfigGetPointScaleFactor(config), 0.0f);

  // Negative values should trigger a fatal assertion (throws std::logic_error)
  ASSERT_THROW(YGConfigSetPointScaleFactor(config, -1.0f), std::logic_error);

  // Verify the value was not changed after the failed set
  ASSERT_EQ(YGConfigGetPointScaleFactor(config), 0.0f);

  YGConfigFree(config);
}

TEST(YogaTest, config_set_logger_null_resets_to_default_logger) {
  YGConfigRef config = YGConfigNew();

  // Track whether our custom logger was called
  bool customLoggerCalled = false;
  YGConfigSetContext(config, &customLoggerCalled);

  YGLogger customLogger = [](YGConfigConstRef cfg,
                             YGNodeConstRef /*node*/,
                             YGLogLevel /*level*/,
                             const char* /*format*/,
                             va_list /*args*/) -> int {
    bool* called = static_cast<bool*>(YGConfigGetContext(cfg));
    *called = true;
    return 0;
  };

  // Set custom logger and verify it's invoked via layout warning
  YGConfigSetLogger(config, customLogger);
  YGNodeRef node = YGNodeNewWithConfig(config);
  YGNodeCalculateLayout(node, YGUndefined, YGUndefined, YGDirectionLTR);
  // Trigger a log by setting a negative point scale factor (which logs before
  // throwing)
  customLoggerCalled = false;
  ASSERT_THROW(YGConfigSetPointScaleFactor(config, -1.0f), std::logic_error);
  ASSERT_TRUE(customLoggerCalled);

  // Reset logger to null — should fall back to default logger
  YGConfigSetLogger(config, nullptr);
  customLoggerCalled = false;
  ASSERT_THROW(YGConfigSetPointScaleFactor(config, -2.0f), std::logic_error);
  // Custom logger should NOT have been called since we reset to default
  ASSERT_FALSE(customLoggerCalled);

  YGNodeFree(node);
  YGConfigFree(config);
}

TEST(YogaTest, config_version_increments_only_on_actual_changes) {
  auto* config = static_cast<yoga::Config*>(YGConfigNew());

  uint32_t initialVersion = config->getVersion();

  // Changing point scale factor should increment version
  config->setPointScaleFactor(2.0f);
  ASSERT_EQ(config->getVersion(), initialVersion + 1);

  // Setting the same value again should NOT increment version
  config->setPointScaleFactor(2.0f);
  ASSERT_EQ(config->getVersion(), initialVersion + 1);

  // Changing errata should increment version
  config->setErrata(yoga::Errata::StretchFlexBasis);
  ASSERT_EQ(config->getVersion(), initialVersion + 2);

  // Setting the same errata again should NOT increment version
  config->setErrata(yoga::Errata::StretchFlexBasis);
  ASSERT_EQ(config->getVersion(), initialVersion + 2);

  // Enabling an experimental feature should increment version
  config->setExperimentalFeatureEnabled(
      yoga::ExperimentalFeature::WebFlexBasis, true);
  ASSERT_EQ(config->getVersion(), initialVersion + 3);

  // Enabling the same feature again should NOT increment version
  config->setExperimentalFeatureEnabled(
      yoga::ExperimentalFeature::WebFlexBasis, true);
  ASSERT_EQ(config->getVersion(), initialVersion + 3);

  YGConfigFree(config);
}

TEST(YogaTest, config_update_invalidates_layout_detects_each_property) {
  auto* config1 = static_cast<yoga::Config*>(YGConfigNew());
  auto* config2 = static_cast<yoga::Config*>(YGConfigNew());

  // Two identical configs should not invalidate layout
  ASSERT_FALSE(yoga::configUpdateInvalidatesLayout(*config1, *config2));

  // Changing errata on one config should invalidate
  config2->setErrata(yoga::Errata::StretchFlexBasis);
  ASSERT_TRUE(yoga::configUpdateInvalidatesLayout(*config1, *config2));

  // Make them match again
  config1->setErrata(yoga::Errata::StretchFlexBasis);
  ASSERT_FALSE(yoga::configUpdateInvalidatesLayout(*config1, *config2));

  // Changing point scale factor should invalidate
  config2->setPointScaleFactor(3.0f);
  ASSERT_TRUE(yoga::configUpdateInvalidatesLayout(*config1, *config2));

  // Make them match again
  config1->setPointScaleFactor(3.0f);
  ASSERT_FALSE(yoga::configUpdateInvalidatesLayout(*config1, *config2));

  // Changing useWebDefaults should invalidate
  config2->setUseWebDefaults(true);
  ASSERT_TRUE(yoga::configUpdateInvalidatesLayout(*config1, *config2));

  // Make them match again
  config1->setUseWebDefaults(true);
  ASSERT_FALSE(yoga::configUpdateInvalidatesLayout(*config1, *config2));

  // Changing experimental features should invalidate
  config2->setExperimentalFeatureEnabled(
      yoga::ExperimentalFeature::WebFlexBasis, true);
  ASSERT_TRUE(yoga::configUpdateInvalidatesLayout(*config1, *config2));

  YGConfigFree(config1);
  YGConfigFree(config2);
}

TEST(YogaTest, config_errata_bitmask_add_remove_operations) {
  auto* config = static_cast<yoga::Config*>(YGConfigNew());

  // Default config carries MinSizeUndefinedInsteadOfAuto (legacy preservation
  // for CSS §4.5 auto-min). Clear it for this test so we can assert exact
  // equality at the end.
  config->removeErrata(yoga::Errata::MinSizeUndefinedInsteadOfAuto);

  // Initially no errata
  ASSERT_FALSE(config->hasErrata(yoga::Errata::StretchFlexBasis));
  ASSERT_FALSE(config->hasErrata(
      yoga::Errata::AbsolutePositionWithoutInsetsExcludesPadding));

  // Add one errata flag
  config->addErrata(yoga::Errata::StretchFlexBasis);
  ASSERT_TRUE(config->hasErrata(yoga::Errata::StretchFlexBasis));
  ASSERT_FALSE(config->hasErrata(
      yoga::Errata::AbsolutePositionWithoutInsetsExcludesPadding));

  // Add another errata flag — first should still be set
  config->addErrata(yoga::Errata::AbsolutePositionWithoutInsetsExcludesPadding);
  ASSERT_TRUE(config->hasErrata(yoga::Errata::StretchFlexBasis));
  ASSERT_TRUE(config->hasErrata(
      yoga::Errata::AbsolutePositionWithoutInsetsExcludesPadding));

  // Remove only the first flag — second should remain
  config->removeErrata(yoga::Errata::StretchFlexBasis);
  ASSERT_FALSE(config->hasErrata(yoga::Errata::StretchFlexBasis));
  ASSERT_TRUE(config->hasErrata(
      yoga::Errata::AbsolutePositionWithoutInsetsExcludesPadding));

  // Remove the second flag
  config->removeErrata(
      yoga::Errata::AbsolutePositionWithoutInsetsExcludesPadding);
  ASSERT_EQ(config->getErrata(), yoga::Errata::None);

  YGConfigFree(config);
}
