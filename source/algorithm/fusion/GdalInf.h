/********************************************************************
	created:	2015/12/07
	created:	7:12:2015   19:36
	filename: 	D:\code\vs2010\C\Fusion\IO.h
	file path:	D:\code\vs2010\C\Fusion
	file base:	GdalInf
	file ext:	h
	author:		YS
	
	purpose:	GdalInf类声明
				处理遥感影像读取、写入的相关信息
*********************************************************************/
#ifndef _GDALINF_H_
#define _GDALINF_H_
#include "gdal.h"		//c   语法
#include "gdal_priv.h"  //c++ 语法
#include "ogr_spatialref.h"
#include "gdalwarper.h"
#include <cassert>
#include <string.h>
#include <iostream>
using namespace std;

typedef unsigned short int ushort;


class Inf{
protected:
	//图像基本信息
	int height;					// 行数
	int width;					// 列数
	int bandcount;				//波段数
	/* 待拓展图像信息 */
	float SpatialSolution;      //空间分辨率
	const char* Description;    //描述信息 图像格式
	const char* ProjectionRef;	//投影信息
	double adfGeoTransform[6];  //地理坐标信息
	const char * Datatype;
public:
	inline int GetHeight(){	//获取图像高度
		return this->height;
	}

	inline void SetHeight(int NewHeight){ //设置图像高度
		this->height = NewHeight;
	}

	inline int GetWidth(){		//获取图像宽度
		return this->width;
	}
	inline void SetWidth(int NewWidth){  //设置图像宽度
		this->width = NewWidth;
	}

	inline int GetBandCount(){		//获取图像波段数
		return this->bandcount;
	}
	inline void SetBandCount(int nband){		//设置图像波段数
		 this->bandcount = nband;
	}
	inline float GetSpatialSolution(){		//获取图像空间分辨率
		return this->SpatialSolution;
	}
	inline void SetSpatialSolution(int NewSpatialSolution){		//设置图像空间分辨率
		this->SpatialSolution = NewSpatialSolution;
	}	
	inline double * GetGeoTransform(){		//获取图像地理信息 2016.3.14
		return this->adfGeoTransform;
		//memcpy(this->adfGeoTransform,newGeoTransform,sizeof(double)*6);
	}
	inline void SetGeoTransform(double *newGeoTransform){		//获取图像地理信息 2016.3.14

		memcpy(this->adfGeoTransform,newGeoTransform,sizeof(double)*6);
	}
};

class GdalInf :public Inf{

private:
	float * imgdata;			// 指向存放元素的空间
public:
	//构造函数，指针初始化为空
	GdalInf();
	
	//获取图像指针
	float * GetImgData();
	
	//图像数据
	void SetImgData(float* NewData);
	
	//释放图像内存
	void ClearImageData();
	
	//Gdal写图像信息
	int WriteImageInf(const char* OutputFileName);
	
	//Gdal写图像
	int WriteImageFromBuff(const char* OutputFileName, int x,int y,int NewWidth,int NewHeight);

	//Gdal读图像信息
	int ReadImageInf(const char* InputFileName);

	//Gdal读图像
	int ReadImageToBuff(const char* InputFileName,int x,int y,int NewWidth,int NewHeight);

	int ReadImageToBuff(const char* InputFileName,int x,int y,int NewWidth,int NewHeight,int *bandlist);
	
	~GdalInf();
};

#endif