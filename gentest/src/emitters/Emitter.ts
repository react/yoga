/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import type {ValueWithUnit, LayoutNode} from '../types.ts';
import {
  parseStyleAttribute,
  applyStyles,
  getFlexDirectionForMeasure,
} from '../CssToYoga.ts';

interface TestCase {
  name: string;
  ltrLayout: LayoutNode;
  rtlLayout: LayoutNode;
  experiments: string[];
  disabled: boolean;
}

/**
 * Abstract base class for language-specific test code emitters.
 * Accumulates lines of output with indentation management.
 */
export default abstract class Emitter {
  protected lines: string[] = [];
  protected indents: string[] = [];
  protected indent: string;

  constructor(indent: string) {
    this.indent = indent;
  }

  push(line: string | string[]): void {
    if (Array.isArray(line)) {
      for (const l of line) {
        this.push(l);
      }
      return;
    }
    if (line.length > 0) {
      line = this.indents.join('') + line;
    }
    this.lines.push(line);
  }

  pushIndent(): void {
    this.indents.push(this.indent);
  }

  popIndent(): void {
    this.indents.pop();
  }

  getOutput(): string {
    return this.lines.join('\n');
  }

  // Comment header
  abstract emitCommentHeader(fixtureName: string): void;

  // Lifecycle
  abstract emitPrologue(fixtureName: string): void;
  abstract emitEpilogue(): void;
  abstract emitTestPrologue(
    name: string,
    experiments: string[],
    disabled: boolean,
  ): void;
  abstract emitTestEpilogue(experiments: string[]): void;
  abstract emitTestTreePrologue(nodeName: string): void;

  // Node operations
  abstract insertChild(parent: string, child: string, index: number): void;
  abstract calculateLayout(
    node: string,
    direction: string,
    experiments: string[],
  ): void;
  abstract setMeasureFunc(
    node: string,
    text: string,
    flexDirection: string,
  ): void;

  // Assertions
  abstract assertEQ(expected: number, actual: string): void;
  abstract layoutGetLeft(node: string): string;
  abstract layoutGetTop(node: string): string;
  abstract layoutGetWidth(node: string): string;
  abstract layoutGetHeight(node: string): string;

  // Style setters
  abstract setDirection(node: string, value: string): void;
  abstract setFlexDirection(node: string, value: string): void;
  abstract setJustifyContent(node: string, value: string): void;
  abstract setAlignContent(node: string, value: string): void;
  abstract setAlignItems(node: string, value: string): void;
  abstract setAlignSelf(node: string, value: string): void;
  abstract setPositionType(node: string, value: string): void;
  abstract setFlexWrap(node: string, value: string): void;
  abstract setOverflow(node: string, value: string): void;
  abstract setDisplay(node: string, value: string): void;
  abstract setBoxSizing(node: string, value: string): void;
  abstract setFlexGrow(node: string, value: string): void;
  abstract setFlexShrink(node: string, value: string): void;
  abstract setFlexBasis(node: string, value: ValueWithUnit): void;
  abstract setWidth(node: string, value: ValueWithUnit): void;
  abstract setHeight(node: string, value: ValueWithUnit): void;
  abstract setMinWidth(node: string, value: ValueWithUnit): void;
  abstract setMinHeight(node: string, value: ValueWithUnit): void;
  abstract setMaxWidth(node: string, value: ValueWithUnit): void;
  abstract setMaxHeight(node: string, value: ValueWithUnit): void;
  abstract setMargin(node: string, edge: string, value: ValueWithUnit): void;
  abstract setPadding(node: string, edge: string, value: ValueWithUnit): void;
  abstract setPosition(node: string, edge: string, value: ValueWithUnit): void;
  abstract setBorder(node: string, edge: string, value: ValueWithUnit): void;
  abstract setGap(node: string, gutter: string, value: ValueWithUnit): void;
  abstract setAspectRatio(node: string, value: ValueWithUnit): void;

  /**
   * Generate test code for a complete fixture.
   */
  generateFixture(fixtureName: string, testCases: TestCase[]): string {
    this.emitCommentHeader(fixtureName);

    this.emitPrologue(fixtureName);

    for (const testCase of testCases) {
      const ltrTree = testCase.ltrLayout;
      const rtlTree = testCase.rtlLayout;

      this.emitTestPrologue(
        testCase.name,
        testCase.experiments,
        testCase.disabled,
      );

      this.setupTestTree(ltrTree, 'root', null, undefined);

      this.calculateLayout('root', 'YGDirectionLTR', testCase.experiments);
      this.push('');

      this.assertTestTree(ltrTree, 'root');
      this.push('');

      this.calculateLayout('root', 'YGDirectionRTL', testCase.experiments);
      this.push('');

      this.assertTestTree(rtlTree, 'root');

      this.emitTestEpilogue(testCase.experiments);
    }

    this.emitEpilogue();

    return this.getOutput();
  }

  private setupTestTree(
    node: LayoutNode,
    nodeName: string,
    parentName: string | null,
    index: number | undefined,
  ): void {
    this.emitTestTreePrologue(nodeName);

    const styles = parseStyleAttribute(node.styleAttr);
    const isRoot = parentName === null;
    applyStyles(this, nodeName, styles, isRoot);

    if (parentName !== null && index !== undefined) {
      this.insertChild(parentName, nodeName, index);
    }

    if (node.innerText && node.children.length === 0) {
      const flexDirection = getFlexDirectionForMeasure(styles);
      this.setMeasureFunc(nodeName, node.innerText, flexDirection);
    }

    for (let i = 0; i < node.children.length; i++) {
      this.push('');
      const childName = nodeName + '_child' + i;
      this.setupTestTree(node.children[i], childName, nodeName, i);
    }
  }

  private assertTestTree(node: LayoutNode, nodeName: string): void {
    this.assertEQ(node.left, this.layoutGetLeft(nodeName));
    this.assertEQ(node.top, this.layoutGetTop(nodeName));
    this.assertEQ(node.width, this.layoutGetWidth(nodeName));
    this.assertEQ(node.height, this.layoutGetHeight(nodeName));

    for (let i = 0; i < node.children.length; i++) {
      this.push('');
      const childName = nodeName + '_child' + i;
      this.assertTestTree(node.children[i], childName);
    }
  }
}
