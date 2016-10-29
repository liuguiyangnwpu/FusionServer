#ifndef IMAGEFUSION_H
#define IMAGEFUSION_H

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <Ice/Ice.h>
#include "rpc/fusion/wisefuseRpc.h"
#include "algorithm/thumb/imagethumb.h"
#include "algorithm/fusion/utils/fusionutils.h"
#include "utils/log.h"
#include "utils/utils.h"
#include "threadpool/threadpool.h"

using namespace std;
using namespace RPCWiseFuse;

typedef struct _FusionTaskStaticResult {
    string task_id;
    FusionArgs input;
    FusionInf output;
}FusionTaskStaticResult;

class ImageFusion : public WiseFusionInf {
public:
    ImageFusion();
    ~ImageFusion();

public:
    virtual bool geneThumbImg(const string& inPath, const string& outPath, const string& bandlist, const Ice::Current&);
    //提交同步任务， 一直等待结果
    virtual ::RPCWiseFuse::FusionInf fuseSyn(const ::RPCWiseFuse::DirArgs& mapArgs, const Ice::Current& );
    //提交异步任务
    virtual int fuseAsyn(const ::RPCWiseFuse::DirArgs& mapArgs, const Ice::Current& );
    //k:id	v:		//任务唯一编号
    virtual string askProcess(const ::RPCWiseFuse::DirArgs& mapArgs, const Ice::Current& );
    //k:id	v:		//任务唯一编号
    virtual ::RPCWiseFuse::FusionInf fetchFuseRes(const ::RPCWiseFuse::DirArgs& mapArgs, const Ice::Current& );

public:
    bool checkFusionArgv(::RPCWiseFuse::DirArgs mapArgs, FusionArgs& args);
    void log_InputParameters(DirArgs mapArgs);
    void log_OutputResult(const FusionInf& destInf);

    int getSerializeTaskResults(string serializePath);
    void serializeTaskResults();
    void fillFinishTaskMap(const string& task_id, const FusionArgs& inParam, const FusionInf& outParam);
    bool packTaskStaticStatus(FusionTaskStaticResult &res, const string task_id, TaskPackStruct& tmp);

private:
    ThreadPool* p_threadPool;

    string m_serializePath;
    string m_serializePathBak;

    map<string, FusionTaskStaticResult> m_finishMap; // for <task_id, task_all_parameters>
    map<string,int> m_fusionalg;
    map<string,int> m_interalg;
};

#endif // IMAGEFUSION_H
