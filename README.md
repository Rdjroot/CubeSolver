# CubeSolver-Cpp
> 基于C++的三阶魔方还原程序

[TOC]

## 项目说明

本项目为基于QT5和OpenCV的图像识别魔方还原程序。

## 开发指南

[三阶魔方基础知识](./Documents/三阶魔方知识.md)

[还原算法](./Documents/魔方还原算法.md)

[界面与识别魔方](./Documents/界面与魔方识别.md)

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
