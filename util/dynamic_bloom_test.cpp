/**
 * 在原版中有#ifndef GFLAGS的语句
 * 但这一宏却没有定义
 * 在没有安装gflag时无法执行 但在安装后却能执行了
 * 也不需要手动设置任何环境变量
 * 实际上是在Makefile中无法看到 但在CMakeLists.txt中定义了
 * find_package(gflags CONFIG) 并定义了相关的宏
 * */