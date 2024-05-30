# Basic_ThreadPool
基础线程池实现

### 介绍

使用c++编写的线程池，利于以后学习

### 线程池设计

线程池基本要素：

* 一组线程
* 任务队列

#### 第一种

使用mutex制造出临界区代码。

#### 第二种

使用条件变量让线程池中的线程在无任务执行时挂起，有任务执行时被唤醒。

#### 第三种

ThreadPoolExecutor

* 核心线程数；
* 最大线程数；
* 任务队列，最大任务数。
