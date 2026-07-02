/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.yoga

import org.junit.Assert.assertEquals
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.Parameterized

@RunWith(Parameterized::class)
class YGAlignBaselineTest {
  companion object {
    @JvmStatic
    @Parameterized.Parameters(name = "{0}")
    fun nodeFactories(): Iterable<TestParametrization.NodeFactory> =
        TestParametrization.nodeFactories()
  }

  @Parameterized.Parameter lateinit var mNodeFactory: TestParametrization.NodeFactory

  private fun getBaselineFunc(): YogaBaselineFunction =
      YogaBaselineFunction { node, width, height ->
        height / 2
      }

  @Test
  fun test_align_baseline_parent_using_child_in_column_as_reference() {
    val config = YogaConfigFactory.create()

    val root = createYGNode(config, YogaFlexDirection.ROW, 1000f, 1000f, true)

    val root_child0 = createYGNode(config, YogaFlexDirection.COLUMN, 500f, 600f, false)
    root.addChildAt(root_child0, 0)

    val root_child1 = createYGNode(config, YogaFlexDirection.COLUMN, 500f, 800f, false)
    root.addChildAt(root_child1, 1)

    val root_child1_child0 = createYGNode(config, YogaFlexDirection.COLUMN, 500f, 300f, false)
    root_child1.addChildAt(root_child1_child0, 0)

    val root_child1_child1 = createYGNode(config, YogaFlexDirection.COLUMN, 500f, 400f, false)
    root_child1_child1.setBaselineFunction(getBaselineFunc())
    root_child1_child1.setIsReferenceBaseline(true)
    root_child1.addChildAt(root_child1_child1, 1)

    root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)

    assertEquals(0f, root_child0.layoutX, 0.0f)
    assertEquals(0f, root_child0.layoutY, 0.0f)

    assertEquals(500f, root_child1.layoutX, 0.0f)
    assertEquals(100f, root_child1.layoutY, 0.0f)

    assertEquals(0f, root_child1_child0.layoutX, 0.0f)
    assertEquals(0f, root_child1_child0.layoutY, 0.0f)

    assertEquals(0f, root_child1_child1.layoutX, 0.0f)
    assertEquals(300f, root_child1_child1.layoutY, 0.0f)
  }

  @Test
  fun test_align_baseline_parent_using_child_in_row_as_reference() {
    val config = YogaConfigFactory.create()

    val root = createYGNode(config, YogaFlexDirection.ROW, 1000f, 1000f, true)

    val root_child0 = createYGNode(config, YogaFlexDirection.COLUMN, 500f, 600f, false)
    root.addChildAt(root_child0, 0)

    val root_child1 = createYGNode(config, YogaFlexDirection.ROW, 500f, 800f, true)
    root.addChildAt(root_child1, 1)

    val root_child1_child0 = createYGNode(config, YogaFlexDirection.COLUMN, 500f, 500f, false)
    root_child1.addChildAt(root_child1_child0, 0)

    val root_child1_child1 = createYGNode(config, YogaFlexDirection.COLUMN, 500f, 400f, false)
    root_child1_child1.setBaselineFunction(getBaselineFunc())
    root_child1_child1.setIsReferenceBaseline(true)
    root_child1.addChildAt(root_child1_child1, 1)

    root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)

    assertEquals(0f, root_child0.layoutX, 0.0f)
    assertEquals(0f, root_child0.layoutY, 0.0f)

    assertEquals(500f, root_child1.layoutX, 0.0f)
    assertEquals(100f, root_child1.layoutY, 0.0f)

    assertEquals(0f, root_child1_child0.layoutX, 0.0f)
    assertEquals(0f, root_child1_child0.layoutY, 0.0f)

    assertEquals(500f, root_child1_child1.layoutX, 0.0f)
    assertEquals(300f, root_child1_child1.layoutY, 0.0f)
  }

  private fun createYGNode(
      config: YogaConfig,
      flexDirection: YogaFlexDirection,
      width: Float,
      height: Float,
      alignBaseline: Boolean,
  ): YogaNode {
    val node = mNodeFactory.create(config)
    node.flexDirection = flexDirection
    node.setWidth(width)
    node.setHeight(height)
    if (alignBaseline) {
      node.alignItems = YogaAlign.BASELINE
    }
    return node
  }
}
