#include "HSI.h"

//将RGB图像转换到HSI空间 MS_RGB_Mat各行依次为R、G、B
bool HSI::RGB2HSI(float *MS_RGB_Mat,int height,int width,int bandcount) {
    if (bandcount != 3) {
        cerr<<"RGB to HSI Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return false;
    }
    int i;
    int pixelcount = height*width; //每个波段的像素数

    float MS_r, MS_g, MS_b;
    float min_rgb;
    float fHue, fSaturation, fIntensity;
    for (i = 0; i < pixelcount; i++) {
        MS_r = DATA2D(MS_RGB_Mat, 0, i, pixelcount);
        MS_g = DATA2D(MS_RGB_Mat, 1, i, pixelcount);
        MS_b = DATA2D(MS_RGB_Mat, 2, i, pixelcount);;

        // Intensity分量[0, 1]
        fIntensity = ((MS_b + MS_g + MS_r) / 3);

        // 得到RGB分量中的最小值
        float fTemp = MS_r < MS_g ? MS_r : MS_g;
        min_rgb = fTemp < MS_b ? fTemp : MS_b;
        // Saturation分量[0, 1]
        fSaturation = 1 - (float)(3 * min_rgb) / (MS_r + MS_g + MS_b);

        // 计算theta角
        float numerator = (MS_r - MS_g + MS_r - MS_b) / 2;
        float denominator = sqrt(pow((MS_r - MS_g), 2) + (MS_r - MS_b)*(MS_g - MS_b));

        // 计算Hue分量
        if (denominator != 0) {
            float theta = acos(numerator / denominator) * 180 / 3.14;

            if (MS_b <= MS_g) {
                fHue = theta;
            } else {
                fHue = 360 - theta;
            }
        } else {
            fHue = 0;
        }
        // 赋值
        DATA2D(MS_RGB_Mat, 0, i, pixelcount) = fHue;
        DATA2D(MS_RGB_Mat, 1, i, pixelcount) = fSaturation;
        DATA2D(MS_RGB_Mat, 2, i, pixelcount) = fIntensity;
    }

    return true;
}

//将HSI图像转换到RGB空间 MS_HSI_Mat各行依次为H、S、I
bool HSI::HSI2RGB(float *MS_HSI_Mat, int height, int width, int bandcount){
    if (bandcount != 3) {
        cerr<<"HSI to RGB Error."<<endl;
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        return false;
    }

    int i;

    int pixelcount = height*width;//每个波段的像素数

    float fB, fG, fR;
    for (i = 0; i < pixelcount; i++) {
        // 该点的色度H
        float fH = DATA2D(MS_HSI_Mat, 0, i, pixelcount);
        // 该点的色饱和度S
        float fS = DATA2D(MS_HSI_Mat, 1, i, pixelcount);
        // 该点的亮度I
        float fI = DATA2D(MS_HSI_Mat, 2, i, pixelcount);

        float fTempB, fTempG, fTempR;
        // RG扇区
        if (fH < 120 && fH >= 0) {
            // 将H转为弧度表示
            fH = fH * 3.1415926 / 180;
            fTempB = fI * (1 - fS);
            fTempR = fI * (1 + (fS * cos(fH)) / cos(3.1415926 / 3 - fH));
            fTempG = (3 * fI - (fTempR + fTempB));
        } else if (fH < 240 && fH >= 120) {
            fH -= 120;
            // 将H转为弧度表示
            fH = fH * 3.1415926 / 180;
            fTempR = fI * (1 - fS);
            fTempG = fI * (1 + fS * cos(fH) / cos(3.1415926 / 3 - fH));
            fTempB = (3 * fI - (fTempR + fTempG));
        } else {
            fH -= 240;
            fH = fH * 3.1415926 / 180;
            fTempG = fI * (1 - fS);
            fTempB = fI * (1 + (fS * cos(fH)) / cos(3.1415926 / 3 - fH));
            fTempR = (3 * fI - (fTempG + fTempB));
        }

        fB = fTempB ;
        fG = fTempG ;
        fR = fTempR ;

        DATA2D(MS_HSI_Mat, 0, i, pixelcount) = fR;
        DATA2D(MS_HSI_Mat, 1, i, pixelcount) = fG;
        DATA2D(MS_HSI_Mat, 2, i, pixelcount) = fB;
    }
    return true;
}

