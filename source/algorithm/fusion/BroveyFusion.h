/********************************************************************
    created:	2015/12/09
    created:	9:12:2015   16:16
    filename: 	D:\code\vs2010\C\Fusion\BroveyFusion.h
    file path:	D:\code\vs2010\C\Fusion
    file base:	BroveyFusion
    file ext:	h
    author:		YS

    purpose:	BroveyFusion类声明
                Brovey融合算法（色彩标准化变换融合）
                （1）实现了分块处理
                （2）仅适用于三个波段，对波段的读取顺序没有要求

*********************************************************************/
#ifndef _BroveyFusion_H_
#define _BroveyFusion_H_

#include "GdalInf.h"
#include "Tools.h"

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif

class BroveyFusion: public Tools{

public:
    //通过Brovey算法实现PAN影像与MS影像融合
    bool Brovey_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);
};



#endif
