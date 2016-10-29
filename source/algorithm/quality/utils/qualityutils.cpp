#include "qualityutils.h"

//write status into log file
void WriteMsg(char* , int32_t statusnum, const char* statusmsg) {
    Log::Info("%d\t%s", statusnum, statusmsg);
}

void utils_serialize_quality(int ) {
;
}

void serializeImageQualityOnTime(int seconds) {
    struct itimerval tick;
    memset(&tick, 0, sizeof(tick));
    tick.it_interval.tv_sec = seconds;
    tick.it_interval.tv_usec = 0;
    tick.it_value.tv_sec = seconds;
    tick.it_value.tv_usec = 0;
    signal(SIGALRM, utils_serialize_quality);
    if(setitimer(ITIMER_REAL, &tick, NULL) < 0) {
        Log::Error("Set Timer to serialize ImageQuality Dict Failed !");
        throw runtime_error("Set Timer to Serialize ImageQuality Dict Failed !");
    }
}

void calculateAvg(vector<double>& result) {
    double total = std::accumulate(result.begin(), result.end(), 0.0);
    result.insert(result.begin(), total*1.0/result.size());
}

void init_evaluatealg(map<string,int>& evaluatealg) {
    evaluatealg["Clarity_1_0"]=1;
    evaluatealg["ContrastRatio_1_0"]=2;
    evaluatealg["Entropy_1_0"]=3;
    evaluatealg["Mean_1_0"]=4;
    evaluatealg["SignaltoNoiseRatio_1_0"]=5;
    evaluatealg["Striperesidual_1_0"]=6;

    evaluatealg["CrossEntropy_1_0"]=7;
    evaluatealg["MutualInformation_1_0"]=8;
    evaluatealg["SpectralAngleMatrix_1_0"]=9;
    evaluatealg["StructureSimilarity_1_0"]=10;

    evaluatealg["DynamicRange_1_0"] = 11;
    evaluatealg["Variance_1_0"] = 12;
    evaluatealg["RadiationUniform_1_0"] = 13;
}

void combine_InputArgs(QualityInputStruct* tmp, vector<string>& inputPathVec, vector<int>& bandlist) {
    inputPathVec.resize(3);
    inputPathVec[0] = tmp->inputMap["f1"].filePath;
    inputPathVec[1] = tmp->inputMap["f2"].filePath;
    inputPathVec[2] = tmp->inputMap["f3"].filePath;
    string strlist = tmp->inputMap["f2"].bandIdList;
    while(strlist.size()) {
        size_t pos = strlist.find_first_of('|');
        if(pos == string::npos) {
            break;
        }
        bandlist.push_back(stoi(strlist.substr(0, pos)));
        strlist = strlist.substr(pos+1);
    }
    bandlist.push_back(stoi(strlist));
}

void quality_calculate_part1(QualityResMap*p_resMap, vector<string> inputPathVec, char* logfilepath, vector<double> qualityRes) {
    int flag;
    vector<double> avgVec;
    avgVec.resize(5);

    flag = mainSignaltoNoiseRatio(inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["信噪比"].assign(qualityRes.begin(), qualityRes.end());
    avgVec[0] = qualityRes[0];

    flag = mainClarity(inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["清晰度"].assign(qualityRes.begin(), qualityRes.end());
    avgVec[1] = qualityRes[0];

    flag = mainContrastRatio(inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["对比度"].assign(qualityRes.begin(), qualityRes.end());
    avgVec[2] = qualityRes[0];

    flag = mainEntropy(inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["熵信息"].assign(qualityRes.begin(), qualityRes.end());
    avgVec[3] = qualityRes[0];

    flag = mainRadiationUniform(inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["均匀辐射"].assign(qualityRes.begin(), qualityRes.end());
    avgVec[4] = qualityRes[0];

    double total_score = mainComprehensiveEvaluate(avgVec);
    p_resMap->status = (int)(total_score+0.5);
    Log::Info("Comprehensive Evaluate Scoree is %lf", total_score);
}

void quality_calculate_part2(QualityResMap*p_resMap, vector<string> inputPathVec, char* logfilepath, vector<double> qualityRes) {
    int flag;
    flag = mainMean(inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["均值"].assign(qualityRes.begin(), qualityRes.end());

    flag = mainStriperesidual(inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["条纹残余度"].assign(qualityRes.begin(), qualityRes.end());

    flag = mainDynamicRange(inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["动态变化范围"].assign(qualityRes.begin(), qualityRes.end());

    flag = mainVariance(inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["方差"].assign(qualityRes.begin(), qualityRes.end());
}

void quality_calculate_part3(QualityResMap*p_resMap, vector<string> inputPathVec, char* logfilepath, vector<double> qualityRes) {
    int flag;
    flag = mainCrossEntropy(inputPathVec[0], inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["交叉熵"].assign(qualityRes.begin(), qualityRes.end());

    flag = mainMutualInformation(inputPathVec[0], inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["互信息"].assign(qualityRes.begin(), qualityRes.end());

    flag = mainStructureSimilarity(inputPathVec[0], inputPathVec[2], logfilepath, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["结构相似度"].assign(qualityRes.begin(), qualityRes.end());
}

void quality_calculate_part4(QualityResMap* p_resMap, vector<string> inputPathVec, char* logfilepath, vector<int> bandlist, string interkind, vector<double> qualityRes) {
    int flag;
    flag = mainSpectralAngleMatrix(inputPathVec[1], inputPathVec[2], logfilepath, bandlist, interkind, qualityRes);
    calculateAvg(qualityRes);
    p_resMap->res["光谱角矩阵"].assign(qualityRes.begin(), qualityRes.end());
}

void* qualityInterface(void *args) {
    if(args == NULL)
        return NULL;
    QualityInputStruct * tmp = (QualityInputStruct*)args;

    vector<string> inputPathVec;
    vector<int> bandlist;

    combine_InputArgs(tmp, inputPathVec, bandlist);

    QualityResMap* p_resMap = new(std::nothrow) QualityResMap;
    if(p_resMap == NULL) {
        return NULL;
    }

    vector<double> qualityRes;
    char* logfilepath = NULL;

    quality_calculate_part1(p_resMap, inputPathVec, logfilepath, qualityRes);

    quality_calculate_part2(p_resMap, inputPathVec, logfilepath, qualityRes);

    quality_calculate_part3(p_resMap, inputPathVec, logfilepath, qualityRes);

    quality_calculate_part4(p_resMap, inputPathVec, logfilepath, bandlist, tmp->algorithmkind, qualityRes);

    return p_resMap;
}
