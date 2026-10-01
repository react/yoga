/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

function quoteWithCEscapes(value: string, verticalTabEscape: string): string {
  let literal = '"';

  for (const character of value) {
    switch (character) {
      case '"':
        literal += '\\"';
        break;
      case '\\':
        literal += '\\\\';
        break;
      case '\b':
        literal += '\\b';
        break;
      case '\t':
        literal += '\\t';
        break;
      case '\n':
        literal += '\\n';
        break;
      case '\v':
        literal += verticalTabEscape;
        break;
      case '\f':
        literal += '\\f';
        break;
      case '\r':
        literal += '\\r';
        break;
      default: {
        const codePoint = character.codePointAt(0);
        if (codePoint != null && (codePoint < 0x20 || codePoint === 0x7f)) {
          // A fixed-width octal escape cannot consume a following hex digit,
          // unlike C++'s variable-width \x escape.
          literal += '\\' + codePoint.toString(8).padStart(3, '0');
        } else {
          literal += character;
        }
      }
    }
  }

  return literal + '"';
}

export function quoteCppString(value: string): string {
  return quoteWithCEscapes(value, '\\v');
}

export function quoteJavaString(value: string): string {
  // Java has no \v escape, but does support fixed-width octal escapes.
  return quoteWithCEscapes(value, '\\013');
}

export function quoteJavascriptString(value: string): string {
  const literal = JSON.stringify(value);
  if (literal == null) {
    throw new Error('Unable to serialize JavaScript string literal');
  }

  // Keep generated source valid in engines which treat these as line breaks
  // inside a quoted string.
  return literal.replace(/\u2028/g, '\\u2028').replace(/\u2029/g, '\\u2029');
}
