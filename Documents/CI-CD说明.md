# CubeSolver GitHub Actions CI/CD 说明

本文记录 CubeSolver 第一次接入 GitHub Actions 自动构建和打包的做法。

## 1. 当前实现目标

当前第一版只做 Windows 自动打包：

1. 推送代码到任意分支时自动运行。
2. 提交或更新任意 pull request 时自动运行。
3. 在 GitHub Actions 页面可以手动运行。
4. 使用 GitHub 提供的 Windows runner。
5. 使用 MSVC 2022 编译。
6. 安装 Qt 6.7.2 的 MSVC 桌面套件。
7. 使用 vcpkg 安装 OpenCV。
8. 使用 CMake 生成并编译 Release 版本。
9. 使用 `windeployqt` 和 CMake 的部署逻辑收集运行依赖。
10. 上传 `CubeSolver-windows-msvc2022-x64.zip` 作为可下载 artifact。

workflow 文件位置：

```text
.github/workflows/windows-package.yml
```

## 2. CI/CD 是什么

CI 是 Continuous Integration，意思是持续集成。对这个项目来说，最实际的作用是：每次把代码推到 GitHub 后，GitHub 自动拿一台干净的 Windows 虚拟机重新编译项目，尽早发现“我本机能编译，别人机器不能编译”的问题。

CD 是 Continuous Delivery 或 Continuous Deployment。当前项目暂时采用比较轻量的交付方式：自动生成 zip 包并挂到 GitHub Actions 的构建结果里。后续如果需要正式发布版本，可以再扩展为打 tag 后自动创建 GitHub Release。

## 3. 本机 Qt 和 GitHub Actions 的关系

本机安装的 Qt 6.7.2 只对本机编译有用。GitHub Actions 运行在 GitHub 远端的临时虚拟机里，远端机器看不到本机的 `D:/IDEs/QT/6.7.2`。

所以 workflow 中必须显式安装 Qt：

```yaml
- name: Set up Qt
  uses: jurplel/install-qt-action@v4
  with:
    version: 6.7.2
    host: windows
    target: desktop
    arch: win64_msvc2019_64
```

这里的 `win64_msvc2019_64` 可以和 MSVC 2022 配合使用。Visual Studio 2019 和 Visual Studio 2022 的 C++ ABI 在这个场景下兼容。

## 4. 为什么不是只生成一个 exe

Qt 桌面程序通常不能只发一个 `.exe`。程序运行时还需要：

1. Qt DLL，例如 `Qt6Core.dll`、`Qt6Gui.dll`、`Qt6Widgets.dll`。
2. Qt 平台插件，例如 `platforms/qwindows.dll`。
3. OpenCV DLL。
4. MSVC 运行库。
5. 程序自己的资源文件。当前主要资源已经通过 `.qrc` 编进程序。

开发机能直接运行，是因为开发机已经安装了 Qt、OpenCV 或相关运行环境。用户机器不一定有这些依赖，所以 CI 打包时会把依赖一起放进 zip。

Windows 下 Qt 官方工具 `windeployqt` 的作用就是扫描 exe，并复制 Qt 运行所需的 DLL 和插件。

## 5. 当前 workflow 的主要步骤

### 5.1 Checkout

拉取仓库代码：

```yaml
uses: actions/checkout@v4
```

### 5.2 初始化 MSVC

让命令行环境可以使用 MSVC x64 编译器：

```yaml
uses: ilammy/msvc-dev-cmd@v1
```

### 5.3 安装 Qt

安装 Qt 6.7.2 的 Windows MSVC 桌面版本：

```yaml
uses: jurplel/install-qt-action@v4
```

### 5.4 安装 OpenCV

使用 GitHub Windows runner 上的 vcpkg 安装 OpenCV：

```powershell
vcpkg install "opencv4[core,highgui,jpeg,png,tiff,quirc,thread,dshow]:x64-windows"
```

这里选择 `x64-windows`，因为本次 CI 使用 MSVC，而不是 MinGW。

### 5.5 配置 CMake

使用 Visual Studio 2022 生成器：

```powershell
cmake -S . -B build `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_INSTALLATION_ROOT\scripts\buildsystems\vcpkg.cmake" `
  -DVCPKG_TARGET_TRIPLET="x64-windows"
```

### 5.6 编译

```powershell
cmake --build build --config Release --parallel
```

编译完成后，`CMakeLists.txt` 中的 `POST_BUILD` 步骤会自动运行 `windeployqt` 并复制 OpenCV 运行库。

### 5.7 打包和上传

workflow 会把 `build/Release` 复制到 `package/CubeSolver-windows-msvc2022-x64`，再压缩为：

```text
CubeSolver-windows-msvc2022-x64.zip
```

最后通过 `actions/upload-artifact` 上传。

## 6. 如何下载构建产物

1. 打开 GitHub 仓库页面。
2. 进入 `Actions` 标签页。
3. 选择 `Windows Package` 工作流。
4. 点进一次成功的运行记录。
5. 在页面底部找到 `Artifacts`。
6. 下载 `CubeSolver-windows-msvc2022-x64`。
7. 解压后运行里面的 `CubeSolver.exe`。

注意：GitHub Actions artifact 不是永久发布包，默认有保留期限。正式版本建议后续再接 GitHub Release。

## 7. 本次对 CMake 的配套调整

原来的 Windows 部署逻辑主要服务于本机 MinGW 构建，里面有固定的 MinGW 运行库路径。CI 使用 MSVC 2022 时不能继续无条件复制这些 MinGW DLL。

现在逻辑调整为：

1. MinGW 构建时，从当前 MinGW 编译器目录推导运行库位置。
2. MSVC 构建时，不复制 MinGW DLL。
3. MSVC 构建时让 `windeployqt` 尝试带上编译器运行库。
4. OpenCV 运行库仍然从 `OpenCV_DIR` 推导出的 vcpkg 安装目录复制。

这样本机 MinGW 构建和 GitHub Actions MSVC 构建可以共用同一个 `CMakeLists.txt`。

## 8. 常见问题

### 8.1 workflow 找不到 Qt

优先检查 `Set up Qt` 步骤是否成功，以及 `CMAKE_PREFIX_PATH` 是否指向 `$env:QT_ROOT_DIR`。

### 8.2 workflow 找不到 OpenCV

优先检查：

1. `Install OpenCV` 步骤是否成功。
2. `VCPKG_TARGET_TRIPLET` 是否是 `x64-windows`。
3. `OpenCV_DIR` 是否指向 `installed/x64-windows/share/opencv4`。

### 8.3 zip 里有 exe 但运行失败

优先检查 zip 中是否包含：

1. Qt DLL。
2. `platforms/qwindows.dll`。
3. OpenCV DLL。
4. MSVC 运行库或 VC redistributable。

如果缺 Qt 依赖，重点看 `windeployqt` 是否执行。  
如果缺 OpenCV 依赖，重点看 `CMakeLists.txt` 中复制 OpenCV runtime 的步骤。

### 8.4 vcpkg 安装 OpenCV 很慢

第一次运行可能比较慢。当前 workflow 已经配置了 vcpkg binary cache，后续相同依赖通常会快很多。

## 9. 后续可以扩展什么

### 9.1 加 GitHub Release

当前只是上传 Actions artifact。后续可以扩展为：

1. 推送 `v1.0.0` 这种 tag。
2. GitHub Actions 自动构建。
3. 自动创建 GitHub Release。
4. 把 zip 上传为 Release 附件。

### 9.2 加 Linux 构建

Linux 可以先做“编译检查”，暂时不做通用分发包。因为 Linux 桌面程序分发比 Windows 更复杂，常见方案包括：

1. AppImage。
2. deb 包。
3. Flatpak。

第一次 CI/CD 建议先把 Windows 打包跑稳定，再扩展 Linux。

### 9.3 使用 CMake install/CPack

当前打包方式直接复制 `build/Release` 目录，简单直接。后续如果要更规范，可以补充：

1. `install(TARGETS ...)`
2. `install(FILES ...)`
3. CPack zip 或 NSIS 安装包

这会让本地打包和 CI 打包使用同一套安装规则。
