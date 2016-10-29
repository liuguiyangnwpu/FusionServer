#include "../utils/qualityutils.h"

int32_t Variance(char* filepath1,char* logfilepath, vector<double>& varianceresult) {
	GDALDataset *poDataset;
	GDALAllRegister();
	poDataset=(GDALDataset *)GDALOpen(filepath1,GA_ReadOnly);
	if( poDataset == NULL ) {
		WriteMsg(logfilepath,-1,"Image file open error!");
		return -1;
    } else {
		WriteMsg(logfilepath,0,"Variance algorithm is executing!");
	}

	int32_t n,i,j;
	int32_t bandnum,width,height;
	bandnum=poDataset->GetRasterCount();
	width=poDataset->GetRasterXSize();
	height=poDataset->GetRasterYSize();
	GDALRasterBand *pband;
	uint16_t *banddata;
	banddata=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width*height);

	for(n=0;n<bandnum;n++) {
		pband=poDataset->GetRasterBand(n+1);
		pband->RasterIO(GF_Read,0,0,width,height,banddata,width,height,GDT_UInt16,0,0);
		int64_t count=0;
		double sum=0.0;
		double bandmean=0.0;

		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				if(banddata[i*width+j]>0) {
					sum=sum+banddata[i*width+j];
					count++;
				}
			}
		}
		if (count==0) {
			bandmean=0.0;
		} else {
			bandmean=sum/count;
		}

		count=0;
		sum=0.0;
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				if(banddata[i*width+j]>0) {
					sum +=(banddata[i*width+j]-bandmean)*(banddata[i*width+j]-bandmean);
					count++;
				}
			}
		}

		varianceresult.push_back(1.0*sum/count);

		GDALClose(pband);
		pband=NULL;

		//Writing the process and status of this Algorithm.
		int32_t temp = (int)(100.0*(n+1)/bandnum);
		temp = (temp>99) ? 99:temp;
		WriteMsg(logfilepath,temp,"Variance algorithm is executing!");
	}

	CPLFree(banddata);
	banddata=NULL;

	GDALClose(poDataset);
	poDataset=NULL;
	return 1;
}

bool mainVariance(string& filepath, char* logfilepath, vector<double>& m_qRes) {
    m_qRes.clear();
    int32_t res = Variance(const_cast<char*>(filepath.c_str()),logfilepath, m_qRes);
    if(res != 1) {
        printf("Algorithm executing error!\n");
        WriteMsg(logfilepath,-1,"Algorithm executing error!");
        return false;
    }
    return true;
}
