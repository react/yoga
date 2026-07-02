/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.yoga

import java.lang.ref.WeakReference
import org.junit.Assert.assertEquals
import org.junit.Assert.fail
import org.junit.Test

class YogaLoggerTest {

  @Test
  fun testRemovingLoggerFromConfig() {
    val logs = mutableListOf<String>()

    val config = YogaConfigFactory.create()
    var logger: YogaLogger? = YogaLogger { level, message -> logs.add(message) }
    config.setLogger(logger)

    val root = YogaNodeFactory.create(config)
    root.flexDirection = YogaFlexDirection.ROW
    root.alignItems = YogaAlign.BASELINE

    val child1 = YogaNodeFactory.create(config)
    root.addChildAt(child1, 0)

    val child2 = YogaNodeFactory.create(config)
    child2.setBaselineFunction(YogaBaselineFunction { node, width, height -> Float.NaN })
    root.addChildAt(child2, 1)

    assertEquals(0, logs.size)
    try {
      root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
      fail("Expected calculateLayout to throw")
    } catch (e: IllegalStateException) {}

    assertEquals(1, logs.size)

    config.setLogger(null)

    try {
      root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
      fail("Expected calculateLayout to throw again")
    } catch (e: IllegalStateException) {}

    assertEquals(1, logs.size)
  }

  @Test
  @Throws(Exception::class)
  fun testLoggerLeak() {
    val config = YogaConfigFactory.create()
    @Suppress("ObjectLiteralToLambda")
    var logger: YogaLogger? =
        object : YogaLogger {
          override fun log(level: YogaLogLevel, message: String) {}
        }
    config.setLogger(logger)
    config.setLogger(null)
    val ref = WeakReference<Any>(logger)
    @Suppress("UNUSED_VALUE")
    logger = null
    for (i in 0 until 50) {
      System.gc()
      if (ref.get() == null) {
        return
      }
      Thread.sleep(100)
    }
    fail("YogaLogger leaked")
  }
}
