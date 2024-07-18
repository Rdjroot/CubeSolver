# CubeSolver-Cpp
> 基于C++的三阶魔方还原程序

[TOC]

## 项目说明

本项目为基于QT5和OpenCV3.41的图像识别魔方还原程序。

还原算法采用科先巴二阶段算法。

实现了输入打乱公式提供还原公式的功能，以及通过摄像头扫描识别魔方六面，获取魔方还原公式，且提供3D魔方图像动画演示。

以下展示用法

### 快速使用

- 输入打乱公式，获取还原公式

![quickUse](https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202406082308034.gif)

### 扫描使用

#### 魔方识别并给出还原公式和动画

![camerause](https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202406082309082.gif)

#### 输入打乱公式获取还原公式和动画

![latex](https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202406082310234.gif)

## 项目目录

1. 主要Demo(文件夹)

```makefile
|---Code
├── algorithm/                    
│   ├── Cube_Algorithm/				# 魔方还原算法
│   │   ├── CubieCube.h/cpp			# 魔方数据结构
│   │   └── Solver.h/cpp			# 还原算法
|	|  	└── testMain.cpp			# 测试
|	|
├── demo/                           # opencv 魔方图像识别demo
│   ├── ColorPickerDemo.cpp			# 筛选六色HSV值
│   ├── getCubeInfos.cpp			# 颜色识别轮廓勾勒demo
│   └──  CubeFrameReco.cpp			# 划定区域识别每面颜色
└── 
```

2. QT程序（项目）

```makefile
CubeSolver/
├── Headers/
│   ├── buildcube.h				# opengl 魔方3D图像窗口
|	├── camerathread.h 			# 开启摄像头，传递画面线程类
|	├── cubedetec.h 			# 监控摄像画面，识别魔方颜色窗口
│   ├── cubiecube.h				# 魔方数据结构类
│   ├── mainwindow.h			# 多功能窗口
│   ├── solver.h				# 魔方还原算法
│   └── welcome.h				# 程序首窗口
├── Sources/
│   ├── buildcube.cpp				
|	├── camerathread.cpp 			
|	├── cubedetec.cpp			
│   ├── cubiecube.cpp				
│   ├── mian.cpp				# 程序入口
│   ├── mainwindow.cpp			
│   ├── solver.cpp				
│   └── welcome.cpp				
├── Forms/
│   ├── buildcube.ui
│   ├── cubedetec.ui
│   ├── mainwindow.ui
│   └── welcome.ui
├── Resources/
│   ├── /
│   ├── shapes.frag				# opengl渲染文件
│   ├── shapes.vert
│   └── Resource/
│   	└── cubeIcon.png		# 图标
└── CubeSolver.pro
```

## 开发指南

[使用OpenCV识别魔方颜色](./Documents/博客/魔方色块识别.md)（完成）

[三阶魔方基础知识](./Documents/三阶魔方知识.md)（未完成）

[还原算法](./Documents/魔方还原算法.md)（未完成）

[界面与识别魔方](./Documents/界面与魔方识别.md)（未完成）

## 迭代日志

### version 1.0

#### 05.19

**主要工作：获取三阶魔方六色HSV值，通过摄像头获取信息。**

- 获取用于魔方颜色识别的HSV值demo：[ColorPickerDemo.cpp](./Code/demo/ColorPickerDemo.cpp)。

![colorPicker](https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202405221124215.png)

<center>效果展示：滑动窗口改变HSV值域，识别出来的颜色会是白色，由此获取HSV值</center>

- 颜色识别轮廓勾勒demo：[getCubeInfos.cpp](./Code/demo/getCubeInfos.cpp)

![getCubeInfo](https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202405221130392.png)

<center>效果展示：勾勒识别结果，标记颜色，识别精度不够</center>

- 划定魔方识别区域，获取魔方色块颜色：[cubeFrameReco.cpp](./Code/demo/cubeFrameReco.cpp)

![cubeFrameReco](https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202405221141785.png)

<center>效果展示：将魔方置于框中，只识别九个点位对应颜色，画框标记</center>

#### 05.21

工作内容：使用QT绘制界面，打开摄像头获取魔方六面颜色信息，传回主窗口

![recoWindow_1](https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202405221147078.gif)

<center>效果展示</center>

#### 05.31

工作内容：实现还原算法；可输入打乱公式，获取还原公式

还原算法的demo：/Code/algorithm/Cube_Algorithm

- CubieCube.h/cpp：魔方数据结构与旋转方法
- Solver.h/Solver.cpp: 还原算法
- testMain.cpp: 测试代码

![image-20240603233400048](https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202406032334158.png)

<center>效果展示</center>

#### 06.03

工作内容：充实QT界面，实现魔方3D图像展示构建

![image-20240603233631456](https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202406032336507.png)

<center>开始界面</center>

<img src="https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202406032344444.png" alt="image-20240603234402371" style="zoom:67%;" />

<center>识别结果</center>

<img src="https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202406032356179.png" alt="image-20240603235650137" style="zoom:67%;" />

<img src="https://raw.githubusercontent.com/Rdjroot/Img_beds/master/img/202406032358331.png" alt="image-20240603235833288" style="zoom:67%;" />

<center>按照识别结果展示</center>

### 06.07

- 完成version1.0程序
  - 实现QT窗口
  - 输入打乱公式——获取还原公式
  - 摄像头扫描魔方六面——获取还原公式
  - opengl窗口展示3D魔方还原动画

### 06.13

- 添加注释，修改旋转bug

### 06.14

- 修复功能bug
