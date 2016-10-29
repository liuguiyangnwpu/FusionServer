#include "imagequality.h"

extern map<string, string> g_ConfMap;

ImageQuality::ImageQuality() {
    m_serializePath = g_ConfMap["QUALTYSerializePath"];
    m_serializePathBak = g_ConfMap["QUALTYSerializePathBak"];

    p_threadPool = NULL;
    p_threadPool = ThreadPool::getSingleInstance();

    m_fakeMap.clear();
}

ImageQuality::~ImageQuality() {
    p_threadPool->revokeSingleInstance();
    p_threadPool = NULL;
}

bool ImageQuality::checkQualityArgv(const QualityInputStruct &inputArgs) {
    // main to change the filePath is Right !
    for(QualityMapArgs::const_iterator it=inputArgs.inputMap.begin(); it!=inputArgs.inputMap.end(); it++) {
        if(access(it->second.filePath.c_str(), 0) != 0) {
            Log::Error("FilePath Not Exists ! Please Check !");
            return false;
        }
    }

    if(inputArgs.inputMap.size() != 3) {
        Log::Error("InputMap Size Error ! Please Check !");
        return false;
    }

    if(inputArgs.inputMap.count("f2") == 0 || inputArgs.inputMap.at("f2").bandIdList.empty()) {
        Log::Error("InputMap[f2] is empty ! Please Check !");
        return false;
    }
    return true;
}

bool ImageQuality::returnFakeMapRes(const QualityInputStruct& inputArgs, QualityInfo &res) {
    vector<string> targets;
    for(QualityMapArgs::const_iterator it=inputArgs.inputMap.begin(); it!=inputArgs.inputMap.end(); it++) {
        targets.push_back(it->second.filePath);
    }
    if(m_fakeMap.count(targets) == 0)
        return false;
    return true;
}

void ImageQuality::log_InputParameters(const QualityInputStruct &inputArgs) {
    string str = "";
    str += ("task_id="+inputArgs.id+"#");
    str += ("algorithmkind="+inputArgs.algorithmkind+"#");
    for(QualityMapArgs::const_iterator it=inputArgs.inputMap.begin(); it!=inputArgs.inputMap.end(); it++) {
        str += (it->first+"$"+"filePath="+it->second.filePath);
        str += ("#bandlist="+it->second.bandIdList);
    }
    Log::Info(str);
}

void ImageQuality::log_OutputResult(const QualityInfo &outRes) {
    string str = "";
    str += ("status="+to_string(outRes.status)+"#");
    for(DatasMap::const_iterator it=outRes.imgsquality.begin(); it!=outRes.imgsquality.end(); it++) {
        str += ("key="+it->first+"#");
        str += "bandValue=";
        for(DataArray::const_iterator vit=it->second.begin(); vit!= it->second.end(); vit++) {
            str += (to_string(*vit)+",");
        }
    }
    Log::Info(str);
}

QualityInfo ImageQuality::qualitySyn(const QualityInputStruct &inputArgs, const Ice::Current &) {
    string task_id = inputArgs.id;
    if(m_finishMap.count(task_id) != 0) {
        return m_finishMap[task_id].output;
    }

    QualityInfo quaRes;
    quaRes.status = -1;
    bool flag = checkQualityArgv(inputArgs);
    if(flag == false)
        return quaRes;
    flag = returnFakeMapRes(inputArgs, quaRes);
    if(flag == false) {
        vector<string> targets;
        for(QualityMapArgs::const_iterator it=inputArgs.inputMap.begin(); it!=inputArgs.inputMap.end(); it++) {
            targets.push_back(it->second.filePath);
        }

        QualityResMap* tmp = NULL;
        tmp = (QualityResMap*)qualityInterface((void*)(&inputArgs));
        if(tmp == NULL) {
            return quaRes;
        }

        quaRes.status = 1;
        for(map<string, vector<double> >::iterator it=tmp->res.begin(); it!=tmp->res.end(); it++) {
            quaRes.imgsquality[it->first] = it->second;
        }
        //fillFinishTaskMap(task_id, inputArgs, quaRes);
        m_fakeMap[targets] = quaRes;
        return quaRes;
    } else {
        return quaRes;
    }
}

int ImageQuality::qualityAsyn(const QualityInputStruct &inputArgs, const Ice::Current &) {
    string task_id = inputArgs.id;

    if(m_finishMap.count(task_id) != 0) {
        return 1;
    }

    QualityInputStruct* args = new(std::nothrow) QualityInputStruct;
    if(args == NULL) {
        Log::Error("qualityAsyn ## new QualityInputStruct Failed !");
        return -1;
    }

    log_InputParameters(inputArgs);
    bool flag = checkQualityArgv(inputArgs);
    if(flag == false) {
        delete args;
        Log::Error("qualityAsyn ## Image ImageQuality Parameters Error !");
        return -1; // push task error
    }

    args->id = inputArgs.id;
    args->algorithmkind = inputArgs.algorithmkind;
    for(auto it=inputArgs.inputMap.begin(); it!=inputArgs.inputMap.end(); it++) {
        args->inputMap[it->first] = it->second;
    }

    Task* task = new(std::nothrow) Task(&qualityInterface, (void*)args);
    if(task == NULL) {
        delete args;
        Log::Error("qualityAsyn ## new Task Failed !");
        return -1;
    }

    task->setTaskID(task_id);
    if(p_threadPool->add_task(task, task_id) != 0) {
        Log::Error("qualityAsyn ## thread Pool add Task Failed !");
        delete args;
        delete task;
        return -1; // Means For Add Task Failed !
    }
    return 3; // Means For Add Task Success !
}

QualityInfo ImageQuality::fetchQualityRes(const string &inputArgs, const Ice::Current &) {
    QualityInfo obj;
    string task_id = inputArgs;

    if(m_finishMap.count(task_id) != 0) {
        return m_finishMap[task_id].output;
    }
    Log::Info("fetchQualityRes ## inputArgs %s", task_id.c_str());
    TaskPackStruct tmp;
    int flag = p_threadPool->fetchResultByTaskID(task_id, tmp);
    if(flag == 0) {
        obj.status = 0;
        Log::Info("fetchQualityRes ## fetch task id %s Running !", task_id.c_str());
    } else if(flag == 1){
        Log::Info("fetchQualityRes ## fetch task id %s Result Successful !", task_id.c_str());
        QualityResMap* t = (QualityResMap*)tmp.output;
        if(t != NULL) {
            if(t->status == 0.0) {
                obj.status = 60;
            } else {
                obj.status = t->status;
//            obj.status = 100;
            }
            for(auto it=t->res.begin(); it!=t->res.end(); it++) {
                obj.imgsquality[it->first] = it->second;
            }
        }
        log_OutputResult(obj);
        delete (QualityInputStruct*)(tmp.input);
        delete (QualityResMap*)(tmp.output);
    } else if(flag == -1) {
        obj.status = -1;
        Log::Error("fetchQualityRes ## fetch task id %s result Failed !", task_id.c_str());
    }
    return obj;
}

void ImageQuality::fillFinishTaskMap(const string &task_id, const QualityInputStruct &inParam, const QualityInfo &outParam) {
    if(m_finishMap.count(task_id) == 0) {
        QualityTaskStaticResult tmp;
        tmp.task_id.assign(task_id);
        tmp.input = inParam;
        tmp.output = outParam;
        m_finishMap[task_id] = tmp;
        Log::Info("Finish Task size is %d !", m_finishMap.size());
    }
}

bool ImageQuality::packTaskStaticStatus(QualityTaskStaticResult &res, const string task_id, TaskPackStruct &tmp) {
    QualityResMap* out_res = (QualityResMap*)tmp.output;
    res.task_id.assign(task_id);
    res.output.status = out_res->status;
    for(auto it=out_res->res.begin(); it!=out_res->res.end(); it++) {
        res.output.imgsquality[it->first] = it->second;
    }
    delete out_res;

    QualityInputStruct* param = (QualityInputStruct*) tmp.input;
    res.input.id.assign(param->id);
    res.input.algorithmkind = param->algorithmkind;
    for(QualityMapArgs::iterator it=param->inputMap.begin(); it!=param->inputMap.end(); it++) {
        ImageParameter tmp;
        tmp.filePath = it->second.filePath;
        res.input.inputMap[it->first] = tmp;
    }
    delete param;

    return true;
}

