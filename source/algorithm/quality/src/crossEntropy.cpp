#include "../utils/qualityutils.h"

int32_t CrossEntropy(char* filepath1,char* filepath2,char* logfilepath, vector<double>& CrossEntropyresult) {
	GDALDataset *poDataset1,*poDataset2;
	GDALAllRegister();
	poDataset1=(GDALDataset *)GDALOpen(filepath1,GA_ReadOnly);
	poDataset2=(GDALDataset *)GDALOpen(filepath2,GA_ReadOnly);
	if( (poDataset1 == NULL) || (poDataset2 == NULL) ) {
		WriteMsg(logfilepath,-1,"Image file open error!");

		GDALClose(poDataset1);
		poDataset1=NULL;
		GDALClose(poDataset2);
		poDataset2=NULL;
		return -1;
	} else {
		WriteMsg(logfilepath,0,"CrossEntropy algorithm is executing!");
	}

	int32_t n,i,j;
	int32_t bandnum,width,height;
	if((poDataset1->GetRasterXSize() != poDataset2->GetRasterXSize()) || (poDataset1->GetRasterYSize() != poDataset2->GetRasterYSize())) {
		WriteMsg(logfilepath,-1,"The size of input Images are not match!");
		GDALClose(poDataset1);
		poDataset1=NULL;
		GDALClose(poDataset2);
		poDataset2=NULL;
		return -1;
	}
	bandnum=poDataset2->GetRasterCount();
	width=poDataset2->GetRasterXSize();
	height=poDataset2->GetRasterYSize();
	GDALRasterBand *pband;
	uint16_t *banddata1,*banddata2;
	banddata1=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width*height);
	banddata2=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width*height);

	pband=poDataset1->GetRasterBand(1);
	pband->RasterIO(GF_Read,0,0,width,height,banddata1,width,height,GDT_UInt16,0,0);
	GDALClose(pband);
	pband=NULL;

	uint32_t histogram1[256]={0};
	int64_t count1=0;
	int32_t tempnum=0;
	int32_t max1=0;
	int32_t min1=65535;

	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			tempnum=banddata1[i*width+j];
			if(tempnum>0) {
				if(max1<tempnum)  {max1=tempnum;}
				if(min1>tempnum)  {min1=tempnum;}
			}
		}
	}

	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			tempnum=banddata1[i*width+j];
			if(tempnum>0) {
				int32_t tempindex=(int32_t)(1.0*(tempnum-min1)/(max1-min1)*256);
				histogram1[tempindex]=histogram1[tempindex]+1;
				count1++;
			}
		}
	}

	uint32_t histogram2[256]={0};
	for(n=0;n<bandnum;n++) {
		pband=poDataset2->GetRasterBand(n+1);
		pband->RasterIO(GF_Read,0,0,width,height,banddata2,width,height,GDT_UInt16,0,0);

		memset(histogram2, 0, 256* sizeof(uint32_t));

		int64_t count2=0;
		double sum=0.0;
		int32_t max2=0;
		int32_t min2=65535;

		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				tempnum=banddata2[i*width+j];
				if(tempnum>0) {
					if(max2<tempnum)  {max2=tempnum;}
					if(min2>tempnum)  {min2=tempnum;}
				}
			}
		}
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				tempnum=banddata2[i*width+j];
				if(tempnum>0) {
					int32_t tempindex=(int32_t)(1.0*(tempnum-min2)/(max2-min2)*256);
					histogram2[tempindex]=histogram2[tempindex]+1;
					count2++;
				}
			}
		}

		for(i=0;i<256;i++) {
			if(histogram1[i]==0 || histogram2[i]==0){ continue;}
			sum+=(1.0*histogram1[i]/count1)*log((1.0*histogram1[i]/count1)/(1.0*histogram2[i]/count2))/log(2.0);
		}
		GDALClose(pband);
		pband=NULL;

		CrossEntropyresult.push_back(sum);
		int32_t temp = (int)(100.0*(n+1)/bandnum);
		temp = (temp>99) ? 99:temp;
		WriteMsg(logfilepath,temp,"CrossEntropy algorithm is executing!");
	}

	CPLFree(banddata1);
	banddata1=NULL;
	CPLFree(banddata2);
	banddata2=NULL;

	GDALClose(poDataset1);
	poDataset1=NULL;
	GDALClose(poDataset2);
	poDataset2=NULL;
	return 1;
}

// filePath1 and filePath3 and logfilePath and res
bool mainCrossEntropy(string filepath1, string filepath3, char* logfilepath, vector<double>& m_qRes) {
    m_qRes.clear();
    int32_t res = CrossEntropy(const_cast<char*>(filepath1.c_str()), const_cast<char*>(filepath3.c_str()), logfilepath, m_qRes);
    if(res != 1) {
        WriteMsg(logfilepath,-1,"Algorithm mainCrossEntropy executing error!");
        return false;
    }
    return true;
}

