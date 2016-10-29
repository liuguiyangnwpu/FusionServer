/********************************************************************
    created:	2016/03/11
    created:	11:3:2016   10:37
    filename: 	D:\code\vs2010\C\Fusion\HCSFusion.h
    file path:	D:\code\vs2010\C\Fusion
    file base:	HCSFusion
    file ext:	h
    author:		YS

    purpose:	HCSFusion类声明
                HCS融合算法
*********************************************************************/
#ifndef _HCSFusion_H_
#define _HCSFusion_H_

#include "GdalInf.h"
#include "Tools.h"
#include "HCS.h"

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif

class HCSFusion: public Tools,public HCS{

public:
    //版本1
    bool MeanStd_HCS_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);
};

#endif
