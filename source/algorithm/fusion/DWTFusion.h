/********************************************************************
    created:	2016/02/15
    created:	15:2:2016   15:21
    filename: 	D:\code\vs2010\C\Fusion\DWTFusion.h
    file path:	D:\code\vs2010\C\Fusion
    file base:	DWTFusion
    file ext:	h
    author:		YS

    purpose:	DWTFusion类声明
                DWT与HIS结合的融合算法
                适用于三个波段
*********************************************************************/
#ifndef _DWTFusion_H_
#define _DWTFusion_H_

#include "GdalInf.h"
#include "Tools.h"
#include "HSI.h"

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif

class DWTFusion: public Tools,public HSI{

public:
    //通过DWT算法实现PAN影像与MS影像融合
    bool MeanStd_DWT_HIS_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);
};

#endif
