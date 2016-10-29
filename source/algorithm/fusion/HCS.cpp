#include "HCS.h"

//将图像从原始的颜色空间转换到HCS空间（第一个为I分量与HIS区分开）
bool HCS::HCS_Trans(float *MS_Mat,int height,int width,int bandcount){

    //HCS_Trans	原始的颜色空间转换到HCS空间
    //考虑节约内存，不返回值，而是在原数据基础上做变换
    //日期：2016.3.2
    //作者：YS

    int i,j,k;
    int pixelcount = height*width; //每个波段的像素数

    float *MS_hcs = new(std::nothrow) float[bandcount]; //hyperspherical color sharpening
    float *MS_ncs = new(std::nothrow) float[bandcount]; //native color space
    if (NULL == MS_hcs || NULL == MS_ncs) {
        cerr<<"HCS Transform Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return false;
    }
    for (i = 0; i < pixelcount; i++) {
        for (j=0;j<bandcount;j++) {
            MS_ncs[j] = DATA2D(MS_Mat, j, i, pixelcount);
        }

        MS_hcs[0] = 0;
        for (j=0;j<bandcount;j++) {
            MS_hcs[0] += (MS_ncs[j]*MS_ncs[j]);
        }
        MS_hcs[0] = sqrt(MS_hcs[0]);

        for (j=1;j<bandcount;j++) {
            MS_hcs[j] = 0;
            for (k=j;k<bandcount;k++) {
                MS_hcs[j] += (MS_ncs[k]*MS_ncs[k]);
            }
            MS_hcs[j] = atan(sqrt(MS_hcs[j])/MS_ncs[j-1]);
        }

        for (j=0;j<bandcount;j++) {
            DATA2D(MS_Mat, j, i, pixelcount) = MS_hcs[j];
        }
    }
    delete[] MS_hcs;
    delete[] MS_ncs;
    return true;
}

//将图像从HCS空间转换到原始的颜色空间（第一个为I分量与HIS区分开）
bool HCS::HCS_Itrans(float *MS_Mat, int height, int width, int bandcount) {
    //HCS_Trans	HCS空间转换到原始的颜色空间
    //考虑节约内存，不返回值，而是在原数据基础上做变换
    //日期：2016.3.2
    //作者：YS

    int i,j,k;
    int pixelcount = height*width; //每个波段的像素数

    float *MS_hcs = new(std::nothrow) float[bandcount]; //hyperspherical color sharpening
    float *MS_ncs = new(std::nothrow) float[bandcount]; //native color space

    if (NULL == MS_hcs || NULL == MS_ncs) {
        cerr<<"HCS Itransform Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return false;
    }
    for (i = 0; i < pixelcount; i++) {
        for (j=0;j<bandcount;j++) {
            MS_hcs[j] = DATA2D(MS_Mat, j, i, pixelcount);
        }


        for (j=0;j<bandcount-1;j++) {
            MS_ncs[j] = MS_hcs[0];
            for (k=0;k<j;k++) {
                MS_ncs[j] = MS_ncs[j]*sin(MS_hcs[k+1]);
            }
            MS_ncs[j] = MS_ncs[j]*cos(MS_hcs[j+1]);
        }

        MS_ncs[bandcount-1] = MS_hcs[0];
        for (k=0;k<bandcount-1;k++) {
            MS_ncs[bandcount-1] = MS_ncs[bandcount-1]*sin(MS_hcs[k+1]);
        }

        // 赋值
        for (j=0;j<bandcount;j++) {
            DATA2D(MS_Mat, j, i, pixelcount) = MS_ncs[j];
        }
    }

    delete[] MS_hcs;
    delete[] MS_ncs;
    return true;
}
