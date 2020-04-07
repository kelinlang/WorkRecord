# 简析Window、Activity、DecorView以及ViewRoot之间的错综关系

- ##### Activity

  - ##### Window：实例类是PhoneWindow

    - DecorView:FrameLayout的子类，它可以被认为是Android视图树的根节点视图。

- ##### ViewRoot

  - **ViewRootImpl类，它是连接WindowManagerService和DecorView的纽带**

- 事件流程

  **硬件 -> ViewRootImpl -> DecorView -> PhoneWindow -> Activity**

- 小结
  - 通过以上了解可以知道，Activity就像个控制器，不负责视图部分。Window像个承载器，装着内部视图。DecorView就是个顶层视图，是所有View的最外层布局。ViewRoot像个连接器，负责沟通，通过硬件的感知来通知视图，进行用户之间的交互。
- 参考网址
  - https://www.jianshu.com/p/8766babc40e0