/********************************************************************
    created:	2016/04/15
    created:	15:4:2016   14:27
    filename: 	D:\code\vs2010\C\Fusion\PgInf.cpp
    file path:	D:\code\vs2010\C\Fusion
    file base:	PgInf
    file ext:	cpp
    author:		YS

    purpose:	PgInf类实现

*********************************************************************/

#include "PgInf.h"
//构造函数
PgInf::PgInf(){
    //初始化
    finf.cnttimeuse = 0;				//所用时间
    finf.producetime = "";			//产品生成时间
    finf.ulcoorvalid_longitude = 0;	//有效融合区域左上角经度坐标
    finf.ulcoorvalid_latitude  = 0;	//有效融合区域左上角纬度坐标
    finf.brcoorvalid_longitude = 0;	//有效融合区域右下角经度坐标
    finf.brcoorvalid_latitude  = 0;	//有效融合区域右下角纬度坐标
    finf.ulcoorwhole_longitude = 0;	//全局左上角经度坐标
    finf.ulcoorwhole_latitude  = 0;	//全局左上角纬度坐标
    finf.brcoorwhole_longitude = 0;	//全局右下角经度坐标
    finf.brcoorwhole_latitude  = 0;	//全局右下角纬度坐标
    finf.resolution = 0;				//融合后产品分辨率
    finf.product_format = "";		//产品格式
    finf.projectiontype = "";		//投影类型
    finf.datumname = "";				//投影椭球体
    finf.projectioncode = "";		//投影编码
    finf.projectionunits = "";		//投影单位
    finf.projcentralmeridian =0;		//投影带中央经线
    finf.status = -1; // -1 meas failed; 1 means True;
}

void PgInf::DataDeepCopy(FusionStruct **dest) {
    (*dest)->brcoorvalid_latitude = this->finf.brcoorvalid_latitude;
    (*dest)->brcoorvalid_longitude = this->finf.brcoorvalid_longitude;
    (*dest)->brcoorwhole_latitude = this->finf.brcoorwhole_latitude;
    (*dest)->brcoorwhole_longitude = this->finf.brcoorwhole_longitude;
    (*dest)->cnttimeuse = this->finf.cnttimeuse;
    (*dest)->datumname.assign(this->finf.datumname);
    (*dest)->producetime.assign(this->finf.producetime);
    (*dest)->product_format.assign(this->finf.product_format);
    (*dest)->projcentralmeridian = this->finf.projcentralmeridian;
    (*dest)->projectioncode.assign(this->finf.projectioncode);
    (*dest)->projectiontype.assign(this->finf.projectiontype);
    (*dest)->projectionunits.assign(this->finf.projectionunits);
    (*dest)->resolution = this->finf.resolution;
    (*dest)->status = this->finf.status;
}

//记录算法所用时间
inline void PgInf::SetCnttimeuse(double Cnttimeuse){
    this->finf.cnttimeuse = Cnttimeuse;
}

//记录融合产品生成时间
inline void PgInf::SetProducetime(string Producetime){
    this->finf.producetime=Producetime;
}

inline void PgInf::SetStatus(bool status) {
    if(status == false)
        this->finf.status = -1;
    this->finf.status = 1;
}

//记录融合产品地理、分辨率等信息
int PgInf::SetProductInf(string Outrl){
    /*SetProductInf 解析融合产品，获取产品相关信息
    *OutUrl			融合产品路径
    *0				成功
    *-1				失败
    *作者：YS
    *日期：2016.04.12
    */

    GDALAllRegister();         //利用GDAL读取图片，先要进行注册
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径
    GDALDataset *ReadDataSet = (GDALDataset*)GDALOpen(Outrl.c_str(), GA_ReadOnly);
    if(NULL==ReadDataSet){
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return -1;
    }

    double adfGeoTransform[6] = {0,0,0,0,0,0};
    if (ReadDataSet->GetGeoTransform(adfGeoTransform) == CE_None){
        this->finf.resolution = fabs(adfGeoTransform[1]); //空间分辨率
    }
    else{
        this->finf.resolution = 0; //空间分辨率
    }
    cout<<"resolution:"<<this->finf.resolution<<endl;

    int nXsize= ReadDataSet->GetRasterXSize();
    int nYsize= ReadDataSet->GetRasterYSize();

    //Proj4
    //2016.04.11
    projPJ pj_utm = NULL;
    projPJ pj_latlon = NULL;
    if(!(pj_utm = pj_init_plus("+proj=utm +zone=50 +lon_0=0 +k=1 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"))){
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        GDALClose(ReadDataSet);ReadDataSet = NULL; //释放内存
        return -1;
    }
    if(!(pj_latlon = pj_init_plus("+proj=longlat +datum=WGS84 +no_defs"))){
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        //释放内存
        pj_free(pj_utm);pj_utm = NULL;
        GDALClose(ReadDataSet);ReadDataSet = NULL;
        return -1;
    }

    //左上角
    double lat = adfGeoTransform[3] + 0.5 * adfGeoTransform[4] + 0.5 * adfGeoTransform[5]; //纬度
    double lon = adfGeoTransform[0] + 0.5 * adfGeoTransform[1] + 0.5 * adfGeoTransform[2]; //经度

    pj_transform(pj_utm,pj_latlon,1,1,&lon,&lat,NULL);

    cout.precision(12);
    cout<<"lon:"<<lon/DEG_TO_RAD<<" lat:"<<lat/DEG_TO_RAD<<endl;

    this->finf.ulcoorvalid_longitude = lon/DEG_TO_RAD;	//有效融合区域左上角经度坐标
    this->finf.ulcoorvalid_latitude = lat/DEG_TO_RAD;	//有效融合区域左上角纬度坐标
    this->finf.ulcoorwhole_longitude = lon/DEG_TO_RAD;	//全局左上角经度坐标
    this->finf.ulcoorwhole_latitude = lat/DEG_TO_RAD;	//全局左上角纬度坐标

    //右下角
    lat = adfGeoTransform[3] + (nXsize-0.5) * adfGeoTransform[4] + (nYsize-0.5) * adfGeoTransform[5]; //纬度
    lon = adfGeoTransform[0] + (nXsize-0.5) * adfGeoTransform[1] + (nYsize-0.5) * adfGeoTransform[2]; //经度

    pj_transform(pj_utm,pj_latlon,1,1,&lon,&lat,NULL);

    cout<<"lon:"<<lon/DEG_TO_RAD<<" lat:"<<lat/DEG_TO_RAD<<endl;
    this->finf.brcoorvalid_longitude = lon/DEG_TO_RAD;		//有效融合区域右下角经度坐标
    this->finf.brcoorvalid_latitude = lat/DEG_TO_RAD;		//有效融合区域右下角纬度坐标
    this->finf.brcoorwhole_longitude = lon/DEG_TO_RAD;		//全局右下角经度坐标
    this->finf.brcoorwhole_latitude = lat/DEG_TO_RAD;		//全局右下角纬度坐标


    //其他信息
    this->finf.product_format = "GEOTIFF";		//产品格式
    this->finf.projectiontype = "WGS84";			//投影类型
    this->finf.datumname = "WGS84";				//投影椭球体
    this->finf.projectioncode = "WGS84";			//投影编码
    this->finf.projectionunits = "WGS84";		//投影单位
    this->finf.projcentralmeridian = 0;			//投影带中央经线

    //释放内存
    pj_free(pj_utm);pj_utm = NULL;
    pj_free(pj_latlon);pj_latlon = NULL;
    GDALClose(ReadDataSet);ReadDataSet = NULL;
    return 0;
}

//融合产品信息写入数据库
int PgInf::ReadInfToDB(double Cnttimeuse, string OutUrl, string Producetime, bool status){
    /*ReadInfToDB 待写入数据库的相关信息
    *Cnttimeuse		融合算法所用时间
    *OutUrl			融合产品路径
    *Producetime	融合产品生产时间
    *0				获取信息成功
    *-1				获取信息失败
    *作者：YS
    *日期：2016.04.12
    */
    this->SetCnttimeuse(Cnttimeuse);
    this->SetProducetime(Producetime);

    if(this->SetProductInf(OutUrl) == 0 && status) {
        this->SetStatus(true);
        return true;
    }
    else
        this->SetStatus(false);
    return false;
}
