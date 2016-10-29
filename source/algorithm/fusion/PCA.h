/********************************************************************
    purpose:	PCA类声明
                PCA变换与反变换
*********************************************************************/
#ifndef _PCA_H_
#define _PCA_H_

#include <math.h>
#include <iostream>
using namespace std;

#define EPS    0.000001   /* 计算精度 */
#define Iteration   60   /* 求取特征量的最多迭代次数 */

class PCA{
public:
    /*
    用豪斯荷尔德（Householder）变换将n阶实对称矩阵约化为对称三对角阵
    徐士良. 常用算法程序集（C语言描述），第3版. 清华大学出版社. 2004

    float a[] --- 维数为nxn。存放n阶实对称矩阵A
    int n      --- 实对称矩阵A的阶数
    float q[] --- 维数nxn。返回Householder变换的乘积矩阵Q。当与
    Tri_Symmetry_Diagonal_Eigenvector()函数联用，若将
    Q矩阵作为该函数的一个参数时，可计算实对称阵A的特征
    值与相应的特征向量
    float b[] --- 维数为n。返回对称三对角阵中的主对角线元素
    float c[] --- 长度n。前n－1个元素返回对称三对角阵中的次对角线元素。
    */
    void Householder_Tri_Symetry_Diagonal(float a[], int n, float q[],float b[], float c[]);

    /*
    计算实对称三对角阵的全部特征值与对应特征向量
    徐士良. 常用算法程序集（C语言描述），第3版. 清华大学出版社. 2004

    int n    --- 实对称三对角阵的阶数
    float b --- 长度为n，存放n阶对称三对角阵的主对角线上的各元素；
    返回时存放全部特征值
    float c --- 长度为n，前n－1个元素存放n阶对称三对角阵的次对角
    线上的元素
    float q --- 维数为nxn，若存放单位矩阵，则返回n阶实对称三对角
    阵T的特征向量组；若存放Householder_Tri_Symetry_Diagonal()
    函数所返回的一般实对称矩阵A的豪斯荷尔得变换的乘
    积矩阵Q，则返回实对称矩阵A的特征向量组。其中，q中
    的的j列为与数组b中第j个特征值对应的特征向量
    float eps --- 本函数在迭代过程中的控制精度要求
    int l    --- 为求得一个特征值所允许的最大迭代次数
    返回值：
    若返回标记小于0，则说明迭代了l次还未求得一个特征值，并有fail
    信息输出；若返回标记大于0，则说明程序工作正常，全部特征值由一
    维数组b给出，特征向量组由二维数组q给出
    */
    int Tri_Symmetry_Diagonal_Eigenvector(int n, float b[], float c[],float q[], float eps, int l);

    /*
    计算实对称阵的全部特征值与对应特征向量
    int n              --- 实对称阵的阶数
    float * CovMatrix --- 维数为nxn，存放n阶对称阵的各元素；
    float * Eigen     --- 长度为n，为n个特征值
    float * EigenVector --- 维数为nxn，返回n阶实对称阵的特征向量组其中，
    EigenVector中的j列为与数组Eigen中第j个特征值
    对应的特征向量
    返回值：
    若返回标记小于0，则说明迭代了Iteration次还未求得一个特征值；
    若返回标记大于0，则说明程序工作正常，全部特征值由一
    维数组Eigen给出，特征向量组由二维数组EigenVector给出
    */
    int SymmetricRealMatrix_Eigen(float CovMatrix[], int n,float Eigen[], float EigenVector[]);

    /*
    PCA: Perform PCA using covariance.
    data     --- MxN matrix of input data ( M dimensions, N trials )
    行数为原始数据维数；每列数据为一个样本
    signals  --- MxN matrix of projected data
    PC       --- each column is a PC
    V        --- Mx1 matrix of variances
    row = M dimensions, col = N trials
    */
    int PCAProject(float * data, int row, int col, float * mean, float * signals, float * PC, float * V);
    int PCAReproject(float * signal, int row, int col, float * mean, float * PC, float * newdata);

    /*
    *数据本身做PCA正反变换 节省内存
    *日期：2015.12.31
    *作者：YS
    */
    int PCAProject(float * data, int row, int col, float * mean, float * PC, float * V);
    int PCAReproject(float * signal, int row, int col, float * mean, float * PC);
};
#endif


