/*
//Image SignaltoNoiseRatio algorithm for geotiff (using GDAL)
//tif图像-影像信噪比
//Author: bangyong Qin
//Date:2015-11-17
*/

#include "../utils/qualityutils.h"

//SignaltoNoiseRatio-影像信噪比
int32_t SignaltoNoiseRatio(char* filepath,char* logfilepath, vector<double>& snrresult) {
    //定义数据集，打开文件
    GDALDataset *poDataset = NULL;
    GDALAllRegister();
    poDataset=(GDALDataset *)GDALOpen(filepath,GA_ReadOnly);
    if( poDataset == NULL ) {
        printf("Image file open error!\n");
        WriteMsg(logfilepath,-1,"Image file open error!");
        return -1;
    } else {
        printf("SignaltoNoiseRatio algorithm is executing!\n");
        WriteMsg(logfilepath,0,"SignaltoNoiseRatio algorithm is executing!");
    }

    //开始解析图像数据集
    int32_t n,i,j,s,t;
    int32_t bandnum,width,height;
    bandnum=poDataset->GetRasterCount();
    width=poDataset->GetRasterXSize();
    height=poDataset->GetRasterYSize();
    int16_t blksize=4;//子块划分大小，4代表：4x4像素
    int16_t blkcols=width/blksize;
    int16_t blkrows=height/blksize;
    GDALRasterBand *pband;
    uint16_t *banddata=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width*height);

    //loop for every bands
    for(n=0;n<bandnum;n++) {
        pband=poDataset->GetRasterBand(n+1);
        pband->RasterIO(GF_Read,0,0,width,height,banddata,width,height,GDT_UInt16,0,0);

        //define local variable//定义局部变量
        double imagemean=0.0;
        double imagestddev=0.0;
        int64_t validblks=0;

        //统计各个子块的均值和标准差
        for(i=0;i<blkrows;i++) {
            for(j=0;j<blkcols;j++) {
                //定义变量存储本子块的均值和标准差
                double blkmean=0;
                double blkstddev=0;
                //*(pblkstddev+i*blkcols+j)=0.0;
                int32_t datamark=1;
                int32_t tempdata;
                //求取子块均值
                for(s=0;s<blksize;s++) {
                    if(datamark==0)
                        break;
                    for(t=0;t<blksize;t++) {
                        tempdata=*(banddata+(i*blksize+s)*width+j*blksize+t);
                        if(tempdata==0) {
                            datamark=0;
                            break;
                        } else {
                            blkmean+=1.0*tempdata/(blksize*blksize);
                        }
                    }
                }
                //子块无效，均值和标准差设为0
                if(datamark==0) {
                    blkmean=0.0;
                    blkstddev=0.0;
                    continue;
                } else {
                    imagemean+=blkmean;
                    validblks++;
                }
                //求取子块标准差
                for(s=0;s<blksize;s++) {
                    for(t=0;t<blksize;t++) {
                        tempdata=*(banddata+(i*blksize+s)*width+j*blksize+t);
                        blkstddev+=((tempdata-blkmean)*(tempdata-blkmean))/(blksize*blksize);
                    }
                }
                blkstddev=sqrt(blkstddev);
                imagestddev += blkstddev;
            }
        }

        if(validblks==0) {
            snrresult.push_back(0);
        } else {
            //计算图像均值
            imagemean=imagemean/validblks;

            //计算图像标准差
            imagestddev=imagestddev/validblks;

            //计算图像信噪比
            if(imagemean==0 || imagestddev==0){
                snrresult.push_back(0);
            } else {
                snrresult.push_back(20*log10(imagemean/imagestddev));
            }
        }

        //关闭数据集
        GDALClose(pband);
        pband=NULL;

        //Writing the process and status of this Algorithm.
        int32_t temp = (int32_t)(100.0*(n+1)/bandnum);
        temp = (temp>99) ? 99:temp;
        printf("SignaltoNoiseRatio algorithm is executing %d%%!\n",temp);
        WriteMsg(logfilepath,temp,"SignaltoNoiseRatio algorithm is executing!");
    }

    //释放内存，关闭数据集
    CPLFree(banddata);
    banddata=NULL;

    GDALClose(poDataset);
    poDataset=NULL;

    return 1;
}

//主函数
bool mainSignaltoNoiseRatio(string& filepath, char* logfilepath, vector<double> &m_qRes) {
    m_qRes.clear();
    int32_t res = SignaltoNoiseRatio(const_cast<char*>(filepath.c_str()),logfilepath, m_qRes);
    if(res != 1) {
        printf("Algorithm executing error!\n");
        WriteMsg(logfilepath,-1,"Algorithm executing error!");
        return false;
    }
    return true;
}
