/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

// @ts-nocheck

import {Unit, Direction} from './generated/YGEnums.ts';
import YGEnums from './generated/YGEnums.ts';

import type {
  Align,
  BoxSizing,
  Display,
  Edge,
  Errata,
  ExperimentalFeature,
  FlexDirection,
  Gutter,
  Justify,
  MeasureMode,
  Overflow,
  PositionType,
  Wrap,
} from './generated/YGEnums.ts';

type Layout = {
  left: number;
  right: number;
  top: number;
  bottom: number;
  width: number;
  height: number;
  hadOverflow: boolean;
};

type Size = {
  width: number;
  height: number;
};

type Value = {
  unit: Unit;
  value: number;
};

export type Config = {
  isExperimentalFeatureEnabled(feature: ExperimentalFeature): boolean;
  setExperimentalFeatureEnabled(
    feature: ExperimentalFeature,
    enabled: boolean,
  ): void;
  setPointScaleFactor(factor: number): void;
  getErrata(): Errata;
  setErrata(errata: Errata): void;
  useWebDefaults(): boolean;
  setUseWebDefaults(useWebDefaults: boolean): void;
};

export type DirtiedFunction = (node: Node) => void;

export type MeasureFunction = (
  width: number,
  widthMode: MeasureMode,
  height: number,
  heightMode: MeasureMode,
) => Size;

export type Node = {
  calculateLayout(
    width: number | 'auto' | undefined,
    height: number | 'auto' | undefined,
    direction?: Direction,
  ): void;
  copyStyle(node: Node): void;
  getAlignContent(): Align;
  getAlignItems(): Align;
  getAlignSelf(): Align;
  getAspectRatio(): number;
  getBorder(edge: Edge): number;
  getChild(index: number): Node;
  getChildCount(): number;
  getComputedBorder(edge: Edge): number;
  getComputedBottom(): number;
  getComputedHadOverflow(): boolean;
  getComputedHeight(): number;
  getComputedLayout(): Layout;
  getComputedLeft(): number;
  getComputedMargin(edge: Edge): number;
  getComputedPadding(edge: Edge): number;
  getComputedRight(): number;
  getComputedTop(): number;
  getComputedWidth(): number;
  getDirection(): Direction;
  getDisplay(): Display;
  getFlexBasis(): Value;
  getFlexDirection(): FlexDirection;
  getFlexGrow(): number;
  getFlexShrink(): number;
  getFlexWrap(): Wrap;
  getHeight(): Value;
  getJustifyContent(): Justify;
  getGap(gutter: Gutter): Value;
  getMargin(edge: Edge): Value;
  getMaxHeight(): Value;
  getMaxWidth(): Value;
  getMinHeight(): Value;
  getMinWidth(): Value;
  getOverflow(): Overflow;
  getPadding(edge: Edge): Value;
  getParent(): Node | null;
  getPosition(edge: Edge): Value;
  getPositionType(): PositionType;
  getBoxSizing(): BoxSizing;
  getWidth(): Value;
  insertChild(child: Node, index: number): void;
  isDirty(): boolean;
  isReferenceBaseline(): boolean;
  markDirty(): void;
  hasNewLayout(): boolean;
  markLayoutSeen(): void;
  removeChild(child: Node): void;
  reset(): void;
  setAlignContent(alignContent: Align): void;
  setAlignItems(alignItems: Align): void;
  setAlignSelf(alignSelf: Align): void;
  setAspectRatio(aspectRatio: number | undefined): void;
  setBorder(edge: Edge, borderWidth: number | undefined): void;
  setDirection(direction: Direction): void;
  setDisplay(display: Display): void;
  setFlex(flex: number | undefined): void;
  setFlexBasis(
    flexBasis:
      | number
      | 'auto'
      | 'fit-content'
      | 'max-content'
      | 'stretch'
      | `${number}%`
      | undefined,
  ): void;
  setFlexBasisPercent(flexBasis: number | undefined): void;
  setFlexBasisAuto(): void;
  setFlexBasisFitContent(): void;
  setFlexBasisMaxContent(): void;
  setFlexBasisStretch(): void;
  setFlexDirection(flexDirection: FlexDirection): void;
  setFlexGrow(flexGrow: number | undefined): void;
  setFlexShrink(flexShrink: number | undefined): void;
  setFlexWrap(flexWrap: Wrap): void;
  setHeight(
    height:
      | number
      | 'auto'
      | 'fit-content'
      | 'max-content'
      | 'stretch'
      | `${number}%`
      | undefined,
  ): void;
  setIsReferenceBaseline(isReferenceBaseline: boolean): void;
  setHeightAuto(): void;
  setHeightFitContent(): void;
  setHeightMaxContent(): void;
  setHeightPercent(height: number | undefined): void;
  setHeightStretch(): void;
  setJustifyContent(justifyContent: Justify): void;
  setGap(gutter: Gutter, gapLength: number | `${number}%` | undefined): Value;
  setGapPercent(gutter: Gutter, gapLength: number | undefined): Value;
  setMargin(
    edge: Edge,
    margin: number | 'auto' | `${number}%` | undefined,
  ): void;
  setMarginAuto(edge: Edge): void;
  setMarginPercent(edge: Edge, margin: number | undefined): void;
  setMaxHeight(
    maxHeight:
      | number
      | 'fit-content'
      | 'max-content'
      | 'stretch'
      | `${number}%`
      | undefined,
  ): void;
  setMaxHeightFitContent(): void;
  setMaxHeightMaxContent(): void;
  setMaxHeightPercent(maxHeight: number | undefined): void;
  setMaxHeightStretch(): void;
  setMaxWidth(
    maxWidth:
      | number
      | 'fit-content'
      | 'max-content'
      | 'stretch'
      | `${number}%`
      | undefined,
  ): void;
  setMaxWidthFitContent(): void;
  setMaxWidthMaxContent(): void;
  setMaxWidthPercent(maxWidth: number | undefined): void;
  setMaxWidthStretch(): void;
  setDirtiedFunc(dirtiedFunc: DirtiedFunction | null): void;
  setMeasureFunc(measureFunc: MeasureFunction | null): void;
  setMinHeight(
    minHeight:
      | number
      | 'fit-content'
      | 'max-content'
      | 'stretch'
      | `${number}%`
      | undefined,
  ): void;
  setMinHeightFitContent(): void;
  setMinHeightMaxContent(): void;
  setMinHeightPercent(minHeight: number | undefined): void;
  setMinHeightStretch(): void;
  setMinWidth(
    minWidth:
      | number
      | 'fit-content'
      | 'max-content'
      | 'stretch'
      | `${number}%`
      | undefined,
  ): void;
  setMinWidthFitContent(): void;
  setMinWidthMaxContent(): void;
  setMinWidthPercent(minWidth: number | undefined): void;
  setMinWidthStretch(): void;
  setOverflow(overflow: Overflow): void;
  setPadding(edge: Edge, padding: number | `${number}%` | undefined): void;
  setPaddingPercent(edge: Edge, padding: number | undefined): void;
  setPosition(edge: Edge, position: number | `${number}%` | undefined): void;
  setPositionPercent(edge: Edge, position: number | undefined): void;
  setPositionType(positionType: PositionType): void;
  setPositionAuto(edge: Edge): void;
  setBoxSizing(boxSizing: BoxSizing): void;
  setWidth(
    width:
      | number
      | 'auto'
      | 'fit-content'
      | 'max-content'
      | 'stretch'
      | `${number}%`
      | undefined,
  ): void;
  setWidthAuto(): void;
  setWidthFitContent(): void;
  setWidthMaxContent(): void;
  setWidthPercent(width: number | undefined): void;
  setWidthStretch(): void;
  unsetDirtiedFunc(): void;
  unsetMeasureFunc(): void;
  setAlwaysFormsContainingBlock(alwaysFormsContainingBlock: boolean): void;
};

export type Yoga = {
  Config: {
    create(): Config;
  };
  Node: {
    create(config?: Config): Node;
    createDefault(): Node;
    createWithConfig(config: Config): Node;
  };
} & typeof YGEnums;

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export default function wrapAssembly(lib: any): Yoga {
  // Pointer to the static YGValue return buffer (2 floats = 8 bytes).
  const valueBufPtr = lib._jswrap_YGValueBuffer();
  // Byte offset into HEAPF32 (4 bytes per float).
  const valueBufIdx = valueBufPtr >> 2;

  // Callback maps stored on Module for EM_JS access.
  lib._yogaMeasureFuncs = new Map();
  lib._yogaDirtiedFuncs = new Map();

  function readYGValue(): Value {
    return {
      value: lib.HEAPF32[valueBufIdx],
      unit: lib.HEAP32[valueBufIdx + 1],
    };
  }

  // --- Polymorphic setter dispatch ---
  // wasmPointFn is the direct WASM function for Point values, to avoid
  // infinite recursion (since e.g. setWidth is both the polymorphic entry
  // point AND what would resolve for Point suffix '').
  function dispatchSetter(fnName, wasmPointFn, args) {
    const value = args.pop();
    let unit, asNumber;

    if (value === 'auto') {
      unit = Unit.Auto;
      asNumber = undefined;
    } else if (value === 'max-content') {
      unit = Unit.MaxContent;
      asNumber = undefined;
    } else if (value === 'fit-content') {
      unit = Unit.FitContent;
      asNumber = undefined;
    } else if (value === 'stretch') {
      unit = Unit.Stretch;
      asNumber = undefined;
    } else if (typeof value === 'object') {
      unit = value.unit;
      asNumber = value.valueOf();
    } else {
      unit =
        typeof value === 'string' && value.endsWith('%')
          ? Unit.Percent
          : Unit.Point;
      asNumber = parseFloat(value);
      if (
        value !== undefined &&
        !Number.isNaN(value) &&
        Number.isNaN(asNumber)
      ) {
        throw new Error(`Invalid value ${value} for ${fnName}`);
      }
    }

    if (unit === Unit.Point) {
      if (asNumber !== undefined) {
        return wasmPointFn(this._ptr, ...args, asNumber);
      } else {
        return wasmPointFn(this._ptr, ...args);
      }
    }

    const suffix = {
      [Unit.Percent]: 'Percent',
      [Unit.Auto]: 'Auto',
      [Unit.MaxContent]: 'MaxContent',
      [Unit.FitContent]: 'FitContent',
      [Unit.Stretch]: 'Stretch',
    }[unit];

    if (suffix === undefined) {
      throw new Error(
        `Failed to execute "${fnName}": Unsupported unit '${value}'`,
      );
    }

    const method = this[`${fnName}${suffix}`];
    if (!method) {
      throw new Error(
        `Failed to execute "${fnName}": Unsupported unit '${value}'`,
      );
    }

    if (asNumber !== undefined) {
      return method.call(this, ...args, asNumber);
    } else {
      return method.call(this, ...args);
    }
  }

  // --- FinalizationRegistry for automatic cleanup ---
  const configRegistry = new FinalizationRegistry((ptr: number) => {
    lib._YGConfigFree(ptr);
  });

  const nodeRegistry = new FinalizationRegistry((ptr: number) => {
    lib._yogaMeasureFuncs.delete(ptr);
    lib._yogaDirtiedFuncs.delete(ptr);
    lib._YGNodeFinalize(ptr);
  });

  // --- Config class ---
  class ConfigImpl {
    _ptr: number;

    constructor(ptr: number) {
      this._ptr = ptr;
      configRegistry.register(this, ptr, this);
    }

    setExperimentalFeatureEnabled(
      feature: ExperimentalFeature,
      enabled: boolean,
    ): void {
      lib._YGConfigSetExperimentalFeatureEnabled(
        this._ptr,
        feature,
        enabled ? 1 : 0,
      );
    }

    isExperimentalFeatureEnabled(feature: ExperimentalFeature): boolean {
      return !!lib._YGConfigIsExperimentalFeatureEnabled(this._ptr, feature);
    }

    setPointScaleFactor(factor: number): void {
      lib._YGConfigSetPointScaleFactor(this._ptr, factor);
    }

    getErrata(): Errata {
      return lib._YGConfigGetErrata(this._ptr);
    }

    setErrata(errata: Errata): void {
      lib._YGConfigSetErrata(this._ptr, errata);
    }

    useWebDefaults(): boolean {
      return !!lib._YGConfigGetUseWebDefaults(this._ptr);
    }

    setUseWebDefaults(useWebDefaults: boolean): void {
      lib._YGConfigSetUseWebDefaults(this._ptr, useWebDefaults ? 1 : 0);
    }
  }

  // --- Node class ---
  class NodeImpl {
    _ptr: number;
    _children: NodeImpl[];
    _parent: NodeImpl | null;

    constructor(ptr: number) {
      this._ptr = ptr;
      this._children = [];
      this._parent = null;
      nodeRegistry.register(this, ptr, this);
    }

    // --- Tree hierarchy ---
    insertChild(child: NodeImpl, index: number): void {
      lib._YGNodeInsertChild(this._ptr, child._ptr, index);
      this._children.splice(index, 0, child);
      child._parent = this;
    }

    removeChild(child: NodeImpl): void {
      lib._YGNodeRemoveChild(this._ptr, child._ptr);
      const idx = this._children.indexOf(child);
      if (idx !== -1) {
        this._children.splice(idx, 1);
      }
      child._parent = null;
    }

    getChildCount(): number {
      return this._children.length;
    }

    getChild(index: number): NodeImpl {
      return this._children[index];
    }

    getParent(): NodeImpl | null {
      return this._parent;
    }

    // --- Lifecycle ---
    reset(): void {
      lib._yogaMeasureFuncs.delete(this._ptr);
      lib._yogaDirtiedFuncs.delete(this._ptr);
      this._children = [];
      this._parent = null;
      lib._YGNodeReset(this._ptr);
    }

    // --- Style setters ---
    copyStyle(other: NodeImpl): void {
      lib._YGNodeCopyStyle(this._ptr, other._ptr);
    }

    setPositionType(positionType: PositionType): void {
      lib._YGNodeStyleSetPositionType(this._ptr, positionType);
    }

    setPosition(edge: Edge, position): void {
      dispatchSetter.call(this, 'setPosition', lib._YGNodeStyleSetPosition, [
        edge,
        position,
      ]);
    }

    setPositionPercent(edge: Edge, position: number): void {
      lib._YGNodeStyleSetPositionPercent(this._ptr, edge, position);
    }

    setPositionAuto(edge: Edge): void {
      lib._YGNodeStyleSetPositionAuto(this._ptr, edge);
    }

    setAlignContent(alignContent: Align): void {
      lib._YGNodeStyleSetAlignContent(this._ptr, alignContent);
    }

    setAlignItems(alignItems: Align): void {
      lib._YGNodeStyleSetAlignItems(this._ptr, alignItems);
    }

    setAlignSelf(alignSelf: Align): void {
      lib._YGNodeStyleSetAlignSelf(this._ptr, alignSelf);
    }

    setFlexDirection(flexDirection: FlexDirection): void {
      lib._YGNodeStyleSetFlexDirection(this._ptr, flexDirection);
    }

    setFlexWrap(flexWrap: Wrap): void {
      lib._YGNodeStyleSetFlexWrap(this._ptr, flexWrap);
    }

    setJustifyContent(justifyContent: Justify): void {
      lib._YGNodeStyleSetJustifyContent(this._ptr, justifyContent);
    }

    setDirection(direction: Direction): void {
      lib._YGNodeStyleSetDirection(this._ptr, direction);
    }

    setMargin(edge: Edge, margin): void {
      dispatchSetter.call(this, 'setMargin', lib._YGNodeStyleSetMargin, [
        edge,
        margin,
      ]);
    }

    setMarginPercent(edge: Edge, margin: number): void {
      lib._YGNodeStyleSetMarginPercent(this._ptr, edge, margin);
    }

    setMarginAuto(edge: Edge): void {
      lib._YGNodeStyleSetMarginAuto(this._ptr, edge);
    }

    setOverflow(overflow: Overflow): void {
      lib._YGNodeStyleSetOverflow(this._ptr, overflow);
    }

    setDisplay(display: Display): void {
      lib._YGNodeStyleSetDisplay(this._ptr, display);
    }

    setFlex(flex: number): void {
      lib._YGNodeStyleSetFlex(this._ptr, flex);
    }

    setFlexBasis(flexBasis): void {
      dispatchSetter.call(this, 'setFlexBasis', lib._YGNodeStyleSetFlexBasis, [
        flexBasis,
      ]);
    }

    setFlexBasisPercent(flexBasis: number): void {
      lib._YGNodeStyleSetFlexBasisPercent(this._ptr, flexBasis);
    }

    setFlexBasisAuto(): void {
      lib._YGNodeStyleSetFlexBasisAuto(this._ptr);
    }

    setFlexBasisMaxContent(): void {
      lib._YGNodeStyleSetFlexBasisMaxContent(this._ptr);
    }

    setFlexBasisFitContent(): void {
      lib._YGNodeStyleSetFlexBasisFitContent(this._ptr);
    }

    setFlexBasisStretch(): void {
      lib._YGNodeStyleSetFlexBasisStretch(this._ptr);
    }

    setFlexGrow(flexGrow: number): void {
      lib._YGNodeStyleSetFlexGrow(this._ptr, flexGrow);
    }

    setFlexShrink(flexShrink: number): void {
      lib._YGNodeStyleSetFlexShrink(this._ptr, flexShrink);
    }

    setWidth(width): void {
      dispatchSetter.call(this, 'setWidth', lib._YGNodeStyleSetWidth, [width]);
    }

    setWidthPercent(width: number): void {
      lib._YGNodeStyleSetWidthPercent(this._ptr, width);
    }

    setWidthAuto(): void {
      lib._YGNodeStyleSetWidthAuto(this._ptr);
    }

    setWidthMaxContent(): void {
      lib._YGNodeStyleSetWidthMaxContent(this._ptr);
    }

    setWidthFitContent(): void {
      lib._YGNodeStyleSetWidthFitContent(this._ptr);
    }

    setWidthStretch(): void {
      lib._YGNodeStyleSetWidthStretch(this._ptr);
    }

    setHeight(height): void {
      dispatchSetter.call(this, 'setHeight', lib._YGNodeStyleSetHeight, [
        height,
      ]);
    }

    setHeightPercent(height: number): void {
      lib._YGNodeStyleSetHeightPercent(this._ptr, height);
    }

    setHeightAuto(): void {
      lib._YGNodeStyleSetHeightAuto(this._ptr);
    }

    setHeightMaxContent(): void {
      lib._YGNodeStyleSetHeightMaxContent(this._ptr);
    }

    setHeightFitContent(): void {
      lib._YGNodeStyleSetHeightFitContent(this._ptr);
    }

    setHeightStretch(): void {
      lib._YGNodeStyleSetHeightStretch(this._ptr);
    }

    setMinWidth(minWidth): void {
      dispatchSetter.call(this, 'setMinWidth', lib._YGNodeStyleSetMinWidth, [
        minWidth,
      ]);
    }

    setMinWidthPercent(minWidth: number): void {
      lib._YGNodeStyleSetMinWidthPercent(this._ptr, minWidth);
    }

    setMinWidthMaxContent(): void {
      lib._YGNodeStyleSetMinWidthMaxContent(this._ptr);
    }

    setMinWidthFitContent(): void {
      lib._YGNodeStyleSetMinWidthFitContent(this._ptr);
    }

    setMinWidthStretch(): void {
      lib._YGNodeStyleSetMinWidthStretch(this._ptr);
    }

    setMinHeight(minHeight): void {
      dispatchSetter.call(this, 'setMinHeight', lib._YGNodeStyleSetMinHeight, [
        minHeight,
      ]);
    }

    setMinHeightPercent(minHeight: number): void {
      lib._YGNodeStyleSetMinHeightPercent(this._ptr, minHeight);
    }

    setMinHeightMaxContent(): void {
      lib._YGNodeStyleSetMinHeightMaxContent(this._ptr);
    }

    setMinHeightFitContent(): void {
      lib._YGNodeStyleSetMinHeightFitContent(this._ptr);
    }

    setMinHeightStretch(): void {
      lib._YGNodeStyleSetMinHeightStretch(this._ptr);
    }

    setMaxWidth(maxWidth): void {
      dispatchSetter.call(this, 'setMaxWidth', lib._YGNodeStyleSetMaxWidth, [
        maxWidth,
      ]);
    }

    setMaxWidthPercent(maxWidth: number): void {
      lib._YGNodeStyleSetMaxWidthPercent(this._ptr, maxWidth);
    }

    setMaxWidthMaxContent(): void {
      lib._YGNodeStyleSetMaxWidthMaxContent(this._ptr);
    }

    setMaxWidthFitContent(): void {
      lib._YGNodeStyleSetMaxWidthFitContent(this._ptr);
    }

    setMaxWidthStretch(): void {
      lib._YGNodeStyleSetMaxWidthStretch(this._ptr);
    }

    setMaxHeight(maxHeight): void {
      dispatchSetter.call(this, 'setMaxHeight', lib._YGNodeStyleSetMaxHeight, [
        maxHeight,
      ]);
    }

    setMaxHeightPercent(maxHeight: number): void {
      lib._YGNodeStyleSetMaxHeightPercent(this._ptr, maxHeight);
    }

    setMaxHeightMaxContent(): void {
      lib._YGNodeStyleSetMaxHeightMaxContent(this._ptr);
    }

    setMaxHeightFitContent(): void {
      lib._YGNodeStyleSetMaxHeightFitContent(this._ptr);
    }

    setMaxHeightStretch(): void {
      lib._YGNodeStyleSetMaxHeightStretch(this._ptr);
    }

    setAspectRatio(aspectRatio: number): void {
      lib._YGNodeStyleSetAspectRatio(this._ptr, aspectRatio);
    }

    setBorder(edge: Edge, border: number): void {
      lib._YGNodeStyleSetBorder(this._ptr, edge, border);
    }

    setPadding(edge: Edge, padding): void {
      dispatchSetter.call(this, 'setPadding', lib._YGNodeStyleSetPadding, [
        edge,
        padding,
      ]);
    }

    setPaddingPercent(edge: Edge, padding: number): void {
      lib._YGNodeStyleSetPaddingPercent(this._ptr, edge, padding);
    }

    setGap(gutter: Gutter, gapLength): void {
      dispatchSetter.call(this, 'setGap', lib._YGNodeStyleSetGap, [
        gutter,
        gapLength,
      ]);
    }

    setGapPercent(gutter: Gutter, gapLength: number): void {
      lib._YGNodeStyleSetGapPercent(this._ptr, gutter, gapLength);
    }

    setBoxSizing(boxSizing: BoxSizing): void {
      lib._YGNodeStyleSetBoxSizing(this._ptr, boxSizing);
    }

    setIsReferenceBaseline(isReferenceBaseline: boolean): void {
      lib._YGNodeSetIsReferenceBaseline(this._ptr, isReferenceBaseline ? 1 : 0);
    }

    setAlwaysFormsContainingBlock(alwaysFormsContainingBlock: boolean): void {
      lib._YGNodeSetAlwaysFormsContainingBlock(
        this._ptr,
        alwaysFormsContainingBlock ? 1 : 0,
      );
    }

    // --- Style getters ---
    getPositionType(): PositionType {
      return lib._YGNodeStyleGetPositionType(this._ptr);
    }

    getPosition(edge: Edge): Value {
      lib._jswrap_YGNodeStyleGetPosition(this._ptr, edge);
      return readYGValue();
    }

    getAlignContent(): Align {
      return lib._YGNodeStyleGetAlignContent(this._ptr);
    }

    getAlignItems(): Align {
      return lib._YGNodeStyleGetAlignItems(this._ptr);
    }

    getAlignSelf(): Align {
      return lib._YGNodeStyleGetAlignSelf(this._ptr);
    }

    getFlexDirection(): FlexDirection {
      return lib._YGNodeStyleGetFlexDirection(this._ptr);
    }

    getFlexWrap(): Wrap {
      return lib._YGNodeStyleGetFlexWrap(this._ptr);
    }

    getJustifyContent(): Justify {
      return lib._YGNodeStyleGetJustifyContent(this._ptr);
    }

    getDirection(): Direction {
      return lib._YGNodeStyleGetDirection(this._ptr);
    }

    getMargin(edge: Edge): Value {
      lib._jswrap_YGNodeStyleGetMargin(this._ptr, edge);
      return readYGValue();
    }

    getOverflow(): Overflow {
      return lib._YGNodeStyleGetOverflow(this._ptr);
    }

    getDisplay(): Display {
      return lib._YGNodeStyleGetDisplay(this._ptr);
    }

    getFlexBasis(): Value {
      lib._jswrap_YGNodeStyleGetFlexBasis(this._ptr);
      return readYGValue();
    }

    getFlexGrow(): number {
      return lib._YGNodeStyleGetFlexGrow(this._ptr);
    }

    getFlexShrink(): number {
      return lib._YGNodeStyleGetFlexShrink(this._ptr);
    }

    getWidth(): Value {
      lib._jswrap_YGNodeStyleGetWidth(this._ptr);
      return readYGValue();
    }

    getHeight(): Value {
      lib._jswrap_YGNodeStyleGetHeight(this._ptr);
      return readYGValue();
    }

    getMinWidth(): Value {
      lib._jswrap_YGNodeStyleGetMinWidth(this._ptr);
      return readYGValue();
    }

    getMinHeight(): Value {
      lib._jswrap_YGNodeStyleGetMinHeight(this._ptr);
      return readYGValue();
    }

    getMaxWidth(): Value {
      lib._jswrap_YGNodeStyleGetMaxWidth(this._ptr);
      return readYGValue();
    }

    getMaxHeight(): Value {
      lib._jswrap_YGNodeStyleGetMaxHeight(this._ptr);
      return readYGValue();
    }

    getAspectRatio(): number {
      return lib._YGNodeStyleGetAspectRatio(this._ptr);
    }

    getBorder(edge: Edge): number {
      return lib._YGNodeStyleGetBorder(this._ptr, edge);
    }

    getPadding(edge: Edge): Value {
      lib._jswrap_YGNodeStyleGetPadding(this._ptr, edge);
      return readYGValue();
    }

    getGap(gutter: Gutter): Value {
      lib._jswrap_YGNodeStyleGetGap(this._ptr, gutter);
      return readYGValue();
    }

    getBoxSizing(): BoxSizing {
      return lib._YGNodeStyleGetBoxSizing(this._ptr);
    }

    isReferenceBaseline(): boolean {
      return !!lib._YGNodeIsReferenceBaseline(this._ptr);
    }

    // --- Measure / Dirtied ---
    setMeasureFunc(measureFunc: MeasureFunction | null): void {
      if (measureFunc) {
        lib._yogaMeasureFuncs.set(this._ptr, measureFunc);
        lib._jswrap_YGNodeSetMeasureFunc(this._ptr);
      } else {
        this.unsetMeasureFunc();
      }
    }

    unsetMeasureFunc(): void {
      lib._yogaMeasureFuncs.delete(this._ptr);
      lib._jswrap_YGNodeUnsetMeasureFunc(this._ptr);
    }

    setDirtiedFunc(dirtiedFunc: DirtiedFunction | null): void {
      if (dirtiedFunc) {
        const nodeWeakRef = new WeakRef(this);
        lib._yogaDirtiedFuncs.set(this._ptr, () => {
          const node = nodeWeakRef.deref();
          if (node) dirtiedFunc(node);
        });
        lib._jswrap_YGNodeSetDirtiedFunc(this._ptr);
      } else {
        this.unsetDirtiedFunc();
      }
    }

    unsetDirtiedFunc(): void {
      lib._yogaDirtiedFuncs.delete(this._ptr);
      lib._jswrap_YGNodeUnsetDirtiedFunc(this._ptr);
    }

    // --- Dirty / Layout ---
    markDirty(): void {
      lib._YGNodeMarkDirty(this._ptr);
    }

    isDirty(): boolean {
      return !!lib._YGNodeIsDirty(this._ptr);
    }

    markLayoutSeen(): void {
      lib._YGNodeSetHasNewLayout(this._ptr, 0);
    }

    hasNewLayout(): boolean {
      return !!lib._YGNodeGetHasNewLayout(this._ptr);
    }

    calculateLayout(
      width: number | 'auto' | undefined = NaN,
      height: number | 'auto' | undefined = NaN,
      direction: Direction = Direction.LTR,
    ): void {
      lib._YGNodeCalculateLayout(this._ptr, width, height, direction);
    }

    // --- Layout getters ---
    getComputedLeft(): number {
      return lib._YGNodeLayoutGetLeft(this._ptr);
    }

    getComputedRight(): number {
      return lib._YGNodeLayoutGetRight(this._ptr);
    }

    getComputedTop(): number {
      return lib._YGNodeLayoutGetTop(this._ptr);
    }

    getComputedBottom(): number {
      return lib._YGNodeLayoutGetBottom(this._ptr);
    }

    getComputedWidth(): number {
      return lib._YGNodeLayoutGetWidth(this._ptr);
    }

    getComputedHeight(): number {
      return lib._YGNodeLayoutGetHeight(this._ptr);
    }

    getComputedHadOverflow(): boolean {
      return !!lib._YGNodeLayoutGetHadOverflow(this._ptr);
    }

    getComputedLayout(): Layout {
      return {
        left: lib._YGNodeLayoutGetLeft(this._ptr),
        right: lib._YGNodeLayoutGetRight(this._ptr),
        top: lib._YGNodeLayoutGetTop(this._ptr),
        bottom: lib._YGNodeLayoutGetBottom(this._ptr),
        width: lib._YGNodeLayoutGetWidth(this._ptr),
        height: lib._YGNodeLayoutGetHeight(this._ptr),
        hadOverflow: !!lib._YGNodeLayoutGetHadOverflow(this._ptr),
      };
    }

    getComputedMargin(edge: Edge): number {
      return lib._YGNodeLayoutGetMargin(this._ptr, edge);
    }

    getComputedBorder(edge: Edge): number {
      return lib._YGNodeLayoutGetBorder(this._ptr, edge);
    }

    getComputedPadding(edge: Edge): number {
      return lib._YGNodeLayoutGetPadding(this._ptr, edge);
    }
  }

  return {
    Config: {
      create(): Config {
        return new ConfigImpl(lib._YGConfigNew());
      },
    },
    Node: {
      create(config?: Config): Node {
        if (config) {
          return new NodeImpl(
            lib._YGNodeNewWithConfig((config as ConfigImpl)._ptr),
          );
        }
        return new NodeImpl(lib._YGNodeNew());
      },
      createDefault(): Node {
        return new NodeImpl(lib._YGNodeNew());
      },
      createWithConfig(config: Config): Node {
        return new NodeImpl(
          lib._YGNodeNewWithConfig((config as ConfigImpl)._ptr),
        );
      },
    },
    ...YGEnums,
  };
}
