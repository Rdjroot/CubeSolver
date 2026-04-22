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
