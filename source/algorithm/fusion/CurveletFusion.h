/********************************************************************
    created:	2016/01/20
    created:	20:1:2016   18:06
    filename: 	D:\code\vs2010\C\Fusion\CurveletFusion.h
    file path:	D:\code\vs2010\C\Fusion
    file base:	CurveletFusion
    file ext:	h
    author:		YS

    purpose:	CurveletFusion类声明
                Curvelet与HIS结合的融合算法
                适用于三个波段
*********************************************************************/
#ifndef _CurveletFusion_H_
#define _CurveletFusion_H_

#include "GdalInf.h"
#include "curvelet/nummat.hpp"
#include "Tools.h"
#include "HSI.h"
#include "HCS.h"
#include "GramSchmidtFusion.h"

#include <math.h>
#include <omp.h>
using namespace fdct_wrapping_ns;

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif

#ifndef PI
#define PI 3.14159265358979323846 
#endif

class CurveletFusion: public HSI,public HCS,public GramSchmidtFusion{ //GramSchmidtFusion:public Tools

public:
    //通过Curvelet算法实现PAN影像与MS影像融合
    //版本1
    bool MeanStd_Curvelet_HIS_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);
    //2016.3.7
    bool MeanStd_Curvelet_HCS_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);
    //2016.3.8
    bool MeanStd_Curvelet_GramSchmidt_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);

    bool MeanStd_Curvelet_HCS_Fusion_New(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod);

    //计算局部均值，标准差及方向信息熵
    void LocalMeanStdEntropy(CpxNumMat &Mat,int i,int j,int localSize,float & Mean,float &Std,float &Entropy){
        int p,q;
        int height= Mat.m();
        int width = Mat.n();
        int tmp1,tmp2;
        Mean = 0;
        Std = 0;
        Entropy = 0;
        for (p=-localSize/2; p<localSize/2;p++)
            for (q=-localSize/2; q<localSize/2;q++){
                tmp1 = i+p;
                tmp2 = j+q;
                if(tmp1 <0)
                    tmp1 = 0;
                if(tmp1 >=height)
                    tmp1 = height-1;
                if(tmp2 <0)
                    tmp2 = 0;
                if(tmp2 >=width)
                    tmp2 = width-1;
                Mean += abs(Mat(tmp1,tmp2));
                if (abs(Mat(tmp1,tmp2)) !=0)
                    Entropy += (pow(abs(Mat(tmp1,tmp2)),2)*log10(pow(abs(Mat(tmp1,tmp2)),2))/log10(2.0));
            }

        Mean = Mean /(localSize*localSize);

        Entropy = Entropy /(localSize*localSize);

        for (p=-localSize/2; p<localSize/2;p++)
            for (q=-localSize/2; q<localSize/2;q++){
                tmp1 = i+p;
                tmp2 = j+q;
                if(tmp1 <0)
                    tmp1 = 0;
                if(tmp1 >=height)
                    tmp1 = height-1;
                if(tmp2 <0)
                    tmp2 = 0;
                if(tmp2 >=width)
                    tmp2 = width-1;
                Std += pow(abs(Mat(tmp1,tmp2))-Mean,2);
            }

        Std = sqrt(Std/(localSize*localSize));

    }

	//2016.08.30
	bool MeanStd_Curvelet_HIS_Fusion_SAR(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName, const char* LogName, int* bandlist,int InterpolationMethod);

	bool LocalFeature(CpxNumMat &Mat,int x,int y,int localSize,float* Gx,float* Gy,float &feature){
		int p,q,i,j;
		int height= Mat.m();
		int width = Mat.n();
		int tmp1,tmp2;
		float Mean = 0;
		float Std = 0;

		float grad_x = 0;
		float grad_y = 0;

		for (p=-localSize/2,i=0; p<localSize/2;p++,i++)
			for (q=-localSize/2,j=0; q<localSize/2;q++,j++){
				tmp1 = x+p;
				tmp2 = y+q;
				if(tmp1 <0) tmp1 = 0;
				if(tmp1 >=height) tmp1 = height-1;
				if(tmp2 <0) tmp2 = 0;
				if(tmp2 >=width) tmp2 = width-1;
				grad_x += abs(Mat(tmp1,tmp2))*Gx[i*localSize+j];
				grad_y += abs(Mat(tmp1,tmp2))*Gy[i*localSize+j];
				Mean += abs(Mat(tmp1,tmp2));
			}
		
		Mean = Mean /(localSize*localSize);
		for (p=-localSize/2; p<localSize/2;p++)
			for (q=-localSize/2; q<localSize/2;q++){
				tmp1 = x+p;
				tmp2 = y+q;
				if(tmp1 <0) tmp1 = 0;
				if(tmp1 >=height) tmp1 = height-1;
				if(tmp2 <0) tmp2 = 0;
				if(tmp2 >=width) tmp2 = width-1;
				Std += pow(abs(Mat(tmp1,tmp2))-Mean,2);
			}
		Std = sqrt(Std/(localSize*localSize));

		feature = Std*sqrt((grad_x*grad_x+grad_y*grad_y)/2);
		//cout<<feature<<endl;
		return true;
	}
	
	bool GenFilter(float* G,int Wsize,float theta){
		int i,j,k;
		float G2a, G2b, G2c;
		int* x = new(std::nothrow) int[Wsize];
		int* y = new(std::nothrow) int[Wsize];
		
		if(NULL == x || NULL == y) return false;

		for (i = 0,k = -Wsize/2; i < Wsize; i++,k++){
			x[i] = k;
			y[i] = k;
		}
		for (i = 0; i < Wsize; i++)
			for (j = 0; j < Wsize; j++){

				G2a = 0.9213*(2 * x[i] * x[i] - 1)*(float)exp((double)-(x[i] * x[i] + y[j] * y[j]));
				G2b = 1.843*x[i] * y[j] * (float)exp((double)-(x[i] * x[i] + y[j] * y[j]));
				G2c = 0.9213*(2 * y[j] * y[j] - 1)*(float)exp((double)-(x[i] * x[i] + y[j] * y[j]));
				//生成滤波器
				G[i*Wsize+j] = cos(theta)*cos(theta)*G2a - 2 * cos(theta)*sin(theta)*G2b + sin(theta)*sin(theta)*G2c;
			}
		delete[] x; x = NULL;
		delete[] y; y = NULL;
		return true;
	}
};


#endif
