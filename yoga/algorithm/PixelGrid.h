/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <yoga/Yoga.h>
#include <yoga/node/Node.h>

namespace facebook::yoga {

// Round a point value to the nearest physical pixel based on DPI
// (pointScaleFactor), returning the result in pixel space (still scaled by
// pointScaleFactor), where a grid-aligned value is an exact integer. Use this,
// rather than subtracting two `roundValueToPixelGrid()` results, whenever a
// difference of two rounded values is needed.
double roundValueToPixelGridScaled(
    double value,
    double pointScaleFactor,
    bool forceCeil,
    bool forceFloor);

// Round a point value to the nearest physical pixel based on DPI
// (pointScaleFactor)
float roundValueToPixelGrid(
    double value,
    double pointScaleFactor,
    bool forceCeil,
    bool forceFloor);

// Round the layout results of a node and its subtree to the pixel grid.
void roundLayoutResultsToPixelGrid(
    yoga::Node* node,
    double absoluteLeft,
    double absoluteTop);

} // namespace facebook::yoga
