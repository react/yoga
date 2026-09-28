/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cmath>

#include <gtest/gtest.h>

#include <yoga/YGValue.h>
#include <yoga/numeric/Comparison.h>
#include <yoga/numeric/FloatOptional.h>

using namespace facebook;
using namespace facebook::yoga;

constexpr auto empty = FloatOptional{};
constexpr auto zero = FloatOptional{0.0f};
constexpr auto one = FloatOptional{1.0f};
constexpr auto positive = FloatOptional{1234.5f};
constexpr auto negative = FloatOptional{-9876.5f};

TEST(FloatOptional, value) {
  ASSERT_TRUE(YGFloatIsUndefined(empty.unwrap()));
  ASSERT_EQ(zero.unwrap(), 0.0f);
  ASSERT_EQ(one.unwrap(), 1.0f);
  ASSERT_EQ(positive.unwrap(), 1234.5f);
  ASSERT_EQ(negative.unwrap(), -9876.5f);

  ASSERT_TRUE(empty.isUndefined());
  ASSERT_FALSE(zero.isUndefined());
  ASSERT_FALSE(one.isUndefined());
  ASSERT_FALSE(positive.isUndefined());
  ASSERT_FALSE(negative.isUndefined());
}

TEST(FloatOptional, equality) {
  ASSERT_TRUE(empty == empty);
  ASSERT_TRUE(empty == YGUndefined);
  ASSERT_FALSE(empty == zero);
  ASSERT_FALSE(empty == negative);
  ASSERT_FALSE(empty == 12.3f);

  ASSERT_TRUE(zero == zero);
  ASSERT_TRUE(zero == 0.0f);
  ASSERT_FALSE(zero == positive);
  ASSERT_FALSE(zero == -5555.5f);

  ASSERT_TRUE(one == one);
  ASSERT_TRUE(one == 1.0f);
  ASSERT_FALSE(one == positive);

  ASSERT_TRUE(positive == positive);
  ASSERT_TRUE(positive == positive.unwrap());
  ASSERT_FALSE(positive == one);

  ASSERT_TRUE(negative == negative);
  ASSERT_TRUE(negative == negative.unwrap());
  ASSERT_FALSE(negative == zero);
}

TEST(FloatOptional, inequality) {
  ASSERT_FALSE(empty != empty);
  ASSERT_FALSE(empty != YGUndefined);
  ASSERT_TRUE(empty != zero);
  ASSERT_TRUE(empty != negative);
  ASSERT_TRUE(empty != 12.3f);

  ASSERT_FALSE(zero != zero);
  ASSERT_FALSE(zero != 0.0f);
  ASSERT_TRUE(zero != positive);
  ASSERT_TRUE(zero != -5555.5f);

  ASSERT_FALSE(one != one);
  ASSERT_FALSE(one != 1.0f);
  ASSERT_TRUE(one != positive);

  ASSERT_FALSE(positive != positive);
  ASSERT_FALSE(positive != positive.unwrap());
  ASSERT_TRUE(positive != one);

  ASSERT_FALSE(negative != negative);
  ASSERT_FALSE(negative != negative.unwrap());
  ASSERT_TRUE(negative != zero);
}

TEST(FloatOptional, greater_than_with_undefined) {
  ASSERT_FALSE(empty > empty);
  ASSERT_FALSE(empty > zero);
  ASSERT_FALSE(empty > one);
  ASSERT_FALSE(empty > positive);
  ASSERT_FALSE(empty > negative);
  ASSERT_FALSE(zero > empty);
  ASSERT_FALSE(one > empty);
  ASSERT_FALSE(positive > empty);
  ASSERT_FALSE(negative > empty);
}

TEST(FloatOptional, greater_than) {
  ASSERT_TRUE(zero > negative);
  ASSERT_FALSE(zero > zero);
  ASSERT_FALSE(zero > positive);
  ASSERT_FALSE(zero > one);

  ASSERT_TRUE(one > negative);
  ASSERT_TRUE(one > zero);
  ASSERT_FALSE(one > positive);

  ASSERT_TRUE(negative > FloatOptional{-INFINITY});
}

TEST(FloatOptional, less_than_with_undefined) {
  ASSERT_FALSE(empty < empty);
  ASSERT_FALSE(zero < empty);
  ASSERT_FALSE(one < empty);
  ASSERT_FALSE(positive < empty);
  ASSERT_FALSE(negative < empty);
  ASSERT_FALSE(empty < zero);
  ASSERT_FALSE(empty < one);
  ASSERT_FALSE(empty < positive);
  ASSERT_FALSE(empty < negative);
}

TEST(FloatOptional, less_than) {
  ASSERT_TRUE(negative < zero);
  ASSERT_FALSE(zero < zero);
  ASSERT_FALSE(positive < zero);
  ASSERT_FALSE(one < zero);

  ASSERT_TRUE(negative < one);
  ASSERT_TRUE(zero < one);
  ASSERT_FALSE(positive < one);

  ASSERT_TRUE(FloatOptional{-INFINITY} < negative);
}

TEST(FloatOptional, greater_than_equals_with_undefined) {
  ASSERT_TRUE(empty >= empty);
  ASSERT_FALSE(empty >= zero);
  ASSERT_FALSE(empty >= one);
  ASSERT_FALSE(empty >= positive);
  ASSERT_FALSE(empty >= negative);
  ASSERT_FALSE(zero >= empty);
  ASSERT_FALSE(one >= empty);
  ASSERT_FALSE(positive >= empty);
  ASSERT_FALSE(negative >= empty);
}

TEST(FloatOptional, greater_than_equals) {
  ASSERT_TRUE(zero >= negative);
  ASSERT_TRUE(zero >= zero);
  ASSERT_FALSE(zero >= positive);
  ASSERT_FALSE(zero >= one);

  ASSERT_TRUE(one >= negative);
  ASSERT_TRUE(one >= zero);
  ASSERT_FALSE(one >= positive);

  ASSERT_TRUE(negative >= FloatOptional{-INFINITY});
}

TEST(FloatOptional, less_than_equals_with_undefined) {
  ASSERT_TRUE(empty <= empty);
  ASSERT_FALSE(zero <= empty);
  ASSERT_FALSE(one <= empty);
  ASSERT_FALSE(positive <= empty);
  ASSERT_FALSE(negative <= empty);
  ASSERT_FALSE(empty <= zero);
  ASSERT_FALSE(empty <= one);
  ASSERT_FALSE(empty <= positive);
  ASSERT_FALSE(empty <= negative);
}

TEST(FloatOptional, less_than_equals) {
  ASSERT_TRUE(negative <= zero);
  ASSERT_TRUE(zero <= zero);
  ASSERT_FALSE(positive <= zero);
  ASSERT_FALSE(one <= zero);

  ASSERT_TRUE(negative <= one);
  ASSERT_TRUE(zero <= one);
  ASSERT_FALSE(positive <= one);

  ASSERT_TRUE(FloatOptional{-INFINITY} <= negative);
}

TEST(FloatOptional, addition) {
  auto n = negative.unwrap();
  auto p = positive.unwrap();

  ASSERT_EQ(zero + one, one);
  ASSERT_EQ(negative + positive, FloatOptional{n + p});
  ASSERT_EQ(empty + zero, empty);
  ASSERT_EQ(empty + empty, empty);
  ASSERT_EQ(negative + empty, empty);
}

TEST(FloatOptional, maxOrDefined) {
  ASSERT_EQ(yoga::maxOrDefined(empty, empty), empty);
  ASSERT_EQ(yoga::maxOrDefined(empty, positive), positive);
  ASSERT_EQ(yoga::maxOrDefined(negative, empty), negative);
  ASSERT_EQ(yoga::maxOrDefined(negative, FloatOptional{-INFINITY}), negative);
  ASSERT_EQ(
      yoga::maxOrDefined(FloatOptional{1.0f}, FloatOptional{1.125f}),
      FloatOptional{1.125f});
}

TEST(FloatOptional, unwrap) {
  ASSERT_TRUE(YGFloatIsUndefined(empty.unwrap()));
  ASSERT_EQ(zero.unwrap(), 0.0f);
  ASSERT_EQ(FloatOptional{123456.78f}.unwrap(), 123456.78f);
}
