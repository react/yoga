/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import styles from './LayoutBox.module.css';
import clsx from 'clsx';

export type LayoutMetrics = {
  top: number;
  left: number;
  width: number;
  height: number;
  clipPath?: string;
  overflow?: 'visible' | 'hidden' | 'scroll';
  display?: 'flex' | 'none' | 'contents';
  children?: LayoutMetrics[];
};

export type Props = Readonly<{
  metrics: LayoutMetrics;
  className?: string;
  depth: number;
}>;

export default function LayoutBox({metrics, depth, className}: Props) {
  const {children, ...style} = metrics;

  return (
    <div
      className={clsx(
        styles.layoutBox,
        (metrics.height === 0 || metrics.width === 0) && styles.zeroDim,
        depth % 5 == 0 && styles.depthZero,
        depth % 5 == 1 && styles.depthOne,
        depth % 5 == 2 && styles.depthTwo,
        depth % 5 == 3 && styles.depthThree,
        depth % 5 == 4 && styles.depthFour,
        className,
      )}
      style={{
        top: style.top,
        left: style.left,
        width: style.width,
        height: style.height,
        clipPath: style.clipPath,
        overflow: style.overflow,
        position: depth === 0 ? 'relative' : 'absolute',
      }}>
      {children?.map((child, i) => (
        <LayoutBox
          key={i}
          metrics={child}
          depth={style.display === 'contents' ? depth : depth + 1}
        />
      ))}
    </div>
  );
}
