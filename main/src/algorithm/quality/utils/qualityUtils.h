#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include <gdal_priv.h>

#include <iostream>
#include "../../../utils/log.h"
#include "../../../rpc/quality/qualityjudgeRpc.h"
#include "../../../../imagequality.h"

using namespace std;
using namespace RPCQualityJudge;

//statement for integer data length.
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long int int64_t;
//typedef unsigned long long uint64_t;

typedef struct _QualityRes{
    double * data;
    int length;
    int status;//fail means <0; success means >= 0;
}QualityRes;

// write status into log file
void WriteMsg(char*, int32_t statusnum, char* statusmsg);
bool createQualityRes(QualityRes& quaRes, int length);
void revokeQualityRes(QualityRes** p_quaRes);

// algorithm class
bool mainClarity(ImageParameter &testparameter, char* logfilepath, QualityRes &m_qRes);
bool mainContrastRatio(ImageParameter &testparameter, char* logfilepath, QualityRes &m_qRes);
bool mainEntropy(ImageParameter &testparameter, char* logfilepath, QualityRes &m_qRes);
bool mainMean(ImageParameter &testparameter, char* logfilepath, QualityRes &m_qRes);
bool mainSignaltoNoiseRatio(ImageParameter &testparameter, char* logfilepath, QualityRes &m_qRes);
bool mainStriperesidual(ImageParameter &testparameter, char* logfilepath, QualityRes &m_qRes);

// interface
void *qualityInterface(void* args);
void deepCopyQualityRes2Info(const QualityRes& src, QualityInfo& dest);

// set intertimer to serialize the ImageQuality Data !
void utils_serialize_quality(int);
void serializeImageQualityOnTime(int seconds);

#endif // UTILS_H
