/**
 * 在原版中有#ifndef GFLAGS的语句
 * 但这一宏却没有定义
 * 在没有安装gflag时无法执行 但在安装后却能执行了
 * 也不需要手动设置任何环境变量
 * 实际上是在Makefile中无法看到 但在CMakeLists.txt中定义了
 * find_package(gflags CONFIG) 并定义了相关的宏
 * */

#include <algorithm>
#include <atomic>
#include <cinttypes>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include "dynamic_bloom.h"
#include "memory/arena.h"
#include "port/port.h"
#include "test_util/testharness.h"

void test_empty_filter() {
  FakeArena arena;
  DynamicBloom bloom1(&arena, 100, 2);
  ASSERT_TRUE(!bloom1.MayContain("hello"));
  ASSERT_TRUE(!bloom1.MayContain("world"));

  DynamicBloom bloom2(&arena, CACHE_LINE_SIZE * 8 * 2 - 1, 2);
  ASSERT_TRUE(!bloom2.MayContain("hello"));
  ASSERT_TRUE(!bloom2.MayContain("world"));
}

void test_small_filter() {
  FakeArena arena;
  DynamicBloom bloom1(&arena, 100, 2);
  bloom1.Add("hello");
  bloom1.Add("world");
  ASSERT_TRUE(bloom1.MayContain("hello"));
  ASSERT_TRUE(bloom1.MayContain("world"));
  ASSERT_TRUE(!bloom1.MayContain("x"));
  ASSERT_TRUE(!bloom1.MayContain("foo"));

  DynamicBloom bloom2(&arena, CACHE_LINE_SIZE * 8 * 2 - 1, 2);
  bloom2.Add("hello");
  bloom2.Add("world");
  ASSERT_TRUE(bloom2.MayContain("hello"));
  ASSERT_TRUE(bloom2.MayContain("world"));
  ASSERT_TRUE(!bloom2.MayContain("x"));
  ASSERT_TRUE(!bloom2.MayContain("foo"));
}

int main() {
  fprintf(stderr, "test bloom filter\n");
  test_empty_filter();
  test_small_filter();
}