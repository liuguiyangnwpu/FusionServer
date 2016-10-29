/********************************************************************
    purpose:	PCAFusion类声明
                主成分分析（PCA）融合算法
*********************************************************************/
#ifndef _PCAFusion_H_
#define _PCAFusion_H_

#include "GdalInf.h"
#include "Tools.h"
#include "PCA.h"

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif

class PCAFusion: public Tools,public PCA{

public:
    //通过HPF算法实现PAN影像与MS影像融合
    bool MeanStd_PCA_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);
};


#endif
