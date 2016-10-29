#include "Tools.h"

//边界处理——插值内部 扩充边界
inline float Tools::Pixels(float* Mat, int row, int col, int band, int height, int width)
{
    /*
    *Pixels  解决图像处理时出现的越界问题
    *Mat	 图像数据	按行存储的一维向量
    *height  图像高度
    *width   图像宽度
    *row     像素点的行位置
    *col	 像素点的列位置
    *band    图像波段数
    *作者：YS
    *时间：2015.11.03
    */

    if(col < 0)
        col = 0;
    else if(col >= width)
        col = width-1;

    if(row < 0)
        row = 0;
    else if(row>=height)
        row = height-1;

    return DATA3D(Mat, row, col, band, height, width);
}

//三次卷积Sin（x*PI）/（x*PI）
inline float Tools::SinXDivX(float x){
    //该函数计算插值曲线Sin（x*PI）/（x*PI）的值
    //下面是它的近似拟合表达式
    const float a = -1; //a还可以取-2，-1，-0.75，-0.5等，起到调节锐化或模糊程度的作用

    if (x < 0) x = -x;//x=abs(x)
    float x2 = x*x;
    float x3 = x2*x;
    if (x <= 1)
        return (a+2)*x3 - (a+3)*x2 + 1;
    else if (x <= 2)
        return a*x3 - 5*a*x2 + 8*a*x - 4*a;
    else
        return 0;
}

//最临近插值（只适用于放大图像）
void Tools::Nearest_Interpolation_Base(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width){

    //版本：1.0
    //修改内容：修改插值过程中边界会越界的错误
    //作者：YS
    //修改时间：2015.11.03

    int new_i, new_j, k;
    //反变换得到的浮点坐标为(i,j)=(real_i+complex_i,real_j+complex_j)
    float i, j;
    int real_i, real_j;//在原图位置的整数部分(四舍五入)

    float ratio=(float)height / new_height;
    for (new_i = 1; new_i <= new_height; new_i++){
        i = new_i*ratio;
        real_i = (int)(i+0.5);//四舍五入
        //real_i = (int)(i);
        for (new_j = 1; new_j <= new_width; new_j++){
            j = new_j*ratio;
            real_j = (int)(j+0.5);//四舍五入
            //real_j = (int)(j);
            for (k = 0; k < bandcount; k++)
            {
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        Pixels(Mat, real_i-1, real_j-1, k, height, width);
            }
        }
    }
}

void Tools::Nearest_Interpolation(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width){
    //上一版本：Nearest_Interpolation1
    //修改内容：修改插值过程中边界会越界的错误
    //作者：YS
    //修改时间：2015.11.03

    //版本：1.1
    //修改内容：优化边界处理，减少函数的调用
    //作者：YS
    //修改时间：2015.12.23

    int bound=20;
    if (new_height <= bound*2 || new_width <= bound*2){
        this->Nearest_Interpolation_Base(Mat, height, width, bandcount, New_Mat, new_height, new_width);
        return;
    }

    //反变换得到的浮点坐标为(i,j)=(real_i+complex_i,real_j+complex_j)
    float ratio=(float)height / new_height;
#pragma parallel for
    for (int new_i = bound+1; new_i <= new_height-bound; new_i++){
        float i = new_i*ratio;
        int real_i = (int)(i+0.5);//四舍五入
        //real_i = (int)(i);
        for (int new_j = bound+1; new_j <= new_width-bound; new_j++){
            float j = new_j*ratio;
            int real_j = (int)(j+0.5);//四舍五入
            //real_j = (int)(j);
            for (int k = 0; k < bandcount; k++)
            {
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        DATA3D(Mat, real_i-1, real_j-1, k, height, width);
            }
        }
    }

    //边界处理
#pragma parallel for
    for (int new_i=1;new_i<=bound;new_i++){
        float i = new_i*ratio;
        int real_i = (int)(i+0.5);//四舍五入
        //real_i = (int)(i);
        for(int new_j=1;new_j<=new_width;new_j++){
            float j = new_j*ratio;
            int real_j = (int)(j+0.5);//四舍五入
            //real_j = (int)(j);
            for (int k = 0; k < bandcount; k++)
            {
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        Pixels(Mat, real_i-1, real_j-1, k, height, width);
            }
        }
    }

#pragma parallel for
    for (int new_i=new_height-bound;new_i<=new_height;new_i++){
        float i = new_i*ratio;
        int real_i = (int)(i+0.5);//四舍五入
        //real_i = (int)(i);
        for(int new_j=1;new_j<=new_width;new_j++){
            float j = new_j*ratio;
            int real_j = (int)(j+0.5);//四舍五入
            //real_j = (int)(j);
            for (int k = 0; k < bandcount; k++)
            {
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        Pixels(Mat, real_i-1, real_j-1, k, height, width);
            }
        }
    }

#pragma parallel for
    for (int new_i=bound+1;new_i<=new_height-bound;new_i++){
        float i = new_i*ratio;
        int real_i = (int)(i+0.5);//四舍五入
        //real_i = (int)(i);
        for(int new_j=1;new_j<=bound;new_j++){
            float j = new_j*ratio;
            int real_j = (int)(j+0.5);//四舍五入
            //real_j = (int)(j);
            for (int k = 0; k < bandcount; k++)
            {
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        Pixels(Mat, real_i-1, real_j-1, k, height, width);
            }
        }
    }

#pragma parallel for
    for (int new_i=bound+1;new_i<=new_height-bound;new_i++){
        float i = new_i*ratio;
        int real_i = (int)(i+0.5);//四舍五入
        //real_i = (int)(i);
        for(int new_j=new_width-bound;new_j<=new_width;new_j++){
            float j = new_j*ratio;
            int real_j = (int)(j+0.5);//四舍五入
            //real_j = (int)(j);
            for (int k = 0; k < bandcount; k++)
            {
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        Pixels(Mat, real_i-1, real_j-1, k, height, width);
            }
        }
    }

}

//双线性插值（只适用于放大图像）
void Tools::Linear_Interpolation_Base(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width){

    //版本：
    //修改内容：修改插值过程中边界会越界的错误
    //作者：YS
    //修改时间：2015.11.03

    //反变换得到的浮点坐标为(i,j)=(real_i+complex_i,real_j+complex_j)

    float ratio=(float)height / new_height;

#pragma parallel for
    for (int new_i = 1; new_i <= new_height; new_i++){
        float i = new_i*ratio;
        int real_i = (int)i;
        float complex_i = i - real_i;
        for (int new_j = 1; new_j <= new_width; new_j++){
            float j = new_j*ratio;
            int real_j = (int)j;
            float complex_j = j - real_j;
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        (1 - complex_i)*(1 - complex_j)*Pixels(Mat, real_i-1, real_j-1, k, height, width)\
                        + (1 - complex_i)*complex_j*Pixels(Mat, real_i-1, real_j-1 + 1, k, height, width)\
                        + complex_i*(1 - complex_j)*Pixels(Mat, real_i-1 + 1, real_j-1, k, height, width)\
                        + complex_i*complex_j*Pixels(Mat, real_i-1+1, real_j-1+1, k, height, width);
            }
        }
    }

}

void Tools::Linear_Interpolation(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width){

    //上一版本：Linear_Interpolation1
    //修改内容：修改插值过程中边界会越界的错误
    //作者：YS
    //修改时间：2015.11.03

    //版本：1.1
    //修改内容：优化边界处理，减少函数的调用
    //作者：YS
    //修改时间：2015.12.25

    int bound=20;
    if (new_height <= bound*2 || new_width <= bound*2){
        this->Linear_Interpolation_Base(Mat, height, width, bandcount, New_Mat, new_height, new_width);
        return;
    }

    //反变换得到的浮点坐标为(i,j)=(real_i+complex_i,real_j+complex_j)

    float ratio=(float)height / new_height;
#pragma parallel for
    for (int new_i = bound+1; new_i <= new_height-bound; new_i++){
        float i, j;
        int real_i, real_j;//在原图位置的整数部分
        float complex_i, complex_j;//在原图位置的小数部分
        i = new_i*ratio;
        real_i = (int)i;
        complex_i = i - real_i;
        for (int new_j = bound+1; new_j <= new_width-bound; new_j++){
            j = new_j*ratio;
            real_j = (int)j;
            complex_j = j - real_j;
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        (1 - complex_i)*(1 - complex_j)*DATA3D(Mat, real_i-1, real_j-1, k, height, width)\
                        + (1 - complex_i)*complex_j*DATA3D(Mat, real_i-1, real_j-1 + 1, k, height, width)\
                        + complex_i*(1 - complex_j)*DATA3D(Mat, real_i-1 + 1, real_j-1, k, height, width)\
                        + complex_i*complex_j*DATA3D(Mat, real_i-1+1, real_j-1+1, k, height, width);
            }
        }
    }

    //边界处理
#pragma parallel for
    for (int new_i=1;new_i<=bound;new_i++){
        float i, j;
        int real_i, real_j;//在原图位置的整数部分
        float complex_i, complex_j;//在原图位置的小数部分
        i = new_i*ratio;
        real_i = (int)i;
        complex_i = i - real_i;
        for(int new_j=1;new_j<=new_width;new_j++){
            j = new_j*ratio;
            real_j = (int)j;
            complex_j = j - real_j;
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        (1 - complex_i)*(1 - complex_j)*Pixels(Mat, real_i-1, real_j-1, k, height, width)\
                        + (1 - complex_i)*complex_j*Pixels(Mat, real_i-1, real_j-1 + 1, k, height, width)\
                        + complex_i*(1 - complex_j)*Pixels(Mat, real_i-1 + 1, real_j-1, k, height, width)\
                        + complex_i*complex_j*Pixels(Mat, real_i-1+1, real_j-1+1, k, height, width);
            }
        }
    }

#pragma parallel for
    for (int new_i=new_height-bound;new_i<=new_height;new_i++){
        float i, j;
        int real_i, real_j;//在原图位置的整数部分
        float complex_i, complex_j;//在原图位置的小数部分
        i = new_i*ratio;
        real_i = (int)i;
        complex_i = i - real_i;
        for(int new_j=1;new_j<=new_width;new_j++){
            j = new_j*ratio;
            real_j = (int)j;
            complex_j = j - real_j;
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        (1 - complex_i)*(1 - complex_j)*Pixels(Mat, real_i-1, real_j-1, k, height, width)\
                        + (1 - complex_i)*complex_j*Pixels(Mat, real_i-1, real_j-1 + 1, k, height, width)\
                        + complex_i*(1 - complex_j)*Pixels(Mat, real_i-1 + 1, real_j-1, k, height, width)\
                        + complex_i*complex_j*Pixels(Mat, real_i-1+1, real_j-1+1, k, height, width);
            }
        }
    }

#pragma parallel for
    for (int new_i=bound+1;new_i<=new_height-bound;new_i++){
        float i, j;
        int real_i, real_j;//在原图位置的整数部分
        float complex_i, complex_j;//在原图位置的小数部分
        i = new_i*ratio;
        real_i = (int)i;
        complex_i = i - real_i;
        for(int new_j=1;new_j<=bound;new_j++){
            j = new_j*ratio;
            real_j = (int)j;
            complex_j = j - real_j;
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        (1 - complex_i)*(1 - complex_j)*Pixels(Mat, real_i-1, real_j-1, k, height, width)\
                        + (1 - complex_i)*complex_j*Pixels(Mat, real_i-1, real_j-1 + 1, k, height, width)\
                        + complex_i*(1 - complex_j)*Pixels(Mat, real_i-1 + 1, real_j-1, k, height, width)\
                        + complex_i*complex_j*Pixels(Mat, real_i-1+1, real_j-1+1, k, height, width);
            }
        }
    }

#pragma parallel for
    for (int new_i=bound+1;new_i<=new_height-bound;new_i++){
        float i, j;
        int real_i, real_j;//在原图位置的整数部分
        float complex_i, complex_j;//在原图位置的小数部分
        i = new_i*ratio;
        real_i = (int)i;
        complex_i = i - real_i;
        for(int new_j=new_width-bound;new_j<=new_width;new_j++){
            j = new_j*ratio;
            real_j = (int)j;
            complex_j = j - real_j;
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        (1 - complex_i)*(1 - complex_j)*Pixels(Mat, real_i-1, real_j-1, k, height, width)\
                        + (1 - complex_i)*complex_j*Pixels(Mat, real_i-1, real_j-1 + 1, k, height, width)\
                        + complex_i*(1 - complex_j)*Pixels(Mat, real_i-1 + 1, real_j-1, k, height, width)\
                        + complex_i*complex_j*Pixels(Mat, real_i-1+1, real_j-1+1, k, height, width);
            }
        }
    }
}

//三次卷积插值（只适用于放大图像）
void Tools::CubicConv_Interpolation_Base(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width){

    //版本：1.0
    //修改内容：修改插值过程中边界会越界的错误
    //作者：YS
    //修改时间：2015.11.03

    //修改内容：截断
    //修改时间：2016.3.7

    //反变换得到的浮点坐标为(i,j)=(real_i+complex_i,real_j+complex_j)

    float ratio=(float)height / new_height;
#pragma parallel for
    for (int new_i = 1; new_i <= new_height; new_i++){
        float U[4]={0};
        float V[4]={0};
        int real_i, real_j;//在原图位置的整数部分
        float i, j;
        i = new_i*ratio;
        real_i = (int)i;
        float complex_i = i - real_i;
        for (int new_j = 1; new_j <= new_width; new_j++){
            j = new_j*ratio;
            real_j = (int)j;
            float complex_j = j - real_j;
            for (int n=0; n<4; n++)
            {
                U[n]=SinXDivX(complex_i+1-n);
                V[n]=SinXDivX(complex_j+1-n);
            }
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        V[0]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 -1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 -1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 -1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 -1 , k , height, width))\
                        + V[1]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1    , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1    , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1    , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1    , k , height, width))\
                        + V[2]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +1 , k , height, width))\
                        + V[3]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +2 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +2 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +2 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +2 , k , height, width));
                //2016.3.7
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) > 0 ? DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) :0;
            }
        }
    }
}

void Tools::CubicConv_Interpolation(float* Mat, int height, int width, int bandcount, float *New_Mat, int new_height, int new_width){

    //上一版本：1.0
    //修改内容：修改插值过程中边界会越界的错误
    //作者：YS
    //修改时间：2015.11.03

    //版本：1.1
    //修改内容：优化边界处理，减少函数的调用
    //作者：YS
    //修改时间：2015.12.25

    //修改内容：截断
    //修改时间：2016.3.7

    int bound=50;
    if (new_height <= bound*2 || new_width <= bound*2){
        this->CubicConv_Interpolation_Base(Mat, height, width, bandcount, New_Mat, new_height, new_width);
        return;
    }

    //反变换得到的浮点坐标为(i,j)=(real_i+complex_i,real_j+complex_j)

    float ratio=(float)height / new_height;

#pragma parallel for
    for (int new_i = bound+1; new_i <= new_height-bound; new_i++){
        float U[4]={0};
        float V[4]={0};
        float i = new_i*ratio;
        float complex_i, complex_j;//在原图位置的小数部分
        int real_i = (int)i;
        complex_i = i - real_i;
        for (int new_j = bound+1; new_j <= new_width-bound; new_j++){
            float j = new_j*ratio;
            int real_j = (int)j;
            complex_j = j - real_j;
            for (int n=0; n<4; n++)
            {
                U[n]=SinXDivX(complex_i+1-n);
                V[n]=SinXDivX(complex_j+1-n);
            }
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        V[0]*(U[0]*DATA3D(Mat, real_i-1 -1 , real_j-1 -1 , k , height, width) + U[1]*DATA3D(Mat, real_i-1  , real_j-1 -1 , k , height, width) + U[2]*DATA3D(Mat, real_i-1 +1 , real_j-1 -1 , k , height, width) + U[3]*DATA3D(Mat, real_i-1 +2 , real_j-1 -1 , k , height, width))\
                        + V[1]*(U[0]*DATA3D(Mat, real_i-1 -1 , real_j-1    , k , height, width) + U[1]*DATA3D(Mat, real_i-1  , real_j-1    , k , height, width) + U[2]*DATA3D(Mat, real_i-1 +1 , real_j-1    , k , height, width) + U[3]*DATA3D(Mat, real_i-1 +2 , real_j-1    , k , height, width))\
                        + V[2]*(U[0]*DATA3D(Mat, real_i-1 -1 , real_j-1 +1 , k , height, width) + U[1]*DATA3D(Mat, real_i-1  , real_j-1 +1 , k , height, width) + U[2]*DATA3D(Mat, real_i-1 +1 , real_j-1 +1 , k , height, width) + U[3]*DATA3D(Mat, real_i-1 +2 , real_j-1 +1 , k , height, width))\
                        + V[3]*(U[0]*DATA3D(Mat, real_i-1 -1 , real_j-1 +2 , k , height, width) + U[1]*DATA3D(Mat, real_i-1  , real_j-1 +2 , k , height, width) + U[2]*DATA3D(Mat, real_i-1 +1 , real_j-1 +2 , k , height, width) + U[3]*DATA3D(Mat, real_i-1 +2 , real_j-1 +2 , k , height, width));
                //2016.3.7
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) > 0 ? DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) :0;

            }
        }
    }

    //边界处理
#pragma parallel for
    for (int new_i=1;new_i<=bound;new_i++){
        float U[4]={0};
        float V[4]={0};
        float i = new_i*ratio;
        int real_i = (int)i;
        float complex_i = i - real_i;
        for(int new_j=1;new_j<=new_width;new_j++){
            float j = new_j*ratio;
            int real_j = (int)j;
            float complex_j = j - real_j;
            for (int n=0; n<4; n++)
            {
                U[n]=SinXDivX(complex_i+1-n);
                V[n]=SinXDivX(complex_j+1-n);
            }
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        V[0]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 -1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 -1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 -1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 -1 , k , height, width))\
                        + V[1]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1    , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1    , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1    , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1    , k , height, width))\
                        + V[2]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +1 , k , height, width))\
                        + V[3]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +2 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +2 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +2 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +2 , k , height, width));
                //2016.3.7
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) > 0 ? DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) :0;

            }
        }
    }

#pragma parallel for
    for (int new_i=new_height-bound;new_i<=new_height;new_i++){
        float U[4]={0};
        float V[4]={0};
        float i = new_i*ratio;
        int real_i = (int)i;
        float complex_i = i - real_i;
        for(int new_j=1;new_j<=new_width;new_j++){
            float j = new_j*ratio;
            int real_j = (int)j;
            float complex_j = j - real_j;
            for (int n=0; n<4; n++)
            {
                U[n]=SinXDivX(complex_i+1-n);
                V[n]=SinXDivX(complex_j+1-n);
            }
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        V[0]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 -1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 -1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 -1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 -1 , k , height, width))\
                        + V[1]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1    , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1    , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1    , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1    , k , height, width))\
                        + V[2]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +1 , k , height, width))\
                        + V[3]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +2 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +2 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +2 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +2 , k , height, width));
                //2016.3.7
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) > 0 ? DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) :0;

            }
        }
    }

#pragma parallel for
    for (int new_i=bound+1;new_i<=new_height-bound;new_i++){
        float U[4]={0};
        float V[4]={0};
        float i = new_i*ratio;
        int real_i = (int)i;
        float complex_i = i - real_i;
        for(int new_j=1;new_j<=bound;new_j++){
            float j = new_j*ratio;
            int real_j = (int)j;
            float complex_j = j - real_j;
            for (int n=0; n<4; n++)
            {
                U[n]=SinXDivX(complex_i+1-n);
                V[n]=SinXDivX(complex_j+1-n);
            }
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        V[0]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 -1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 -1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 -1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 -1 , k , height, width))\
                        + V[1]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1    , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1    , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1    , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1    , k , height, width))\
                        + V[2]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +1 , k , height, width))\
                        + V[3]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +2 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +2 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +2 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +2 , k , height, width));

                //2016.3.7
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) > 0 ? DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) :0;

            }
        }
    }

#pragma parallel for
    for (int new_i=bound+1;new_i<=new_height-bound;new_i++){
        float U[4]={0};
        float V[4]={0};
        float i = new_i*ratio;
        int real_i = (int)i;
        float complex_i = i - real_i;
        for(int new_j=new_width-bound;new_j<=new_width;new_j++){
            float j = new_j*ratio;
            int real_j = (int)j;
            float complex_j = j - real_j;
            for (int n=0; n<4; n++)
            {
                U[n]=SinXDivX(complex_i+1-n);
                V[n]=SinXDivX(complex_j+1-n);
            }
            for (int k = 0; k < bandcount; k++)
            {
                //插值公式
                //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1)

                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = \
                        V[0]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 -1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 -1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 -1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 -1 , k , height, width))\
                        + V[1]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1    , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1    , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1    , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1    , k , height, width))\
                        + V[2]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +1 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +1 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +1 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +1 , k , height, width))\
                        + V[3]*(U[0]*Pixels(Mat, real_i-1 -1 , real_j-1 +2 , k , height, width) + U[1]*Pixels(Mat, real_i-1  , real_j-1 +2 , k , height, width) + U[2]*Pixels(Mat, real_i-1 +1 , real_j-1 +2 , k , height, width) + U[3]*Pixels(Mat, real_i-1 +2 , real_j-1 +2 , k , height, width));
                //2016.3.7
                DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) = DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) > 0 ? DATA3D(New_Mat, new_i-1, new_j-1, k, new_height, new_width) :0;

            }
        }
    }
}

//插值（只适用于放大图像）
void Tools::Interpolation(float* Mat, int height, int width, int bandcount, float *NewMat, int new_height, int new_width,int method){

    switch (method)
    {
    //case Nearest:
    //	Nearest_Interpolation(Mat, height, width, bandcount,NewMat,new_height, new_width);
    //	break;
    case Linear://双线性插值
        Linear_Interpolation(Mat, height, width, bandcount,NewMat, new_height, new_width);
        break;
    case CubicConv://三次卷积插值
        CubicConv_Interpolation(Mat, height, width, bandcount,NewMat, new_height, new_width);
        break;
    default: //默认是最邻近插值
        Nearest_Interpolation(Mat, height, width, bandcount,NewMat, new_height, new_width);
        break;
    }
}

//图像直方图匹配 使Mat与Base_Mat进行匹配 返回Obj_Mat
float* Tools::HistMatch(float *Mat, float* Base_Mat, int height, int width, int flag){//flag为图像灰度级

    //数据应该保持为非负

    int *hist = new int[flag];
    int *histv = new int[flag];

    int i, j;
    float* Obj_Mat = (float*)malloc(height*width*sizeof(float));
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
        {
            hist[(int)DATA2D(Mat, i, j, width)]++;
            histv[(int)DATA2D(Base_Mat, i, j, width)]++;
        }

    int val_1 = 0;
    int val_2 = 0;
    int *T = new int[flag];
    double *S = new double[flag];
    double *G = new double[flag];
    for (int index = 0; index<flag; ++index)
    {
        //cout << hist->mat.data.db[index]<<endl;
        val_1 += hist[index];//cvQueryHistValue_1D(hist, index);
        val_2 += histv[index];

        S[index] = val_1 / (height*width);
        G[index] = val_2 / (height*width);
    }

    double min_val = 0.0;
    int PG = 0;
    for (i = 0; i<flag; ++i)
    {
        min_val = 1.0;
        for (j = 0; j<flag; ++j)
        {

            if (abs(G[j] - S[i]) - min_val< 1.0E-5)
            {
                min_val = abs(G[j] - S[i]);
                PG = j;
            }

        }
        T[i] = PG;
    }


    for (i = 0; i<height; i++)
    {
        for (j = 0; j<width; j++)
        {
            int temp = (int)DATA2D(Mat, i, j, width);
            DATA2D(Obj_Mat, i, j, width) = (float)T[temp];
        }
    }
    return Obj_Mat;
}

//求解灰度图像最大值最小值
void Tools::MaxMin(float *Img, int height, int width, float &max,float &min)//flag为图像灰度级
{
    /*
    *MaxMin 求解灰度图像的最大值和最小值
    *Img	 图像数据	按行存储的一维向量
    *height  图像高度
    *width   图像宽度
    *max     存放最大值
    *min	 存放最小值
    *作者：YS
    *时间：2015.11.04
    */
    int i;
    if (NULL == Img)
    {
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        exit(1);
    }
    max = Img[0];
    min = Img[0];
    for (i = 1; i < width*height; i++){
        if (max < Img[i])
            max = Img[i];
        else if (min>Img[i])
            min = Img[i];

    }
    if (max == min) {
        cerr<<"MaxMin Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        exit(1);
    }

}

//求解灰度图像均值和方差
void Tools::MeanStd(float *Img, int height, int width, float &std,float &mean)
{
    /*
    *MaxMin 求解灰度图像的均值和方差
    *Img	 图像数据	按行存储的一维向量
    *height  图像高度
    *width   图像宽度
    *mean    图像均值
    *std	 图像标准差
    *作者：YS
    *时间：2015.11.04
    */
    int i;
    double sum=0;

    for (i = 0; i < width*height; i++){
        sum += Img[i];
    }

    mean = sum/(width*height);
    //printf("%f\n",mean);
    sum = 0;
    for (i = 0; i < width*height; i++)
        sum += ((Img[i]-mean)*(Img[i]-mean));

    std = sqrt(sum/width*height);
}

//图像分块
void Tools::ImageBlock(int ImageHeight,int ImageWidth,int &BlockHeight,int &BlockWidth){
    /*
    *ImageBlock   对多光谱图像分块
    *ImageHeight  多光谱图像高度
    *ImageWidth	  多光谱图像宽度
    *BlockHeight  分块图像高度
    *BlockWidth   分块图像宽度
    *作者：YS
    *时间：2015.12.23
    */

    /*
    *待改进：根据分辨率比值选择合适的块宽、块高
    */
    int size = 100000;//每块大小
    if (ImageWidth <= 0 || ImageHeight <= 0 || ImageWidth>size )
    {

        cerr<<"Block Error.\n";
        cerr<<"file："<<__FILE__<<"line："<<__LINE__<<"time："<<__DATE__<<" "<<__TIME__<<endl;
        exit(1);
    }
    BlockWidth = ImageWidth;


    BlockHeight =  size/ImageWidth;
    if (BlockHeight > ImageHeight)
    {
        BlockHeight = ImageHeight;
    }

}

//写日志 NULL
void Tools::Log(const char* LogName,const char* str) {
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    cout << str << "|"<<asctime(timenow);
}
