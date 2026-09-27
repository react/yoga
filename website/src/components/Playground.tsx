/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import React, {
  CSSProperties,
  Suspense,
  lazy,
  useCallback,
  useEffect,
  useLayoutEffect,
  useRef,
  useState,
} from 'react';

import {usePrismTheme} from '@docusaurus/theme-common';
import clsx from 'clsx';
import {LiveProvider, LiveEditor, LivePreview, LiveError} from 'react-live';
import EditorToolbar from './EditorToolbar';

import type {FlexStyle} from './FlexStyle';
import type {StyleNode} from './YogaViewer';

import styles from './Playground.module.css';
import useIsBrowser from '@docusaurus/useIsBrowser';

export type Props = Readonly<{
  code: string;
  height?: CSSProperties['height'];
  autoFocus?: boolean;
}>;

export default function Playground({code, height, autoFocus}: Props) {
  const prismTheme = usePrismTheme();
  const editorScrollRef = useRef<HTMLDivElement>(null);
  const isBrowser = useIsBrowser();

  const [liveCode, setLiveCode] = useState(code);
  const [hasCodeChanged, setHasCodeChanged] = useState(false);
  const [scrollbarWidth, setScrollbarWidth] = useState(0);

  // Once react-live has hydrated the content-editable area, set focus to it
  // if requested
  useEffect(() => {
    if (autoFocus && hasCodeChanged) {
      const codeElem = editorScrollRef?.current?.querySelector('.prism-code');
      const sel = window.getSelection();
      if (codeElem?.clientHeight && sel != null) {
        sel.selectAllChildren(codeElem);
        sel.collapseToStart();
      }
    }
  }, [autoFocus, hasCodeChanged]);

  useLayoutEffect(() => {
    // The toolbar is positioned relative to the outside of the scrolling
    // container so it stays in the same place when scrolling, but this means
    // it isn't automatically adjusted for scrollbar width. If code change
    // causes overflow/scrollbar, adjust its position based on its width programmatically.
    if (editorScrollRef.current) {
      setScrollbarWidth(
        editorScrollRef.current.offsetWidth -
          editorScrollRef.current.clientWidth,
      );
    }
  }, [editorScrollRef, code]);

  const heightStyle = height
    ? ({'--yg-playground-height': height} as React.CSSProperties)
    : undefined;

  const handleCodeChange = useCallback((code: string) => {
    setHasCodeChanged(true);
    setLiveCode(code);
  }, []);

  return (
    <LiveProvider
      code={liveCode}
      theme={prismTheme}
      scope={{Node: LiveNode, Layout: RootLiveNode}}>
      <div className={styles.wrapper} style={heightStyle}>
        <div className={clsx(styles.playgroundRow)}>
          <div className={clsx(styles.editorColumn, 'playground-editor')}>
            <div className={styles.editorScroll} ref={editorScrollRef}>
              <EditorToolbar
                code={liveCode}
                className={styles.editorToolbar}
                style={{paddingRight: scrollbarWidth + 'px'}}
              />

              {isBrowser ? (
                <LiveEditor
                  className={clsx(styles.playgroundEditor)}
                  onChange={handleCodeChange}
                />
              ) : (
                <LiveEditorFallback code={liveCode} />
              )}
            </div>
          </div>
          <div className={clsx(styles.previewColumn)}>
            <LivePreview className={clsx(styles.livePreview)} />
            <LiveError className={clsx(styles.liveError)} />
          </div>
        </div>
      </div>
    </LiveProvider>
  );
}

/**
 * Provides a non-editable approximation of the LiveEditor result, without
 * relying on prism rendering, for use during SSR.
 * See https://github.com/facebook/docusaurus/issues/9629
 */
function LiveEditorFallback({code}: Readonly<{code: string}>) {
  return (
    <div className={clsx(styles.playgroundEditor)}>
      <pre className={clsx('prism-code', styles.liveEditorFallback)}>
        {code}
      </pre>
    </div>
  );
}

type RootLiveNodeProps = Readonly<{
  children: React.ReactNode;
  config?: {useWebDefaults?: boolean};
}>;

function RootLiveNode({children, config}: RootLiveNodeProps) {
  if (React.Children.count(children) !== 1) {
    return null;
  }

  const child = React.Children.only(children);
  if (!React.isValidElement(child) || child.type !== LiveNode) {
    return null;
  }

  const styleNode = styleNodeFromLiveNode(child as unknown as LiveNode);

  return (
    <Suspense fallback={null}>
      <LazyYogaViewer
        rootNode={styleNode}
        useWebDefaults={config?.useWebDefaults}
      />
    </Suspense>
  );
}

type LiveNodeProps = Readonly<{
  children: React.ReactNode;
  style: FlexStyle;
}>;

class LiveNode extends React.PureComponent<LiveNodeProps> {}

function styleNodeFromLiveNode(
  liveNode: React.ElementRef<typeof LiveNode>,
): StyleNode {
  const children: StyleNode[] = [];

  React.Children.forEach(liveNode.props.children, child => {
    if (React.isValidElement(child) && child.type === LiveNode) {
      children.push(styleNodeFromLiveNode(child as unknown as LiveNode));
    }
  });

  return {
    style: liveNode.props.style,
    children,
  };
}

// Docusaurus SSR does not correctly support top-level await in the import
// chain
// 1. https://github.com/facebook/docusaurus/issues/7238
// 2. https://github.com/facebook/docusaurus/issues/9468
const LazyYogaViewer = lazy(() => import('./YogaViewer'));
