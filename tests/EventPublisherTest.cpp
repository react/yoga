/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/Yoga.h>
#include <yoga/event/event.h>

#include <array>
#include <string_view>
#include <unordered_set>

namespace facebook::yoga::test {

// Every enum value declared in LayoutPassReason (excluding the COUNT sentinel).
// Kept in sync with yoga/event/event.h; a missing entry causes the "all known
// reasons" test to catch the omission on the next run.
constexpr std::array<LayoutPassReason, 9> kAllKnownReasons = {
    LayoutPassReason::kInitial,
    LayoutPassReason::kAbsLayout,
    LayoutPassReason::kStretch,
    LayoutPassReason::kMultilineStretch,
    LayoutPassReason::kFlexLayout,
    LayoutPassReason::kMeasureChild,
    LayoutPassReason::kAbsMeasureChild,
    LayoutPassReason::kFlexMeasure,
    LayoutPassReason::kGridLayout,
};

TEST(
    LayoutPassReasonToStringTest,
    everyKnownReasonMapsToDistinctNonUnknownLabel) {
  std::unordered_set<std::string_view> labels;
  for (auto reason : kAllKnownReasons) {
    const char* label = LayoutPassReasonToString(reason);
    ASSERT_NE(label, nullptr)
        << "LayoutPassReasonToString returned nullptr for enum value "
        << static_cast<int>(reason);
    EXPECT_STRNE(label, "")
        << "Empty label for enum value " << static_cast<int>(reason);
    EXPECT_STRNE(label, "unknown")
        << "Enum value " << static_cast<int>(reason)
        << " unexpectedly fell through to the default case";
    labels.emplace(label);
  }
  EXPECT_EQ(labels.size(), kAllKnownReasons.size())
      << "At least two LayoutPassReason values share the same label";
}

TEST(LayoutPassReasonToStringTest, fallsBackToUnknownForUnrecognizedValues) {
  // Out-of-range integer cast: exercises the switch's default branch, which
  // guards against silent regressions when new enum values are added without
  // updating the switch.
  EXPECT_STREQ(
      LayoutPassReasonToString(static_cast<LayoutPassReason>(9999)), "unknown");

  // COUNT is the sentinel used to size arrays and is not a real reason, so
  // it must also fall through to the default case.
  EXPECT_STREQ(LayoutPassReasonToString(LayoutPassReason::COUNT), "unknown");
}

class EventPublisherTest : public ::testing::Test {
 protected:
  // Event maintains a process-global subscriber list; clear it around each
  // test to keep the tests independent regardless of ordering.
  void SetUp() override {
    Event::reset();
  }
  void TearDown() override {
    Event::reset();
  }
};

TEST_F(
    EventPublisherTest,
    resetDetachesPreviouslyRegisteredSubscribersFromFutureEvents) {
  int callCount = 0;
  Event::subscribe(
      [&](YGNodeConstRef, Event::Type, Event::Data) { ++callCount; });

  // Sanity-check that the subscriber is actually wired up before reset.
  Event::publish<Event::LayoutPassStart>(nullptr);
  ASSERT_EQ(callCount, 1);

  Event::reset();
  Event::publish<Event::LayoutPassStart>(nullptr);
  Event::publish<Event::LayoutPassEnd>(
      nullptr, Event::TypedData<Event::LayoutPassEnd>{nullptr});

  EXPECT_EQ(callCount, 1)
      << "Subscriber should not have been invoked after Event::reset()";
}

TEST_F(EventPublisherTest, publishForwardsToEverySubscriberWithNodeAndType) {
  YGNodeRef sentinelNode = YGNodeNew();

  int subscriberACallCount = 0;
  int subscriberBCallCount = 0;
  YGNodeConstRef subscriberAReceivedNode = nullptr;
  Event::Type subscriberAReceivedType = Event::NodeAllocation;

  Event::subscribe([&](YGNodeConstRef node, Event::Type type, Event::Data) {
    ++subscriberACallCount;
    subscriberAReceivedNode = node;
    subscriberAReceivedType = type;
  });
  Event::subscribe([&](YGNodeConstRef, Event::Type, Event::Data) {
    ++subscriberBCallCount;
  });

  Event::publish<Event::LayoutPassStart>(sentinelNode);

  EXPECT_EQ(subscriberACallCount, 1);
  EXPECT_EQ(subscriberBCallCount, 1);
  EXPECT_EQ(subscriberAReceivedNode, sentinelNode);
  EXPECT_EQ(subscriberAReceivedType, Event::LayoutPassStart);

  YGNodeFree(sentinelNode);
}

} // namespace facebook::yoga::test
