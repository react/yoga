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
class YogaNodeStylePropertiesTest {

  companion object {
    @JvmStatic
    @Parameterized.Parameters(name = "{0}")
    fun nodeFactories(): Iterable<TestParametrization.NodeFactory> =
        TestParametrization.nodeFactories()

    private val UNDEFINED = YogaValue.UNDEFINED.value
  }

  @Parameterized.Parameter lateinit var mNodeFactory: TestParametrization.NodeFactory

  @Test
  fun testDirectionDefault() {
    val node = createNode()

    assertEquals(node.styleDirection, YogaDirection.INHERIT)
    assertEquals(node.layoutDirection, YogaDirection.INHERIT)
  }

  @Test
  fun testDirectionAssignment() {
    val node = createNode()
    node.setDirection(YogaDirection.LTR)
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(YogaDirection.LTR, node.styleDirection)
    assertEquals(YogaDirection.LTR, node.layoutDirection)
  }

  @Test
  fun testDirectionAffectsLayout() {
    val node =
        style().direction(YogaDirection.RTL).width(200f).children(style().widthPercent(40f)).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(120f, node.getChildAt(0).layoutX, 0f)
  }

  @Test
  fun testFlexDirectionDefault() {
    val node = createNode()

    assertEquals(YogaFlexDirection.COLUMN, node.flexDirection)
  }

  @Test
  fun testFlexDirectionAssignment() {
    val node = style().flexDirection(YogaFlexDirection.COLUMN_REVERSE).node()

    assertEquals(YogaFlexDirection.COLUMN_REVERSE, node.flexDirection)
  }

  @Test
  fun testFlexDirectionAffectsLayout() {
    val node =
        style()
            .flexDirection(YogaFlexDirection.ROW_REVERSE)
            .width(200f)
            .children(style().widthPercent(40f))
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(120f, node.getChildAt(0).layoutX, 0f)
  }

  @Test
  fun testJustifyContentDefault() {
    val node = createNode()

    assertEquals(YogaJustify.FLEX_START, node.justifyContent)
  }

  @Test
  fun testJustifyContentAssignment() {
    val node = createNode()
    node.justifyContent = YogaJustify.SPACE_EVENLY

    assertEquals(YogaJustify.SPACE_EVENLY, node.justifyContent)
  }

  @Test
  fun testJustifyContentAffectsLayout() {
    val node =
        style()
            .justifyContent(YogaJustify.CENTER)
            .height(200f)
            .children(style().heightPercent(40f))
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(60f, node.getChildAt(0).layoutY, 0f)
  }

  @Test
  fun testAlignItemsDefault() {
    val node = createNode()

    assertEquals(YogaAlign.STRETCH, node.alignItems)
  }

  @Test
  fun testAlignItemsAssignment() {
    val node = createNode()
    node.alignItems = YogaAlign.SPACE_AROUND

    assertEquals(YogaAlign.SPACE_AROUND, node.alignItems)
  }

  @Test
  fun testAlignItemsAffectsLayout() {
    val node =
        style().alignItems(YogaAlign.CENTER).height(200f).children(style().widthPercent(40f)).node()
    node.calculateLayout(200f, UNDEFINED)

    assertEquals(60f, node.getChildAt(0).layoutX, 0f)
  }

  @Test
  fun testAlignSelfDefault() {
    val node = createNode()

    assertEquals(YogaAlign.AUTO, node.alignSelf)
  }

  @Test
  fun testAlignSelfAssignment() {
    val node = createNode()
    node.alignSelf = YogaAlign.FLEX_END

    assertEquals(YogaAlign.FLEX_END, node.alignSelf)
  }

  @Test
  fun testAlignSelfAffectsLayout() {
    val node =
        style().height(200f).children(style().alignSelf(YogaAlign.CENTER).widthPercent(40f)).node()
    node.calculateLayout(200f, UNDEFINED)

    assertEquals(60f, node.getChildAt(0).layoutX, 0f)
  }

  @Test
  fun testAlignContentDefault() {
    val node = createNode()

    assertEquals(YogaAlign.FLEX_START, node.alignContent)
  }

  @Test
  fun testAlignContentAssignment() {
    val node = createNode()
    node.alignContent = YogaAlign.BASELINE

    assertEquals(YogaAlign.BASELINE, node.alignContent)
  }

  @Test
  fun testAlignContentAffectsLayout() {
    val node =
        style()
            .alignContent(YogaAlign.SPACE_AROUND)
            .flexWrap(YogaWrap.WRAP)
            .height(200f)
            .width(200f)
            .children(
                style().widthPercent(20f).heightPercent(60f),
                style().widthPercent(20f).heightPercent(60f),
            )
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(30f, node.getChildAt(0).layoutX, 0f)
  }

  @Test
  fun testPositionTypeDefault() {
    val node = createNode()

    assertEquals(YogaPositionType.RELATIVE, node.positionType)
  }

  @Test
  fun testPositionTypeAssignment() {
    val node = createNode()
    node.positionType = YogaPositionType.ABSOLUTE

    assertEquals(YogaPositionType.ABSOLUTE, node.positionType)
  }

  @Test
  fun testPositionTypeAffectsLayout() {
    val node =
        style()
            .height(200f)
            .children(
                style().height(100f),
                style().height(100f).positionType(YogaPositionType.ABSOLUTE),
            )
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(0f, node.getChildAt(1).layoutY, 0f)
  }

  @Test
  fun testWrapAffectsLayout() {
    val node =
        style()
            .width(200f)
            .height(200f)
            .flexWrap(YogaWrap.WRAP_REVERSE)
            .children(style().width(10f).heightPercent(60f), style().heightPercent(60f))
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(190f, node.getChildAt(0).layoutX, 0f)
  }

  @Test
  fun testOverflowDefault() {
    val node = createNode()

    assertEquals(YogaOverflow.VISIBLE, node.overflow)
  }

  @Test
  fun testOverflowAssignment() {
    val node = createNode()
    node.overflow = YogaOverflow.SCROLL

    assertEquals(YogaOverflow.SCROLL, node.overflow)
  }

  // TODO add testOverflowAffectsLayout()

  @Test
  fun testDisplayDefault() {
    val node = createNode()

    assertEquals(YogaDisplay.FLEX, node.display)
  }

  @Test
  fun testDisplayAssignment() {
    val node = createNode()
    node.display = YogaDisplay.NONE

    assertEquals(YogaDisplay.NONE, node.display)
  }

  @Test
  fun testDisplayAffectsLayout() {
    val node =
        style()
            .children(style().flexGrow(1f).display(YogaDisplay.NONE), style().flexGrow(1f))
            .node()
    node.calculateLayout(200f, 200f)

    assertEquals(200f, node.getChildAt(1).layoutHeight, 0f)
  }

  @Test
  fun testFlexAffectsLayoutGrowing() {
    val node = style().height(200f).children(style().height(100f).flex(1.25f)).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(200f, node.getChildAt(0).layoutHeight, 0f)
  }

  @Test
  fun testFlexAffectsLayoutShrinking() {
    val node = style().height(200f).children(style().height(300f).flex(1.25f)).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(200f, node.getChildAt(0).layoutHeight, 0f)
  }

  @Test
  fun testFlexGrowDefault() {
    val node = createNode()

    assertEquals(0f, node.flexGrow, 0f)
  }

  @Test
  fun testFlexGrowAssignment() {
    val node = createNode()
    node.flexGrow = 2.5f

    assertEquals(2.5f, node.flexGrow, 0f)
  }

  @Test
  fun testFlexGrowAffectsLayout() {
    val node =
        style().height(200f).children(style().height(50f).flexGrow(1f), style().height(50f)).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(150f, node.getChildAt(0).layoutHeight, 0f)
  }

  @Test
  fun testFlexShrinkDefault() {
    val node = createNode()

    assertEquals(0f, node.flexShrink, 0f)
  }

  @Test
  fun testFlexShrinkAssignment() {
    val node = createNode()
    node.flexShrink = 2.5f

    assertEquals(2.5f, node.flexShrink, 0f)
  }

  @Test
  fun testFlexShrinkAffectsLayout() {
    val node =
        style()
            .height(200f)
            .children(style().height(150f).flexShrink(1f), style().height(150f))
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(50f, node.getChildAt(0).layoutHeight, 0f)
  }

  @Test
  fun testFlexBasisDefault() {
    val node = createNode()

    assertEquals(YogaValue.AUTO, node.flexBasis)
  }

  @Test
  fun testFlexBasisAssignment() {
    val node = createNode()
    node.setFlexBasis(50f)
    assertEquals(YogaValue(50f, YogaUnit.POINT), node.flexBasis)

    node.setFlexBasisPercent(20f)
    assertEquals(YogaValue(20f, YogaUnit.PERCENT), node.flexBasis)

    node.setFlexBasisAuto()
    assertEquals(YogaValue.AUTO, node.flexBasis)
  }

  @Test
  fun testFlexBasisAffectsLayout() {
    val node =
        style()
            .height(200f)
            .children(
                style().flexBasis(150f).flexShrink(1f),
                style().flexBasis(150f).flexShrink(1f),
            )
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(100f, node.getChildAt(0).layoutHeight, 0f)
  }

  @Test
  fun testFlexBasisPercentAffectsLayout() {
    val node =
        style()
            .height(200f)
            .children(style().flexBasisPercent(60f), style().flexBasisPercent(40f))
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(80f, node.getChildAt(1).layoutHeight, 0f)
  }

  @Test
  fun testMarginDefault() {
    val node = createNode()
    for (edge in YogaEdge.values()) {
      assertEquals(YogaValue.UNDEFINED, node.getMargin(edge))
    }
  }

  @Test
  fun testMarginAssignment() {
    val node = createNode()
    for (edge in YogaEdge.values()) {
      node.setMargin(edge, 25f)
      assertEquals(YogaValue(25f, YogaUnit.POINT), node.getMargin(edge))

      node.setMarginPercent(edge, 5f)
      assertEquals(YogaValue(5f, YogaUnit.PERCENT), node.getMargin(edge))

      node.setMarginAuto(edge)
      assertEquals(YogaValue.AUTO, node.getMargin(edge))
    }
  }

  @Test
  fun testNegativeMarginAssignment() {
    val node = createNode()
    for (edge in YogaEdge.values()) {
      node.setMargin(edge, -25f)
      assertEquals(YogaValue(-25f, YogaUnit.POINT), node.getMargin(edge))

      node.setMarginPercent(edge, -5f)
      assertEquals(YogaValue(-5f, YogaUnit.PERCENT), node.getMargin(edge))

      node.setMarginAuto(edge)
      assertEquals(YogaValue.AUTO, node.getMargin(edge))
    }
  }

  @Test
  fun testMarginPointAffectsLayout() {
    val node = style().margin(YogaEdge.TOP, 42f).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(42f, node.layoutY, 0f)
  }

  @Test
  fun testMarginPercentAffectsLayout() {
    val node =
        style().height(200f).children(style().flexGrow(1f).marginPercent(YogaEdge.TOP, 20f)).node()
    node.calculateLayout(200f, 200f)

    assertEquals(40f, node.getChildAt(0).layoutY, 0f)
  }

  @Test
  fun testMarginAutoAffectsLayout() {
    val node =
        style()
            .width(200f)
            .flexDirection(YogaFlexDirection.ROW)
            .children(style().marginAuto(YogaEdge.LEFT).marginAuto(YogaEdge.RIGHT).width(100f))
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(50f, node.getChildAt(0).layoutX, 0f)
  }

  @Test
  fun testPaddingDefault() {
    val node = createNode()
    for (edge in YogaEdge.values()) {
      assertEquals(YogaValue.UNDEFINED, node.getPadding(edge))
    }
  }

  @Test
  fun testPaddingAssignment() {
    val node = createNode()
    for (edge in YogaEdge.values()) {
      node.setPadding(edge, 25f)
      assertEquals(YogaValue(25f, YogaUnit.POINT), node.getPadding(edge))

      node.setPaddingPercent(edge, 5f)
      assertEquals(YogaValue(5f, YogaUnit.PERCENT), node.getPadding(edge))
    }
  }

  @Test
  fun testPaddingPointAffectsLayout() {
    val node = style().padding(YogaEdge.TOP, 42f).children(style()).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(42f, node.getChildAt(0).layoutY, 0f)
  }

  @Test
  fun testPaddingPercentAffectsLayout() {
    val node =
        style().height(200f).paddingPercent(YogaEdge.TOP, 20f).children(style().flexGrow(1f)).node()
    node.calculateLayout(200f, 200f)

    assertEquals(40f, node.getChildAt(0).layoutY, 0f)
  }

  @Test
  fun testBorderDefault() {
    val node = createNode()
    for (edge in YogaEdge.values()) {
      assertEquals(UNDEFINED, node.getBorder(edge), 0f)
    }
  }

  @Test
  fun testBorderAssignment() {
    val node = createNode()
    for (edge in YogaEdge.values()) {
      node.setBorder(edge, 2.5f)
      assertEquals(2.5f, node.getBorder(edge), 0f)
    }
  }

  @Test
  fun testBorderAffectsLayout() {
    val node = style().border(YogaEdge.TOP, 42f).children(style()).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(42f, node.getChildAt(0).layoutY, 0f)
  }

  @Test
  fun testPositionDefault() {
    val node = createNode()
    for (edge in YogaEdge.values()) {
      assertEquals(YogaValue.UNDEFINED, node.getPosition(edge))
    }
  }

  @Test
  fun testPositionAssignment() {
    val node = createNode()
    for (edge in YogaEdge.values()) {
      node.setPosition(edge, 25f)
      assertEquals(YogaValue(25f, YogaUnit.POINT), node.getPosition(edge))

      node.setPositionPercent(edge, 5f)
      assertEquals(YogaValue(5f, YogaUnit.PERCENT), node.getPosition(edge))
    }
  }

  @Test
  fun testPositionAffectsLayout() {
    val node =
        style()
            .height(100f)
            .children(
                style()
                    .positionType(YogaPositionType.ABSOLUTE)
                    .position(YogaEdge.TOP, 11f)
                    .position(YogaEdge.BOTTOM, 22f)
            )
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(67f, node.getChildAt(0).layoutHeight, 0f)
  }

  @Test
  fun testPositionPercentAffectsLayout() {
    val node =
        style()
            .width(100f)
            .children(
                style()
                    .positionType(YogaPositionType.ABSOLUTE)
                    .positionPercent(YogaEdge.LEFT, 11f)
                    .positionPercent(YogaEdge.RIGHT, 22f)
            )
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(67f, node.getChildAt(0).layoutWidth, 0f)
  }

  @Test
  fun testWidthDefault() {
    val node = createNode()

    assertEquals(YogaValue.AUTO, node.width)
  }

  @Test
  fun testWidthAssignment() {
    val node = createNode()
    node.setWidth(123f)
    assertEquals(YogaValue(123f, YogaUnit.POINT), node.width)

    node.setWidthPercent(45f)
    assertEquals(YogaValue(45f, YogaUnit.PERCENT), node.width)
  }

  @Test
  fun testWidthAffectsLayout() {
    val node = style().width(123f).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(123f, node.layoutWidth, 0f)
  }

  @Test
  fun testWidthPercentAffectsLayout() {
    val node = style().widthPercent(75f).node()
    node.calculateLayout(200f, UNDEFINED)

    assertEquals(150f, node.layoutWidth, 0f)
  }

  // TODO: testWidthAutoAffectsLayout

  @Test
  fun testHeightDefault() {
    val node = createNode()

    assertEquals(YogaValue.AUTO, node.height)
  }

  @Test
  fun testHeightAssignment() {
    val node = createNode()
    node.setHeight(123f)
    assertEquals(YogaValue(123f, YogaUnit.POINT), node.height)

    node.setHeightPercent(45f)
    assertEquals(YogaValue(45f, YogaUnit.PERCENT), node.height)
  }

  @Test
  fun testHeightAffectsLayout() {
    val node = style().height(123f).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(123f, node.layoutHeight, 0f)
  }

  @Test
  fun testHeightPercentAffectsLayout() {
    val node = style().heightPercent(75f).node()
    node.calculateLayout(UNDEFINED, 200f)

    assertEquals(150f, node.layoutHeight, 0f)
  }

  // TODO: testHeightAutoAffectsLayout

  @Test
  fun testMinWidthDefault() {
    val node = createNode()

    assertEquals(YogaValue.UNDEFINED, node.minWidth)
  }

  @Test
  fun testMinWidthAssignment() {
    val node = createNode()
    node.setMinWidth(123f)
    assertEquals(YogaValue(123f, YogaUnit.POINT), node.minWidth)

    node.setMinWidthPercent(45f)
    assertEquals(YogaValue(45f, YogaUnit.PERCENT), node.minWidth)
  }

  @Test
  fun testMinWidthAffectsLayout() {
    val node = style().minWidth(123f).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(123f, node.layoutWidth, 0f)
  }

  @Test
  fun testMinWidthPercentAffectsLayout() {
    val node = style().minWidthPercent(120f).node()
    node.calculateLayout(200f, UNDEFINED)

    assertEquals(240f, node.layoutWidth, 0f)
  }

  @Test
  fun testMinHeightDefault() {
    val node = createNode()

    assertEquals(YogaValue.UNDEFINED, node.minHeight)
  }

  @Test
  fun testMinHeightAssignment() {
    val node = createNode()
    node.setMinHeight(123f)
    assertEquals(YogaValue(123f, YogaUnit.POINT), node.minHeight)

    node.setMinHeightPercent(45f)
    assertEquals(YogaValue(45f, YogaUnit.PERCENT), node.minHeight)
  }

  @Test
  fun testMinHeightAffectsLayout() {
    val node = style().minHeight(123f).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(123f, node.layoutHeight, 0f)
  }

  @Test
  fun testMinHeightPercentAffectsLayout() {
    val node = style().minHeightPercent(120f).node()
    node.calculateLayout(UNDEFINED, 200f)

    assertEquals(240f, node.layoutHeight, 0f)
  }

  @Test
  fun testMaxWidthDefault() {
    val node = createNode()

    assertEquals(YogaValue.UNDEFINED, node.maxWidth)
  }

  @Test
  fun testMaxWidthAssignment() {
    val node = createNode()
    node.setMaxWidth(123f)
    assertEquals(YogaValue(123f, YogaUnit.POINT), node.maxWidth)

    node.setMaxWidthPercent(45f)
    assertEquals(YogaValue(45f, YogaUnit.PERCENT), node.maxWidth)
  }

  @Test
  fun testMaxWidthAffectsLayout() {
    val node = style().width(200f).children(style().maxWidth(123f)).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(123f, node.getChildAt(0).layoutWidth, 0f)
  }

  @Test
  fun testMaxWidthPercentAffectsLayout() {
    val node = style().width(200f).children(style().maxWidthPercent(80f)).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(160f, node.getChildAt(0).layoutWidth, 0f)
  }

  @Test
  fun testMaxHeightDefault() {
    val node = createNode()

    assertEquals(YogaValue.UNDEFINED, node.maxHeight)
  }

  @Test
  fun testMaxHeightAssignment() {
    val node = createNode()
    node.setMaxHeight(123f)
    assertEquals(YogaValue(123f, YogaUnit.POINT), node.maxHeight)

    node.setMaxHeightPercent(45f)
    assertEquals(YogaValue(45f, YogaUnit.PERCENT), node.maxHeight)
  }

  @Test
  fun testMaxHeightAffectsLayout() {
    val node =
        style()
            .height(200f)
            .flexDirection(YogaFlexDirection.ROW)
            .children(style().maxHeight(123f))
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(123f, node.getChildAt(0).layoutHeight, 0f)
  }

  @Test
  fun testMaxHeightPercentAffectsLayout() {
    val node =
        style()
            .flexDirection(YogaFlexDirection.ROW)
            .height(200f)
            .children(style().maxHeightPercent(80f))
            .node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(160f, node.getChildAt(0).layoutHeight, 0f)
  }

  @Test
  fun testAspectRatioDefault() {
    val node = createNode()

    assertEquals(UNDEFINED, node.aspectRatio, 0f)
  }

  @Test
  fun testAspectRatioAssignment() {
    val node = createNode()
    node.aspectRatio = 2.75f

    assertEquals(2.75f, node.aspectRatio, 0f)
  }

  @Test
  fun aspectRatioAffectsLayoutWithGivenWidth() {
    val node = style().children(style().width(300f).aspectRatio(1.5f)).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(200f, node.getChildAt(0).layoutHeight, 0f)
  }

  @Test
  fun aspectRatioAffectsLayoutWithGivenHeight() {
    val node = style().children(style().height(300f).aspectRatio(1.5f)).node()
    node.calculateLayout(UNDEFINED, UNDEFINED)

    assertEquals(450f, node.getChildAt(0).layoutWidth, 0f)
  }

  private fun createNode(): YogaNode = mNodeFactory.create()

  private fun style(): StyledNode = StyledNode(mNodeFactory)

  private class StyledNode(nodeFactory: TestParametrization.NodeFactory) {
    private val mNode: YogaNode = nodeFactory.create()

    fun node(): YogaNode = mNode

    fun children(vararg children: StyledNode): StyledNode {
      var i = mNode.childCount
      while (--i >= 0) {
        mNode.removeChildAt(i)
      }
      for (j in children.indices) {
        mNode.addChildAt(children[j].node(), j)
      }
      return this
    }

    fun direction(direction: YogaDirection): StyledNode {
      mNode.setDirection(direction)
      return this
    }

    fun width(width: Float): StyledNode {
      mNode.setWidth(width)
      return this
    }

    fun widthPercent(width: Float): StyledNode {
      mNode.setWidthPercent(width)
      return this
    }

    fun flexDirection(direction: YogaFlexDirection): StyledNode {
      mNode.flexDirection = direction
      return this
    }

    fun justifyContent(justify: YogaJustify): StyledNode {
      mNode.justifyContent = justify
      return this
    }

    fun height(height: Float): StyledNode {
      mNode.setHeight(height)
      return this
    }

    fun heightPercent(height: Float): StyledNode {
      mNode.setHeightPercent(height)
      return this
    }

    fun alignItems(align: YogaAlign): StyledNode {
      mNode.alignItems = align
      return this
    }

    fun alignSelf(align: YogaAlign): StyledNode {
      mNode.alignSelf = align
      return this
    }

    fun alignContent(align: YogaAlign): StyledNode {
      mNode.alignContent = align
      return this
    }

    fun flexWrap(wrap: YogaWrap): StyledNode {
      mNode.wrap = wrap
      return this
    }

    fun positionType(positionType: YogaPositionType): StyledNode {
      mNode.positionType = positionType
      return this
    }

    fun overflow(overflow: YogaOverflow): StyledNode {
      mNode.overflow = overflow
      return this
    }

    fun flexShrink(flexShrink: Float): StyledNode {
      mNode.flexShrink = flexShrink
      return this
    }

    fun display(display: YogaDisplay): StyledNode {
      mNode.display = display
      return this
    }

    fun flexGrow(flexGrow: Float): StyledNode {
      mNode.flexGrow = flexGrow
      return this
    }

    fun flex(flex: Float): StyledNode {
      mNode.flex = flex
      return this
    }

    fun flexBasis(flexBasis: Float): StyledNode {
      mNode.setFlexBasis(flexBasis)
      return this
    }

    fun flexBasisPercent(flexBasis: Float): StyledNode {
      mNode.setFlexBasisPercent(flexBasis)
      return this
    }

    fun margin(edge: YogaEdge, margin: Float): StyledNode {
      mNode.setMargin(edge, margin)
      return this
    }

    fun marginPercent(edge: YogaEdge, margin: Float): StyledNode {
      mNode.setMarginPercent(edge, margin)
      return this
    }

    fun marginAuto(edge: YogaEdge): StyledNode {
      mNode.setMarginAuto(edge)
      return this
    }

    fun padding(edge: YogaEdge, padding: Float): StyledNode {
      mNode.setPadding(edge, padding)
      return this
    }

    fun paddingPercent(edge: YogaEdge, padding: Float): StyledNode {
      mNode.setPaddingPercent(edge, padding)
      return this
    }

    fun border(edge: YogaEdge, border: Float): StyledNode {
      mNode.setBorder(edge, border)
      return this
    }

    fun position(edge: YogaEdge, position: Float): StyledNode {
      mNode.setPosition(edge, position)
      return this
    }

    fun positionPercent(edge: YogaEdge, position: Float): StyledNode {
      mNode.setPositionPercent(edge, position)
      return this
    }

    fun minWidth(minWidth: Float): StyledNode {
      mNode.setMinWidth(minWidth)
      return this
    }

    fun minWidthPercent(minWidth: Float): StyledNode {
      mNode.setMinWidthPercent(minWidth)
      return this
    }

    fun minHeight(minHeight: Float): StyledNode {
      mNode.setMinHeight(minHeight)
      return this
    }

    fun minHeightPercent(minHeight: Float): StyledNode {
      mNode.setMinHeightPercent(minHeight)
      return this
    }

    fun maxWidth(maxWidth: Float): StyledNode {
      mNode.setMaxWidth(maxWidth)
      return this
    }

    fun maxWidthPercent(maxWidth: Float): StyledNode {
      mNode.setMaxWidthPercent(maxWidth)
      return this
    }

    fun maxHeight(maxHeight: Float): StyledNode {
      mNode.setMaxHeight(maxHeight)
      return this
    }

    fun maxHeightPercent(maxHeight: Float): StyledNode {
      mNode.setMaxHeightPercent(maxHeight)
      return this
    }

    fun aspectRatio(aspectRatio: Float): StyledNode {
      mNode.aspectRatio = aspectRatio
      return this
    }
  }
}
