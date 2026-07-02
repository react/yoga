/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import {FlexDirection, MeasureMode} from 'yoga-layout';

type MeasureContext = {
  text: string;
  flexDirection: FlexDirection;
};

export function intrinsicSizeMeasureFunc(
  this: MeasureContext,
  width: number,
  widthMode: MeasureMode,
  height: number,
  heightMode: MeasureMode,
): {width: number; height: number} {
  const textLength = this.text.length;
  const words = this.text.split(' ');
  const flexDirection = this.flexDirection;
  const widthPerChar = 10;
  const heightPerChar = 10;

  let measuredWidth: number;
  let measuredHeight: number;

  switch (widthMode) {
    case MeasureMode.Exactly:
      measuredWidth = width;
      break;
    case MeasureMode.AtMost:
      measuredWidth = Math.min(width, textLength * widthPerChar);
      break;
    default:
      measuredWidth = textLength * widthPerChar;
  }

  switch (heightMode) {
    case MeasureMode.Exactly:
      measuredHeight = height;
      break;
    case MeasureMode.AtMost:
      measuredHeight = Math.min(height, calculateHeight());
      break;
    default:
      measuredHeight = calculateHeight();
  }

  function longestWordWidth() {
    return Math.max(...words.map(word => word.length)) * widthPerChar;
  }

  function calculateHeight() {
    if (textLength * widthPerChar <= measuredWidth) {
      return heightPerChar;
    }

    const maxLineWidth =
      flexDirection == FlexDirection.Column
        ? measuredWidth
        : Math.max(longestWordWidth(), measuredWidth);

    //if fixed width < width of widest word, take width of widest word

    let lines = 1;
    let currentLineLength = 0;
    for (const word of words) {
      const wordWidth = word.length * widthPerChar;
      if (wordWidth > maxLineWidth) {
        if (currentLineLength > 0) {
          lines++;
        }
        lines++;
        currentLineLength = 0;
      } else if (currentLineLength + wordWidth <= maxLineWidth) {
        currentLineLength += widthPerChar + wordWidth;
      } else {
        lines++;
        currentLineLength = widthPerChar + wordWidth;
      }
    }
    return (currentLineLength === 0 ? lines - 1 : lines) * heightPerChar;
  }

  return {width: measuredWidth, height: measuredHeight};
}
