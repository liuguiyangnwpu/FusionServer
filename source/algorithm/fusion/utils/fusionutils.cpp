#include "fusionutils.h"
#include "../../../imagefusion.h"

extern ImageFusion* g_ImgFusion;

void* fusionInterface(void * args) {
    FusionArgs* param = (FusionArgs*) args;
    FusionStruct* pObj = NULL;
    int band[3];
    for(size_t i=0;i<param->band.size();++i)
        band[i] = param->band[i];
    pObj = fusion(param->panurl, param->msurl, param->outurl, param->logurl, param->idalg, band, param->idinter);
    cout << "Fusion Interface finishd @@@@@@@@" << endl;
    return (void*)pObj;
}

void deepCopyTaskInputParameter(const FusionArgs &src, FusionArgs &dest) {
    dest.band.assign(src.band.begin(), src.band.end());
    dest.idalg = src.idalg;
    dest.idinter = src.idinter;
    dest.logurl = src.logurl;
    dest.msurl = src.msurl;
    dest.outurl = src.outurl;
    dest.panurl = src.panurl;
}

void deepCopyTaskResult(const FusionInf &src, FusionInf &dest) {
    dest.brcoorvalidLatitude = src.brcoorvalidLatitude;
    dest.brcoorvalidLongitude = src.brcoorvalidLongitude;
    dest.brcoorwholeLatitude = src.brcoorwholeLatitude;
    dest.brcoorwholeLongitude = src.brcoorwholeLongitude;

    dest.cnttimeuse = src.cnttimeuse;
    dest.datumname.assign(src.datumname);
    dest.producetime.assign(src.producetime);
    dest.productFormat.assign(src.productFormat);
    dest.projcentralmeridian = src.projcentralmeridian;
    dest.projectioncode.assign(src.projectioncode);
    dest.projectiontype.assign(src.projectiontype);
    dest.projectionunits.assign(src.projectionunits);

    dest.resolution = src.resolution;
    dest.status = src.status;
    dest.ulcoorvalidLatitude = src.ulcoorvalidLatitude;
    dest.ulcoorvalidLongitude = src.ulcoorvalidLongitude;
    dest.ulcoorwholeLatitude = src.ulcoorwholeLatitude;
    dest.ulcoorwholeLongitude = src.ulcoorwholeLongitude;
}

void deepCopyTask2RpcResult(const FusionStruct &src, FusionInf &dest) {
    dest.brcoorvalidLatitude = src.brcoorvalid_latitude;
    dest.brcoorvalidLongitude = src.brcoorvalid_longitude;
    dest.brcoorwholeLatitude = src.brcoorwhole_latitude;
    dest.brcoorwholeLongitude = src.brcoorwhole_longitude;

    dest.ulcoorvalidLatitude = src.ulcoorvalid_latitude;
    dest.ulcoorvalidLongitude = src.ulcoorvalid_longitude;
    dest.ulcoorwholeLatitude = src.ulcoorwhole_latitude;
    dest.ulcoorwholeLongitude = src.ulcoorwhole_longitude;

    dest.cnttimeuse = src.cnttimeuse;
    dest.datumname.assign(src.datumname);
    dest.producetime.assign(src.producetime);
    dest.productFormat.assign(src.product_format);
    dest.projcentralmeridian = src.projcentralmeridian;
    dest.projectioncode.assign(src.projectioncode);
    dest.projectiontype.assign(src.projectiontype);
    dest.projectionunits.assign(src.projectionunits);
    dest.resolution = src.resolution;

    dest.status = src.status;
}

void utils_serialize_fusion(int) {
    if(g_ImgFusion != NULL) {
        g_ImgFusion->serializeTaskResults();
    }
}

void serializeImageFusionOnTime(int seconds) {
    struct itimerval tick;
    memset(&tick, 0, sizeof(tick));
    tick.it_interval.tv_sec = seconds;
    tick.it_interval.tv_usec = 0;
    tick.it_value.tv_sec = seconds;
    tick.it_value.tv_usec = 0;
    signal(SIGALRM, utils_serialize_fusion);
    if(setitimer(ITIMER_REAL, &tick, NULL) < 0) {
        Log::Error("Set Timer to serialize ImageQuality Dict Failed !");
        throw runtime_error("Set Timer to Serialize ImageQuality Dict Failed !");
    }
}
