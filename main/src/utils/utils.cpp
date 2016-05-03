#include "utils.h"

void checkFileInfo(string filePath, ResultStatus& status) {
    if(access(filePath.c_str(), 0) == 0) {

        status.stauts = 0;
        status.mode = EXISTS;
        status.desc = "EXISTS";

        if(access(filePath.c_str(), 2) == 0) {
            status.stauts = 0;
            status.mode = WRITEABLE;
            status.desc = "WRITEABLE";
        }

        if(access(filePath.c_str(), 4) == 0) {
            status.stauts = 0;
            status.mode = READABLE;
            status.desc = "READABLE";
        }

        if(access(filePath.c_str(), 6) == 0) {
            status.stauts = 0;
            status.mode = RWABLE;
            status.desc = "RWABLE";
        }
    } else {
        status.stauts = -1;
        status.mode = NOTEXISTS;
        status.desc = "NOTEXISTS";
    }
}

//FusionStruct fusion(string PanUrl,string MsUrl,string OutUrl,string LogUrl,int idalg,int* band,int interpolation);
void* fusionInterface(void * args) {
    FusionArgs* param = (FusionArgs*) args;
    FusionStruct* pObj = NULL;
    int band[3];
    for(size_t i=0;i<param->band.size();++i)
        band[i] = param->band[i];
    pObj = fusion(param->panurl, param->msurl, param->outurl, param->logurl, param->idalg, band, param->idinter);
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