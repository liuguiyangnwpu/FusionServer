/********************************************************************
    created:	2016/03/01
    created:	1:3:2016   14:47
    filename: 	D:\code\vs2010\C\Fusion\GramSchmidtFusion.h
    file path:	D:\code\vs2010\C\Fusion
    file base:	GramSchmidtFusion
    file ext:	h
    author:		YS

    purpose:	GramSchmidtFusion类声明
                GramSchmidt融合算法
                光谱保真度较好
*********************************************************************/
#ifndef _GramSchmidtFusion_H_
#define _GramSchmidtFusion_H_

#include "GdalInf.h"
#include "Tools.h"

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif

class GramSchmidtFusion: public Tools{

public:
    //通过GramSchmidt算法实现PAN影像与MS影像融合
    //版本1
    //数组自身点积
    float Norm(float * vec,int length){
        int i;
        double sum=0;
        for (i=0;i<length;i++)
        {
            sum+= vec[i]*vec[i];
        }
        return sum;
    }
    //数组点积
    float DotProduct(float * vec1,float * vec2,int length){
        int i;
        double sum=0;
        for (i=0;i<length;i++)
        {
            sum+= vec1[i]*vec2[i];
        }
        return sum;
    }
    //协方差
    float Cov(float * vec1,float * vec2,int length){
        double mean1=0;
        double mean2=0;
        double mean=0;
        int i;
        for (i=0;i<length;i++)
        {
            mean1 += vec1[i];
            mean2 += vec2[i];
            mean += vec1[i]*vec2[i];
        }
        return mean/length-(mean1/length)*(mean2/length);
    }
    float Mean(float * vec,int length){
        int i;
        double m=0;
        for (i=0;i<length;i++)
        {
            m+= vec[i];
        }
        return m/length;
    }

    /*
    *《Process for enhangcing the spatial resolution of mutispectral imagery using pan-sharpening》
    *author:Laten et al.	2000
    */
    bool GramSchmidt_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);

};



#endif
