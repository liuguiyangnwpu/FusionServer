/********************************************************************
    created:	2016/01/20
    created:	20:1:2016   18:09
    filename: 	D:\code\vs2010\C\Fusion\CurveletFusion.cpp
    file path:	D:\code\vs2010\C\Fusion
    file base:	CurveletFusion
    file ext:	cpp
    author:		YS

    purpose:	CurveletFusion类实现
                Curvelet与HIS结合的融合算法
*********************************************************************/
#include "CurveletFusion.h"

#include "./curvelet/fdct_wrapping.hpp"
#include "./curvelet/fdct_wrapping_inline.hpp"

using namespace fdct_wrapping_ns;

//通过Curvelet算法实现PAN影像与MS影像融合
bool CurveletFusion::MeanStd_Curvelet_HIS_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod){
    /*
     *融合方法：Curvelet与HSI
     *替换规则：I分量替换
     *Input_PAN_FileName		高分辨率图像路径及名称
     *Input_MS_FileName			多光谱图像路径及名称（必须为三个波段）
     *Output_MS_FileName		融合图像路径及名称
     *LogName                   日志文件存放路径
     *bandlist					多光谱或高光谱参与融合的波段顺序
     *InterpolationMethod		插值方法
     *作者：YS
     *日期：2016.2.28
     */

    GDALAllRegister();         //利用GDAL读取图片，先要进行注册
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径

    size_t i,j,p,q;
    Log(LogName,"01|01");//写入log日志

    //读取PAN影像信息
    GdalInf* PANInf = new(std::nothrow) GdalInf();
    if (NULL == PANInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return false;
    }

    if (PANInf->ReadImageInf(Input_PAN_FileName) != 0) {
        cerr<<"Read PAN Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }

    //读取MS影像信息
    GdalInf* MSInf = new(std::nothrow) GdalInf();
    if (NULL == MSInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }
    if (MSInf->ReadImageInf(Input_MS_FileName)!=0) {
        cerr<<"Read MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    int PAN_Width = PANInf->GetWidth(); //PAN宽度（MS影像插值后宽度）
    int PAN_Height= PANInf->GetHeight();//PAN高度（MS影像插值后高度）

    int MS_Width = MSInf->GetWidth();  //MS宽度
    int MS_Height = MSInf->GetHeight();//MS高度
    int MS_Bandcount = MSInf->GetBandCount(); //MS影像波段数

    if (MS_Bandcount < 3) {
        cerr<<"HSI Fusion Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    } else if(MS_Bandcount > 3) {//如果波段数大于3 取前3个波段
        MS_Bandcount = 3;
        MSInf->SetBandCount(MS_Bandcount);
    }

    float* PANData = NULL;    //记录PAN数据
    float* MSData = NULL;    //记录MS原始数据
    float* New_MSData = NULL;  //记录MS插值后数据

    //读取MS图像数据
    if (MSInf->ReadImageToBuff(Input_MS_FileName,0,0,MS_Width,MS_Height,bandlist) != 0) {
        cerr<<"Read MS Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    Log(LogName,"01|02");//写入log日志

    //MS图像数据指针
    MSData = MSInf->GetImgData();
    New_MSData = new(std::nothrow) float[PAN_Height*PAN_Width*MS_Bandcount];  //保存MS插值后的影像
    if (NULL == New_MSData) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }
    //插值
    //插值方法：Nearest/Linear/CubicConv

    Interpolation(MSData, MS_Height,MS_Width ,MS_Bandcount, New_MSData, PAN_Height, PAN_Width, InterpolationMethod);

    //释放内存
    MSInf->ClearImageData(); MSData = NULL;

    //读取PAN图像数据
    if (PANInf->ReadImageToBuff(Input_PAN_FileName,0,0,PAN_Width,PAN_Height) != 0) {
        cerr<<"Read PAN Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete New_MSData;
        return false;
    }

    PANData = PANInf->GetImgData();    //PAN图像数据指针

    Log(LogName,"01|03");//写入log日志

    //		int ROWS = MS_Bandcount;
    size_t COLS = PAN_Width*PAN_Height;

    //对MS做HSI变换
    RGB2HSI(New_MSData, PAN_Height, PAN_Width, MS_Bandcount);

    //对PAN影像灰度拉伸

    float mean,s;//均值 标准差

    MeanStd(PANData,PAN_Height, PAN_Width,s,mean);

    for (i = 0; i < COLS; i++)
        PANData[i] = (PANData[i]-mean)/(s);

    MeanStd(New_MSData+2*COLS,PAN_Height, PAN_Width,s,mean);

    for (i = 0; i < COLS; i++)
        PANData[i] = (PANData[i]*(s)+mean) > 0? (PANData[i]*(s)+mean) : 0;


    //进行PAN影像与MS影像融合

    //  nbscales -- the total number of scales for subband decomposition
    int nbscales=3;
    //  nbangles_coarse -- the number of angles in the 2nd coarest scale
    int nbangles_coarse=4;
    //  ac -- ac==1 use curvelet at the finest level, ac==0, use wavelet at the finest level
    int ac=1;
    CpxNumMat PanMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++){
            PanMat(i,j)= cpx(DATA2D(PANData, i, j, PAN_Width),0);
        }
    //释放内存
    PANInf->ClearImageData();
    PANData=NULL;

    vector< vector<CpxNumMat> > CPanMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, PanMat, CPanMat);

    delete[] PanMat._data;
    PanMat._data=NULL;//clear(PanMat);

    CpxNumMat IMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++) {
            IMat(i,j) = cpx(New_MSData[2*COLS+i*PAN_Width+j],0);
        }

    vector< vector<CpxNumMat> > CIMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, IMat, CIMat);

    //释放内存
    //IMat.~NumMat();
    delete[] IMat._data;
    IMat._data=NULL;//clear(IMat);

    for (i=0;i<CPanMat[0][0].m();i++)
        for (j=0;j<CPanMat[0][0].n();j++){
            CPanMat[0][0](i,j)=0.5*CPanMat[0][0](i,j)+0.5*CIMat[0][0](i,j);
        }
    for (i=1;i<CPanMat.size();i++)
        for (j=0;j<CPanMat[i].size();j++)
            for (p=0;p<CPanMat[i][j].m();p++)
                for (q=0;q<CPanMat[i][j].n();q++) {
                    CPanMat[i][j](p,q)=	abs(CPanMat[i][j](p,q)) > abs(CIMat[i][j](p,q))? CPanMat[i][j](p,q):CIMat[i][j](p,q);
                }
    //释放内存
    CIMat.clear();

    CpxNumMat Fusion(PAN_Height, PAN_Width);
    ifdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, CPanMat, Fusion);

    //释放内存
    CPanMat.clear();
    //用融合后的图像替换I分量生成新HCS
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++){
            DATA2D(New_MSData, 2, i*PAN_Width+j, COLS) = Fusion(i,j).real() > 0 ? (float)Fusion(i,j).real():0;//
        }
    //释放内存
    delete[] Fusion._data;
    Fusion._data=NULL;//clear(Fusion);

    //将MS从HSI变换到RGB空间
    HSI2RGB(New_MSData, PAN_Height, PAN_Width, MS_Bandcount);

    //GDAL写文件
    Log(LogName,"01|04");//写入log日志
    
    MSInf->SetHeight(PAN_Height); //更新MS影像高度
    MSInf->SetWidth(PAN_Width);	  //更新MS影像宽度
    MSInf->SetGeoTransform(PANInf->GetGeoTransform());//更新地理信息
    //创建文件，写入头文件信息
    if (MSInf->WriteImageInf(Output_MS_FileName)!=0) {
        cerr<<"Write MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete New_MSData;
        return false;
    }

    MSInf->SetImgData(New_MSData);

    if(MSInf->WriteImageFromBuff(Output_MS_FileName,0,0,PAN_Width,PAN_Height)!=0){
        cerr<<"Read PAN Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete New_MSData;
        return false;
    }

    //释放内存
    MSInf->ClearImageData();
    PANData = NULL;
    MSData = NULL;
    New_MSData = NULL;

    //释放内存
    delete MSInf;	MSInf = NULL;
    delete PANInf;	PANInf = NULL;

    Log(LogName,"01|05");//写入log日志
    return true;
}

bool CurveletFusion::MeanStd_Curvelet_HCS_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod){
    /*
     *融合方法：Curvelet与HCS
     *替换规则：I分量替换
     *Input_PAN_FileName		高分辨率图像路径及名称
     *Input_MS_FileName			多光谱图像路径及名称（必须为三个波段）
     *Output_MS_FileName		融合图像路径及名称
     *LogName                   日志文件存放路径
     *bandlist					多光谱或高光谱参与融合的波段顺序
     *InterpolationMethod		插值方法
     *作者：YS
     *日期：2016.3.7
     */
    GDALAllRegister();         //利用GDAL读取图片，先要进行注册
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径

    int i,j,p,q;
    Log(LogName,"01|01");//写入log日志

    //读取PAN影像信息
    GdalInf* PANInf = new(std::nothrow) GdalInf();
    if (NULL == PANInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return false;
    }

    if (PANInf->ReadImageInf(Input_PAN_FileName) != 0) {
        cerr<<"Read PAN Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }

    //读取MS影像信息
    GdalInf* MSInf = new(std::nothrow) GdalInf();
    if (NULL == MSInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }
    if (MSInf->ReadImageInf(Input_MS_FileName)!=0) {
        cerr<<"Read MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    int PAN_Width = PANInf->GetWidth(); //PAN宽度（MS影像插值后宽度）
    int PAN_Height= PANInf->GetHeight();//PAN高度（MS影像插值后高度）

    int MS_Width = MSInf->GetWidth();  //MS宽度
    int MS_Height = MSInf->GetHeight();//MS高度
    int MS_Bandcount = MSInf->GetBandCount(); //MS影像波段数

    if (MS_Bandcount < 3) {
        cerr<<"HSI Fusion Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    } else if(MS_Bandcount > 3) {//如果波段数大于3 取前3个波段
        MS_Bandcount = 3;
        MSInf->SetBandCount(MS_Bandcount);
    }


    float* PANData = NULL;    //记录PAN数据
    float* MSData = NULL;    //记录MS原始数据
    float* New_MSData = NULL;  //记录MS插值后数据

    //读取MS图像数据
    if (MSInf->ReadImageToBuff(Input_MS_FileName,0,0,MS_Width,MS_Height,bandlist) != 0) {
        cerr<<"Read MS Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    Log(LogName,"01|02");//写入log日志

    //MS图像数据指针
    MSData = MSInf->GetImgData();
    New_MSData = new(std::nothrow) float[PAN_Height*PAN_Width*MS_Bandcount];  //保存MS插值后的影像
    if (NULL == New_MSData) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }
    //插值
    //插值方法：Nearest/Linear/CubicConv

    Interpolation(MSData, MS_Height,MS_Width ,MS_Bandcount, New_MSData, PAN_Height, PAN_Width, InterpolationMethod);

    //释放内存
    MSInf->ClearImageData(); MSData = NULL;

    //读取PAN图像数据
    if (PANInf->ReadImageToBuff(Input_PAN_FileName,0,0,PAN_Width,PAN_Height) != 0) {
        cerr<<"Read PAN Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete New_MSData;
        return false;
    }

    PANData = PANInf->GetImgData();    //PAN图像数据指针

    Log(LogName,"01|03");//写入log日志

    //		int ROWS = MS_Bandcount;
    int COLS = PAN_Width*PAN_Height;

    //对MS做HSI变换
    HCS_Trans(New_MSData, PAN_Height, PAN_Width, MS_Bandcount);

    //对PAN影像灰度拉伸
    float mean,s;//均值 标准差
    for (i = 0; i < COLS; i++){
        PANData[i] = (PANData[i]*PANData[i]);
        DATA2D(New_MSData, 0, i, COLS) = DATA2D(New_MSData, 0, i, COLS)*DATA2D(New_MSData, 0, i, COLS);
    }

    MeanStd(PANData,PAN_Height, PAN_Width,s,mean);

    for (i = 0; i < COLS; i++)
        PANData[i] = (PANData[i]-mean+s)/(s);

    MeanStd(New_MSData,PAN_Height, PAN_Width,s,mean);

    for (i = 0; i < COLS; i++){
        PANData[i] = PANData[i]*(s)+mean-s > 0 ? sqrt(PANData[i]*(s)+mean-s) : 0; //截断
        DATA2D(New_MSData, 0, i, COLS) = sqrt(DATA2D(New_MSData, 0, i, COLS));    //开平方 恢复原来的图像
    }

    //进行PAN影像与MS影像融合

    //  nbscales -- the total number of scales for subband decomposition
    int nbscales=3;
    //  nbangles_coarse -- the number of angles in the 2nd coarest scale
    int nbangles_coarse=4;
    //  ac -- ac==1 use curvelet at the finest level, ac==0, use wavelet at the finest level
    int ac=1;
    CpxNumMat PanMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++){
            PanMat(i,j)= cpx(DATA2D(PANData, i, j, PAN_Width),0);
            //IMat(i,j) = cpx(DATA2D(I->imgdata, i, j, PANData->width),0);
        }

    //释放内存
    PANInf->ClearImageData();PANData=NULL;

    vector< vector<CpxNumMat> > CPanMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, PanMat, CPanMat);
    //释放内存
    //PanMat.~NumMat();
    delete[] PanMat._data;
    PanMat._data=NULL;//clear(PanMat);

    CpxNumMat IMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++){
            //PanMat(i,j)= cpx(DATA2D(PANData->imgdata, i, j, PANData->width),0);
            IMat(i,j) = cpx(New_MSData[i*PAN_Width+j],0);  //第一行为HCS变换后的I分量
        }

    vector< vector<CpxNumMat> > CIMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, IMat, CIMat);

    //释放内存
    //IMat.~NumMat();
    delete[] IMat._data;
    IMat._data=NULL;//clear(IMat);

    for (i=0;i<CPanMat[0][0].m();i++)
        for (j=0;j<CPanMat[0][0].n();j++){
            CPanMat[0][0](i,j)=0.5*CPanMat[0][0](i,j)+0.5*CIMat[0][0](i,j);
        }
    for (i=1;i<CPanMat.size();i++)
        for (j=0;j<CPanMat[i].size();j++)
            for (p=0;p<CPanMat[i][j].m();p++)
                for (q=0;q<CPanMat[i][j].n();q++){
                    CPanMat[i][j](p,q)=	abs(CPanMat[i][j](p,q)) > abs(CIMat[i][j](p,q))? CPanMat[i][j](p,q):CIMat[i][j](p,q);
                }
    //释放内存
    CIMat.clear();

    CpxNumMat Fusion(PAN_Height, PAN_Width);
    ifdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, CPanMat, Fusion);

    //释放内存
    CPanMat.clear();
    //用融合后的图像替换I分量生成新HCS
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++) {
            DATA2D(New_MSData, 0, i*PAN_Width+j, COLS) = Fusion(i,j).real() > 0 ? (float)Fusion(i,j).real():0;//
        }
    //释放内存
    delete[] Fusion._data;
    Fusion._data=NULL;//clear(Fusion);

    //将MS从HSI变换到RGB空间
    HCS_Itrans(New_MSData, PAN_Height, PAN_Width, MS_Bandcount);

    //GDAL写文件
    Log(LogName,"01|04");//写入log日志

    MSInf->SetHeight(PAN_Height); //更新MS影像高度
    MSInf->SetWidth(PAN_Width);	  //更新MS影像宽度
    MSInf->SetGeoTransform(PANInf->GetGeoTransform());//更新地理信息
    //创建文件，写入头文件信息
    if (MSInf->WriteImageInf(Output_MS_FileName)!=0) {
        cerr<<"Write MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete New_MSData;
        return false;
    }

    MSInf->SetImgData(New_MSData);

    if(MSInf->WriteImageFromBuff(Output_MS_FileName,0,0,PAN_Width,PAN_Height)!=0){
        cerr<<"Read PAN Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete New_MSData;
        return false;
    }

    //释放内存
    MSInf->ClearImageData();
    PANData = NULL;
    MSData = NULL;
    New_MSData = NULL;

    //释放内存
    delete MSInf;	MSInf = NULL;
    delete PANInf;	PANInf = NULL;

    Log(LogName,"01|05");//写入log日志
    return true;
}

bool CurveletFusion::MeanStd_Curvelet_GramSchmidt_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod){
    /*
     *融合方法：MeanStd_Curvelet_GramSchmidt_Fusion（用多光谱波段影像模拟全色图像，取均值）
     *Input_PAN_FileName		高分辨率图像路径及名称
     *Input_MS_FileName			多光谱图像路径及名称（必须为三个波段）
     *Output_MS_FileName		融合图像路径及名称
     *LogName                   日志文件存放路径
     *bandlist					多光谱或高光谱参与融合的波段顺序
     *InterpolationMethod		插值方法
     *作者：YS
     *日期：2016.3.1
     */


    GDALAllRegister();         //利用GDAL读取图片，先要进行注册
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径

    int i,j,k,p,q;
    Log(LogName,"01|01");//写入log日志

    //读取PAN影像信息
    GdalInf* PANInf = new(std::nothrow) GdalInf();
    if (NULL == PANInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return false;
    }

    if (PANInf->ReadImageInf(Input_PAN_FileName) != 0) {
        cerr<<"Read PAN Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }

    //读取MS影像信息
    GdalInf* MSInf = new(std::nothrow) GdalInf();
    if (NULL == MSInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }
    if (MSInf->ReadImageInf(Input_MS_FileName) != 0) {
        cerr<<"Read MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    int PAN_Width = PANInf->GetWidth(); //PAN宽度（MS影像插值后宽度）
    int PAN_Height= PANInf->GetHeight();//PAN高度（MS影像插值后高度）

    int MS_Width = MSInf->GetWidth();  //MS宽度
    int MS_Height = MSInf->GetHeight();//MS高度
    int MS_Bandcount = MSInf->GetBandCount(); //MS影像波段数

    if (MS_Bandcount < 3) {
        cerr<<"Curvelet_GramSchmidt Fusion Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    } else if(MS_Bandcount > 3) { //如果波段数大于3 取前3个波段
        MS_Bandcount = 3;
        MSInf->SetBandCount(MS_Bandcount);
    }


    float* PANData =NULL;    //记录PAN数据
    float* MSData = NULL;    //记录MS原始数据
    float* New_MSData=NULL;  //记录MS插值后数据

    if (MSInf->ReadImageToBuff(Input_MS_FileName,0,0,MS_Width,MS_Height,bandlist) != 0) { //读取MS图像数据
        cerr<<"Read MS Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    //MS图像数据指针

    Log(LogName,"01|02");//写入log日志

    MSData = MSInf->GetImgData();
    New_MSData = new(std::nothrow) float[PAN_Height*PAN_Width*(MS_Bandcount+1)];  //保存MS插值后的影像
    if (NULL == New_MSData) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }
    //插值
    //插值方法：Nearest/Linear/CubicConv

    Interpolation(MSData, MS_Height,MS_Width ,MS_Bandcount, New_MSData+PAN_Height*PAN_Width, PAN_Height, PAN_Width, InterpolationMethod);

    //释放内存
    MSInf->ClearImageData(); MSData = NULL;

    //读取PAN图像数据
    if (PANInf->ReadImageToBuff(Input_PAN_FileName,0,0,PAN_Width,PAN_Height) != 0) {
        cerr<<"Read PAN Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        delete New_MSData;
        return false;
    }

    PANData = PANInf->GetImgData();    //PAN图像数据指针

    int ROWS = MS_Bandcount+1;
    int COLS = PAN_Width*PAN_Height;

    //对MS做GramSchmidt融合
    Log(LogName,"01|03");//写入log日志
    //通过取MS的平均值模拟全色

    double sum;
    for(i=0;i<COLS;i++){
        sum = 0.0;
        for(j=1;j<ROWS;j++)
            sum += pow(DATA2D(New_MSData,j,i,COLS),2); //2)
        DATA2D(New_MSData,0,i,COLS) = sqrt(sum/MS_Bandcount);//2) 比1）降低了偏差
    }

    float *R = new(std::nothrow) float[ROWS*ROWS];
    float *m = new(std::nothrow) float[ROWS];
    if (NULL == R || NULL == m) {
        cerr<<"RGramSchmidt Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        delete New_MSData;
        return false;
    }
    for (i=0;i<ROWS;i++) {
        R[i*MS_Bandcount+i] = Cov(New_MSData+(i)*COLS,New_MSData+(i)*COLS,COLS);
        R[i*MS_Bandcount+i] = R[i*MS_Bandcount+i]*R[i*MS_Bandcount+i];

        for (j=i+1;j<ROWS;j++) {
            R[i*MS_Bandcount+j] = Cov(New_MSData+(i)*COLS,New_MSData+(j)*COLS,COLS);
        }
        m[i] = Mean(New_MSData+(i)*COLS,COLS);

        for (j=0;j<COLS;j++) {
            DATA2D(New_MSData,i,j,COLS) -= m[i];
        }

        for (j=0;j<COLS;j++) {
            for (k=0;k<i;k++) {
                DATA2D(New_MSData,i,j,COLS) -= (DATA2D(New_MSData,k,j,COLS)*R[k*MS_Bandcount+i]/R[k*MS_Bandcount+k]);
            }
        }
    }
    //拉伸

    float pan_mean,pan_std;
    float ms_mean,ms_std;

    MeanStd(PANData,PAN_Height, PAN_Width,pan_std,pan_mean);

    MeanStd(New_MSData,PAN_Height, PAN_Width,ms_std,ms_mean);

    for (i = 0; i < COLS; i++)
        PANData[i] = ((PANData[i]-pan_mean)/pan_std) * ms_std + ms_mean;

    //进行PAN影像与MS影像融合

    //  nbscales -- the total number of scales for subband decomposition
    int nbscales=3;
    //  nbangles_coarse -- the number of angles in the 2nd coarest scale
    int nbangles_coarse=4;
    //  ac -- ac==1 use curvelet at the finest level, ac==0, use wavelet at the finest level
    int ac=1;
    CpxNumMat PanMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++){
            PanMat(i,j)= cpx(DATA2D(PANData, i, j, PAN_Width),0);
        }
    //释放内存

    PANInf->ClearImageData();PANData=NULL;

    vector< vector<CpxNumMat> > CPanMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, PanMat, CPanMat);
    //释放内存
    //PanMat.~NumMat();
    delete[] PanMat._data;PanMat._data=NULL;//clear(PanMat);

    CpxNumMat IMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++) {
            IMat(i,j) = cpx(New_MSData[0*COLS+i*PAN_Width+j],0);
        }

    vector< vector<CpxNumMat> > CIMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, IMat, CIMat);

    //释放内存
    //IMat.~NumMat();
    delete[] IMat._data;IMat._data=NULL;//clear(IMat);

    for (i=0;i<CPanMat[0][0].m();i++)
        for (j=0;j<CPanMat[0][0].n();j++){
            CPanMat[0][0](i,j)=0.5*CPanMat[0][0](i,j)+0.5*CIMat[0][0](i,j);
        }
    for (i=1;i<CPanMat.size();i++)
        for (j=0;j<CPanMat[i].size();j++)
            for (p=0;p<CPanMat[i][j].m();p++)
                for (q=0;q<CPanMat[i][j].n();q++){
                    CPanMat[i][j](p,q)=	abs(CPanMat[i][j](p,q)) > abs(CIMat[i][j](p,q))? CPanMat[i][j](p,q):CIMat[i][j](p,q);
                }
    //释放内存
    CIMat.clear();

    CpxNumMat Fusion(PAN_Height, PAN_Width);
    ifdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, CPanMat, Fusion);

    //释放内存
    CPanMat.clear();
    //用融合后的图像替换
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++) {
            DATA2D(New_MSData, 0, i*PAN_Width+j, COLS) = Fusion(i,j).real() > 0 ? (float)Fusion(i,j).real():0;//
        }
    //释放内存
    delete[] Fusion._data;Fusion._data = NULL;//clear(Fusion);

    //反变换
    for (i=ROWS-1;i>=0;i--) {
        for (j=0;j<COLS;j++) {
            DATA2D(New_MSData,i,j,COLS) += m[i];
        }

        for (j=0;j<COLS;j++) {
            for (k=0;k<i;k++) {
                DATA2D(New_MSData,i,j,COLS) += (DATA2D(New_MSData,k,j,COLS)*R[k*MS_Bandcount+i]/R[k*MS_Bandcount+k]);
            }
            DATA2D(New_MSData,i,j,COLS) = DATA2D(New_MSData,i,j,COLS)>0 ? DATA2D(New_MSData,i,j,COLS):0; //截断
        }
    }

    delete[] R;
    delete[] m;

    //释放内存
    PANInf->ClearImageData();PANData = NULL;
    delete PANInf;	PANInf = NULL;

    //GDAL写文件
    Log(LogName,"01|04");//写入log日志

    MSInf->SetHeight(PAN_Height); //更新MS影像高度
    MSInf->SetWidth(PAN_Width);	  //更新MS影像宽度
    MSInf->SetGeoTransform(PANInf->GetGeoTransform());//更新地理信息
    if (MSInf->WriteImageInf(Output_MS_FileName)!=0) //创建文件，写入头文件信息
    {
        cerr<<"Write MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete MSInf; MSInf = NULL;
        delete[] New_MSData;
        return false;
    }

    MSInf->SetImgData((New_MSData+PAN_Height*PAN_Width));
    if(MSInf->WriteImageFromBuff(Output_MS_FileName,0,0,PAN_Width,PAN_Height)!=0){
        cerr<<"Write MS Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        MSInf->SetImgData(NULL);
        delete MSInf; MSInf = NULL;
        delete[] New_MSData ;New_MSData = NULL;
        return false;
    }
    //释放内存
    MSInf->SetImgData(NULL);
    delete MSInf; MSInf = NULL;
    delete[] New_MSData ;New_MSData = NULL;

    Log(LogName,"01|05");//写入log日志

    return true;
}

//改进 2016.3.17
bool CurveletFusion::MeanStd_Curvelet_HCS_Fusion_New(const char* Input_PAN_FileName,
                                                     const char* Input_MS_FileName,
                                                     const char* Output_MS_FileName,
                                                     const char* LogName,
                                                     int* bandlist,int InterpolationMethod) {
    /*
     *融合方法：Curvelet与HCS
     *替换规则：I分量替换
     *Input_PAN_FileName		高分辨率图像路径及名称
     *Input_MS_FileName			多光谱图像路径及名称（必须为三个波段）
     *Output_MS_FileName		融合图像路径及名称
     *LogName                   日志文件存放路径
     *bandlist					多光谱或高光谱参与融合的波段顺序
     *InterpolationMethod		插值方法
     *作者：YS
     *日期：2016.3.7
     */

    GDALAllRegister();         //利用GDAL读取图片，先要进行注册
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径

    size_t i,j;
    int p,q;
    Log(LogName,"01|01");//写入log日志

    //读取PAN影像信息
    GdalInf* PANInf = new(std::nothrow) GdalInf();
    if (NULL == PANInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return false;
    }

    if (PANInf->ReadImageInf(Input_PAN_FileName) != 0) {
        cerr<<"Read PAN Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }

    //读取MS影像信息
    GdalInf* MSInf = new(std::nothrow) GdalInf();
    if (NULL == MSInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }
    if (MSInf->ReadImageInf(Input_MS_FileName) != 0) {
        cerr<<"Read MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    int PAN_Width = PANInf->GetWidth(); //PAN宽度（MS影像插值后宽度）
    int PAN_Height= PANInf->GetHeight();//PAN高度（MS影像插值后高度）

    int MS_Width = MSInf->GetWidth();  //MS宽度
    int MS_Height = MSInf->GetHeight();//MS高度
    int MS_Bandcount = MSInf->GetBandCount(); //MS影像波段数

    if (MS_Bandcount < 3) {
        cerr<<"Curvelet_HCS Fusion Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    } else if(MS_Bandcount > 3) { //如果波段数大于3 取前3个波段
		MS_Bandcount = 3;
		MSInf->SetBandCount(MS_Bandcount);
    }


    float* PANData = NULL;    //记录PAN数据
    float* MSData = NULL;    //记录MS原始数据
    float* New_MSData = NULL;  //记录MS插值后数据

    if (MSInf->ReadImageToBuff(Input_MS_FileName,0,0,MS_Width,MS_Height,bandlist) != 0) { //读取MS图像数据
        cerr<<"Read MS Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    Log(LogName,"01|02");//写入log日志

    //MS图像数据指针
    MSData = MSInf->GetImgData();
    New_MSData = new(std::nothrow) float[PAN_Height*PAN_Width*MS_Bandcount];  //保存MS插值后的影像
    if (NULL == New_MSData) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }
    //插值
    //插值方法：Nearest/Linear/CubicConv

    Interpolation(MSData, MS_Height,MS_Width ,MS_Bandcount, New_MSData, PAN_Height, PAN_Width, InterpolationMethod);

    //释放内存
    MSInf->ClearImageData(); MSData = NULL;

    //读取PAN图像数据
    if (PANInf->ReadImageToBuff(Input_PAN_FileName,0,0,PAN_Width,PAN_Height) != 0) {
        cerr<<"Read PAN Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        delete New_MSData;
        return false;
    }

    PANData = PANInf->GetImgData();    //PAN图像数据指针

    Log(LogName,"01|03");//写入log日志

    //		int ROWS = MS_Bandcount;
    int COLS = PAN_Width*PAN_Height;

    //对MS做HSI变换
    HCS_Trans(New_MSData, PAN_Height, PAN_Width, MS_Bandcount);

    //对PAN影像灰度拉伸

    float mean,s;//均值 标准差

    for (i = 0; i < COLS; i++) {
        PANData[i] = (PANData[i]*PANData[i]);
        DATA2D(New_MSData, 0, i, COLS) = DATA2D(New_MSData, 0, i, COLS)*DATA2D(New_MSData, 0, i, COLS);
    }

    MeanStd(PANData,PAN_Height, PAN_Width,s,mean);

    for (i = 0; i < COLS; i++)
        PANData[i] = (PANData[i]-mean+s)/(s);

    MeanStd(New_MSData,PAN_Height, PAN_Width,s,mean);

    for (i = 0; i < COLS; i++){
        PANData[i] = PANData[i]*(s)+mean-s > 0 ? sqrt(PANData[i]*(s)+mean-s) : 0; //截断
        DATA2D(New_MSData, 0, i, COLS) = sqrt(DATA2D(New_MSData, 0, i, COLS));    //开平方 恢复原来的图像
    }

    //进行PAN影像与MS影像融合

    //  nbscales -- the total number of scales for subband decomposition
    int nbscales=4;
    //  nbangles_coarse -- the number of angles in the 2nd coarest scale
    int nbangles_coarse=4;
    //  ac -- ac==1 use curvelet at the finest level, ac==0, use wavelet at the finest level
    int ac=1;
    CpxNumMat PanMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++){
            PanMat(i,j)= cpx(DATA2D(PANData, i, j, PAN_Width),0);
            //IMat(i,j) = cpx(DATA2D(I->imgdata, i, j, PANData->width),0);
        }
    //释放内存

    PANInf->ClearImageData();PANData=NULL;

    vector< vector<CpxNumMat> > CPanMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, PanMat, CPanMat);
    //释放内存
    //PanMat.~NumMat();
    delete[] PanMat._data;PanMat._data=NULL;//clear(PanMat);

    CpxNumMat IMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++) {
            IMat(i,j) = cpx(New_MSData[i*PAN_Width+j],0);  //第一行为HCS变换后的I分量
        }

    vector< vector<CpxNumMat> > CIMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, IMat, CIMat);

    //释放内存
    //IMat.~NumMat();
    delete[] IMat._data;IMat._data=NULL;//clear(IMat);

    Log(LogName,"01|04");//写入log日志

	CpxNumMat LPFusion(CPanMat[0][0].m(),CPanMat[0][0].n());
	#pragma omp parallel for
    for (int i=0;i<CPanMat[0][0].m();i++)
        for (int j=0;j<CPanMat[0][0].n();j++) {
			float pan_mean,pan_std,pan_entropy;
			float ms_mean,ms_std,ms_entropy;
			int localSize = 5;
			float x1,x2;
            LocalMeanStdEntropy(CPanMat[0][0],i,j,localSize,pan_mean,pan_std,pan_entropy);
            LocalMeanStdEntropy(CIMat[0][0],i,j,localSize,ms_mean,ms_std,ms_entropy);
            x1 = ((pan_std+ms_std)+abs(pan_std-ms_std))/(2*sqrt(pan_std*pan_std+ms_std*ms_std));
            x2 = ((pan_std+ms_std)-abs(pan_std-ms_std))/(2*sqrt(pan_std*pan_std+ms_std*ms_std));

            if(pan_entropy > ms_entropy) {
                LPFusion(i,j) = cpx(x1*CPanMat[0][0](i,j).real()+x2*CIMat[0][0](i,j).real(),x1*CPanMat[0][0](i,j).imag()+x2*CIMat[0][0](i,j).imag());
            } else {
                LPFusion(i,j) = cpx(x2*CPanMat[0][0](i,j).real()+x1*CIMat[0][0](i,j).real(),x2*CPanMat[0][0](i,j).imag()+x1*CIMat[0][0](i,j).imag());
            }
        }
	CPanMat[0][0] = LPFusion;
    for(i=1;i<CPanMat.size();i++)
        for(j=0;j<CPanMat[i].size();j++)
            for(p=0;p<CPanMat[i][j].m();p++)
                for (q=0;q<CPanMat[i][j].n();q++) {
                    CPanMat[i][j](p,q)=	abs(CPanMat[i][j](p,q)) > abs(CIMat[i][j](p,q))? CPanMat[i][j](p,q):CIMat[i][j](p,q);
                }
    //释放内存
    CIMat.clear();

    CpxNumMat Fusion(PAN_Height, PAN_Width);
    ifdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, CPanMat, Fusion);

    //释放内存
    CPanMat.clear();
    //用融合后的图像替换I分量生成新HCS
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++){

            DATA2D(New_MSData, 0, i*PAN_Width+j, COLS) = Fusion(i,j).real() > 0 ? (float)Fusion(i,j).real():0;//
        }
    //释放内存
    delete[] Fusion._data;Fusion._data=NULL;//clear(Fusion);

    //将MS从HSI变换到RGB空间
    HCS_Itrans(New_MSData, PAN_Height, PAN_Width, MS_Bandcount);

    //GDAL写文件
    Log(LogName,"01|05");//写入log日志
    MSInf->SetHeight(PAN_Height); //更新MS影像高度
    MSInf->SetWidth(PAN_Width);	  //更新MS影像宽度
    MSInf->SetGeoTransform(PANInf->GetGeoTransform());//更新地理信息
    //创建文件，写入头文件信息
    if (MSInf->WriteImageInf(Output_MS_FileName)!=0) {
        cerr<<"Write MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        delete New_MSData;
        return false;
    }

    MSInf->SetImgData(New_MSData);

    if(MSInf->WriteImageFromBuff(Output_MS_FileName,0,0,PAN_Width,PAN_Height)!=0){
        cerr<<"Read PAN Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }
    //释放内存
    MSInf->ClearImageData();
    New_MSData = NULL;
    PANData = NULL;
    MSData = NULL;

    //释放内存
    delete MSInf;	MSInf = NULL;
    delete PANInf;	PANInf = NULL;

    Log(LogName,"01|06");//写入log日志
    return true;
}

//改进 2016.08.30
bool CurveletFusion::MeanStd_Curvelet_HIS_Fusion_SAR(const char* Input_PAN_FileName,
                                                     const char* Input_MS_FileName,
                                                     const char* Output_MS_FileName,
                                                     const char* LogName,
                                                     int* bandlist,int InterpolationMethod) {
    /*
     *融合方法：Curvelet与HCS
     *替换规则：I分量替换
     *Input_PAN_FileName		高分辨率图像路径及名称
     *Input_MS_FileName			多光谱图像路径及名称（必须为三个波段）
     *Output_MS_FileName		融合图像路径及名称
     *LogName                   日志文件存放路径
     *bandlist					多光谱或高光谱参与融合的波段顺序
     *InterpolationMethod		插值方法
     *作者：YS
     *日期：2016.8.30
     */

    GDALAllRegister();         //利用GDAL读取图片，先要进行注册
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径

    size_t i,j;
    int p,q;
    Log(LogName,"01|01");//写入log日志

    //读取PAN影像信息
    GdalInf* PANInf = new(std::nothrow) GdalInf();
    if (NULL == PANInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return false;
    }

    if (PANInf->ReadImageInf(Input_PAN_FileName) != 0) {
        cerr<<"Read PAN Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }

    //读取MS影像信息
    GdalInf* MSInf = new(std::nothrow) GdalInf();
    if (NULL == MSInf) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        return false;
    }
    if (MSInf->ReadImageInf(Input_MS_FileName) != 0) {
        cerr<<"Read MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    int PAN_Width = PANInf->GetWidth(); //PAN宽度（MS影像插值后宽度）
    int PAN_Height= PANInf->GetHeight();//PAN高度（MS影像插值后高度）

    int MS_Width = MSInf->GetWidth();  //MS宽度
    int MS_Height = MSInf->GetHeight();//MS高度
    int MS_Bandcount = MSInf->GetBandCount(); //MS影像波段数

    if (MS_Bandcount < 3) {
        cerr<<"Band Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    } else if(MS_Bandcount > 3) { //如果波段数大于3 取前3个波段
        MS_Bandcount = 3;
        MSInf->SetBandCount(MS_Bandcount);
    }


    float* PANData = NULL;    //记录PAN数据
    float* MSData = NULL;    //记录MS原始数据
    float* New_MSData = NULL;  //记录MS插值后数据

    if (MSInf->ReadImageToBuff(Input_MS_FileName,0,0,MS_Width,MS_Height,bandlist) != 0) { //读取MS图像数据
        cerr<<"Read MS Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }

    Log(LogName,"01|02");//写入log日志

    //MS图像数据指针
    MSData = MSInf->GetImgData();
    New_MSData = new(std::nothrow) float[PAN_Height*PAN_Width*MS_Bandcount];  //保存MS插值后的影像
    if (NULL == New_MSData) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }
    //插值
    //插值方法：Nearest/Linear/CubicConv

    Interpolation(MSData, MS_Height,MS_Width ,MS_Bandcount, New_MSData, PAN_Height, PAN_Width, InterpolationMethod);

    //释放内存
    MSInf->ClearImageData(); MSData = NULL;

    //读取PAN图像数据
    if (PANInf->ReadImageToBuff(Input_PAN_FileName,0,0,PAN_Width,PAN_Height) != 0) {
        cerr<<"Read PAN Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        delete[] New_MSData;
        return false;
    }

    PANData = PANInf->GetImgData();    //PAN图像数据指针

    Log(LogName,"01|03");//写入log日志

    //		int ROWS = MS_Bandcount;
    int COLS = PAN_Width*PAN_Height;

    //对MS做HSI变换
    RGB2HSI(New_MSData, PAN_Height, PAN_Width, MS_Bandcount);

	//对PAN影像灰度拉伸

	float mean,s;//均值 标准差
	//for (i = 0; i < COLS; i++) //log2
	//	PANData[i] = log(PANData[i]==0? 1:PANData[i])/log(2.0);

	MeanStd(PANData,PAN_Height, PAN_Width,s,mean);

	for (i = 0; i < COLS; i++)
		PANData[i] = (PANData[i]-mean)/(s);

	MeanStd(New_MSData+2*COLS,PAN_Height, PAN_Width,s,mean);

	for (i = 0; i < COLS; i++)
		PANData[i] = (PANData[i]*(s)+mean) > 0? (PANData[i]*(s)+mean) : 0;

    //进行PAN影像与MS影像融合

    //  nbscales -- the total number of scales for subband decomposition
    int nbscales=4;
    //  nbangles_coarse -- the number of angles in the 2nd coarest scale
    int nbangles_coarse=4;
    //  ac -- ac==1 use curvelet at the finest level, ac==0, use wavelet at the finest level
    int ac=1;
    CpxNumMat PanMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++){
            PanMat(i,j)= cpx(DATA2D(PANData, i, j, PAN_Width),0);
        }

    //释放内存
    PANInf->ClearImageData();PANData=NULL;

    vector< vector<CpxNumMat> > CPanMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, PanMat, CPanMat);
    //释放内存
    //PanMat.~NumMat();
    delete[] PanMat._data;PanMat._data=NULL;//clear(PanMat);

    CpxNumMat IMat(PAN_Height, PAN_Width);
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++) {
            IMat(i,j) = cpx(New_MSData[2*COLS+i*PAN_Width+j],0);  //
        }

    vector< vector<CpxNumMat> > CIMat;
    fdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, IMat, CIMat);

    //释放内存
    //IMat.~NumMat();
    delete[] IMat._data;IMat._data=NULL;//clear(IMat);

    Log(LogName,"01|04");//写入log日志

	int localSize = 5;
	//x1 = x2 = 0.5;
	float* Gx = new(std::nothrow) float[localSize*localSize];//水平方向滤波器
	float* Gy = new(std::nothrow) float[localSize*localSize];//垂直方向滤波器
	if(!Gx||!Gy||!GenFilter(Gx,localSize,0)||!GenFilter(Gy,localSize,PI/2)){
		cerr<<"Generate Filter Error."<<endl;
		cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
		delete PANInf;
		delete MSInf;
		delete[] New_MSData;
		return false;
	}
	cout<<0<<" "<<CPanMat[0].size()<<" "<<0<<" "<<CPanMat[0][0].m()<<" "<<CPanMat[0][0].n()<<endl;
	//低频：自适应选择
	CpxNumMat LPFusion(CPanMat[0][0].m(),CPanMat[0][0].n());
	#pragma omp parallel for
    for (int i=0;i<CPanMat[0][0].m();i++)
        for (int j=0;j<CPanMat[0][0].n();j++) {
			float pan_mean,pan_std,pan_entropy;
			float ms_mean,ms_std,ms_entropy;
			
			float x1,x2;
			LocalMeanStdEntropy(CPanMat[0][0],i,j,localSize,pan_mean,pan_std,pan_entropy);
			LocalMeanStdEntropy(CIMat[0][0],i,j,localSize,ms_mean,ms_std,ms_entropy);
			x1 = ((pan_std+ms_std)+abs(pan_std-ms_std))/(2*sqrt(pan_std*pan_std+ms_std*ms_std));
			x2 = ((pan_std+ms_std)-abs(pan_std-ms_std))/(2*sqrt(pan_std*pan_std+ms_std*ms_std));

			if(pan_entropy > ms_entropy) {
				LPFusion(i,j)=cpx(x1*CPanMat[0][0](i,j).real()+x2*CIMat[0][0](i,j).real(),x1*CPanMat[0][0](i,j).imag()+x2*CIMat[0][0](i,j).imag());
			} else {
				LPFusion(i,j)=cpx(x2*CPanMat[0][0](i,j).real()+x1*CIMat[0][0](i,j).real(),x2*CPanMat[0][0](i,j).imag()+x1*CIMat[0][0](i,j).imag());
			}
			
			//CPanMat[0][0](i,j)=cpx(x2*CPanMat[0][0](i,j).real()+x1*CIMat[0][0](i,j).real(),x2*CPanMat[0][0](i,j).imag()+x1*CIMat[0][0](i,j).imag());
        }
	CPanMat[0][0] = LPFusion;
	//中频：特征量积
    for(i=1;i<CPanMat.size()-1;i++){
        for(j=0;j<CPanMat[i].size();j++){
			cout<<i<<" "<<CPanMat[i].size()<<" "<<j<<" "<<CPanMat[i][j].m()<<" "<<CPanMat[i][j].n()<<endl;
			#pragma omp parallel for			
            for(int p=0;p<CPanMat[i][j].m();p++){
                for (int q=0;q<CPanMat[i][j].n();q++) {
					//(1)
					float ms_feature;
					float pan_feature;
					if(!LocalFeature(CPanMat[i][j],p,q,localSize,Gx,Gy,pan_feature)||!LocalFeature(CIMat[i][j],p,q,localSize,Gx,Gy,ms_feature)){
						cerr<<"LocalFeature Error."<<endl;
						cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
						delete PANInf;
						delete MSInf;
						delete[] New_MSData;
						//return false;
					}
					
					CPanMat[i][j](p,q)=	pan_feature > ms_feature? CPanMat[i][j](p,q):CIMat[i][j](p,q);
					//(2)
					//CPanMat[i][j](p,q)=	cpx(x2*CPanMat[i][j](p,q).real()+x1*CIMat[i][j](p,q).real(),x2*CPanMat[i][j](p,q).imag()+x1*CIMat[i][j](p,q).imag());
                }
			}
		}
	}
	//高频：去噪声（置为0）
	for(j=0;j<CPanMat[CPanMat.size()-1].size();j++){
		cout<<CPanMat.size()-1<<" "<<CPanMat[CPanMat.size()-1].size()<<" "<<j<<" "<<CPanMat[i][j].m()<<" "<<CPanMat[i][j].n()<<endl;
#pragma omp parallel for			
		for(int p=0;p<CPanMat[i][j].m();p++){
			for (int q=0;q<CPanMat[i][j].n();q++) {
				//（3）
				//float ms_feature;
				//float pan_feature;
				//if(!LocalFeature(CPanMat[i][j],p,q,localSize,Gx,Gy,pan_feature)||!LocalFeature(CIMat[i][j],p,q,localSize,Gx,Gy,ms_feature)){
				//	cerr<<"LocalFeature Error."<<endl;
				//	cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
				//	delete PANInf;
				//	delete MSInf;
				//	delete[] New_MSData;
				//	//return false;
				//}
				//
				//CPanMat[i][j](p,q)=	pan_feature > ms_feature? CPanMat[i][j](p,q):CIMat[i][j](p,q);
				//（4）
				CPanMat[i][j](p,q)=	cpx(0,0);
			}
		}
	}
    //释放内存
	delete[] Gx;Gx=NULL;
	delete[] Gy;Gy=NULL;
    CIMat.clear();

    CpxNumMat Fusion(PAN_Height, PAN_Width);
    ifdct_wrapping(PAN_Height, PAN_Width, nbscales, nbangles_coarse, ac, CPanMat, Fusion);

    //释放内存
    CPanMat.clear();

    //用融合后的图像替换I分量生成新HCS
    for(i=0; i<PAN_Height; i++)
        for(j=0; j<PAN_Width; j++){
            DATA2D(New_MSData, 2, i*PAN_Width+j, COLS) = Fusion(i,j).real() > 0 ? (float)Fusion(i,j).real():0;//
        }
    //释放内存
    delete[] Fusion._data;Fusion._data=NULL;//clear(Fusion);

    //将MS从HSI变换到RGB空间
    HSI2RGB(New_MSData, PAN_Height, PAN_Width, MS_Bandcount);

    //GDAL写文件
    Log(LogName,"01|05");//写入log日志
    MSInf->SetHeight(PAN_Height); //更新MS影像高度
    MSInf->SetWidth(PAN_Width);	  //更新MS影像宽度
    MSInf->SetGeoTransform(PANInf->GetGeoTransform());//更新地理信息
    //创建文件，写入头文件信息
    if (MSInf->WriteImageInf(Output_MS_FileName)!=0) {
        cerr<<"Write MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        delete[] New_MSData;
        return false;
    }

    MSInf->SetImgData(New_MSData);

    if(MSInf->WriteImageFromBuff(Output_MS_FileName,0,0,PAN_Width,PAN_Height)!=0){
        cerr<<"Read PAN Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete PANInf;
        delete MSInf;
        return false;
    }
    //释放内存
    MSInf->ClearImageData();
    New_MSData = NULL;
    PANData = NULL;
    MSData = NULL;

    //释放内存
    delete MSInf;	MSInf = NULL;
    delete PANInf;	PANInf = NULL;

    Log(LogName,"01|06");//写入log日志
    return true;
}
