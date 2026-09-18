/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <stdexcept>

#include <gtest/gtest.h>

namespace facebook::yoga {

class Config;
class Node;

[[noreturn]] void fatalWithMessage(const char* message);
void assertFatal(bool condition, const char* message);
void assertFatalWithNode(
    const yoga::Node* node,
    bool condition,
    const char* message);
void assertFatalWithConfig(
    const yoga::Config* config,
    bool condition,
    const char* message);

namespace {

template <typename Callback>
void expectLogicErrorMessage(Callback callback, const char* expectedMessage) {
  try {
    callback();
    FAIL() << "Expected std::logic_error";
  } catch (const std::logic_error& error) {
    EXPECT_STREQ(error.what(), expectedMessage);
  }
}

} // namespace

TEST(AssertFatalTest, testFatalWithMessageThrowsLogicErrorMessage) {
  expectLogicErrorMessage(
      [] { fatalWithMessage("fatal message"); }, "fatal message");
}

TEST(AssertFatalTest, testAssertFatalFalseThrowsLogicErrorMessage) {
  expectLogicErrorMessage(
      [] { assertFatal(false, "global failure"); }, "global failure");
}

TEST(AssertFatalTest, testAssertFatalWithNodeFalseThrowsLogicErrorMessage) {
  expectLogicErrorMessage(
      [] { assertFatalWithNode(nullptr, false, "node failure"); },
      "node failure");
}

TEST(AssertFatalTest, testAssertFatalWithConfigFalseThrowsLogicErrorMessage) {
  expectLogicErrorMessage(
      [] { assertFatalWithConfig(nullptr, false, "config failure"); },
      "config failure");
}

TEST(AssertFatalTest, testPassingConditionsDoNotThrow) {
  EXPECT_NO_THROW(assertFatal(true, "global success"));
  EXPECT_NO_THROW(assertFatalWithNode(nullptr, true, "node success"));
  EXPECT_NO_THROW(assertFatalWithConfig(nullptr, true, "config success"));
}

} // namespace facebook::yoga
