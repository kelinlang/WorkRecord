### 1.内存泄漏定义

- 该回收的内存没有被回收，导致后续内存无法被使用
- 内存泄漏积累一定的程度会导致内存溢出

### 2.内存泄漏原因

- 静态变量保存activity

  - 弱引用

- #### Static Views

- Handler

- 内部类

- 匿名类

- 注册

- 单例

- Context

- 注册监听没有取消

- 容器对象保存的，没有清楚

- 新进程开启webview

- 忘记释放分配的内存的。（Cursor忘记关闭等）。

### 3. 检测内存泄漏方法

- 1.使用 Android studio自带**静态代码分析工具-Lint** 检查内存泄漏
  -  Analyze -> Inspect Code  选择想要扫面的区域即可，可能引起泄漏的编码，lint都会提示
  - LeakCanary 工具
    - 开发阶段可以加入三方库，当内存泄漏发生时，LeakCanary 会弹窗提示并生成对应的堆存储信息记录
  - Android Monitor
    - 可以手动触发GC,可以进入HPROF Viewer界面，查看Java的Heap,Reference Tree代表指向该实例的引用，可以从这里面查看内存泄漏的原因，Shallow Size指的是该对象本身占用内存的大小，Retained Size代表该对象被释放后，垃圾回收器能回收的内存总和。
  - 

