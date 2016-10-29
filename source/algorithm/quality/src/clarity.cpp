/*
//Image Clarity algorithm for geotiff (using GDAL)
//tif图像-清晰度算法
//Author: bangyong Qin
//Date:2015-11-17
*/

#include "../utils/qualityutils.h"

//Clarity-影像清晰度（点锐度算法）
int32_t Clarity(char* filepath,char* logfilepath, vector<double>& result) {
	GDALDataset *poDataset = NULL;
	GDALAllRegister();
	poDataset=(GDALDataset *)GDALOpen(filepath,GA_ReadOnly);
    if(poDataset == NULL) {
		printf("Image file open error!\n");
		WriteMsg(logfilepath,-1,"Image file open error!");
		return -1; 
    } else {
		printf("Clarity algorithm is executing!\n");
		WriteMsg(logfilepath,0,"Clarity algorithm is executing!");
	}

	//开始解析图像数据集
	int32_t n,i,j;
	int32_t bandnum,width,height;
	bandnum=poDataset->GetRasterCount(); 
	width=poDataset->GetRasterXSize();
	height=poDataset->GetRasterYSize();
	GDALRasterBand *pband = NULL;
	uint16_t *banddata=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width*height);

    for(n=0;n<bandnum;n++) {
		pband=poDataset->GetRasterBand(n+1);
		pband->RasterIO(GF_Read,0,0,width,height,banddata,width,height,GDT_UInt16,0,0);
		double clarity=0.0;
		int32_t blkcount=0;

        for(i=1;i<height-1;i++) {
            for(j=1;j<width-1;j++) {
                if(banddata[i*width+j]==0)
                    continue;
				double colmean=0.0;
				colmean += fabs(1.0*banddata[(i-1)*width+(j-1)]-banddata[i*width+j])/sqrt(2.0);
				colmean += fabs(1.0*banddata[(i-1)*width+j]-banddata[i*width+j]);
				colmean += fabs(1.0*banddata[(i-1)*width+(j+1)]-banddata[i*width+j])/sqrt(2.0);
				colmean += fabs(1.0*banddata[i*width+(j-1)]-banddata[i*width+j]);
				colmean += fabs(1.0*banddata[i*width+(j+1)]-banddata[i*width+j]);
				colmean += fabs(1.0*banddata[(i+1)*width+(j-1)]-banddata[i*width+j])/sqrt(2.0);
				colmean += fabs(1.0*banddata[(i+1)*width+j]-banddata[i*width+j]);
				colmean += fabs(1.0*banddata[(i+1)*width+(j+1)]-banddata[i*width+j])/sqrt(2.0);
				colmean = colmean/8.0;
				clarity +=colmean;
				blkcount++;
			}
		}
        result.push_back(clarity/blkcount);
		GDALClose(pband);
		pband=NULL;

		//Writing the process and status of this Algorithm.
		int32_t temp = (int)(100.0*(n+1)/bandnum);
		temp = (temp>99) ? 99:temp;
		printf("Clarity algorithm is executing %d%%!\n",temp);
		WriteMsg(logfilepath,temp,"Clarity algorithm is executing!");
    }

	CPLFree(banddata);
	banddata=NULL;

	GDALClose(poDataset);
	poDataset=NULL;

	return 1;
}

bool mainClarity(string& filepath , char* logfilepath, vector<double>& m_qRes) {
    m_qRes.clear();
    int32_t res = Clarity(const_cast<char*>(filepath.c_str()),logfilepath, m_qRes);
	if(res != 1) {
		printf("Algorithm executing error!\n");
		WriteMsg(logfilepath,-1,"Algorithm executing error!");
        return false;
	}
    return true;
}
