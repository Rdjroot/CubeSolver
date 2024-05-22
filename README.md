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

