# CubeSolver 从 qmake 迁移到 CMake 的说明

## 1. 本次迁移结果

本项目已经从 qmake 工程迁移为 CMake 工程，并在当前机器环境下使用 **Qt 6.7.2 + MinGW 64-bit + CMake 3.29.8 + vcpkg OpenCV 4** 实际编译通过。

当前可执行文件位置：

- `d:/Codes/Project_Demo/CubeSolver/build-cmake-mingw/CubeSolver.exe`

当前新增/修改的关键文件：

- `d:/Codes/Project_Demo/CubeSolver/CMakeLists.txt`
- `d:/Codes/Project_Demo/CubeSolver/.claude/vcpkg-triplets/x64-qt-mingw-dynamic.cmake`
- `d:/Codes/Project_Demo/CubeSolver/CubeSolver/camerathread.cpp`
- `d:/Codes/Project_Demo/CubeSolver/CubeSolver/cubedetect.cpp`
- `d:/Codes/Project_Demo/CubeSolver/CubeSolver/buildcube.cpp`
- `d:/Codes/Project_Demo/CubeSolver/CubeSolver/buildcube.h`
- `d:/Codes/Project_Demo/CubeSolver/CubeSolver/main.cpp`
- `d:/Codes/Project_Demo/CubeSolver/CubeSolver/mainwindow.ui`
- `d:/Codes/Project_Demo/CubeSolver/CubeSolver/mainwindow.cpp`
- `d:/Codes/Project_Demo/CubeSolver/CubeSolver/welcome.cpp`
- `d:/Codes/Project_Demo/CubeSolver/CubeSolver/solver.h`

## 2. 原工程问题概述

原 qmake 工程主要问题有：

1. `.pro` 中 OpenCV 使用了硬编码本地路径，原路径已不存在。
2. 项目依赖 Qt 6.7.2 MinGW 64-bit，但系统 PATH 中同时存在一个 32 位 MinGW，容易把构建链污染掉。
3. 代码里有少量 Qt6 / OpenCV4 兼容性问题，导致迁移后不能直接编译。

## 3. 本次做了哪些修改

### 3.1 构建系统迁移

新增根目录 `CMakeLists.txt`，完成以下映射：

- qmake `SOURCES` -> CMake 源文件列表
- qmake `HEADERS` -> CMake 头文件列表
- qmake `FORMS` -> `AUTOUIC`
- qmake `RESOURCES` -> `AUTORCC`
- Qt 模块依赖 -> `find_package(Qt6 COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)`
- OpenCV 依赖 -> `find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui imgcodecs videoio)`

### 3.2 vcpkg 工具链修正

新增：

- `.claude/vcpkg-triplets/x64-qt-mingw-dynamic.cmake`

作用：

- 强制 `vcpkg` 使用 Qt 自带的 `mingw1120_64`
- 避免误用系统 PATH 中的 `D:/EnvSoftware/mingw32/bin` 这个 32 位编译器

### 3.3 源码兼容性修正

为了让项目能在 Qt6 / OpenCV4 下编译，通过最小修改处理了以下问题：

1. `CV_CAP_PROP_FOURCC` / `CV_FOURCC` 改为 OpenCV4 写法
2. `CV_BGR2RGB` 改为 `cv::COLOR_BGR2RGB`
3. `QPalette::Background` 改为 `QPalette::Window`
4. 修复 `solver.h` 中 `#include<cmath>;` 末尾多余分号

### 3.4 OpenGL 展示修复

原界面文件中 `openGLWidget` 自身就是 `QOpenGLWidget`，但运行时代码又把真正负责绘制的 `BuildCube` 也作为 `QOpenGLWidget` 塞到它里面，形成嵌套 OpenGL 控件。迁移到 Qt6 后这个结构容易出现只显示灰色背景、不绘制 3D 魔方的问题。

本次已调整为：

1. `mainwindow.ui` 中的 `openGLWidget` 改为普通 `QWidget` 容器
2. `mainwindow.cpp` 中运行时创建 `BuildCube` 并挂载到该普通容器
3. `BuildCube` 设置 OpenGL 3.3 Core Profile 和 24 位深度缓冲
4. `BuildCube` 增加 VAO，避免 Core Profile 下顶点属性状态未绑定导致不绘制
5. `BuildCube` 增加 OpenGL context、shader、首帧绘制日志

### 3.5 日志输出

本次增加了 Qt 消息处理器，日志会同时输出到：

1. VS Code/终端的 stderr
2. 项目目录 `Documents/logs/` 下的日志文件

日志文件示例：

- `d:/Codes/Project_Demo/CubeSolver/Documents/logs/CubeSolver_20260422_162814.log`

可用于确认：

- 程序是否进入 `main()`
- `BuildCube` 是否挂载到主界面
- `initializeGL()` 是否真正创建了 OpenGL context
- `paintGL()` 是否执行首帧绘制

## 4. 当前使用的环境

本次验证通过的关键环境如下：

- Qt: `D:/IDEs/QT/6.7.2/mingw_64`
- Qt MinGW: `D:/IDEs/QT/Tools/mingw1120_64`
- Ninja: `D:/IDEs/QT/Tools/Ninja/ninja.exe`
- CMake: `D:/EnvSoftware/CMake/bin/cmake.exe`
- vcpkg: `D:/EnvSoftware/vcpkg/vcpkg.exe`
- OpenCV 安装位置：`D:/EnvSoftware/vcpkg/installed/x64-qt-mingw-dynamic`

## 5. 如果你要在另一台机器上复现

### 5.1 需要具备的基础工具

至少准备好以下组件：

1. Qt 6.7.x，且安装 **mingw_64** 套件
2. Qt 自带或独立的 **Ninja**
3. CMake 3.21+
4. vcpkg
5. Windows 下可用的 PowerShell（系统自带即可）

### 5.2 需要特别注意的点

一定要避免以下情况：

- PATH 里优先出现 32 位 MinGW
- CMake 用的是 Qt MinGW，但 vcpkg 又用的是另一个 MinGW
- Qt 是 MSVC 套件，但项目又试图和 MinGW 版 OpenCV 混链

**结论：Qt、编译器、OpenCV 三者必须是同一套 ABI。**

## 6. vcpkg 安装依赖的方法

### 6.1 当前项目使用的 triplet

项目内 triplet 文件：

- `d:/Codes/Project_Demo/CubeSolver/.claude/vcpkg-triplets/x64-qt-mingw-dynamic.cmake`

### 6.2 安装命令

在项目根目录或任意目录执行都可以：

```bash
"d:/EnvSoftware/vcpkg/vcpkg.exe" install "opencv4[core,highgui,jpeg,png,tiff,quirc,thread,dshow]:x64-qt-mingw-dynamic" --overlay-triplets="d:/Codes/Project_Demo/CubeSolver/.claude/vcpkg-triplets" --x-install-root="d:/EnvSoftware/vcpkg/installed"
```

说明：

- 这里没有安装 OpenCV 的默认全家桶 feature
- 这样做是为了避开 `dnn/abseil` 等对 MinGW 更敏感的依赖
- 对当前项目来说，这组 feature 已够用

## 7. CMake 配置与编译步骤

### 7.1 配置命令

```bash
cmake -S "d:/Codes/Project_Demo/CubeSolver" \
  -B "d:/Codes/Project_Demo/CubeSolver/build-cmake-mingw" \
  -G Ninja \
  -DCMAKE_MAKE_PROGRAM="D:/IDEs/QT/Tools/Ninja/ninja.exe" \
  -DCMAKE_C_COMPILER="D:/IDEs/QT/Tools/mingw1120_64/bin/gcc.exe" \
  -DCMAKE_CXX_COMPILER="D:/IDEs/QT/Tools/mingw1120_64/bin/g++.exe" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="D:/IDEs/QT/6.7.2/mingw_64" \
  -DCMAKE_TOOLCHAIN_FILE="D:/EnvSoftware/vcpkg/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_TARGET_TRIPLET=x64-qt-mingw-dynamic \
  -DVCPKG_OVERLAY_TRIPLETS="d:/Codes/Project_Demo/CubeSolver/.claude/vcpkg-triplets"
```

### 7.2 编译命令

```bash
cmake --build "d:/Codes/Project_Demo/CubeSolver/build-cmake-mingw" --config Debug --parallel 8
```

### 7.3 运行程序

推荐两种方式：

#### 方式 A：在 VS Code 终端直接运行（推荐）

```bash
cmd //c "set PATH=D:\IDEs\QT\Tools\mingw1120_64\bin;%PATH%&& D:\Codes\Project_Demo\CubeSolver\build-cmake-mingw\CubeSolver.exe"
```

这样可以直接在终端看到程序日志。

#### 方式 B：双击或 Windows shell 启动

```bash
cmd //c start "" "D:\Codes\Project_Demo\CubeSolver\build-cmake-mingw\CubeSolver.exe"
```

当前构建目录中已经自动复制：

- Qt 运行库
- OpenCV 运行库
- MinGW 运行库（`libstdc++-6.dll`、`libgcc_s_seh-1.dll`、`libwinpthread-1.dll`）

因此双击运行通常也可以启动；但若你要看日志，仍建议用方式 A。

## 8. VS Code 推荐配置方式

建议在 VS Code 中安装：

- CMake Tools
- C/C++
- Qt 相关扩展（可选）

### 8.1 CMake Tools 推荐设置

你后续在 VS Code 中要确保它识别到这些参数：

- Generator: `Ninja`
- CMake kit/compiler: Qt 的 `mingw1120_64`
- `CMAKE_PREFIX_PATH`: `D:/IDEs/QT/6.7.2/mingw_64`
- `CMAKE_TOOLCHAIN_FILE`: `D:/EnvSoftware/vcpkg/scripts/buildsystems/vcpkg.cmake`
- `VCPKG_TARGET_TRIPLET`: `x64-qt-mingw-dynamic`
- `VCPKG_OVERLAY_TRIPLETS`: `d:/Codes/Project_Demo/CubeSolver/.claude/vcpkg-triplets`

### 8.2 如果 VS Code 编不过，优先检查

1. 是否误用了 32 位 MinGW
2. 是否切换成了 MSVC kit
3. 是否没有把 `CMAKE_PREFIX_PATH` 指向 Qt 的 `mingw_64`
4. 是否没有使用项目内 overlay triplet

### 8.3 本次新增的 VS Code 配置文件

本次已新增：

- `d:/Codes/Project_Demo/CubeSolver/CMakePresets.json`
- `d:/Codes/Project_Demo/CubeSolver/.vscode/tasks.json`
- `d:/Codes/Project_Demo/CubeSolver/.vscode/launch.json`

用途分别是：

1. `CMakePresets.json`：统一 CMake 参数
2. `tasks.json`：一键配置/编译
3. `launch.json`：在 VS Code 中直接运行 `CubeSolver.exe` 并查看日志

推荐使用方式：

1. 在 VS Code 打开项目根目录
2. 运行默认构建任务 `cmake-build-debug`
3. 直接启动 `Run CubeSolver`
4. 在调试控制台或终端观察日志，同时也可查看 `Documents/logs/`

## 9. 已知现象与说明

### 9.1 UI 警告

编译时会看到一些 `uic` 警告，例如：

- 重复 objectName
- spacer 的 z-order 警告

这些在当前构建中 **不影响编译通过**，属于 UI 文件本身的历史问题，不是 CMake 迁移造成的。

### 9.2 qmake 构建目录仍然保留

项目里原先的 qmake 构建产物目录仍然保留，但当前 CMake 构建使用的是：

- `d:/Codes/Project_Demo/CubeSolver/build-cmake-mingw`

后续建议把 VS Code 统一指向这个目录，不要混用旧的 qmake build 目录。

## 10. 后续建议

### 建议 1：保留 qmake 一小段时间

在你确认 CMake 工作流完全稳定前，可以先不删 `.pro` 和旧 build 目录，方便对照。

### 建议 2：补一个 VS Code 的预设

如果你后续希望一键配置/编译，我建议再补：

- `CMakePresets.json`

这样 VS Code 和命令行都能共用一套参数，不必每次手输。

### 建议 3：后续再清理 UI 与旧代码警告

本次目标是“等价迁移并编过”，所以我没有顺手重构 UI 或业务代码。
后续如果你愿意，可以再做一轮：

- 清理 `.ui` 警告
- 替换 `qAsConst` 过时写法
- 梳理 `using namespace std` 等历史写法

## 11. 本次迁移的结论

本次迁移已经达到以下目标：

- qmake -> CMake 迁移完成
- 在当前机器上实际编译通过
- 依赖从本地硬编码 OpenCV 路径切换为 vcpkg 管理
- 构建链统一到 Qt 6.7.2 的 MinGW 64-bit
- 可执行文件和 Qt/OpenCV 运行库已经部署到构建目录中

如果后续你要继续把这个项目打磨成更适合 VS Code/跨平台的状态，下一步最值得做的是补 `CMakePresets.json` 和 `.vscode` 配置。

## 12. 在另一台 Windows + VS Code 电脑上从零打开并编译

这一节假设对方电脑上：

- 已安装 VS Code
- 已安装 Qt
- 已安装 CMake
- 但 Qt / CMake / MinGW / vcpkg 的目录都不确定
- OpenCV 大概率还没装

目标是：**至少能在 VS Code 中成功配置、编译、运行这个项目。**

### 12.1 先确认必须具备的东西

最少需要这几样：

1. **Qt 6.x 的 MinGW 套件**
   - 不能只有 MSVC 套件
   - 这个项目当前走的是 **Qt + MinGW + vcpkg(OpenCV)** 这一套 ABI
2. **Qt 对应的 MinGW 编译器**
   - 常见位置类似：`Qt/Tools/mingwxxxx_64/bin`
3. **CMake**
4. **Ninja**
   - 一般 Qt 安装里会带，也可以单独装
5. **vcpkg**
6. **Git**（如果要重新 clone 或用 VS Code / git 工作流）

### 12.2 在新电脑上先找本机实际路径

建议先在终端里确认这些路径，不要先急着点 VS Code：

```bash
where cmake
where qmake
where g++
where gcc
where ninja
```

重点不是“有没有输出”，而是要确认：

- `qmake` 指向的是 **Qt 的 MinGW 套件**，不是 MSVC 套件
- `g++` / `gcc` 指向的是 **Qt 对应的 64 位 MinGW**
- `cmake` 和 `ninja` 路径真实可用

如果 `where qmake` 指到的是 MSVC 版本，或者根本没进 PATH，也没关系，可以手动去 Qt 安装目录找。常见结构类似：

- `C:/Qt/6.7.2/mingw_64`
- `C:/Qt/Tools/mingw1120_64/bin`
- `C:/Qt/Tools/Ninja/ninja.exe`

不同电脑路径不一样，但目录结构通常相似。

### 12.3 如果新电脑还没有 vcpkg

主线建议还是使用 `vcpkg`。

示例：

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
```

完成后确认：

```bash
vcpkg.exe version
```

如果不想把 `vcpkg` 放进系统 PATH，也可以记住它的绝对路径，后面配置里直接写完整路径。

### 12.4 新电脑最重要的一个原则

**Qt、MinGW、OpenCV 必须使用同一套 ABI。**

也就是：

- Qt 用 MinGW
- CMake 用 MinGW 编译器
- vcpkg 安装 OpenCV 时也必须走 MinGW triplet

不要出现下面这种混搭：

- Qt 是 MSVC，OpenCV 是 MinGW
- Qt 是 MinGW 64 位，但 `g++` 实际指向 32 位 MinGW
- VS Code 里选了 MSVC Kit，但 `vcpkg` 安装的是 MinGW 包

一旦混用，最常见的结果就是：

- 能 configure 但 link 失败
- 能 build 但程序跑不起来
- 运行时直接闪退或缺 DLL

### 12.5 新电脑上需要改哪些文件里的路径

这个项目目前为了保证本机可用，保留了若干**机器相关的绝对路径**。换电脑后，至少要检查下面几个位置：

#### 1. `CMakePresets.json`

这里面通常要改成新电脑自己的路径：

- `CMAKE_MAKE_PROGRAM`
- `CMAKE_C_COMPILER`
- `CMAKE_CXX_COMPILER`
- `CMAKE_PREFIX_PATH`
- `CMAKE_TOOLCHAIN_FILE`

#### 2. `.claude/vcpkg-triplets/x64-qt-mingw-dynamic.cmake`

这里至少要检查：

- `set(ENV{PATH} ".../mingw/bin;...")`
- `set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ".../vcpkg/scripts/toolchains/mingw.cmake")`

也就是要换成新电脑上的：

- Qt MinGW `bin` 路径
- vcpkg 安装路径

#### 3. `CMakeLists.txt`

当前文件里有用于复制 MinGW 运行库的路径，换电脑后如果路径不同，要改成新电脑本机的 MinGW `bin` 路径。主要看：

- `libgcc_s_seh-1.dll`
- `libstdc++-6.dll`
- `libwinpthread-1.dll`

如果这些路径不改，项目可能仍然能编译通过，但运行时会缺 MinGW DLL。

#### 4. `.vscode/launch.json`

如果你要直接用 VS Code 调试运行，还要检查：

- `miDebuggerPath`
- `PATH` 环境变量里附加的 MinGW 路径

### 12.6 在新电脑上安装 OpenCV（主线：vcpkg）

在你已经把上面路径改成新电脑实际路径之后，再安装 OpenCV。

主线命令仍然建议用：

```bash
"<你的 vcpkg 路径>/vcpkg.exe" install "opencv4[core,highgui,jpeg,png,tiff,quirc,thread,dshow]:x64-qt-mingw-dynamic" --overlay-triplets="<项目根目录>/.claude/vcpkg-triplets" --x-install-root="<你的 vcpkg 根目录>/installed"
```

你需要把里面的占位符替换为本机真实路径。

这条命令的意思是：

- 使用项目自带的 MinGW triplet
- 给当前项目安装最小够用的 OpenCV 组件
- 避开不必要的默认 feature，降低 MinGW 构建失败概率

### 12.7 在 VS Code 中推荐怎么做

建议顺序如下：

1. 用 VS Code 打开项目根目录
2. 先手动检查并修改：
   - `CMakePresets.json`
   - `.claude/vcpkg-triplets/x64-qt-mingw-dynamic.cmake`
   - 必要时 `CMakeLists.txt`
   - 必要时 `.vscode/launch.json`
3. 确认 OpenCV 已通过 `vcpkg` 安装完成
4. 执行 configure
5. 执行 build
6. 在 VS Code 中运行 `Run CubeSolver`

如果你用的是我已经补好的 VS Code 配置，那么主流程可以尽量保持成：

- 配置：`cmake --preset qt-mingw-debug`
- 编译：`cmake --build --preset qt-mingw-debug`

但前提是这些 preset 里的绝对路径已经改成新电脑自己的路径。

### 12.8 新电脑上第一次失败时优先检查什么

建议按这个顺序查：

#### 情况 A：configure 失败

先看：

1. `CMAKE_PREFIX_PATH` 是否指向 Qt 的 `mingw_64`
2. `CMAKE_CXX_COMPILER` 是否真的是 64 位 MinGW
3. `CMAKE_TOOLCHAIN_FILE` 是否指向本机的 vcpkg
4. triplet 文件里的 MinGW 路径是不是旧电脑路径

#### 情况 B：vcpkg 安装 OpenCV 失败

先看：

1. triplet 里是不是还写着旧电脑上的 MinGW 路径
2. `g++` 是否其实指向了别的 MinGW 或 32 位 MinGW
3. 是否误用了 MSVC 套件的 Qt

#### 情况 C：build 成功但运行失败

先看：

1. `build-cmake-mingw` 目录下是否有：
   - Qt DLL
   - OpenCV DLL
   - MinGW runtime DLL
2. `CMakeLists.txt` 里的运行库复制路径是否仍是旧电脑路径
3. `.vscode/launch.json` 中 PATH 是否包含正确的 MinGW `bin`

#### 情况 D：程序能启动但 3D 不显示

先看：

1. `Documents/logs/` 是否生成日志文件
2. 日志里是否出现：
   - `Logging initialized`
   - `BuildCube initializeGL context`
   - `BuildCube paintGL first frame`
3. 如果前两条有、第三条没有，重点查渲染链
4. 如果一条都没有，先查程序是否在进入 `main()` 前就因为缺 DLL 退出

### 12.9 给新电脑使用者的最低建议

如果你只想让对方“尽快编出来并跑起来”，最实用的做法是：

1. 先找清楚本机的 Qt MinGW、CMake、Ninja、vcpkg 路径
2. 把项目里所有写死的旧路径改掉
3. 先用 `vcpkg` 安装 OpenCV
4. 再用 CMake configure/build
5. 最后在 VS Code 中运行并检查 `Documents/logs/`

### 12.10 一句话结论

在另一台 Windows 电脑上，这个项目不是“打开 VS Code 就能直接编译”的状态，而是**需要先把机器相关路径改成本机值，再用 vcpkg 补 OpenCV，最后再配置/编译/运行**。

只要保证这三件事成立，通常就能落地：

1. Qt / MinGW / OpenCV 是同一套 ABI
2. 项目内绝对路径都改成了本机路径
3. OpenCV 已通过项目 triplet 安装完成
