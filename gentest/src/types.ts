/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

export type ValueWithUnit =
  | {type: 'points'; value: number}
  | {type: 'percent'; value: number}
  | {type: 'auto'}
  | {type: 'max-content'}
  | {type: 'fit-content'}
  | {type: 'stretch'}
  | {type: 'undefined'};

export type ParsedStyles = Map<string, string>;

export interface LayoutNode {
  id: string;
  left: number;
  top: number;
  width: number;
  height: number;
  styleAttr: string;
  experiments: string[];
  disabled: boolean;
  errata: string | null;
  innerText: string;
  children: LayoutNode[];
}

export interface TestCase {
  name: string;
  ltrLayout: LayoutNode;
  rtlLayout: LayoutNode;
  styleAttr: string;
  experiments: string[];
  disabled: boolean;
  errata: string | null;
}
