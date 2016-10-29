/********************************************************************
    created:	2016/03/11
    created:	11:3:2016   10:32
    filename: 	D:\code\vs2010\C\Fusion\HCS.h
    file path:	D:\code\vs2010\C\Fusion
    file base:	HCS
    file ext:	h
    author:

    purpose:	HCS类声明
                HCS变换与反变换
*********************************************************************/
#ifndef _HCS_H_
#define _HCS_H_


#include <math.h>
#include <iostream>
using namespace std;

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif

class HCS{
public:

    //将图像转换到HCS(hyperspherical color sharpening)空间
    bool HCS_Trans(float *MS_Mat,int height,int width,int bandcount);

    //将图像转换到原始空间
    bool HCS_Itrans(float *MS_Mat, int height, int width, int bandcount);
};


#endif
