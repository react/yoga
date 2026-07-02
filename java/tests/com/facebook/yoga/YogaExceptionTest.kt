/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.yoga

import java.util.concurrent.atomic.AtomicBoolean
import java.util.concurrent.atomic.AtomicReference
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.fail
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.Parameterized

@RunWith(Parameterized::class)
class YogaExceptionTest {
  companion object {
    @JvmStatic
    @Parameterized.Parameters(name = "{0}")
    fun nodeFactories(): Iterable<TestParametrization.NodeFactory> =
        TestParametrization.nodeFactories()
  }

  @Parameterized.Parameter lateinit var mNodeFactory: TestParametrization.NodeFactory

  @Test(expected = RuntimeException::class)
  fun testBaselineThrows() {
    val root = createNode()
    root.flexDirection = YogaFlexDirection.ROW
    root.alignItems = YogaAlign.BASELINE

    val child1 = createNode()
    root.addChildAt(child1, 0)

    val child2 = createNode()
    child2.setBaselineFunction(
        YogaBaselineFunction { node, width, height -> throw RuntimeException() }
    )
    root.addChildAt(child2, 1)

    root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
  }

  @Test
  fun testBaselineThrowsAndStops() {
    val root = createNode()
    root.flexDirection = YogaFlexDirection.ROW
    root.alignItems = YogaAlign.BASELINE

    val child1 = createNode()
    root.addChildAt(child1, 0)

    val child2 = createNode()
    val expected = AtomicReference<Exception>()
    child2.setBaselineFunction(
        YogaBaselineFunction { node, width, height ->
          val e = RuntimeException()
          expected.set(e)
          throw e
        }
    )
    root.addChildAt(child2, 1)

    val child3 = createNode()
    val child3Called = AtomicBoolean()
    child3.setBaselineFunction(
        YogaBaselineFunction { node, width, height ->
          child3Called.set(true)
          1.0f
        }
    )
    root.addChildAt(child3, 2)

    try {
      root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
      fail()
    } catch (e: RuntimeException) {
      assertEquals(expected.get(), e)
    }
    assertFalse(child3Called.get())
  }

  @Test(expected = RuntimeException::class)
  fun testMeasureThrows() {
    val node = createNode()
    node.setMeasureFunction(
        YogaMeasureFunction { node, width, widthMode, height, heightMode ->
          throw RuntimeException()
        }
    )
    node.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
  }

  @Test
  fun testMeasureThrowsAndStops() {
    val root = createNode()
    root.flexDirection = YogaFlexDirection.ROW
    root.alignItems = YogaAlign.BASELINE

    val child1 = createNode()
    root.addChildAt(child1, 0)

    val child2 = createNode()
    val expected = AtomicReference<Exception>()
    child2.setMeasureFunction(
        YogaMeasureFunction { node, width, widthMode, height, heightMode ->
          val e = RuntimeException()
          expected.set(e)
          throw e
        }
    )
    root.addChildAt(child2, 1)

    val child3 = createNode()
    val child3Called = AtomicBoolean()
    child3.setMeasureFunction(
        YogaMeasureFunction { node, width, widthMode, height, heightMode ->
          child3Called.set(true)
          1L
        }
    )
    root.addChildAt(child3, 2)

    try {
      root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
      fail()
    } catch (e: RuntimeException) {
      assertEquals(expected.get(), e)
    }
    assertFalse(child3Called.get())
  }

  private fun createNode(): YogaNode = mNodeFactory.create()

  private fun createNode(config: YogaConfig): YogaNode = mNodeFactory.create(config)
}
