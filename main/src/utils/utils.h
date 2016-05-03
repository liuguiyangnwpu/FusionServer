#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <unistd.h>
#include <vector>
#include <exception>

#include "../algorithm/fusion/Fusion.h"
#include "../rpc/wisefuseRpc.h"

using namespace std;
using namespace RPCWiseFuse;

typedef struct _FusionArgs {
    string panurl;
    string msurl;
    string outurl;
    string logurl;
    int idalg;
    vector<int> band;
    int idinter;
}FusionArgs;

enum TASKSTATUS {
    TASKINIT,
    TASKRUNNING,
    TASKCOMPELETE,
    TASKERROR,
    ARGERROR
};

enum FileMode {
    EXISTS,
    WRITEABLE,
    READABLE,
    RWABLE,
    NOTEXISTS
};

typedef struct _ResultStatus {
    int stauts;
    FileMode mode;
    string desc;
}ResultStatus;

typedef struct _TaskStaticResult {
    string task_id;
    FusionArgs input;
    FusionInf output;
}TaskStaticResult;

void checkFileInfo(string filePath, ResultStatus& status);
void* fusionInterface(void* args);

void deepCopyTaskInputParameter(const FusionArgs& src, FusionArgs& dest);
void deepCopyTaskResult(const FusionInf &src, FusionInf &dest);
void deepCopyTask2RpcResult(const FusionStruct& src, FusionInf& dest);


#endif // UTILS_HPP
