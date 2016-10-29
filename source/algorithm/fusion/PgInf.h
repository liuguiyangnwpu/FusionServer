/********************************************************************
	created:	2016/03/15
	created:	15:3:2016   15:34
	filename: 	D:\code\vs2010\C\Fusion\PgInf.h
	file path:	D:\code\vs2010\C\Fusion
	file base:	PgInf
	file ext:	h
	author:		YS
	
	purpose:	PgInf类声明
				写入postgres信息
*********************************************************************/
#ifndef _PGINF_H_
#define _PGINF_H_
#include "gdal_priv.h"  //c++ 语法
#include "ogr_spatialref.h"
#include "proj_api.h"  //proj4
#include <string>
#include <math.h>
#include <time.h>
#include <iostream>

using namespace std;

//待写入数据库的信息
typedef struct _FusionInf{
	double	cnttimeuse;				//所用时间
	string	producetime;			//产品生成时间
	double	ulcoorvalid_longitude;	//有效融合区域左上角经度坐标
	double	ulcoorvalid_latitude ;	//有效融合区域左上角纬度坐标
	double	brcoorvalid_longitude;	//有效融合区域右下角经度坐标
	double	brcoorvalid_latitude ;	//有效融合区域右下角纬度坐标
	double	ulcoorwhole_longitude;	//全局左上角经度坐标
	double	ulcoorwhole_latitude ;	//全局左上角纬度坐标 
	double	brcoorwhole_longitude;	//全局右下角经度坐标
	double	brcoorwhole_latitude ;	//全局右下角纬度坐标
	float	resolution;				//融合后产品分辨率
	string	product_format;			//产品格式
	string	projectiontype;			//投影类型
	string	datumname;				//投影椭球体
	string	projectioncode;			//投影编码
	string	projectionunits;		//投影单位
	float	projcentralmeridian;	//投影带中央经线 
    int status; //任务执行状态
}FusionStruct;

class PgInf{
private:
    FusionStruct finf;
public:
    PgInf();
    void DataDeepCopy(FusionStruct** dest);

	//记录算法所用时间
	inline void SetCnttimeuse(double Cnttimeuse);
	inline void SetProducetime(string Producetime);
    inline void SetStatus(bool status);
	//记录融合产品地理、分辨率等信息
	int SetProductInf(string Outrl);
	
	//待写入数据库的融合产品相关信息
    int ReadInfToDB(double Cnttimeuse,string OutUrl,string Producetime, bool status);
};


#endif
