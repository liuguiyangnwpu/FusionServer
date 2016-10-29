#ifndef _WT_H_
#define _WT_H_
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
//#include <malloc.h>

#ifndef DATA2D
#define DATA2D(Matrix,m,n,SizeCol) Matrix[(m)*(SizeCol)+(n)]
#endif

#define WAVE_DB3 1
#define WAVE_BIOR3_7 2

#define ExtT_ZPD 1
#define ExtT_SYM 2
#define ExtT_PPD 3

#define CONVT_FULL 1
#define CONVT_SAME 2
#define CONVT_VALID 3

#define DIR_ROW 1
#define DIR_COL 2

#define DYAD_EVEN 1
#define DYAD_ODD 2


/* 小波变换数据描述结构体 */

/* 矩阵描述结构体 */
typedef struct dbDataPanel2D {
    float * m_pData2D;			// 指向存放元素的空间
    int m_nSizeRow;	// 行数
    int m_nSizeCol;	// 列数
} DBDataPanel2D;

typedef struct intDataPanel2D {
    int * m_pData2D;
    int m_nSizeRow;
    int m_nSizeCol;
} IntDataPanel2D;

/* 小波变换信息描述结构体 */
typedef struct wtInfo {
    int m_nWaveType;	// 小波类型
    int m_nExtType;		// 延拓方式
    int m_nWTLevel;		// 变换级数
    float * m_pC;		// 变换结果，小波系数，[An, Hn, Vn, Dn, H(n-1), V(n-1), D(n-1)..., H1, V1, D1]
    int * m_pS;			// 变换结果描述，一个2*n数组，其中n = m_nWTLevel+2；
} WTInfo;

/* 小波变换结果描述结构体 */
typedef struct WTCoefSet{
    DBDataPanel2D * A_LL;		// 低通分量
    DBDataPanel2D * H_LH;		// 水平分量
    DBDataPanel2D * V_HL;		// 竖直分量
    DBDataPanel2D * D_HH;		// 对角分量
} WTCOEFSet;

/* 小波基常量描述结构体 */
typedef struct waveletBase {
    int nFilterLen;
    float * LO_D;
    float * HI_D;
    float * LO_R;
    float * HI_R;
}WaveletBASE;

/* db3小波滤波器 */
float LO_D_db3[6] = { 0.0352, -0.0854, -0.1350, 0.4599, 0.8069, 0.3327 };
float HI_D_db3[6] = { -0.3327, 0.8069, -0.4599, -0.1350, 0.0854, 0.0352 };
float LO_R_db3[6] = { 0.3327, 0.8069, 0.4599, -0.1350, -0.0854, 0.0352 };
float HI_R_db3[6] = { 0.0352, 0.0854, -0.1350, -0.4599, 0.8069, -0.3327 };
const WaveletBASE WAVEdb3 = { 6, LO_D_db3, HI_D_db3, LO_R_db3, HI_R_db3 };

/* bior3.7小波滤波器 */
float LO_D_bior3_7[] = { 0.0030, -0.0091, -0.0168, 0.0747, 0.0313, -0.3012, -0.0265, 0.9516, 0.9516, -0.0265, -0.3012, 0.0313, 0.0747, -0.0168, -0.0091, 0.0030 };
float HI_D_bior3_7[] = { 0, 0, 0, 0, 0, 0, -0.1768, 0.5303, -0.5303, 0.1768, 0, 0, 0, 0, 0, 0 };
float LO_R_bior3_7[] = { 0, 0, 0, 0, 0, 0, 0.1768, 0.5303, 0.5303, 0.1768, 0, 0, 0, 0, 0, 0 };
float HI_R_bior3_7[] = { 0.0030, 0.0091, -0.0168, -0.0747, 0.0313, 0.3012, -0.0265, -0.9516, 0.9516, 0.0265, -0.3012, -0.0313, 0.0747, 0.0168, -0.0091, -0.0030 };
const WaveletBASE WAVEbior3_7 = { 16, LO_D_bior3_7, HI_D_bior3_7, LO_R_bior3_7, HI_R_bior3_7 };


/* 函数声明 */
float * SerialReverse(float * pSrcSerial, int nSerialLen);


/* 结构体内存管理相关函数 */
void DDP_FREE(DBDataPanel2D * pDBDataPanel2D)
{
    if (pDBDataPanel2D != NULL) {
        if (pDBDataPanel2D->m_pData2D != NULL)
            free(pDBDataPanel2D->m_pData2D);
        free(pDBDataPanel2D);
    }
}

void IDP_FREE(IntDataPanel2D * pIntDataPanel2D)
{
    if (pIntDataPanel2D != NULL) {
        if (pIntDataPanel2D->m_pData2D != NULL)
            free(pIntDataPanel2D->m_pData2D);
        free(pIntDataPanel2D);
    }
}

void CS_FREE(WTCOEFSet * pWTCoefSet)
{
    if (pWTCoefSet != NULL) {
        if (pWTCoefSet->A_LL != NULL)
            free(pWTCoefSet->A_LL);
        if (pWTCoefSet->H_LH != NULL)
            free(pWTCoefSet->H_LH);
        if (pWTCoefSet->V_HL != NULL)
            free(pWTCoefSet->V_HL);
        if (pWTCoefSet->D_HH != NULL)
            free(pWTCoefSet->D_HH);
        free(pWTCoefSet);
    }
}

void WB_FREE(WaveletBASE * pWaveletBase)
{
    if (pWaveletBase != NULL) {
        if (pWaveletBase->LO_D != NULL)
            free(pWaveletBase->LO_D);
        if (pWaveletBase->HI_D != NULL)
            free(pWaveletBase->HI_D);
        if (pWaveletBase->LO_R != NULL)
            free(pWaveletBase->LO_R);
        if (pWaveletBase->HI_D != NULL)
            free(pWaveletBase->HI_R);
        free(pWaveletBase);
    }
}

void WI_FREE(WTInfo * pWTInfo)
{
    if (pWTInfo != NULL) {
        if (pWTInfo->m_pC != NULL)
            free(pWTInfo->m_pC);
        if (pWTInfo->m_pS != NULL)
            free(pWTInfo->m_pS);
        free(pWTInfo);
    }
}


/* 小波变换相关函数 */

WaveletBASE * SetWaveletBase(int nWaveType)
{
    WaveletBASE * pWaveletBase = NULL;

    pWaveletBase = (WaveletBASE *)calloc(1, sizeof(WaveletBASE));
    if (!pWaveletBase)
    {
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    switch (nWaveType) {
    case WAVE_DB3:
        pWaveletBase->nFilterLen = WAVEdb3.nFilterLen;
        pWaveletBase->LO_D = SerialReverse(WAVEdb3.LO_D, WAVEdb3.nFilterLen);
        pWaveletBase->HI_D = SerialReverse(WAVEdb3.HI_D, WAVEdb3.nFilterLen);
        pWaveletBase->LO_R = SerialReverse(WAVEdb3.LO_R, WAVEdb3.nFilterLen);
        pWaveletBase->HI_R = SerialReverse(WAVEdb3.HI_R, WAVEdb3.nFilterLen);
        break;
    case WAVE_BIOR3_7:
        pWaveletBase->nFilterLen = WAVEbior3_7.nFilterLen;
        pWaveletBase->LO_D = SerialReverse(WAVEbior3_7.LO_D, WAVEbior3_7.nFilterLen);
        pWaveletBase->HI_D = SerialReverse(WAVEbior3_7.HI_D, WAVEbior3_7.nFilterLen);
        pWaveletBase->LO_R = SerialReverse(WAVEbior3_7.LO_R, WAVEbior3_7.nFilterLen);
        pWaveletBase->HI_R = SerialReverse(WAVEbior3_7.HI_R, WAVEbior3_7.nFilterLen);
        break;
    default:
        pWaveletBase->nFilterLen = WAVEdb3.nFilterLen;
        pWaveletBase->LO_D = SerialReverse(WAVEdb3.LO_D, WAVEdb3.nFilterLen);
        pWaveletBase->HI_D = SerialReverse(WAVEdb3.HI_D, WAVEdb3.nFilterLen);
        pWaveletBase->LO_R = SerialReverse(WAVEdb3.LO_R, WAVEdb3.nFilterLen);
        pWaveletBase->HI_R = SerialReverse(WAVEdb3.HI_R, WAVEdb3.nFilterLen);
    }
    return pWaveletBase;
}


/* 延拓 */
DBDataPanel2D * WExtend2D(DBDataPanel2D * pSrcData, int nExtType, int nExtSizeRow, int nExtSizeCol)
{
    int i, j;
    DBDataPanel2D * pResultData = NULL;

    pResultData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D));
    if (!pResultData)
    {
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";

    }
    pResultData->m_nSizeRow = pSrcData->m_nSizeRow + nExtSizeRow * 2;
    pResultData->m_nSizeCol = pSrcData->m_nSizeCol + nExtSizeCol * 2;
    pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
    if (!pResultData->m_pData2D) {
        DDP_FREE(pResultData);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    for (i = 0; i < pSrcData->m_nSizeRow; i++)
        for (j = 0; j < pSrcData->m_nSizeCol; j++)
            DATA2D(pResultData->m_pData2D, i + nExtSizeRow, j + nExtSizeCol, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i, j, pSrcData->m_nSizeCol);

    switch (nExtType) {
    case ExtT_ZPD:	// 零延拓
        for (i = 0; i < nExtSizeRow; i++)
            for (j = 0; j < nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = 0;
                DATA2D(pResultData->m_pData2D, i, pResultData->m_nSizeCol - j - 1, pResultData->m_nSizeCol) = 0;
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - i - 1, j, pResultData->m_nSizeCol) = 0;
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - i - 1, pResultData->m_nSizeCol - j - 1, pResultData->m_nSizeCol) = 0;
            }
        for (i = 0; i < nExtSizeRow; i++)
            for (j = nExtSizeCol; j < pResultData->m_nSizeCol - nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = 0;
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - 1 - i, j, pResultData->m_nSizeCol) = 0;
            }
        for (i = nExtSizeRow; i < pResultData->m_nSizeRow - nExtSizeRow; i++)
            for (j = 0; j < nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = 0;
                DATA2D(pResultData->m_pData2D, i, pResultData->m_nSizeCol - 1 - j, pResultData->m_nSizeCol) = 0;
            }
        break;
    case ExtT_SYM:	// 对称延拓
        for (i = 0; i < nExtSizeRow; i++)
            for (j = 0; j < nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nExtSizeRow - i - 1, nExtSizeCol - j - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, i, pResultData->m_nSizeCol - j - 1, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nExtSizeRow - i - 1, pSrcData->m_nSizeCol - (nExtSizeCol - j - 1) - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - i - 1, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, pSrcData->m_nSizeRow - (nExtSizeRow - i - 1) - 1, nExtSizeCol - j - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - i - 1, pResultData->m_nSizeCol - j - 1, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, pSrcData->m_nSizeRow - (nExtSizeRow - i - 1) - 1, pSrcData->m_nSizeCol - (nExtSizeCol - j - 1) - 1, pSrcData->m_nSizeCol);
            }
        for (i = 0; i < nExtSizeRow; i++)
            for (j = nExtSizeCol; j < pResultData->m_nSizeCol - nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nExtSizeRow - i - 1, j - nExtSizeCol, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - 1 - i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, pSrcData->m_nSizeRow - (nExtSizeRow - i), j - nExtSizeCol, pSrcData->m_nSizeCol);
            }
        for (i = nExtSizeRow; i < pResultData->m_nSizeRow - nExtSizeRow; i++)
            for (j = 0; j < nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i - nExtSizeRow, nExtSizeCol - j - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, i, pResultData->m_nSizeCol - 1 - j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i - nExtSizeRow, pSrcData->m_nSizeCol - (nExtSizeCol - j), pSrcData->m_nSizeCol);
            }
        break;
    case ExtT_PPD:	// 周期延拓
        for (i = 0; i < nExtSizeRow; i++)
            for (j = 0; j < nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, pSrcData->m_nSizeRow - (nExtSizeRow - i - 1) - 1, pSrcData->m_nSizeCol - (nExtSizeCol - j - 1) - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, i, pResultData->m_nSizeCol - j - 1, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, pSrcData->m_nSizeRow - (nExtSizeRow - i - 1) - 1, nExtSizeCol - j - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - i - 1, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nExtSizeRow - i - 1, pSrcData->m_nSizeCol - (nExtSizeCol - j - 1) - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - i - 1, pResultData->m_nSizeCol - j - 1, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nExtSizeRow - i - 1, nExtSizeCol - j - 1, pSrcData->m_nSizeCol);
            }
        for (i = 0; i < nExtSizeRow; i++)
            for (j = nExtSizeCol; j < pResultData->m_nSizeCol - nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, pSrcData->m_nSizeRow - (nExtSizeRow - i), j - nExtSizeCol, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - 1 - i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nExtSizeRow - i - 1, j - nExtSizeCol, pSrcData->m_nSizeCol);
            }
        for (i = nExtSizeRow; i < pResultData->m_nSizeRow - nExtSizeRow; i++)
            for (j = 0; j < nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i - nExtSizeRow, pSrcData->m_nSizeCol - (nExtSizeCol - j), pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, i, pResultData->m_nSizeCol - 1 - j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i - nExtSizeRow, nExtSizeCol - j - 1, pSrcData->m_nSizeCol);
            }
        break;
    default:	// 默认对称延拓
        for (i = 0; i < nExtSizeRow; i++)
            for (j = 0; j < nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nExtSizeRow - i - 1, nExtSizeCol - j - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, i, pResultData->m_nSizeCol - j - 1, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nExtSizeRow - i - 1, pSrcData->m_nSizeCol - (nExtSizeCol - j - 1) - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - i - 1, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, pSrcData->m_nSizeRow - (nExtSizeRow - i - 1) - 1, nExtSizeCol - j - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - i - 1, pResultData->m_nSizeCol - j - 1, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, pSrcData->m_nSizeRow - (nExtSizeRow - i - 1) - 1, pSrcData->m_nSizeCol - (nExtSizeCol - j - 1) - 1, pSrcData->m_nSizeCol);
            }
        for (i = 0; i < nExtSizeRow; i++)
            for (j = nExtSizeCol; j < pResultData->m_nSizeCol - nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nExtSizeRow - i - 1, j - nExtSizeCol, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, pResultData->m_nSizeRow - 1 - i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, pSrcData->m_nSizeRow - (nExtSizeRow - i), j - nExtSizeCol, pSrcData->m_nSizeCol);
            }
        for (i = nExtSizeRow; i < pResultData->m_nSizeRow - nExtSizeRow; i++)
            for (j = 0; j < nExtSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i - nExtSizeRow, nExtSizeCol - j - 1, pSrcData->m_nSizeCol);
                DATA2D(pResultData->m_pData2D, i, pResultData->m_nSizeCol - 1 - j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i - nExtSizeRow, pSrcData->m_nSizeCol - (nExtSizeCol - j), pSrcData->m_nSizeCol);
            }
    }

    return pResultData;
}


/* 序列倒转 */
float * SerialReverse(float * pSrcSerial, int nSerialLen)
{
    int i;
    float * pResult = NULL;

    pResult = (float *)calloc(nSerialLen, sizeof(float));
    if (!pResult){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    for (i = 0; i < nSerialLen; i++)
        pResult[i] = pSrcSerial[nSerialLen - i - 1];
    return pResult;
}

/* 一维卷积 */
DBDataPanel2D * CONV(float * pSerialA, float * pSerialB, int nLengthA, int nLengthB, int nConvType)
{
    int i, j, k;
    int nLengthC;
    int nLenTemp;
    float * pSerialTemp = NULL;
    float * pResultTemp = NULL;
    DBDataPanel2D * pResultData = NULL;

    pResultData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D));
    if (!pResultData){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    /* 计算卷积结果的最大长度、延拓序列A的长度 */
    nLengthC = nLengthA + nLengthB - 1;
    nLenTemp = nLengthA + nLengthB * 2 - 2;

    pSerialTemp = (float *)calloc(nLenTemp, sizeof(float));
    pResultTemp = (float *)calloc(nLengthC, sizeof(float));
    if (!pResultTemp || !pSerialTemp) {
        if (!pSerialTemp)	free(pSerialTemp);
        if (!pResultTemp)	free(pResultTemp);
        DDP_FREE(pResultData);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    pResultData->m_nSizeRow = 1;
    if (nConvType == CONVT_SAME)
        pResultData->m_nSizeCol = nLengthA;
    else
        pResultData->m_nSizeCol = nLengthC;
    pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeCol, sizeof(float));
    if (!pResultData->m_pData2D) {
        if (!pSerialTemp)	free(pSerialTemp);
        if (!pResultTemp)	free(pResultTemp);
        DDP_FREE(pResultData);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    /* 延拓序列A并赋给pSerialTemp */
    for (i = 0; i < nLengthB - 2; i++) {
        pSerialTemp[i] = 0;
        pSerialTemp[nLenTemp - i - 1] = 0;
    }
    for (i = 0; i < nLengthA; i++) {
        pSerialTemp[i + nLengthB - 1] = pSerialA[i];
    }

    /* 计算卷积 */
    for (k = 0; k < nLengthC; k++) {
        pResultTemp[k] = 0;
        for (j = 0; j < nLengthB; j++) {
            pResultTemp[k] += pSerialTemp[j + k] * pSerialB[j];
        }
    }

    switch (nConvType) {
    case CONVT_FULL:
        for (i = 0; i < nLengthC; i++)
            pResultData->m_pData2D[i] = pResultTemp[i];
        break;
    case CONVT_SAME:
        for (i = 0; i < nLengthA; i++)
            pResultData->m_pData2D[i] = pResultTemp[(nLengthC - nLengthA) / 2 + i];
        break;
    case CONVT_VALID:
        i = 0;
        while (i < nLengthC && pResultTemp[i] == 0)	i++;
        j = 0;
        while (j < nLengthC && pResultTemp[nLengthC - j - 1] == 0)	j++;
        pResultData->m_nSizeCol = nLengthC - i - j;
        for (k = 0; k < pResultData->m_nSizeCol; k++)
            pResultData->m_pData2D[k] = pResultTemp[k + i];
        break;
    default:
        for (i = 0; i < nLengthC; i++)
            pResultData->m_pData2D[i] = pResultTemp[i];
    }

    free(pSerialTemp);
    free(pResultTemp);
    return pResultData;
}


/* 二维卷积 */
DBDataPanel2D * CONV2D(DBDataPanel2D * pSrcData, float * pFilter, int nFilterLen, int nRowOrCol, int nConvType)
{
    int i, j;
    DBDataPanel2D * pResultData = NULL;
    float * pTempSerialA = NULL, *pTempSerialB = NULL;
    DBDataPanel2D * pTempSerialC = NULL;

    if (!pSrcData || !pFilter){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    pResultData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D));
    if (!pResultData){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    if (nConvType == CONVT_FULL && nRowOrCol == DIR_ROW) {
        pTempSerialA = (float *)calloc(pSrcData->m_nSizeCol, sizeof(float));
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol + nFilterLen - 1;
    }
    else if (nConvType == CONVT_FULL && nRowOrCol == DIR_COL) {
        pTempSerialA = (float *)calloc(pSrcData->m_nSizeRow, sizeof(float));
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow + nFilterLen - 1;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol;
    }
    else if (nConvType == CONVT_SAME && nRowOrCol == DIR_ROW) {
        pTempSerialA = (float *)calloc(pSrcData->m_nSizeCol, sizeof(float));
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol;
    }
    else if (nConvType == CONVT_SAME && nRowOrCol == DIR_COL) {
        pTempSerialA = (float *)calloc(pSrcData->m_nSizeRow, sizeof(float));
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol;
    }
    pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
    if (!pResultData->m_pData2D || !pTempSerialA) {
        DDP_FREE(pResultData);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }
    pTempSerialB = pFilter;

    if (nRowOrCol == DIR_ROW) {
        for (i = 0; i < pResultData->m_nSizeRow; i++) {
            for (j = 0; j < pSrcData->m_nSizeCol; j++)
                pTempSerialA[j] = DATA2D(pSrcData->m_pData2D, i, j, pSrcData->m_nSizeCol);

            pTempSerialC = CONV(pTempSerialA, pTempSerialB, pSrcData->m_nSizeCol, nFilterLen, nConvType);

            for (j = 0; j < pResultData->m_nSizeCol; j++)
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = pTempSerialC->m_pData2D[j];
            DDP_FREE(pTempSerialC);
        }
    }
    else {
        for (j = 0; j < pResultData->m_nSizeCol; j++) {
            for (i = 0; i < pSrcData->m_nSizeRow; i++)
                pTempSerialA[i] = DATA2D(pSrcData->m_pData2D, i, j, pSrcData->m_nSizeCol);
            pTempSerialC = CONV(pTempSerialA, pTempSerialB, pSrcData->m_nSizeRow, nFilterLen, nConvType);
            for (i = 0; i < pResultData->m_nSizeRow; i++)
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = pTempSerialC->m_pData2D[i];
            DDP_FREE(pTempSerialC);
        }
    }

    free(pTempSerialA);
    return pResultData;
}


/* 下抽样 */
DBDataPanel2D * DYADDOWN(DBDataPanel2D * pSrcData, int nEvenOrOdd, int nRowOrCol)
{
    int i, j;
    DBDataPanel2D * pResultData = NULL;

    pResultData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D));
    if (!pResultData){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    if (nEvenOrOdd == DYAD_EVEN && nRowOrCol == DIR_ROW) {
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow / 2;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }
        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i * 2 + 1, j, pSrcData->m_nSizeCol);
            }
    }
    else if (nEvenOrOdd == DYAD_EVEN && nRowOrCol == DIR_COL) {
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol / 2;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }
        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i, j * 2 + 1, pSrcData->m_nSizeCol);
            }
    }
    else if (nEvenOrOdd == DYAD_ODD && nRowOrCol == DIR_ROW) {
        pResultData->m_nSizeRow = (pSrcData->m_nSizeRow + 1) / 2;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }
        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i * 2, j, pSrcData->m_nSizeCol);
            }
    }
    else if (nEvenOrOdd == DYAD_ODD && nRowOrCol == DIR_COL) {
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow;
        pResultData->m_nSizeCol = (pSrcData->m_nSizeCol + 1) / 2;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }
        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i, j * 2, pSrcData->m_nSizeCol);
            }
    }
    return pResultData;
}


/* 插值 */
DBDataPanel2D * DYADUP(DBDataPanel2D * pSrcData, int nEvenOrOdd, int nRowOrCol)
{
    int i, j;
    DBDataPanel2D * pResultData = NULL;

    pResultData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D));
    if (!pResultData){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    if (nEvenOrOdd == DYAD_EVEN && nRowOrCol == DIR_ROW) {
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow * 2;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }
        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                if (i % 2 == 0)
                    DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i / 2, j, pSrcData->m_nSizeCol);
                else
                    DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = 0;
            }
    }
    else if (nEvenOrOdd == DYAD_EVEN && nRowOrCol == DIR_COL) {
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol * 2;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }
        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                if (j % 2 == 0)
                    DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i, j / 2, pSrcData->m_nSizeCol);
                else
                    DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = 0;
            }
    }
    else if (nEvenOrOdd == DYAD_ODD && nRowOrCol == DIR_ROW) {
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow * 2;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }
        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                if (i % 2 != 0)
                    DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i / 2, j, pSrcData->m_nSizeCol);
                else
                    DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = 0;
            }
    }
    else if (nEvenOrOdd == DYAD_ODD && nRowOrCol == DIR_COL) {
        pResultData->m_nSizeRow = pSrcData->m_nSizeRow;
        pResultData->m_nSizeCol = pSrcData->m_nSizeCol * 2;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }
        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                if (j % 2 != 0)
                    DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, i, j / 2, pSrcData->m_nSizeCol);
                else
                    DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = 0;
            }
    }
    return pResultData;
}


/* 矩阵重定义行列数 */
DBDataPanel2D * ReShape(DBDataPanel2D * pSrcData, int nNewSizeRow, int nNewSizeCol)
{
    int i, j, k;
    DBDataPanel2D * pResultData = NULL;

    if (nNewSizeRow*nNewSizeCol != (pSrcData->m_nSizeRow)*(pSrcData->m_nSizeCol)){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    pResultData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D));
    if (!pResultData){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    pResultData->m_nSizeRow = nNewSizeRow;
    pResultData->m_nSizeCol = nNewSizeCol;
    pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
    if (!pResultData->m_pData2D) {
        DDP_FREE(pResultData);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    k = 0;
    for (i = 0; i < pResultData->m_nSizeRow; i++)
        for (j = 0; j < pResultData->m_nSizeCol; j++) {
            DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = pSrcData->m_pData2D[k];
            k++;
        }

    return pResultData;
}


/* 截取子矩阵 */
DBDataPanel2D * GetSubMatrix(DBDataPanel2D * pSrcData, int nSubSizeRow, int nSubSizeCol)
{
    int i, j;
    int nOffsetRow, nOffsetCol;
    DBDataPanel2D * pResultData = NULL;

    if (!(nSubSizeRow <= pSrcData->m_nSizeRow && nSubSizeCol <= pSrcData->m_nSizeCol)){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    pResultData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D));
    if (!pResultData){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    pResultData->m_nSizeRow = nSubSizeRow;
    pResultData->m_nSizeCol = nSubSizeCol;
    //printf("%d %d\n",nSubSizeRow,nSubSizeCol);
    pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
    if (!pResultData->m_pData2D) {
        DDP_FREE(pResultData);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    nOffsetRow = (pSrcData->m_nSizeRow - nSubSizeRow) / 2;
    nOffsetCol = (pSrcData->m_nSizeCol - nSubSizeCol) / 2;
    //printf("%d %d\n",nOffsetRow,nOffsetCol);
    for (i = 0; i < pResultData->m_nSizeRow; i++)
        for (j = 0; j < pResultData->m_nSizeCol; j++) {
            DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcData->m_pData2D, nOffsetRow + i, nOffsetCol + j, pSrcData->m_nSizeCol);
        }

    return pResultData;
}


/* 拷贝矩阵 */
DBDataPanel2D * CopyMatrix(DBDataPanel2D * pSrcMatrix)
{
    return(GetSubMatrix(pSrcMatrix, pSrcMatrix->m_nSizeRow, pSrcMatrix->m_nSizeCol));
}


/* 拼接矩阵 */
DBDataPanel2D * SpliceMatrix(DBDataPanel2D * pSrcMatrixA, DBDataPanel2D * pSrcMatrixB, int nRowOrCol)
{
    int i, j;
    DBDataPanel2D * pResultData = NULL;

    if (!pSrcMatrixA || !pSrcMatrixB){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    pResultData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D));
    if (!pResultData){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    if (nRowOrCol == DIR_ROW) {
        if (pSrcMatrixA->m_nSizeRow != pSrcMatrixB->m_nSizeRow) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }

        pResultData->m_nSizeRow = pSrcMatrixA->m_nSizeRow;
        pResultData->m_nSizeCol = pSrcMatrixA->m_nSizeCol + pSrcMatrixB->m_nSizeCol;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }

        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pSrcMatrixA->m_nSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcMatrixA->m_pData2D, i, j, pSrcMatrixA->m_nSizeCol);
            }
        for (i = 0; i < pResultData->m_nSizeRow; i++)
            for (j = 0; j < pSrcMatrixB->m_nSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j + pSrcMatrixA->m_nSizeCol, pResultData->m_nSizeCol) = DATA2D(pSrcMatrixB->m_pData2D, i, j, pSrcMatrixB->m_nSizeCol);
            }

    }
    else {
        if (pSrcMatrixA->m_nSizeCol != pSrcMatrixB->m_nSizeCol) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }

        pResultData->m_nSizeRow = pSrcMatrixA->m_nSizeRow + pSrcMatrixB->m_nSizeRow;
        pResultData->m_nSizeCol = pSrcMatrixA->m_nSizeCol;
        pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
        if (!pResultData->m_pData2D) {
            DDP_FREE(pResultData);
            printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
            throw "DWT error";
        }

        for (i = 0; i < pSrcMatrixA->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) = DATA2D(pSrcMatrixA->m_pData2D, i, j, pSrcMatrixA->m_nSizeCol);
            }
        for (i = 0; i < pSrcMatrixB->m_nSizeRow; i++)
            for (j = 0; j < pResultData->m_nSizeCol; j++) {
                DATA2D(pResultData->m_pData2D, i + pSrcMatrixA->m_nSizeRow, j, pResultData->m_nSizeCol) = DATA2D(pSrcMatrixB->m_pData2D, i, j, pSrcMatrixB->m_nSizeCol);
            }
    }

    return pResultData;
}


/* 矩阵求和 */
DBDataPanel2D * SumMatrix(DBDataPanel2D * pSrcMatrixA, DBDataPanel2D * pSrcMatrixB)
{
    int i, j;
    DBDataPanel2D *pResultData = NULL;

    if (!pSrcMatrixA || !pSrcMatrixB){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    if ((pSrcMatrixA->m_nSizeRow != pSrcMatrixB->m_nSizeRow) ||
            (pSrcMatrixA->m_nSizeCol != pSrcMatrixB->m_nSizeCol)){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);

        throw "DWT error";
    }

    pResultData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D));
    if (!pResultData){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }
    pResultData->m_nSizeRow = pSrcMatrixA->m_nSizeRow;
    pResultData->m_nSizeCol = pSrcMatrixA->m_nSizeCol;

    pResultData->m_pData2D = (float *)calloc(pResultData->m_nSizeRow*pResultData->m_nSizeCol, sizeof(float));
    if (!pResultData->m_pData2D) {
        DDP_FREE(pResultData);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    for (i = 0; i < pResultData->m_nSizeRow; i++)
        for (j = 0; j < pResultData->m_nSizeCol; j++) {
            DATA2D(pResultData->m_pData2D, i, j, pResultData->m_nSizeCol) =
                    DATA2D(pSrcMatrixA->m_pData2D, i, j, pSrcMatrixA->m_nSizeCol)
                    + DATA2D(pSrcMatrixB->m_pData2D, i, j, pSrcMatrixB->m_nSizeCol);
        }

    return pResultData;
}




//小波变换
WTCOEFSet * SubWaveDec2(DBDataPanel2D * pSrcData, WaveletBASE * pWaveBase, int nExtType);
//WTInfo * waveDec2(DBDataPanel2D * pSrcData, int nWTLevel, int nWaveType, int nExtType);
//WTInfo * waveDec2Int(IntDataPanel2D * pSrcData, int nWTLevel, int nWaveType, int nExtType);
//
//小波反变换
DBDataPanel2D * SubWaveRec2(WTCOEFSet * pSrcWTCoef, WaveletBASE * pWaveBase);
//DBDataPanel2D * waveRec2(WTInfo * pSrcWTInfo);
//IntDataPanel2D * waveRec2Int(WTInfo * pSrcWTInfo);

//小波变换
WTInfo * waveDec2(DBDataPanel2D * pSrcData, int nWTLevel, int nWaveType, int nExtType)
{
    int i, j, k;
    int nCoefLen;
    WTInfo * pDecResult = NULL;

    DBDataPanel2D pTempCoefPanel;
    WTCOEFSet * * pWTCoefSet = NULL;
    WaveletBASE * pWaveletBase = NULL;

    /* 预备存放结果的结构体 */
    if (!(pDecResult = (WTInfo *)calloc(1, sizeof(WTInfo)))){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }
    pDecResult->m_nExtType = nExtType;
    pDecResult->m_nWaveType = nWaveType;
    pDecResult->m_nWTLevel = nWTLevel;
    pDecResult->m_pC = NULL;
    pDecResult->m_pS = NULL;

    /* 分配空间，用于存放指向小波系数的指针 */
    pWTCoefSet = (WTCOEFSet * *)calloc(nWTLevel, sizeof(WTCOEFSet *));
    if (!pWTCoefSet) {
        WI_FREE(pDecResult);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    /* 根据nWaveType获取小波基的滤波器系数【为了结合卷积运算，首先对各系数做了倒序】 */
    if (!(pWaveletBase = SetWaveletBase(nWaveType))) {
        WI_FREE(pDecResult);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }


    pTempCoefPanel.m_nSizeRow = pSrcData->m_nSizeRow;
    pTempCoefPanel.m_nSizeCol = pSrcData->m_nSizeCol;
    //printf("%d  %d\n",pTempCoefPanel.m_nSizeRow  ,pTempCoefPanel.m_nSizeCol);
    pTempCoefPanel.m_pData2D = pSrcData->m_pData2D;
    for (i = 0; i < nWTLevel; i++) {
        pWTCoefSet[i] = SubWaveDec2(&pTempCoefPanel, pWaveletBase, nExtType);
        pTempCoefPanel.m_nSizeRow = pWTCoefSet[i]->A_LL->m_nSizeRow;
        pTempCoefPanel.m_nSizeCol = pWTCoefSet[i]->A_LL->m_nSizeCol;
        //printf("%d  %d\n",pTempCoefPanel.m_nSizeRow  ,pTempCoefPanel.m_nSizeCol);
        pTempCoefPanel.m_pData2D = pWTCoefSet[i]->A_LL->m_pData2D;
    }

    /* 统计小波系数的总长度 */
    nCoefLen = 0;
    for (i = 0; i < nWTLevel; i++) {
        nCoefLen += pWTCoefSet[i]->H_LH->m_nSizeRow*pWTCoefSet[i]->H_LH->m_nSizeCol;
        nCoefLen += pWTCoefSet[i]->V_HL->m_nSizeRow*pWTCoefSet[i]->V_HL->m_nSizeCol;
        nCoefLen += pWTCoefSet[i]->D_HH->m_nSizeRow*pWTCoefSet[i]->D_HH->m_nSizeCol;
        if (i == nWTLevel - 1)
            nCoefLen += pWTCoefSet[i]->A_LL->m_nSizeRow*pWTCoefSet[i]->A_LL->m_nSizeCol;
    }

    /* 收集小波分解系数 */
    pDecResult->m_pC = (float *)calloc(nCoefLen, sizeof(float));
    pDecResult->m_pS = (int *)calloc((nWTLevel + 2) * 2, sizeof(int));
    if (pDecResult->m_pC == NULL || pDecResult->m_pS == NULL) {
        WB_FREE(pWaveletBase);
        WI_FREE(pDecResult);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    /* 记录第n级的低频分量以及其他级的水平、垂直、对角分量 */
    k = 0;
    for (i = nWTLevel - 1; i >= 0; i--) {
        if (i == nWTLevel - 1) {
            for (j = 0; j < pWTCoefSet[i]->A_LL->m_nSizeRow*pWTCoefSet[i]->A_LL->m_nSizeCol; j++) {
                pDecResult->m_pC[k] = pWTCoefSet[i]->A_LL->m_pData2D[j];
                k++;
            }
            DATA2D(pDecResult->m_pS, 0, 0, 2) = pWTCoefSet[nWTLevel - 1]->A_LL->m_nSizeRow;
            DATA2D(pDecResult->m_pS, 0, 1, 2) = pWTCoefSet[nWTLevel - 1]->A_LL->m_nSizeCol;
        }
        for (j = 0; j < pWTCoefSet[i]->H_LH->m_nSizeRow*pWTCoefSet[i]->H_LH->m_nSizeCol; j++) {
            pDecResult->m_pC[k] = pWTCoefSet[i]->H_LH->m_pData2D[j];
            k++;
        }
        for (j = 0; j < pWTCoefSet[i]->V_HL->m_nSizeRow*pWTCoefSet[i]->V_HL->m_nSizeCol; j++) {
            pDecResult->m_pC[k] = pWTCoefSet[i]->V_HL->m_pData2D[j];
            k++;
        }
        for (j = 0; j < pWTCoefSet[i]->D_HH->m_nSizeRow*pWTCoefSet[i]->D_HH->m_nSizeCol; j++) {
            pDecResult->m_pC[k] = pWTCoefSet[i]->D_HH->m_pData2D[j];
            k++;
        }
        DATA2D(pDecResult->m_pS, nWTLevel - i, 0, 2) = pWTCoefSet[i]->A_LL->m_nSizeRow;
        DATA2D(pDecResult->m_pS, nWTLevel - i, 1, 2) = pWTCoefSet[i]->A_LL->m_nSizeCol;
    }

    DATA2D(pDecResult->m_pS, nWTLevel + 1, 0, 2) = pSrcData->m_nSizeRow;
    DATA2D(pDecResult->m_pS, nWTLevel + 1, 1, 2) = pSrcData->m_nSizeCol;

    WB_FREE(pWaveletBase);
    return pDecResult;
}


WTCOEFSet * SubWaveDec2(DBDataPanel2D * pSrcData, WaveletBASE * pWaveBase, int nExtType)
{
    WTCOEFSet * pWTResult = NULL;
    DBDataPanel2D * pTempDataA_LL = NULL, *pTempDataH_LH = NULL, *pTempDataV_HL = NULL, *pTempDataD_HH = NULL;
    DBDataPanel2D * pTempDataA_LL2 = NULL, *pTempDataH_LH2 = NULL, *pTempDataV_HL2 = NULL, *pTempDataD_HH2 = NULL;
    DBDataPanel2D * pTempDataExt = NULL, *pTempDataA_H = NULL, *pTempDataV_D = NULL;
    DBDataPanel2D * pTempDataA_H2 = NULL, *pTempDataV_D2 = NULL;

    if (!(pWTResult = (WTCOEFSet*)calloc(1, sizeof(WTCOEFSet)))){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    /* 根据nExtType对原始数据进行延拓 */
    pTempDataExt = WExtend2D(pSrcData, nExtType, pWaveBase->nFilterLen - 1, pWaveBase->nFilterLen - 1);
    //pTempDataExt = WExtend2D(pSrcData, nExtType, 0, 0);
    //printf("%d  %d\n",pTempDataExt->m_nSizeRow ,pTempDataExt->m_nSizeCol);

    /* 行方向做低通滤波 列采样 */
    pTempDataA_H = CONV2D(pTempDataExt, pWaveBase->LO_D, pWaveBase->nFilterLen, DIR_ROW, CONVT_SAME);
    pTempDataA_H2 = DYADDOWN(pTempDataA_H, DYAD_EVEN, DIR_COL);
    DDP_FREE(pTempDataA_H);

    /* 列方向做低通/高通滤波，行采样，得到低频分量和水平分量 */
    pTempDataA_LL = CONV2D(pTempDataA_H2, pWaveBase->LO_D, pWaveBase->nFilterLen, DIR_COL, CONVT_SAME);
    pTempDataA_LL2 = DYADDOWN(pTempDataA_LL, DYAD_EVEN, DIR_ROW);
    DDP_FREE(pTempDataA_LL);

    pTempDataH_LH = CONV2D(pTempDataA_H2, pWaveBase->HI_D, pWaveBase->nFilterLen, DIR_COL, CONVT_SAME);
    pTempDataH_LH2 = DYADDOWN(pTempDataH_LH, DYAD_EVEN, DIR_ROW);
    DDP_FREE(pTempDataH_LH);

    /* 行方向做高通滤波 列采样 */
    pTempDataV_D = CONV2D(pTempDataExt, pWaveBase->HI_D, pWaveBase->nFilterLen, DIR_ROW, CONVT_SAME);
    pTempDataV_D2 = DYADDOWN(pTempDataV_D, DYAD_EVEN, DIR_COL);
    DDP_FREE(pTempDataV_D);

    /* 列方向做低通/高通滤波，行采样，得到垂直分量和对角分量 */
    pTempDataV_HL = CONV2D(pTempDataV_D2, pWaveBase->LO_D, pWaveBase->nFilterLen, DIR_COL, CONVT_SAME);
    pTempDataV_HL2 = DYADDOWN(pTempDataV_HL, DYAD_EVEN, DIR_ROW);
    DDP_FREE(pTempDataV_HL);

    pTempDataD_HH = CONV2D(pTempDataV_D2, pWaveBase->HI_D, pWaveBase->nFilterLen, DIR_COL, CONVT_SAME);
    pTempDataD_HH2 = DYADDOWN(pTempDataD_HH, DYAD_EVEN, DIR_ROW);
    DDP_FREE(pTempDataD_HH);

    pWTResult->A_LL = pTempDataA_LL2;
    pWTResult->H_LH = pTempDataH_LH2;
    pWTResult->V_HL = pTempDataV_HL2;
    pWTResult->D_HH = pTempDataD_HH2;
    //printf("%d  %d\n",pTempDataA_LL2->m_nSizeRow ,pTempDataA_LL2->m_nSizeCol);

    DDP_FREE(pTempDataExt);	DDP_FREE(pTempDataA_H2);	DDP_FREE(pTempDataV_D2);
    return pWTResult;
}


WTInfo * waveDec2Int(IntDataPanel2D * pSrcData, int nWTLevel, int nWaveType, int nExtType)
{
    int i, j;
    DBDataPanel2D * pTempSrcData = NULL;
    WTInfo * pWTResult = NULL;

    if (!pSrcData){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    if (!(pTempSrcData = (DBDataPanel2D *)calloc(1, sizeof(DBDataPanel2D)))){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    pTempSrcData->m_nSizeRow = pSrcData->m_nSizeRow;
    pTempSrcData->m_nSizeCol = pSrcData->m_nSizeCol;
    pTempSrcData->m_pData2D = (float *)calloc(pTempSrcData->m_nSizeRow*pTempSrcData->m_nSizeCol, sizeof(float));
    if (!pTempSrcData->m_pData2D) {
        DDP_FREE(pTempSrcData);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    for (i = 0; i < pTempSrcData->m_nSizeRow; i++)
        for (j = 0; j < pTempSrcData->m_nSizeCol; j++)
            DATA2D(pTempSrcData->m_pData2D, i, j, pTempSrcData->m_nSizeCol) = (float)DATA2D(pSrcData->m_pData2D, i, j, pSrcData->m_nSizeCol);

    pWTResult = waveDec2(pTempSrcData, nWTLevel, nWaveType, nExtType);

    DDP_FREE(pTempSrcData);
    return pWTResult;
}


//小波反变换

DBDataPanel2D * waveRec2(WTInfo * pSrcWTInfo)
{
    int i, j;
    DBDataPanel2D * pTempCoefA_LL = NULL;
    WaveletBASE * pWaveletBase = NULL;
    DBDataPanel2D * pRecResult = NULL;

    WTCOEFSet WTCoefSet;
    DBDataPanel2D pCoefPanel[4];

    if (!(pRecResult = (DBDataPanel2D*)calloc(1, sizeof(DBDataPanel2D)))){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }
    /* 根据nWaveType获取小波基的滤波器系数 */
    if (!(pWaveletBase = SetWaveletBase(pSrcWTInfo->m_nWaveType))) {
        DDP_FREE(pRecResult);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    /* 小波系数集合与四个2D数据关联 */
    WTCoefSet.A_LL = &(pCoefPanel[0]);	WTCoefSet.H_LH = &(pCoefPanel[1]);
    WTCoefSet.V_HL = &(pCoefPanel[2]);	WTCoefSet.D_HH = &(pCoefPanel[3]);

    /* 获取最高层的小波系数，0-低通分量，1-水平分量，2-垂直分量，3-对角分量 */
    pCoefPanel[0].m_pData2D = pSrcWTInfo->m_pC;
    pCoefPanel[0].m_nSizeRow = DATA2D(pSrcWTInfo->m_pS, 0, 0, 2);
    pCoefPanel[0].m_nSizeCol = DATA2D(pSrcWTInfo->m_pS, 0, 1, 2);
    for (i = 1; i < 4; i++) {
        pCoefPanel[i].m_pData2D = pCoefPanel[i - 1].m_pData2D + pCoefPanel[i - 1].m_nSizeRow*pCoefPanel[i - 1].m_nSizeCol;
        pCoefPanel[i].m_nSizeRow = DATA2D(pSrcWTInfo->m_pS, 1, 0, 2);
        pCoefPanel[i].m_nSizeCol = DATA2D(pSrcWTInfo->m_pS, 1, 1, 2);
    }

    for (i = 1; i <= pSrcWTInfo->m_nWTLevel; i++) {
        pTempCoefA_LL = SubWaveRec2(&WTCoefSet, pWaveletBase);
        WTCoefSet.A_LL = GetSubMatrix(pTempCoefA_LL, DATA2D(pSrcWTInfo->m_pS, i + 1, 0, 2), DATA2D(pSrcWTInfo->m_pS, i + 1, 1, 2));
        DDP_FREE(pTempCoefA_LL);
        if (i == pSrcWTInfo->m_nWTLevel)
            break;
        for (j = 1; j < 4; j++) {
            if (j == 1)
                pCoefPanel[j].m_pData2D = pCoefPanel[3].m_pData2D + pCoefPanel[3].m_nSizeRow*pCoefPanel[3].m_nSizeCol;
            else
                pCoefPanel[j].m_pData2D = pCoefPanel[j - 1].m_pData2D + pCoefPanel[j - 1].m_nSizeRow*pCoefPanel[j - 1].m_nSizeCol;
            pCoefPanel[j].m_nSizeRow = DATA2D(pSrcWTInfo->m_pS, i + 1, 0, 2);
            pCoefPanel[j].m_nSizeCol = DATA2D(pSrcWTInfo->m_pS, i + 1, 1, 2);
        }
    }

    pRecResult = WTCoefSet.A_LL;
    WB_FREE(pWaveletBase);
    return pRecResult;
}


DBDataPanel2D * SubWaveRec2(WTCOEFSet * pSrcWTCoef, WaveletBASE * pWaveBase)
{
    DBDataPanel2D * pResultA_LL = NULL;

    DBDataPanel2D * pTempDataA_LL = NULL, *pTempDataH_LH = NULL, *pTempDataV_HL = NULL, *pTempDataD_HH = NULL;
    DBDataPanel2D * pTempDataA_LL2 = NULL, *pTempDataH_LH2 = NULL, *pTempDataV_HL2 = NULL, *pTempDataD_HH2 = NULL;
    DBDataPanel2D * pTempDataAH = NULL, *pTempDataVD = NULL;
    DBDataPanel2D * pTempDataAH2 = NULL, *pTempDataVD2 = NULL;

    /* 行插值 */
    pTempDataA_LL = DYADUP(pSrcWTCoef->A_LL, DYAD_ODD, DIR_ROW);
    pTempDataH_LH = DYADUP(pSrcWTCoef->H_LH, DYAD_ODD, DIR_ROW);
    pTempDataV_HL = DYADUP(pSrcWTCoef->V_HL, DYAD_ODD, DIR_ROW);
    pTempDataD_HH = DYADUP(pSrcWTCoef->D_HH, DYAD_ODD, DIR_ROW);

    /* 列滤波 */
    pTempDataA_LL2 = CONV2D(pTempDataA_LL, pWaveBase->LO_R, pWaveBase->nFilterLen, DIR_COL, CONVT_SAME);
    pTempDataH_LH2 = CONV2D(pTempDataH_LH, pWaveBase->HI_R, pWaveBase->nFilterLen, DIR_COL, CONVT_SAME);
    pTempDataV_HL2 = CONV2D(pTempDataV_HL, pWaveBase->LO_R, pWaveBase->nFilterLen, DIR_COL, CONVT_SAME);
    pTempDataD_HH2 = CONV2D(pTempDataD_HH, pWaveBase->HI_R, pWaveBase->nFilterLen, DIR_COL, CONVT_SAME);
    DDP_FREE(pTempDataA_LL);	DDP_FREE(pTempDataH_LH);	DDP_FREE(pTempDataV_HL);	DDP_FREE(pTempDataD_HH);

    /* 组合后列差值 */
    pTempDataAH = SumMatrix(pTempDataA_LL2, pTempDataH_LH2);
    DDP_FREE(pTempDataA_LL2);	DDP_FREE(pTempDataH_LH2);
    pTempDataAH2 = DYADUP(pTempDataAH, DYAD_ODD, DIR_COL);
    DDP_FREE(pTempDataAH);

    pTempDataVD = SumMatrix(pTempDataV_HL2, pTempDataD_HH2);
    DDP_FREE(pTempDataV_HL2);	DDP_FREE(pTempDataD_HH2);
    pTempDataVD2 = DYADUP(pTempDataVD, DYAD_ODD, DIR_COL);
    DDP_FREE(pTempDataVD);

    /* 行滤波 */
    pTempDataAH = CONV2D(pTempDataAH2, pWaveBase->LO_R, pWaveBase->nFilterLen, DIR_ROW, CONVT_SAME);
    DDP_FREE(pTempDataAH2);
    pTempDataVD = CONV2D(pTempDataVD2, pWaveBase->HI_R, pWaveBase->nFilterLen, DIR_ROW, CONVT_SAME);
    DDP_FREE(pTempDataVD2);

    /* 组合 */
    pResultA_LL = SumMatrix(pTempDataAH, pTempDataVD);

    DDP_FREE(pTempDataAH);
    DDP_FREE(pTempDataVD);
    //printf("%d  %d\n",pResultA_LL->m_nSizeRow ,pResultA_LL->m_nSizeCol);
    //printf("%d\n", pResultA_LL->m_nSizeCol);
    return pResultA_LL;
}


IntDataPanel2D * waveRec2Int(WTInfo * pSrcWTInfo)
{
    int i;
    IntDataPanel2D * pRecResult = NULL;
    DBDataPanel2D * pDBRecResult = NULL;

    if (!pSrcWTInfo){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    if (!(pRecResult = (IntDataPanel2D *)calloc(1, sizeof(IntDataPanel2D)))){
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }

    if (!(pDBRecResult = waveRec2(pSrcWTInfo))) {
        IDP_FREE(pRecResult);
        printf("file：%s line：%d time：%s %s\n",__FILE__,__LINE__,__DATE__,__TIME__);
        throw "DWT error";
    }
    pRecResult->m_nSizeRow = pDBRecResult->m_nSizeRow;
    pRecResult->m_nSizeCol = pDBRecResult->m_nSizeCol;

    if (!(pRecResult->m_pData2D = (int *)calloc(pRecResult->m_nSizeRow*pRecResult->m_nSizeCol, sizeof(int)))) {
        IDP_FREE(pRecResult);
        DDP_FREE(pDBRecResult);
    };

    for (i = 0; i < (pRecResult->m_nSizeRow)*(pRecResult->m_nSizeCol); i++)
        pRecResult->m_pData2D[i] = (unsigned int)pDBRecResult->m_pData2D[i];

    DDP_FREE(pDBRecResult);
    return pRecResult;
}

#endif
