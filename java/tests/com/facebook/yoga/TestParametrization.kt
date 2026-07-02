/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.yoga

object TestParametrization {
  @JvmStatic
  fun nodeFactories(): Iterable<NodeFactory> {
    val nodeFactory =
        object : NodeFactory {
          override fun create(): YogaNode = YogaNodeFactory.create()

          override fun create(config: YogaConfig): YogaNode = YogaNodeFactory.create(config)

          override fun toString(): String = "JNI"
        }
    return listOf(nodeFactory)
  }

  interface NodeFactory {
    fun create(): YogaNode

    fun create(config: YogaConfig): YogaNode
  }
}
