/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import type {WebDriver} from 'selenium-webdriver';
import type {LayoutNode} from './types.ts';

/**
 * Build the layout tree by querying the DOM via a single executeScript call.
 * Sets the document direction first, then walks the DOM under #container.
 */
export default async function buildLayoutTree(
  driver: WebDriver,
  direction: 'ltr' | 'rtl',
): Promise<LayoutNode[]> {
  const rawNodes = await driver.executeScript<RawNode[]>(`
    var container = document.getElementById('container');

    // Save original style attributes before we modify them with direction.
    var originalStyles = new Map();
    function saveStyles(parent) {
      for (var i = 0; i < parent.children.length; i++) {
        var child = parent.children[i];
        originalStyles.set(child, child.getAttribute('style') || '');
        saveStyles(child);
      }
    }
    saveStyles(container);

    // Set direction on each test case element (not on <html>) so that the
    // container stays LTR and root elements remain at (0, 0).
    for (var i = 0; i < container.children.length; i++) {
      container.children[i].style.direction = '${direction}';
    }

    function getRoundedSize(node) {
      var rect = node.getBoundingClientRect();
      return {
        width: Math.round(rect.right) - Math.round(rect.left),
        height: Math.round(rect.bottom) - Math.round(rect.top)
      };
    }

    function walkTree(parent, parentLeft, parentTop) {
      var result = [];
      for (var i = 0; i < parent.children.length; i++) {
        var child = parent.children[i];
        var rect = child.getBoundingClientRect();
        var size = getRoundedSize(child);
        var left = Math.round(rect.left - parentLeft);
        var top = Math.round(rect.top - parentTop);

        var innerText = '';
        if (child.children.length === 0 && child.innerText) {
          innerText = child.innerText;
        }

        result.push({
          id: child.id || '',
          left: left,
          top: top,
          width: size.width,
          height: size.height,
          styleAttr: originalStyles.get(child) || '',
          experiments: child.dataset.experiments ? child.dataset.experiments.split(' ') : [],
          disabled: child.dataset.disabled === 'true',
          innerText: innerText,
          children: walkTree(child, rect.left, rect.top)
        });
      }
      return result;
    }

    var containerRect = container.getBoundingClientRect();
    return walkTree(container, containerRect.left, containerRect.top);
  `);

  return rawNodes as LayoutNode[];
}

// Internal type for the raw JSON returned from executeScript
interface RawNode {
  id: string;
  left: number;
  top: number;
  width: number;
  height: number;
  styleAttr: string;
  experiments: string[];
  disabled: boolean;
  innerText: string;
  children: RawNode[];
}
