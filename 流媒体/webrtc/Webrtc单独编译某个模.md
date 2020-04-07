# WebRTC Native 模块单独编译静态库（iOS）

项目里面用到了 WebRTC 某些模块，以前是源码拷贝集成的。这样不仅需要处理平台的宏（WebRTC Native 跨平台），而且升级版本也会涉及依赖等问题，所以探索了一下怎么用编码编译出可用的静态库。

网上相关的资料很少，且大部分都是 Android 平台编译单独的 apm（Audio Processing Module）进行使用，但我想找到更通用的方法。

## WebRTC 编译流程

要编译单独的模块，首先要了解完整的 WebRTC 源码是如何编译的。根据 [官网教程](https://webrtc.org/native-code/development/)，WebRTC 项目出自于 Chromium，所以使用的工具和流程基本都一致。

- 安装 [Chromium depot tools](http://dev.chromium.org/developers/how-tos/install-depot-tools)，这是 Chromium 特有的工具集，包含 git 管理，编译工具等
- 使用该工具下载源码（Android 依赖 Linux 环境）
- 使用 GN 命令生成编译配置
- 使用 ninja 进行编译

## Ninja

Ninja 是一个小型轻量的 C++ 构建系统，使用 build.ninja （类似 Makefile）来进行编译。而一般需要用工具来生成这个描述文件，而不是手动编写。而官方采用的就是 GN（Generate Ninja）工具。

## GN

GN 是专门用于创建 Ninja 描述文件的工具。可以对项目多个模块分别编写 BUILD.gn 文件，对模块描述依赖，编译文件，头文件，可见性，命名空间等。算是一种 DSL，用于描述模块，和 Cocoapods 里的 Podspec 文件作用差不多。

## 单独编译

虽然可以进行整体编译了，但是单独编译有没有办法呢？先查看 [Ninja 官方文档](https://ninja-build.org/manual.html) 发现 ninja 命令是可以带上编译目标的。编译目标的名字，是用 GN 生成的。查看 BUILD.gn 文件，发现基本都有 `rtc_static_library`。这个模版被定义在 webrtc.gni 文件里。

```
template("rtc_static_library") {
  static_library(target_name) {
    forward_variables_from(invoker,
                           "*",
                           [
                             "configs",
                             "public_configs",
                             "suppressed_configs",
                             "visibility",
                           ])
    forward_variables_from(invoker, [ "visibility" ])
		// ...
  }
}
复制代码
```

看来这个就是要找的 target_name。比如需要单独编译 modules/audio_processing 模块，查看 modules/audio_processing/BUILD.gn，发现里面定义了

```
rtc_static_library("audio_processing")
复制代码
```

尝试使用在 gn 执行后使用

```
ninja -C out/Default audio_processing
复制代码
```

可以看到，在 out/Default/obj/modules/audio_processing 目录下生成了 libaudio_processing.a。

但这样编译出来的静态库大小不大，怀疑是没有包含依赖的编译产物的。使用命令 nm 查看

```
nm -u libaudio_processing.a
复制代码
```

发现有未定义的符号，联想 ninja 是个轻量构建系统，感觉很合理。只能再翻翻文档，最后在 [GN 文档](https://gn.googlesource.com/gn/+/master/docs/reference.md) 中发现，BUILD.gn 定义了 complete_static_lib 参数，可以用来设置在编译时是否链接依赖产物。在 BUILD.gn 中 rtc_static_library 中加上 `omplete_static_lib = true`，再次尝试，发现正常了。

## 提取头文件

WebRTC 的头文件有文件层次结构，C++ 依赖头文件的路径，所以按照原始文件目录层次提出头文件就是必要的。Linux 系统可以是用 `cp --parents` 来进行保留文件目录的拷贝，但 Mac 上的 cp 命令并不支持 --parents 参数，只有 ditto 命令代替。

## 最后

笔者写了一个 [脚本](https://github.com/Nemocdz/WebRTC-Hack)，包含编译，提取头文件，合并架构来完成这整个流程。欢迎使用～

## 参考链接

- [GN Reference](https://gn.googlesource.com/gn/+/master/docs/reference.md#)
- [Using GN build](https://docs.google.com/presentation/d/15Zwb53JcncHfEwHpnG_PoIbbzQ3GQi_cpujYwbpcbZo/edit#slide=id.g1199fa62d0_2_6)
- [WebRTC Development](https://webrtc.org/native-code/development/)
- [The Ninja build system](https://ninja-build.org/manual.html)