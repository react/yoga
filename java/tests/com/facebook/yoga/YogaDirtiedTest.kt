/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.yoga

import java.lang.ref.WeakReference
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertSame
import org.junit.Assert.assertTrue
import org.junit.Assert.fail
import org.junit.Test

class YogaDirtiedTest {
  @Test
  fun callbackRunsOncePerDirtyTransitionAndPropagatesToParent() {
    val parent = YogaNodeFactory.create()
    val child = YogaNodeFactory.create()
    parent.addChildAt(child, 0)
    parent.calculateLayout(100f, 100f)
    val dirtied = mutableListOf<YogaNode>()
    parent.setDirtiedFunction { dirtied.add(it) }
    child.setDirtiedFunction { dirtied.add(it) }

    child.setWidth(10f)
    child.setHeight(20f)

    assertEquals(listOf(child, parent), dirtied)
    assertTrue(parent.isDirty())
    parent.calculateLayout(100f, 100f)
    child.setWidth(30f)
    assertEquals(listOf(child, parent, child, parent), dirtied)
  }

  @Test
  fun replacingAndRemovingCallbackTakesEffectImmediately() {
    val node = YogaNodeFactory.create()
    var oldCalls = 0
    var newCalls = 0
    node.setDirtiedFunction { oldCalls++ }
    node.setDirtiedFunction { newCalls++ }
    node.calculateLayout(100f, 100f)
    node.setWidth(10f)
    assertEquals(0, oldCalls)
    assertEquals(1, newCalls)
    node.calculateLayout(100f, 100f)
    node.setDirtiedFunction(null)
    node.setWidth(20f)
    assertEquals(1, newCalls)
  }

  @Test
  fun resetRemovesCallback() {
    val node = YogaNodeFactory.create()
    var calls = 0
    node.setDirtiedFunction { calls++ }
    node.reset()
    node.calculateLayout(100f, 100f)
    node.setWidth(10f)
    assertEquals(0, calls)
  }

  @Test
  fun clonesCallBackWithTheirOwnNode() {
    val original = YogaNodeFactory.create()
    val dirtied = mutableListOf<YogaNode>()
    original.setDirtiedFunction { dirtied.add(it) }
    val withoutChildren = original.cloneWithoutChildren()
    val withChildren = original.cloneWithChildren()
    for (node in listOf(original, withoutChildren, withChildren)) {
      node.calculateLayout(100f, 100f)
      node.setWidth(10f)
    }
    assertEquals(listOf(original, withoutChildren, withChildren), dirtied)
    withoutChildren.setDirtiedFunction(null)
    original.calculateLayout(100f, 100f)
    original.setWidth(20f)
    assertSame(original, dirtied.last())
  }

  @Test
  fun callbackExceptionReachesCallerAfterDirtyPropagation() {
    val parent = YogaNodeFactory.create()
    val child = YogaNodeFactory.create()
    parent.addChildAt(child, 0)
    parent.calculateLayout(100f, 100f)
    val expected = IllegalStateException("dirtied callback")
    child.setDirtiedFunction { throw expected }
    parent.setDirtiedFunction { fail("No Java callback should run with a pending exception") }
    try {
      child.setWidth(10f)
      fail("Expected callback exception")
    } catch (actual: IllegalStateException) {
      assertSame(expected, actual)
    }
    assertTrue(child.isDirty())
    assertTrue(parent.isDirty())
    child.setDirtiedFunction(null)
    parent.setDirtiedFunction(null)
    parent.calculateLayout(100f, 100f)
    assertFalse(parent.isDirty())
    assertEquals(10f, child.layoutWidth)
  }

  @Test
  fun callbackMayUnregisterAndMutateAnotherNode() {
    val node = YogaNodeFactory.create()
    val other = YogaNodeFactory.create()
    node.calculateLayout(100f, 100f)
    other.calculateLayout(100f, 100f)
    var calls = 0
    node.setDirtiedFunction {
      calls++
      it.setDirtiedFunction(null)
      other.setWidth(25f)
    }
    node.setWidth(10f)
    assertEquals(1, calls)
    assertTrue(other.isDirty())
    node.calculateLayout(100f, 100f)
    node.setWidth(20f)
    assertEquals(1, calls)
  }

  @Test
  fun throwingInsertionPreservesJavaAndNativeTree() {
    val parent = YogaNodeFactory.create()
    val child = YogaNodeFactory.create()
    parent.calculateLayout(100f, 100f)
    val expected = IllegalStateException("insertion")
    parent.setDirtiedFunction { throw expected }
    try {
      parent.addChildAt(child, 0)
      fail("Expected callback exception")
    } catch (actual: IllegalStateException) {
      assertSame(expected, actual)
    }
    assertEquals(1, parent.childCount)
    assertSame(child, parent.getChildAt(0))
    assertSame(parent, child.getOwner())
    parent.setDirtiedFunction(null)
    parent.calculateLayout(100f, 100f)
    assertSame(child, parent.removeChildAt(0))
    assertEquals(0, parent.childCount)
  }

  @Test
  fun callbackCanReplaceItself() {
    val node = YogaNodeFactory.create()
    var calls = 0
    node.setDirtiedFunction {
      calls++
      it.setDirtiedFunction { calls += 10 }
    }
    node.calculateLayout(100f, 100f)
    node.setWidth(10f)
    node.calculateLayout(100f, 100f)
    node.setWidth(20f)
    assertEquals(11, calls)
  }

  @Test
  fun cloningTreeBindsChildCallbackToClonedChild() {
    val parent = YogaNodeFactory.create()
    val child = YogaNodeFactory.create()
    parent.addChildAt(child, 0)
    val dirtied = mutableListOf<YogaNode>()
    child.setDirtiedFunction { dirtied.add(it) }
    parent.calculateLayout(100f, 100f)
    val clone = parent.cloneWithChildren()
    clone.calculateLayout(100f, 100f)
    dirtied.clear()
    clone.getChildAt(0).setWidth(10f)
    assertEquals(listOf(clone.getChildAt(0)), dirtied)
    assertFalse(child.isDirty())
  }

  @Test
  fun callbackRunsOnCallingJavaThread() {
    val node = YogaNodeFactory.create()
    var failure: Throwable? = null
    val thread = Thread {
      try {
        var callbackThread: Thread? = null
        node.setDirtiedFunction { callbackThread = Thread.currentThread() }
        node.calculateLayout(100f, 100f)
        node.setWidth(10f)
        assertSame(Thread.currentThread(), callbackThread)
      } catch (error: Throwable) {
        failure = error
      }
    }
    thread.start()
    thread.join()
    failure?.let { throw it }
  }

  @Test
  fun repeatedCallbackExceptionsDoNotBreakLaterLayout() {
    val node = YogaNodeFactory.create()
    val expected = IllegalArgumentException("repeat")
    node.setDirtiedFunction { throw expected }
    for (width in 1..1000) {
      node.calculateLayout(100f, 100f)
      try {
        node.setWidth(width.toFloat())
        fail("Expected callback exception")
      } catch (actual: IllegalArgumentException) {
        assertSame(expected, actual)
      }
    }
    node.setDirtiedFunction(null)
    node.calculateLayout(100f, 100f)
    assertEquals(1000f, node.layoutWidth)
    assertFalse(node.isDirty())
  }

  @Test
  fun explicitFinalizationAllowsLaterNodesToRegisterCallbacks() {
    repeat(100) {
      val node = YogaNodeJNIFinalizer()
      node.setDirtiedFunction { fail("Finalized node callback must not run") }
      node.freeNatives()
      node.freeNatives()
      val replacement = YogaNodeJNIFinalizer()
      var calls = 0
      replacement.setDirtiedFunction { calls++ }
      replacement.calculateLayout(100f, 100f)
      replacement.setWidth(10f)
      assertEquals(1, calls)
      replacement.freeNatives()
    }
  }

  @Test
  fun registeredCallbackDoesNotKeepItsNodeAlive() {
    fun createReference(): WeakReference<YogaNode> {
      val node = YogaNodeFactory.create()
      node.setDirtiedFunction { assertSame(node, it) }
      return WeakReference(node)
    }
    assertCollected(createReference())
  }

  @Test
  fun unregisterReleasesCallback() {
    val node = YogaNodeFactory.create()
    fun registerCallback(): WeakReference<YogaDirtiedFunction> {
      val callback =
          object : YogaDirtiedFunction {
            override fun dirtied(node: YogaNode) {}
          }
      node.setDirtiedFunction(callback)
      return WeakReference(callback)
    }
    val reference = registerCallback()
    node.setDirtiedFunction(null)
    assertCollected(reference)
  }

  private fun assertCollected(reference: WeakReference<*>) {
    repeat(50) {
      System.gc()
      if (reference.get() == null) {
        return
      }
      Thread.sleep(100)
    }
    fail("Dirtied callback registration leaked its Java reference")
  }
}
