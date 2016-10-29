#include "../utils/qualityutils.h"

int32_t StructureSimilarity(char* filepath1, char* filepath3, char* logfilepath, vector<double>& StructureSimilarityresult) {
	GDALDataset *poDataset1,*poDataset3;
	GDALAllRegister();
	poDataset1=(GDALDataset *)GDALOpen(filepath1,GA_ReadOnly);
	poDataset3=(GDALDataset *)GDALOpen(filepath3,GA_ReadOnly);
	if( (poDataset1 == NULL) || (poDataset3 == NULL) ) {
		printf("Image file open error!\n");
		WriteMsg(logfilepath,-1,"Image file open error!");

		GDALClose(poDataset1);
		poDataset1=NULL;
		GDALClose(poDataset3);
		poDataset3=NULL;
		return -1;
	} else {
		WriteMsg(logfilepath,0,"StructureSimilarity algorithm is executing!");
	}

	int32_t n,i,j;
	int32_t bandnum,width,height;
	if((poDataset1->GetRasterXSize() != poDataset3->GetRasterXSize())
       || (poDataset1->GetRasterYSize() != poDataset3->GetRasterYSize())) {
		WriteMsg(logfilepath,-1,"The size of input Images are not match!");
		GDALClose(poDataset1);
		poDataset1=NULL;
		GDALClose(poDataset3);
		poDataset3=NULL;
		return -1;
	}
	bandnum=poDataset3->GetRasterCount();
	width=poDataset3->GetRasterXSize();
	height=poDataset3->GetRasterYSize();
	GDALRasterBand *pband;
	uint16_t *banddata1,*banddata3;
	banddata1=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width*height);
	banddata3=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width*height);

	pband=poDataset1->GetRasterBand(1);
	pband->RasterIO(GF_Read,0,0,width,height,banddata1,width,height,GDT_UInt16,0,0);
	GDALClose(pband);
	pband=NULL;

	double mean1=0.0;
	long double variance1=0.0;
	int64_t count1=0;
	int64_t sum1=0;
	int32_t tempnum1=0;

	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			tempnum1=banddata1[i*width+j];
			if(tempnum1>0) {
				sum1=sum1+tempnum1;
				count1++;
			}
		}
	}
	mean1=(1.0*sum1)/count1;

	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			tempnum1=banddata1[i*width+j];
			if(tempnum1>0) {
				variance1+=(tempnum1-mean1)*(tempnum1-mean1);
			}
		}
	}
	variance1=variance1/count1;

	for(n=0;n<bandnum;n++) {
		pband=poDataset3->GetRasterBand(n+1);
		pband->RasterIO(GF_Read,0,0,width,height,banddata3,width,height,GDT_UInt16,0,0);
		GDALClose(pband);
		pband=NULL;

		double mean3=0.0;
		long double  variance3=0.0;
		long double variance13=0.0;
		int64_t count3=0;
		int64_t sum3=0;
		int32_t tempnum3=0;

		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				tempnum3=banddata3[i*width+j];
				if(tempnum3>0) {
					sum3=sum3+tempnum3;
					count3++;
				}
			}
		}
		mean3=(1.0*sum3)/count3;
		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				tempnum3=banddata3[i*width+j];
				if(tempnum3>0) {
					variance3+=(tempnum3-mean3)*(tempnum3-mean3);
				}
			}
		}
		variance3=variance3/count3;

		for(i=0;i<height;i++) {
			for(j=0;j<width;j++) {
				tempnum1=banddata1[i*width+j];
				tempnum3=banddata3[i*width+j];
				if(tempnum1>0 && tempnum3>0) {
					variance13+=(tempnum1-mean1)*(tempnum3-mean3);
				}
			}
		}
		variance13=variance13/count3;

		double tempdbl1=(2*variance13+0.000001)*(2*mean1*mean3+0.000001);
		double tempdbl2=variance1+variance3+0.000001;
		double tempdbl3=mean1*mean1+mean3*mean3+0.000001;

		StructureSimilarityresult.push_back(fabs((tempdbl1/(tempdbl2*tempdbl3))));
		//Writing the process and status of this Algorithm.
		int32_t temp = (int)(100.0*(n+1)/bandnum);
		temp = (temp>99) ? 99:temp;
		WriteMsg(logfilepath,temp,"StructureSimilarity algorithm is executing!");
	}

	CPLFree(banddata1);
	banddata1=NULL;
	CPLFree(banddata3);
	banddata3=NULL;

	GDALClose(poDataset1);
	poDataset1=NULL;
	GDALClose(poDataset3);
	poDataset3=NULL;
	return 1;
}

bool mainStructureSimilarity(string filepath1, string filepath3, char* logfile, vector<double>& m_qRes) {
    m_qRes.clear();
    int32_t res = StructureSimilarity(const_cast<char*>(filepath1.c_str()), const_cast<char*>(filepath3.c_str()), logfile, m_qRes);
    if(res != 1) {
        printf("Algorithm executing error!\n");
        WriteMsg(logfile, -1, "Algorithm executing error!");
        return false;
    }
    return true;
}