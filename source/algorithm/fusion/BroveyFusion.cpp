#include "BroveyFusion.h"

//通过Brovey算法实现PAN影像与MS影像融合
bool BroveyFusion::Brovey_Fusion(const char* Input_PAN_FileName, const char* Input_MS_FileName, const char* Output_MS_FileName,const char* LogName,int* bandlist,int InterpolationMethod){
    /*
     *融合方法：Brovey
     *Input_PAN_FileName		高分辨率图像路径及名称
     *Input_MS_FileName			多光谱图像路径及名称（必须为三个波段）
     *Output_MS_FileName		融合图像路径及名称
     *LogName                   日志文件存放路径
     *bandlist					多光谱或高光谱参与融合的波段顺序
     *InterpolationMethod		插值方法
     *作者：YS
     *日期：2016.2.
     */

    GDALAllRegister();         //利用GDAL读取图片，先要进行注册
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径

    int i,j;
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
        return false;
    }
    if (MSInf->ReadImageInf(Input_MS_FileName) != 0) {
        cerr<<"Read MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete MSInf;
        delete PANInf;
        return false;
    }

    int PAN_Width = PANInf->GetWidth(); //PAN宽度（MS影像插值后宽度）
    int PAN_Height= PANInf->GetHeight();//PAN高度（MS影像插值后高度）

    int MS_Width = MSInf->GetWidth();  //MS宽度
    int MS_Height = MSInf->GetHeight();//MS高度
    int MS_Bandcount = MSInf->GetBandCount(); //MS影像波段数

    if (MS_Bandcount < 3) {
        cerr<<"Brovey Fusion Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete MSInf;
        delete PANInf;
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
        delete MSInf;
        delete PANInf;
        return false;
    }

    //MS图像数据指针

    Log(LogName,"01|02");//写入log日志

    MSData = MSInf->GetImgData();
    New_MSData = new(std::nothrow) float[PAN_Height*PAN_Width*MS_Bandcount];  //保存MS插值后的影像
    if (NULL == New_MSData) {
        cerr<<"Memory Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete MSInf;
        delete PANInf;
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
        delete MSInf;
        delete PANInf;
        delete New_MSData;
        return false;
    }

    PANData = PANInf->GetImgData();    //PAN图像数据指针

    int ROWS = MS_Bandcount;
    int COLS = PAN_Width*PAN_Height;

    //对MS做Brovey融合
    Log(LogName,"01|03");//写入log日志

    float sum;

    for(i=0;i<COLS;i++) {
        sum=0.0;
        for(j=0;j<ROWS;j++)
            sum+=DATA2D(New_MSData,j,i,COLS);
        //避免sum为0 可以sum=sum+很小的值； 就不需要加判断

        if (sum!=0) {
            for(j=0;j<ROWS;j++) {
                DATA2D(New_MSData,j,i,COLS)=DATA2D(New_MSData,j,i,COLS)*PANData[i]/sum;
            }
        } else {
            /*多光谱图像存在像素为0的点，赋值为0*/
            for(j=0;j<ROWS;j++) {
                DATA2D(New_MSData,j,i,COLS)=0;
            }
        }
    }

    //释放内存
    PANInf->ClearImageData();PANData = NULL;

    //GDAL写文件
    Log(LogName,"01|04");//写入log日志

    MSInf->SetHeight(PAN_Height); //更新MS影像高度
    MSInf->SetWidth(PAN_Width);	  //更新MS影像宽度
    MSInf->SetGeoTransform(PANInf->GetGeoTransform());//更新地理信息
    
    if (MSInf->WriteImageInf(Output_MS_FileName) != 0) { //创建文件，写入头文件信息
        cerr<<"Write MS Image Inf Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete MSInf;
        delete PANInf;
        delete New_MSData;
        return false;
    }

    MSInf->SetImgData(New_MSData);
    if(MSInf->WriteImageFromBuff(Output_MS_FileName,0,0,PAN_Width,PAN_Height) != 0) {
        cerr<<"Write MS Image Data Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        delete MSInf;
        delete PANInf;
        return false;
    }
    //释放内存
    MSInf->ClearImageData();New_MSData = NULL;
    delete MSInf;	MSInf = NULL;
    delete PANInf;	PANInf = NULL;

    Log(LogName,"01|05");//写入log日志
    return true;
}
