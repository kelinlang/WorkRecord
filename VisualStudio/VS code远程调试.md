## 使用 VSCode 远程访问代码以及远程 GDB 调试

### 准备工作

本文目标是搭建一个 VSCode 环境, 可以从 Windows 远程访问 Linux 的 C 项目源码, 以及执行 GDB 远程调试

首先准备以下软件

在开发机器上:

- 安装 [VS Code Insiders](https://code.visualstudio.com/insiders/), 在 2019年05月, 只有这个版本支持 Remote - Development 插件
- 安装 VSCode 扩展 “Remote - Development”, 方法是左下角 `齿轮图标` -> `扩展`, 直接搜索商店
- 安装兼容 OpenSSH 的 SSH 客户端, 对于 Windows 7, 安装 [Git for Windows](https://git-scm.com/download/win) 即可, Windows 10 的选择更多

在执行机器上:

- 安装 gdb, 对于 CentOS 是 `yum install gdb`
- 安装 gdbserver, 对于 CentOS 是 `yum install gdb-gdbserver`
- 启动 ssh 服务端, 一般都有

### 使用 VSCode 远程访问代码

以前的办法是通过 sftp, samba 等协议, 把 Linux 环境的文件映射到 Windows 做成一个虚拟盘, 这类工具有很多, 可以选 [RaiDrive](https://www.raidrive.com/) 等等, 现在 VSCode 的官方扩展 “Remote - Development” 可以直接走 ssh 协议访问远程文件, 就不必映射虚拟盘了

安装好上述扩展, 首先改设置, 左下角 `齿轮图标` -> `设置`, 搜 “remote.SSH”, 勾选 `remote.SSH.showLoginTerminal` (或者直接在 settings.json 里添加一行 `"remote.SSH.showLoginTerminal": true` 也可)

完后就可以连接到远程机器了, 在 VSCode 主界面 `ctrl+shift+p` 选 `Remote.SSH: Connect to host`, 输入 `root@`, 如果是密码认证, 之后留意终端窗口, 在里面输入密码

首次连接会多次要求输入密码, 成功后界面左下角会有 `SSH: ` 的已连接状态, 同时这个 VSCode 也变成了该远程连接的专属实例, 然后就可以从侧边栏打开项目路径了, 打开后如图:

![vscode-remote-ssh.png](https://raw.githubusercontent.com/WarmGrid/warmgrid.github.io/master/_posts/remote-debug-in-vscode-insiders/vscode-remote-ssh.png)

图示通过 SSH 连接后的远程 VSCode 项目

之后根据项目类型, 可以安装一些基本的扩展, 如:

- C/C++ IntelliSense, debugging, and code browsing
- autoconf
- Bookmarks

等等, 注意有些扩展是要安装在 SSH 的目标机器上, 安装时会提示你 `install on SSH: xxxx`, 而主题类扩展和一部分功能类扩展是安装在本地机器上

如果 Remote-SSH 连接到远程机器是密码认证的, 频繁使用时每次都要输密码, 很麻烦, 这时用公钥认证更好, 这里参考 [Windows 下 VS Code Remote-Development(ssh) 插件的安装和配置](https://blog.csdn.net/gzj2013/article/details/90236650) 基本思路就是典型的 SSH 密钥方式登录: 生成一对公钥私钥, Linux 执行环境安装公钥, VSCode 使用私钥, 如下:

首先生成密钥:

```
# 生成一对密钥, 在哪个机器生成都行
ssh-keygen -t rsa

# 公钥需要放到 Linux 执行环境里
# 假设生成的公钥是 "vscode_rsa.pub"
cat /root/.ssh/vscode_rsa.pub >> /root/.ssh/authorized_keys
chmod 644 /root/.ssh/authorized_keys

# 私钥需要留在 VSCode 一端 Windows 机器里
# 假设路径是 "D:/.ssh/vscode_rsa"
```

然后在 VSCode 主界面 `ctrl+shift+p` 选 `Remote.SSH: Open Configuration File`, 输入以下内容:

```
Host dev-56144               <- 连接标识, 随便写
    HostName 172.16.xx.xxx           <- 远程机器 IP
    User root
    IdentityFile D:/.ssh/vscode_rsa  <- 私钥的本地完整路径
```

配置好后, 在 `Remote.SSH: Connect to host` 时就会看到这个连接配置了

### 使用 VSCode 和 gdbserver 远程调试 C 代码

在远程 Linux 机器上, 运行 `gdbserver localhost:  `, 比如

```
gdbserver localhost:2333 /path/to/myprogram myarg1 myarg2 myarg3
```

这里假设远程 Linux 机器 IP 是 `172.16.56.144`, gdbserver 的端口设为 `2333`, 在 VSCode 机器上, 进入菜单 `调试` -> `添加配置`, 会生成一个配置文件 `SSH之后的项目根目录/.vscode/launch.json`, 将其修改如下:

```
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "gdb Remote Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "/path/to/myprogram",
            "args": ["myarg1", "myarg2", "myarg3"],
            "stopAtEntry": true,
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "gdb",
            "miDebuggerArgs": "gdb",
            "linux": {
                "MIMode": "gdb",
                "miDebuggerPath": "/usr/bin/gdb",
                "miDebuggerServerAddress": "172.16.56.144:2333",
            },
            "logging": {
                "moduleLoad": false,
                "engineLogging": false,
                "trace": false
            },
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "cwd": "${workspaceFolder}",
        }
    ]
}
```

几个注意的地方:

- 应该是 `"request": "launch"`, 不是 “attach”, 此后也并不需要记录进程ID
- 需要填对 `"miDebuggerServerAddress": "172.16.56.144:2333"`, 有这个设置才会开启 gdb 远程调试
- `"engineLogging": true` 可以看到 gdb 自身的详细消息
- 必须是 `"externalConsole": false` 否则报错
- `/path/to/myprogram` 应该是在 gdbserver 和 launch.json 里都要填一次

之后就可以正常使用调试功能了, 添加断点, 监视等等, 如图:

![vscode-remote-gdb.png](https://raw.githubusercontent.com/WarmGrid/warmgrid.github.io/master/_posts/remote-debug-in-vscode-insiders/vscode-remote-gdb.png)

图示远程 gdb 调试

在 VSCode 的调试控制台输入变量名, 就可以直接打印出来, 想使用默认的调试器命令要在前面加 `-exec`, 如 `-exec p/x args`

附 gdbserver usage

```
gdbserver  --help
Usage:    gdbserver [OPTIONS] COMM PROG [ARGS ...]
    gdbserver [OPTIONS] --attach COMM PID
    gdbserver [OPTIONS] --multi COMM

COMM may either be a tty device (for serial debugging), or
HOST:PORT to listen for a TCP connection.

Options:
  --debug               Enable general debugging output.
  --remote-debug        Enable remote protocol debugging output.
  --version             Display version information and exit.
  --wrapper WRAPPER --  Run WRAPPER to start new programs.
  --once                Exit after the first connection has closed.
Report bugs to "<http://www.gnu.org/software/gdb/bugs/>".
```