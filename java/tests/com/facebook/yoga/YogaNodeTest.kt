/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.yoga

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.Parameterized

@RunWith(Parameterized::class)
class YogaNodeTest {
  companion object {
    @JvmStatic
    @Parameterized.Parameters(name = "{0}")
    fun nodeFactories(): Iterable<TestParametrization.NodeFactory> =
        TestParametrization.nodeFactories()
  }

  @Parameterized.Parameter lateinit var mNodeFactory: TestParametrization.NodeFactory

  @Test
  fun testInit() {
    val node = createNode()
  }

  @Test
  fun testBaseline() {
    val root = createNode()
    root.flexDirection = YogaFlexDirection.ROW
    root.alignItems = YogaAlign.BASELINE
    root.setWidth(100f)
    root.setHeight(100f)

    val child1 = createNode()
    child1.setWidth(40f)
    child1.setHeight(40f)
    root.addChildAt(child1, 0)

    val child2 = createNode()
    child2.setWidth(40f)
    child2.setHeight(40f)
    child2.setBaselineFunction(YogaBaselineFunction { node, width, height -> 0f })
    root.addChildAt(child2, 1)

    root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)

    assertEquals(0, child1.layoutY.toInt())
    assertEquals(40, child2.layoutY.toInt())
  }

  @Test
  fun testMeasure() {
    val node = createNode()
    node.setMeasureFunction(
        YogaMeasureFunction { node, width, widthMode, height, heightMode ->
          YogaMeasureOutput.make(100, 100)
        }
    )
    node.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
    assertEquals(100, node.layoutWidth.toInt())
    assertEquals(100, node.layoutHeight.toInt())
  }

  @Test
  fun testMeasureFloat() {
    val node = createNode()
    node.setMeasureFunction(
        YogaMeasureFunction { node, width, widthMode, height, heightMode ->
          YogaMeasureOutput.make(100.5f, 100.5f)
        }
    )
    node.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
    assertEquals(101f, node.layoutWidth, 0.01f)
    assertEquals(101f, node.layoutHeight, 0.01f)
  }

  @Test
  fun testMeasureFloatMin() {
    val node = createNode()
    node.setMeasureFunction(
        YogaMeasureFunction { node, width, widthMode, height, heightMode ->
          YogaMeasureOutput.make(Float.MIN_VALUE, Float.MIN_VALUE)
        }
    )
    node.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
    assertEquals(Float.MIN_VALUE, node.layoutWidth, 0.01f)
    assertEquals(Float.MIN_VALUE, node.layoutHeight, 0.01f)
  }

  @Test
  fun testMeasureFloatBigNumber() {
    val node = createNode()
    val bigNumber = 10E5.toFloat()
    node.setMeasureFunction(
        YogaMeasureFunction { node, width, widthMode, height, heightMode ->
          YogaMeasureOutput.make(bigNumber, bigNumber)
        }
    )
    node.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
    assertEquals(bigNumber, node.layoutWidth, 0.01f)
    assertEquals(bigNumber, node.layoutHeight, 0.01f)
  }

  @Test
  fun testCopyStyle() {
    val node0 = createNode()
    assertTrue(YogaConstants.isUndefined(node0.maxHeight))

    val node1 = createNode()
    node1.setMaxHeight(100f)

    node0.copyStyle(node1)
    assertEquals(100, node0.maxHeight.value.toInt())
  }

  @Test
  fun testLayoutMargin() {
    val node = createNode()
    node.setWidth(100f)
    node.setHeight(100f)
    node.setMargin(YogaEdge.START, 1f)
    node.setMargin(YogaEdge.END, 2f)
    node.setMargin(YogaEdge.TOP, 3f)
    node.setMargin(YogaEdge.BOTTOM, 4f)
    node.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)

    assertEquals(1, node.getLayoutMargin(YogaEdge.LEFT).toInt())
    assertEquals(2, node.getLayoutMargin(YogaEdge.RIGHT).toInt())
    assertEquals(3, node.getLayoutMargin(YogaEdge.TOP).toInt())
    assertEquals(4, node.getLayoutMargin(YogaEdge.BOTTOM).toInt())
  }

  @Test
  fun testLayoutPadding() {
    val node = createNode()
    node.setWidth(100f)
    node.setHeight(100f)
    node.setPadding(YogaEdge.START, 1f)
    node.setPadding(YogaEdge.END, 2f)
    node.setPadding(YogaEdge.TOP, 3f)
    node.setPadding(YogaEdge.BOTTOM, 4f)
    node.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)

    assertEquals(1, node.getLayoutPadding(YogaEdge.LEFT).toInt())
    assertEquals(2, node.getLayoutPadding(YogaEdge.RIGHT).toInt())
    assertEquals(3, node.getLayoutPadding(YogaEdge.TOP).toInt())
    assertEquals(4, node.getLayoutPadding(YogaEdge.BOTTOM).toInt())
  }

  @Test
  fun testLayoutBorder() {
    val node = createNode()
    node.setWidth(100f)
    node.setHeight(100f)
    node.setBorder(YogaEdge.START, 1f)
    node.setBorder(YogaEdge.END, 2f)
    node.setBorder(YogaEdge.TOP, 3f)
    node.setBorder(YogaEdge.BOTTOM, 4f)
    node.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)

    assertEquals(1, node.getLayoutBorder(YogaEdge.LEFT).toInt())
    assertEquals(2, node.getLayoutBorder(YogaEdge.RIGHT).toInt())
    assertEquals(3, node.getLayoutBorder(YogaEdge.TOP).toInt())
    assertEquals(4, node.getLayoutBorder(YogaEdge.BOTTOM).toInt())
  }

  @Test
  fun testUseWebDefaults() {
    val config = YogaConfigFactory.create()
    config.setUseWebDefaults(true)
    val node = createNode(config)
    assertEquals(YogaFlexDirection.ROW, node.flexDirection)
  }

  @Test
  fun testPercentPaddingOnRoot() {
    val node = createNode()
    node.setPaddingPercent(YogaEdge.ALL, 10f)
    node.calculateLayout(50f, 50f)

    assertEquals(5, node.getLayoutPadding(YogaEdge.LEFT).toInt())
    assertEquals(5, node.getLayoutPadding(YogaEdge.RIGHT).toInt())
    assertEquals(5, node.getLayoutPadding(YogaEdge.TOP).toInt())
    assertEquals(5, node.getLayoutPadding(YogaEdge.BOTTOM).toInt())
  }

  @Test
  fun testDefaultEdgeValues() {
    val node = createNode()

    for (edge in YogaEdge.values()) {
      assertEquals(YogaUnit.UNDEFINED, node.getMargin(edge).unit)
      assertEquals(YogaUnit.UNDEFINED, node.getPadding(edge).unit)
      assertEquals(YogaUnit.UNDEFINED, node.getPosition(edge).unit)
      assertTrue(YogaConstants.isUndefined(node.getBorder(edge)))
    }
  }

  @Test
  fun initiallyHasNewLayout() {
    val root = createNode()
    assertTrue(root.hasNewLayout())
  }

  @Test
  fun initialLayoutCanBeMarkedSeen() {
    val root = createNode()
    root.markLayoutSeen()
    assertFalse(root.hasNewLayout())
  }

  @Test
  fun calculatingLayoutMarksLayoutAsUnseen() {
    val root = createNode()
    root.markLayoutSeen()
    root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
    assertTrue(root.hasNewLayout())
  }

  @Test
  fun calculatedLayoutCanBeMarkedSeen() {
    val root = createNode()
    root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
    root.markLayoutSeen()
    assertFalse(root.hasNewLayout())
  }

  @Test
  fun recalculatingLayoutDoesMarkAsUnseen() {
    val root = createNode()
    root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
    root.markLayoutSeen()
    root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
    assertTrue(root.hasNewLayout())
  }

  @Test
  fun resetAlsoResetsLayoutSeen() {
    val root = createNode()
    root.markLayoutSeen()
    root.reset()
    assertTrue(root.hasNewLayout())
  }

  @Test
  fun directionIsPassedThrough() {
    val root = createNode()

    root.setDirection(YogaDirection.RTL)
    root.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)

    assertEquals(root.layoutDirection, YogaDirection.RTL)
  }

  @Test
  fun testResetApiShouldResetAllLayoutOutputs() {
    val config = YogaConfigFactory.create()
    config.setErrata(YogaErrata.STRETCH_FLEX_BASIS)
    val node = createNode(config)
    node.setWidth(100f)
    node.setHeight(100f)
    node.setMargin(YogaEdge.START, 1f)
    node.setMargin(YogaEdge.END, 2f)
    node.setMargin(YogaEdge.TOP, 3f)
    node.setMargin(YogaEdge.BOTTOM, 4f)
    node.setPadding(YogaEdge.START, 1f)
    node.setPadding(YogaEdge.END, 2f)
    node.setPadding(YogaEdge.TOP, 3f)
    node.setPadding(YogaEdge.BOTTOM, 4f)
    node.setBorder(YogaEdge.START, 1f)
    node.setBorder(YogaEdge.END, 2f)
    node.setBorder(YogaEdge.TOP, 3f)
    node.setBorder(YogaEdge.BOTTOM, 4f)
    node.setDirection(YogaDirection.RTL)
    node.markLayoutSeen()
    node.setMeasureFunction(
        YogaMeasureFunction { node, width, widthMode, height, heightMode ->
          YogaMeasureOutput.make(100, 100)
        }
    )
    node.setBaselineFunction(YogaBaselineFunction { node, width, height -> height })
    node.data = ArrayList<Any>()

    node.calculateLayout(YogaConstants.UNDEFINED, YogaConstants.UNDEFINED)
    node.reset()

    assertEquals(0, node.layoutHeight.toInt())
    assertEquals(0, node.layoutWidth.toInt())
    assertEquals(0, node.getLayoutMargin(YogaEdge.LEFT).toInt())
    assertEquals(0, node.getLayoutMargin(YogaEdge.RIGHT).toInt())
    assertEquals(0, node.getLayoutMargin(YogaEdge.TOP).toInt())
    assertEquals(0, node.getLayoutMargin(YogaEdge.BOTTOM).toInt())
    assertEquals(0, node.getLayoutPadding(YogaEdge.LEFT).toInt())
    assertEquals(0, node.getLayoutPadding(YogaEdge.RIGHT).toInt())
    assertEquals(0, node.getLayoutPadding(YogaEdge.TOP).toInt())
    assertEquals(0, node.getLayoutPadding(YogaEdge.BOTTOM).toInt())
    assertEquals(0, node.getLayoutBorder(YogaEdge.LEFT).toInt())
    assertEquals(0, node.getLayoutBorder(YogaEdge.RIGHT).toInt())
    assertEquals(0, node.getLayoutBorder(YogaEdge.TOP).toInt())
    assertEquals(0, node.getLayoutBorder(YogaEdge.BOTTOM).toInt())
    assertEquals(node.layoutDirection, YogaDirection.INHERIT)
    assertTrue(node.hasNewLayout())
    assertFalse(node.isMeasureDefined)
    assertFalse(node.isBaselineDefined)
    assertNull(node.data)
  }

  private fun createNode(): YogaNode = mNodeFactory.create()

  private fun createNode(config: YogaConfig): YogaNode = mNodeFactory.create(config)
}
