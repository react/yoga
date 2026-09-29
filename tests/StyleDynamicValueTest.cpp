/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>

#include <yoga/node/Node.h>
#include <yoga/style/StyleLength.h>
#include <yoga/style/StyleSizeLength.h>

namespace facebook::yoga {

static YGValue _resolveToHalfReference(
    YGNodeConstRef /*node*/,
    YGValueDynamicID /*id*/,
    YGValueDynamicContext context) {
  return YGValue{context.referenceLength * 0.5f, YGUnitPoint};
}

static YGValue _resolveToZero(
    YGNodeConstRef /*node*/,
    YGValueDynamicID /*id*/,
    YGValueDynamicContext /*context*/) {
  return YGValue{0, YGUnitPoint};
}

TEST(StyleLength, dynamic_resolve_returns_undefined_without_node) {
  const auto resolved =
      StyleLength::dynamic(_resolveToHalfReference, 1).resolve(100.0f, nullptr);

  ASSERT_TRUE(resolved.isUndefined());
}

TEST(StyleLength, dynamic_resolve_returns_undefined_without_callback) {
  const Node node{};
  const auto resolved = StyleLength::dynamic(nullptr, 1).resolve(100.0f, &node);

  ASSERT_TRUE(resolved.isUndefined());
}

TEST(StyleLength, dynamic_resolve_calls_callback) {
  const Node node{};
  const auto resolved =
      StyleLength::dynamic(_resolveToHalfReference, 1).resolve(80.0f, &node);

  ASSERT_TRUE(resolved.isDefined());
  ASSERT_FLOAT_EQ(resolved.unwrap(), 40.0f);
}

TEST(StyleSizeLength, dynamic_resolve_calls_callback) {
  const Node node{};
  const auto resolved = StyleSizeLength::dynamic(_resolveToHalfReference, 1)
                            .resolve(80.0f, &node);

  ASSERT_TRUE(resolved.isDefined());
  ASSERT_FLOAT_EQ(resolved.unwrap(), 40.0f);
}

TEST(StyleLength, dynamic_equality) {
  const auto a = StyleLength::dynamic(_resolveToHalfReference, 1);
  const auto b = StyleLength::dynamic(_resolveToHalfReference, 1);
  const auto differentId = StyleLength::dynamic(_resolveToHalfReference, 2);
  const auto differentCallback = StyleLength::dynamic(_resolveToZero, 1);
  const auto points = StyleLength::points(50);

  ASSERT_TRUE(a == b);
  ASSERT_FALSE(a == differentId);
  ASSERT_FALSE(a == differentCallback);
  ASSERT_FALSE(a == points);
  ASSERT_TRUE(a.inexactEquals(b));
  ASSERT_FALSE(a.inexactEquals(differentId));
  ASSERT_FALSE(a.inexactEquals(differentCallback));
}

TEST(StyleLength, dynamic_ygvalue_conversion) {
  const auto length = StyleLength::dynamic(_resolveToHalfReference, 1);
  const YGValue ygValue = static_cast<YGValue>(length);

  ASSERT_EQ(ygValue.unit, YGUnitDynamic);
  ASSERT_TRUE(YGFloatIsUndefined(ygValue.value));
}

} // namespace facebook::yoga