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
#include <map>
#include <numeric>
#include <vector>

#include "../../../utils/log.h"
#include "../../../rpc/quality/qualityjudgeRpc.h"
#include "../../../imagequality.h"

using namespace std;
using namespace RPCQualityJudge;

//statement for integer data length.
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

// write status into log file
void WriteMsg(char*, int32_t statusnum, const char* statusmsg);

typedef struct _QualityResMap{
    int status;
    map<string, vector<double>> res;
}QualityResMap;

// algorithm class
bool mainClarity(string& filepath, char* logfilepath, vector<double> &m_qRes);
bool mainContrastRatio(string& filepath, char* logfilepath, vector<double> &m_qRes);
bool mainEntropy(string& filepath, char* logfilepath, vector<double> &m_qRes);
bool mainMean(string& filepath, char* logfilepath, vector<double> &m_qRes);
bool mainSignaltoNoiseRatio(string& filepath, char* logfilepath, vector<double> &m_qRes);
bool mainStriperesidual(string& filepath, char* logfilepath, vector<double> &m_qRes);
bool mainDynamicRange(string& filepath, char* logfilepath, vector<double> & m_qRes);
bool mainVariance(string& filepath, char* logfilepath, vector<double> & m_qRes);
bool mainRadiationUniform(string& filepath , char* logfilepath, vector<double>& m_qRes);

bool mainCrossEntropy(string filepath1, string filepath3, char* logfilepath, vector<double> &m_qRes);
bool mainMutualInformation(string filePath1, string filePath3, char* logfile, vector<double>& m_qRes);
bool mainStructureSimilarity(string filepath1, string filepath3, char* logfile, vector<double>& m_qRes);

bool mainSpectralAngleMatrix(string filepath2, string filepath3, char* logfile, vector<int> bandlist, string interkind, vector<double>& m_qRes);
double mainComprehensiveEvaluate(vector<double> resultvalue);
// interface
void *qualityInterface(void* args);

// set intertimer to serialize the ImageQuality Data !
void utils_serialize_quality(int);
void serializeImageQualityOnTime(int seconds);
void calculateAvg(vector<double>& result);

#endif // UTILS_H
