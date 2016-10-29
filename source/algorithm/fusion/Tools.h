/********************************************************************
    created:	2015/12/08
    created:	8:12:2015   14:56
    filename: 	D:\code\vs2010\C\Fusion\Tools.h
    file path:	D:\code\vs2010\C\Fusion
    file base:	Tools
    file ext:	h
    author:		YS

    purpose:	融合算法相关函数
*********************************************************************/
#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <cstdlib> //linux下添加
#include <cmath>
#include <time.h>
#include <string.h>
#include <fstream>
#include <iostream>
using namespace std;

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif
#ifndef DATA3D
#define DATA3D(Matrix,m,n,band,SizeRow,SizeCol) Matrix[(m)*(SizeCol)+(n)+(band)*(SizeRow)*(SizeCol)]
#endif


#define Nearest 1   //最邻近插值
#define Linear 2    //双线性插值
#define CubicConv 3//三次卷积插值

class Tools{
public:
    //边界处理——插值内部 扩充边界
    inline float Pixels(float* Mat, int row, int col, int band, int height, int width);

    //三次卷积Sin（x*PI）/（x*PI）
    inline float SinXDivX(float x);

    //最临近插值（只适用于放大图像）
    void Nearest_Interpolation_Base(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width);//适用于小图像
    void Nearest_Interpolation(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width); //适用于大图像

    //双线性插值（只适用于放大图像）
    void Linear_Interpolation_Base(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width);//适用于小图像
    void Linear_Interpolation(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width);	//适用于大图像

    //三次卷积插值（只适用于放大图像）
    void CubicConv_Interpolation_Base(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width);//适用于小图像
    void CubicConv_Interpolation(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width);	//适用于大图像

    //插值（只适用于放大图像）
    void Interpolation(float* Mat, int height, int width, int bandcount, float *NewMat, int new_height, int new_width,int method);

    //图像直方图匹配 使Mat与Base_Mat进行匹配 返回Obj_Mat
    float* HistMatch(float *Mat, float* Base_Mat, int height, int width, int flag);

    //求解灰度图像最大值最小值
    void MaxMin(float *Img, int height, int width, float &max,float &min);

    //求解灰度图像均值和方差
    void MeanStd(float *Img, int height, int width, float &std,float &mean);

    //图像分块
    void ImageBlock(int ImageHeight,int ImageWidth,int &BlockHeight,int &BlockWidth);

    //写日志
    void Log(const char* LogName,const char* str);
};

#endif
