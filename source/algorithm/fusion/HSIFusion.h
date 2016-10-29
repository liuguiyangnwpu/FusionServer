/********************************************************************
    created:	2015/12/18
    created:	18:12:2015   9:16
    filename: 	D:\code\vs2010\C\Fusion\HSIFusion.h
    file path:	D:\code\vs2010\C\Fusion
    file base:	HSIFusion
    file ext:	h
    author:		YS

    purpose:	HSIFusion类声明
                HSI融合算法
                （1）实现了分块处理
                （2）仅适用于三个波段，对波段的读取顺序有要求，R、G、B
*********************************************************************/
#ifndef _HSIFusion_H_
#define _HSIFusion_H_

#include "GdalInf.h"
#include "Tools.h"
#include "HSI.h"

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif

class HSIFusion: public Tools,public HSI{

public:
    //通过HSI算法实现PAN影像与MS影像融合
    bool MeanStd_HSI_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);
};

#endif
