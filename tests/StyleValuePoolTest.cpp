/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <yoga/style/StyleValuePool.h>

namespace facebook::yoga {

static YGValue
dynamicValueCallback(YGNodeConstRef, YGValueDynamicID, YGValueDynamicContext) {
  return YGValue{0.0f, YGUnitPoint};
}

TEST(StyleValuePool, undefined_at_init) {
  StyleValuePool pool;
  StyleValueHandle handle;

  EXPECT_TRUE(handle.isUndefined());
  EXPECT_FALSE(handle.isDefined());
  EXPECT_EQ(pool.getLength(handle), StyleLength::undefined());
  EXPECT_EQ(pool.getNumber(handle), FloatOptional{});
}

TEST(StyleValuePool, auto_at_init) {
  StyleValuePool pool;
  auto handle = StyleValueHandle::ofAuto();

  EXPECT_TRUE(handle.isAuto());
  EXPECT_EQ(pool.getLength(handle), StyleLength::ofAuto());
}

TEST(StyleValuePool, store_small_int_points) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, StyleLength::points(10));

  EXPECT_EQ(pool.getLength(handle), StyleLength::points(10));
}

TEST(StyleValuePool, store_small_negative_int_points) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, StyleLength::points(-10));

  EXPECT_EQ(pool.getLength(handle), StyleLength::points(-10));
}

TEST(StyleValuePool, store_small_int_percent) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, StyleLength::percent(10));

  EXPECT_EQ(pool.getLength(handle), StyleLength::percent(10));
}

TEST(StyleValuePool, store_large_int_percent) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, StyleLength::percent(262144));

  EXPECT_EQ(pool.getLength(handle), StyleLength::percent(262144));
}

TEST(StyleValuePool, store_large_int_after_small_int) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, StyleLength::percent(10));
  pool.store(handle, StyleLength::percent(262144));

  EXPECT_EQ(pool.getLength(handle), StyleLength::percent(262144));
}

TEST(StyleValuePool, store_small_int_after_large_int) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, StyleLength::percent(262144));
  pool.store(handle, StyleLength::percent(10));

  EXPECT_EQ(pool.getLength(handle), StyleLength::percent(10));
}

TEST(StyleValuePool, store_small_int_number) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, FloatOptional{10.0f});

  EXPECT_EQ(pool.getNumber(handle), FloatOptional{10.0f});
}

TEST(StyleValuePool, store_undefined) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, StyleLength::undefined());

  EXPECT_TRUE(handle.isUndefined());
  EXPECT_FALSE(handle.isDefined());
  EXPECT_EQ(pool.getLength(handle), StyleLength::undefined());
}

TEST(StyleValuePool, store_undefined_after_small_int) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, StyleLength::points(10));
  pool.store(handle, StyleLength::undefined());

  EXPECT_TRUE(handle.isUndefined());
  EXPECT_FALSE(handle.isDefined());
  EXPECT_EQ(pool.getLength(handle), StyleLength::undefined());
}

TEST(StyleValuePool, store_undefined_after_large_int) {
  StyleValuePool pool;
  StyleValueHandle handle;

  pool.store(handle, StyleLength::points(262144));
  pool.store(handle, StyleLength::undefined());

  EXPECT_TRUE(handle.isUndefined());
  EXPECT_FALSE(handle.isDefined());
  EXPECT_EQ(pool.getLength(handle), StyleLength::undefined());
}

TEST(StyleValuePool, store_keywords) {
  StyleValuePool pool;
  StyleValueHandle handleMaxContent;
  StyleValueHandle handleFitContent;
  StyleValueHandle handleStretch;

  pool.store(handleMaxContent, StyleSizeLength::ofMaxContent());
  pool.store(handleFitContent, StyleSizeLength::ofFitContent());
  pool.store(handleStretch, StyleSizeLength::ofStretch());

  EXPECT_EQ(pool.getSize(handleMaxContent), StyleSizeLength::ofMaxContent());
  EXPECT_EQ(pool.getSize(handleFitContent), StyleSizeLength::ofFitContent());
  EXPECT_EQ(pool.getSize(handleStretch), StyleSizeLength::ofStretch());
}

TEST(StyleValuePool, stores_and_recovers_dynamic) {
  StyleValuePool pool;
  StyleValueHandle lengthHandle;
  StyleValueHandle sizeHandle;

  pool.store(lengthHandle, StyleLength::dynamic(dynamicValueCallback, 11));
  pool.store(sizeHandle, StyleSizeLength::dynamic(dynamicValueCallback, 12));

  const auto restoredLength = pool.getLength(lengthHandle);
  const auto restoredSize = pool.getSize(sizeHandle);

  EXPECT_TRUE(restoredLength.isDynamic());
  EXPECT_EQ(restoredLength.callback(), dynamicValueCallback);
  EXPECT_EQ(restoredLength.callbackId(), 11);

  EXPECT_TRUE(restoredSize.isDynamic());
  EXPECT_EQ(restoredSize.callback(), dynamicValueCallback);
  EXPECT_EQ(restoredSize.callbackId(), 12);
}

} // namespace facebook::yoga
