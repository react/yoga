/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.yoga.utils

import com.facebook.yoga.YogaFlexDirection
import com.facebook.yoga.YogaMeasureFunction
import com.facebook.yoga.YogaMeasureMode
import com.facebook.yoga.YogaMeasureOutput
import com.facebook.yoga.YogaNode

class TestUtils {
  class intrinsicMeasureFunction : YogaMeasureFunction {
    override fun measure(
        node: YogaNode,
        width: Float,
        widthMode: YogaMeasureMode,
        height: Float,
        heightMode: YogaMeasureMode,
    ): Long {
      val text = node.data as String
      val measuredWidth: Float =
          if (widthMode == YogaMeasureMode.EXACTLY) {
            width
          } else if (widthMode == YogaMeasureMode.AT_MOST) {
            Math.min(text.length * widthPerChar, width)
          } else {
            text.length * widthPerChar
          }

      val effectiveWidth =
          if (node.flexDirection == YogaFlexDirection.COLUMN) {
            measuredWidth
          } else {
            Math.max(measuredWidth, getWidestWordWidth(text))
          }

      val measuredHeight: Float =
          if (heightMode == YogaMeasureMode.EXACTLY) {
            height
          } else if (heightMode == YogaMeasureMode.AT_MOST) {
            Math.min(caclulateHeight(text, effectiveWidth), height)
          } else {
            caclulateHeight(text, effectiveWidth)
          }

      return YogaMeasureOutput.make(measuredWidth, measuredHeight)
    }

    companion object {
      private const val widthPerChar: Float = 10f
      private const val heightPerChar: Float = 10f

      @JvmStatic
      fun getWidestWordWidth(text: String): Float {
        var widestWordLength = 0
        val words = text.split(" ")
        for (word in words) {
          val wordLength = word.length
          if (widestWordLength < wordLength) {
            widestWordLength = wordLength
          }
        }
        return widestWordLength.toFloat() * widthPerChar
      }

      @JvmStatic
      fun caclulateHeight(text: String, measuredWidth: Float): Float {
        if (text.length * widthPerChar <= measuredWidth) {
          return heightPerChar
        }

        val words = text.split(" ")
        var lines = 1f
        var currentLineLength = 0f
        for (word in words) {
          val wordWidth = word.length * widthPerChar
          if (wordWidth > measuredWidth) {
            if (currentLineLength > 0) {
              lines++
            }
            lines++
            currentLineLength = 0f
          } else if (currentLineLength + wordWidth <= measuredWidth) {
            currentLineLength += wordWidth + widthPerChar
          } else {
            lines++
            currentLineLength = wordWidth + widthPerChar
          }
        }
        return (if (currentLineLength == 0f) lines - 1 else lines) * heightPerChar
      }
    }
  }
}
