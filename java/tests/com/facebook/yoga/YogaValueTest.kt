/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.yoga

import org.junit.Assert.assertEquals
import org.junit.Test

class YogaValueTest {

  @Test
  fun testEquals() {
    assertEquals(YogaValue(0f, YogaUnit.UNDEFINED), YogaValue(0f, YogaUnit.UNDEFINED))
    assertEquals(YogaValue(0f, YogaUnit.POINT), YogaValue(0f, YogaUnit.POINT))
    assertEquals(YogaValue(0f, YogaUnit.PERCENT), YogaValue(0f, YogaUnit.PERCENT))
    assertEquals(YogaValue(0f, YogaUnit.UNDEFINED), YogaValue(1f, YogaUnit.UNDEFINED))
    assertEquals(YogaValue(Float.NaN, YogaUnit.POINT), YogaValue(Float.NaN, YogaUnit.POINT))
  }
}
