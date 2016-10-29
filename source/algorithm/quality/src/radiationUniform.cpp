#include "../utils/qualityutils.h"

int32_t RadiationUniform(char* filepath1, char* logfilepath, vector<double>& striperesult) {
    GDALDataset *poDataset;
    GDALAllRegister();
    poDataset=(GDALDataset *)GDALOpen(filepath1,GA_ReadOnly);
    if( poDataset == NULL ) {
        cerr << "RadiationUniform filepath " << filepath1 << endl;
        WriteMsg(logfilepath,-1,"Image file open error!");
        return -1;
    } else {
        WriteMsg(logfilepath,0,"Striperesidual algorithm is executing!");
    }

    int32_t n,i,j;
    int32_t bandnum,width,height;
    bandnum=poDataset->GetRasterCount();
    width=poDataset->GetRasterXSize();
    height=poDataset->GetRasterYSize();
    GDALRasterBand *pband;
    uint16_t *banddata=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width*height);

    for(n=0;n<bandnum;n++) {
        pband=poDataset->GetRasterBand(n+1);
        pband->RasterIO(GF_Read,0,0,width,height,banddata,width,height,GDT_UInt16,0,0);
        double *colmeans=(double *)CPLMalloc(sizeof(double)*width);
        double imgmean=0.0;

        for(j=0;j<width;j++) {
            colmeans[j]=0.0;
        }

        for(i=0;i<height;i++) {
            for(j=0;j<width;j++) {
                colmeans[j]=colmeans[j]+banddata[i*width+j]/(double)height;
            }
        }

        for(j=0;j<width;j++) {
            imgmean = imgmean+colmeans[j]/(double)width;
        }

        double sum=0.0;
        for(j=1;j<width;j++) {
            sum = sum + (colmeans[j]-imgmean)*(colmeans[j]-imgmean)/width;
        }

        striperesult.push_back(100.0*(1-sqrt(sum)/imgmean));

        CPLFree(colmeans);
        colmeans=NULL;

        GDALClose(pband);
        pband=NULL;

        //Writing the process and status of this Algorithm.
        int32_t temp = (int)(100.0*(n+1)/bandnum);
        temp = (temp>99) ? 99:temp;
        WriteMsg(logfilepath,temp,"Striperesidual algorithm is executing!");
    }

    CPLFree(banddata);
    banddata=NULL;

    GDALClose(poDataset);
    poDataset=NULL;
    return 1;
}

bool mainRadiationUniform(string& filepath , char* logfilepath, vector<double>& m_qRes) {
    m_qRes.clear();
    int32_t res = RadiationUniform(const_cast<char*>(filepath.c_str()),logfilepath, m_qRes);
    if(res != 1) {
        WriteMsg(logfilepath,-1,"Algorithm executing error!");
        return false;
    }
    return true;
}