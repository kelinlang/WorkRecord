

- 工程应用
  - 自己设计框架包装三方框架
    - ffmpeg
    - live555
    - libxxx
- 存在问题
  - 调试费时费力
    - 代码可以独立编译，脱离整体系统编译
    - cmake构建
    - 编写测试应用程序，不依赖上层lua，web环境测试
  - 时间戳处理
  - 延迟
    - 生产者消费者模型
  - 流程
    - 智能指针，lamada表达式，面向对象
  - 性能
    - 智能指针
  - 内存管理
    - 智能指针
  - 项目命名规则
- 软件架构
  - 工程目录结构
    - 三方库
      - lib1
        - 头文件
        - 源码
        - 编译好的静态库和动态库
      - lib2
        - 头文件
        - 源码
        - 编译好的静态库和动态库
      - ...
    - 公共库
      - 日志
      - 线程
      - 时间
      - ...
    - 媒体库（自建框架，封装三方库）
      - 编解码
      - 封装
      - 服务器
    - xx库（比如socket，http库封装）
    - 服务器应用（进程级别）
      - ...
    - 客户端（细分的可以单进程可多进程）
      - lua客户端
        - 编码器
        - 解码器
        - 编解码器
      - C++客户端（方便调试，提高效率，提供命令行调用方法）
        - 编码器
        - 解码器
        - 编解码器
      - 其他客户端