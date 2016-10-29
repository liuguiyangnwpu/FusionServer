/*
//Image Entropy algorithm for geotiff (using GDAL)
//tif图像-影像信息熵
//Author: bangyong Qin
//Date:2015-11-17
*/

#include "../utils/qualityutils.h"

//Entropy-影像信息熵
int32_t Entropy(char* filepath,char* logfilepath, vector<double>& entropyresult) {
	//定义数据集，打开文件
	GDALDataset *poDataset;
	GDALAllRegister();
	poDataset=(GDALDataset *)GDALOpen(filepath,GA_ReadOnly);
    if( poDataset == NULL ) {
		printf("Image file open error!\n");
		WriteMsg(logfilepath,-1,"Image file open error!");
		return -1; 
    } else {
		printf("Entropy algorithm is executing!\n");
		WriteMsg(logfilepath,0,"Entropy algorithm is executing!");
	}

	//开始解析图像数据集
	int32_t n,i,j;
	int32_t bandnum,width,height;
	bandnum=poDataset->GetRasterCount(); 
	width=poDataset->GetRasterXSize();
	height=poDataset->GetRasterYSize();
	GDALRasterBand *pband;
	uint16_t *banddata;
	banddata=(uint16_t *)CPLMalloc(sizeof(uint16_t)*width*height);

	//loop for every bands
    for(n=0;n<bandnum;n++) {
		pband=poDataset->GetRasterBand(n+1);
		pband->RasterIO(GF_Read,0,0,width,height,banddata,width,height,GDT_UInt16,0,0);

		//define local variable//定义局部变量
		uint32_t histogram[65535]={0};	
		int64_t count=0;
		double sum=0.0;
		int32_t tempnum=0;
		
		//统计灰度概率
        for(i=0;i<height;i++) {
            for(j=0;j<width;j++) {
				tempnum =banddata[i*width+j];
                if(tempnum>0) {
					histogram[tempnum-1]=histogram[tempnum-1]+1;
					count++;
				}
			}
		}

		//计算信息熵
		if (count>0) {
            for(i=0;i<65535;i++) {
                if(histogram[i]>0) {
					double tempratio=histogram[i]/(double)count;
					sum=sum-tempratio*log(tempratio)/log(2.0);
				}
			}
			entropyresult.push_back(sum);
        } else {
			entropyresult.push_back(0);
		}

		GDALClose(pband);
		pband=NULL;

		//Writing the process and status of this Algorithm.
		int32_t temp = (int)(100.0*(n+1)/bandnum);
		temp = (temp>99) ? 99:temp;
		printf("Entropy algorithm is executing %d%%!\n",temp);
		WriteMsg(logfilepath,temp,"Entropy algorithm is executing!");
	}

	//释放内存，关闭数据集
	CPLFree(banddata);
	banddata=NULL;

	GDALClose(poDataset);
	poDataset=NULL;

	return 1;
}

//主函数
bool mainEntropy(string& filepath, char* logfilepath, vector<double>& m_qRes) {
    m_qRes.clear();
    int32_t res = Entropy(const_cast<char*>(filepath.c_str()), logfilepath, m_qRes);
    if(res != 1) {
        WriteMsg(logfilepath,-1,"Algorithm executing error!");
        return false;
    }
    return true;
}
