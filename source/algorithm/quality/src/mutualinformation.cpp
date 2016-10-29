#include "../utils/qualityutils.h"

int32_t MutualInformation(char* filepath1, char* filepath3, char* logfilepath, vector<double>& MutualInformation) {

    GDALDataset *poDataset1,*poDataset3;
    GDALAllRegister();
    poDataset1=(GDALDataset *)GDALOpen(filepath1,GA_ReadOnly);
    poDataset3=(GDALDataset *)GDALOpen(filepath3,GA_ReadOnly);
    if( (poDataset1 == NULL) || (poDataset3 == NULL)  ) {
        WriteMsg(logfilepath,-1,"Image file open error!");
        GDALClose(poDataset1);
        poDataset1=NULL;
        GDALClose(poDataset3);
        poDataset3=NULL;
        return -1;
    } else {
        WriteMsg(logfilepath,0,"MutualInformation algorithm is executing!");
    }

    int32_t n,i,j;
    int32_t bandnum1,width1,height1;
    int32_t bandnum3,width3,height3;

    bandnum1=poDataset1->GetRasterCount();
    width1=poDataset1->GetRasterXSize();
    height1=poDataset1->GetRasterYSize();

    bandnum3=poDataset3->GetRasterCount();
    width3=poDataset3->GetRasterXSize();
    height3=poDataset3->GetRasterYSize();

    if((bandnum1 != 1) || (width1 != width3) || (height1!=height3)) {
        WriteMsg(logfilepath,-1,"The size of input Images are not match!");
        GDALClose(poDataset1);
        poDataset1=NULL;
        GDALClose(poDataset3);
        poDataset3=NULL;
        return -1;
    }

    GDALRasterBand *pband;
    uint16_t *banddata1,*banddata3;
    banddata1=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width1*height1);
    banddata3=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width3*height3);

    pband=poDataset1->GetRasterBand(1);
    pband->RasterIO(GF_Read,0,0,width1,height1,banddata1,width1,height1,GDT_UInt16,0,0);
    GDALClose(pband);
    pband=NULL;

    uint32_t histogram1[256]={0};
    int64_t count1=0;
    int32_t tempnum1=0;
    int32_t max1=0;
    int32_t min1=65535;

    for(i=0;i<height1;i++) {
        for(j=0;j<width1;j++) {
            tempnum1=banddata1[i*width1+j];
            if(tempnum1>0) {
                if(max1<tempnum1)  {max1=tempnum1;}
                if(min1>tempnum1)  {min1=tempnum1;}
            }
        }
    }

    for(i=0;i<height1;i++) {
        for(j=0;j<width1;j++) {
            tempnum1=banddata1[i*width1+j];
            if(tempnum1>0) {
                int32_t tempindex=(int32_t)(1.0*(tempnum1-min1)/(max1-min1)*256);
                histogram1[tempindex]=histogram1[tempindex]+1;
                count1++;
            }
        }
    }

    for(n=0;n<bandnum3;n++) {
        pband=poDataset3->GetRasterBand(n+1);
        pband->RasterIO(GF_Read,0,0,width3,height3,banddata3,width3,height3,GDT_UInt16,0,0);

        uint32_t histogram3[256]={0};
        uint32_t histogram13[256][256]={0};
        int64_t count3=0;
        int64_t count13=0;
        int32_t tempnum3=0;
        double sum=0.0;
        int32_t max3=0;
        int32_t min3=65535;

        for(i=0;i<height3;i++) {
            for(j=0;j<width3;j++) {
                tempnum3=banddata3[i*width3+j];
                if(tempnum3>0) {
                    if(max3<tempnum3)  {max3=tempnum3;}
                    if(min3>tempnum3)  {min3=tempnum3;}
                }
            }
        }
        for(i=0;i<height3;i++) {
            for(j=0;j<width3;j++) {
                tempnum3=banddata3[i*width3+j];
                if(tempnum3>0) {
                    int32_t tempindex3=(int32_t) (1.0*(tempnum3-min3)/(max3-min3)*256);
                    histogram3[tempindex3]=histogram3[tempindex3]+1;
                    count3++;

                    tempnum1=banddata1[i*width1+j];
                    if(tempnum1>0) {
                        int32_t tempindex1=(int32_t) (1.0*(tempnum1-min1)/(max1-min1)*256);
                        histogram13[tempindex1][tempindex3]=histogram13[tempindex1][tempindex3]+1;
                        count13++;
                    }
                }
            }
        }
        for(i=0;i<256;i++) {
            for(j=0;j<256;j++) {
                if(histogram1[i]==0 || histogram3[j]==0 || histogram13[i][j]==0) continue;
                sum+= (1.0*histogram13[i][j]/count13) * log((1.0*histogram13[i][j]/count13)/(1.0*histogram1[i]/count1)/(1.0*histogram3[j]/count3)) / log(2.0);//log∫Ø ˝ƒ¨»œ“‘eŒ™µ◊
            }
        }
        GDALClose(pband);
        pband=NULL;

        MutualInformation.push_back(sum);
        int32_t temp = (int)(100.0*(n+1)/bandnum3);
        temp = (temp>99) ? 99:temp;
        WriteMsg(logfilepath,temp,"MutualInformation algorithm is executing!");
    }

    CPLFree(banddata1);
    banddata1=NULL;
    CPLFree(banddata3);
    banddata3=NULL;

    GDALClose(poDataset1);
    poDataset1=NULL;
    GDALClose(poDataset3);
    poDataset3=NULL;
    return 1;
}

bool mainMutualInformation(string filePath1, string filePath3, char* logfile, vector<double>& m_qRes) {
    m_qRes.clear();
    int32_t res = MutualInformation(const_cast<char*>(filePath1.c_str()), const_cast<char*>(filePath3.c_str()), logfile, m_qRes);
    if(res != 1) {
        WriteMsg(logfile,-1,"Algorithm mainMutualInformation executing error!");
        return false;
    }
    return true;
}
