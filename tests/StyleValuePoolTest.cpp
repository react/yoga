/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <array>
#include <gtest/gtest.h>
#include <yoga/style/StyleValuePool.h>

namespace facebook::yoga {

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

TEST(StyleValuePool, store_keywords_after_large_int) {
  StyleValuePool pool;
  std::array<StyleValueHandle, 4> handles;

  // Each handle takes its own pool slot, from 0 to 3
  for (auto& handle : handles) {
    pool.store(handle, StyleSizeLength::points(262144));
  }

  for (auto keyword :
       {StyleSizeLength::ofMaxContent(),
        StyleSizeLength::ofFitContent(),
        StyleSizeLength::ofStretch()}) {
    for (auto& handle : handles) {
      pool.store(handle, keyword);
      EXPECT_EQ(pool.getSize(handle), keyword);
    }
  }
}

TEST(StyleValuePool, store_large_int_and_keyword_repeatedly) {
  StyleValuePool pool;
  StyleValueHandle handle;

  // The handle keeps its pool slot, so the pool must not grow on each store
  for (int i = 0; i < 5000; i++) {
    pool.store(handle, StyleSizeLength::points(262144));
    pool.store(handle, StyleSizeLength::ofMaxContent());
  }
  pool.store(handle, StyleSizeLength::points(262144));

  EXPECT_EQ(pool.getSize(handle), StyleSizeLength::points(262144));
}

} // namespace facebook::yoga
