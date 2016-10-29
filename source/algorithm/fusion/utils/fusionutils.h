#ifndef FUSIONUTILS_H
#define FUSIONUTILS_H

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <exception>

#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "../BroveyFusion.h"
#include "../HSIFusion.h"
#include "../HCSFusion.h"
#include "../PCAFusion.h"
#include "../DWTFusion.h"
#include "../CurveletFusion.h"
#include "../GramSchmidtFusion.h"
#include "../PgInf.h"

#include "../../../rpc/fusion/wisefuseRpc.h"
#include "../../../utils/json/json-forwards.h"
#include "../../../utils/json/json.h"

using namespace std;
using namespace RPCWiseFuse;
using namespace Json;

typedef struct _FusionArgs {
    string panurl;
    string msurl;
    string outurl;
    string logurl;
    vector<int> band;
    int idalg;
    int idinter;
}FusionArgs;

void* fusionInterface(void* args);

void deepCopyTaskInputParameter(const FusionArgs& src, FusionArgs& dest);
void deepCopyTaskResult(const FusionInf &src, FusionInf &dest);
void deepCopyTask2RpcResult(const FusionStruct& src, FusionInf& dest);

void utils_serialize_fusion(int);
void serializeImageFusionOnTime(int seconds);
FusionStruct* fusion(string PanUrl, string MsUrl, string OutUrl, string, int idalg, int* band, int interpolation);

#endif // FUSIONUTILS_H
