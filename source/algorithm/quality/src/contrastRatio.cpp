/*
//Image ContrastRatio algorithm for geotiff (using GDAL)
//tif图像-影像对比度
//Author: bangyong Qin
//Date:2015-11-17
*/

#include "../utils/qualityutils.h"

//GLCM-Gray level Co-occurrence Matrix-灰度共生矩阵函数
//step为距离,orient为方向,gsp为灰度采样级数
int32_t GLCM(uint16_t *banddata,int32_t width,int32_t height,int32_t step,int32_t orient,int32_t gsp,float* GLCMresult) {
	//定义变量
	int32_t i,j;
	int32_t tempvalue;
	int32_t maxvalue,minvalue;
	maxvalue=0;
	minvalue=65535;

	//遍历数据集，寻找最大最小值
    for(i=0;i<height;i++) {
        for(j=0;j<width;j++) {
			tempvalue=*(banddata+i*width+j);
            if(tempvalue>0) {
                if(tempvalue>maxvalue) {
					maxvalue=tempvalue;
				}
                if(tempvalue<minvalue) {
					minvalue=tempvalue;
				}
			}
		}
    }

	//对比最大最小值，判断像素值是否有效
	if(minvalue>=maxvalue){
		return 0;
	}

	//按照灰度采样级数进行采样
	float temprange=((float)(maxvalue-minvalue+1))/gsp;
	int32_t di,dj;//记录像素在灰度共生矩阵中的x、y坐标
	int64_t count=0;
	int32_t tempdata1,tempdata2;
	
	//定义临时数组并初始化
	uint32_t *tempcount=(uint32_t *)CPLMalloc(sizeof(uint32_t)*gsp*gsp);
	for(i=0;i<gsp;i++){
		for(j=0;j<gsp;j++){
			tempcount[i*gsp+j]=0;
		}
	}
	
	switch(orient){
	case 0:	
        for(i=0;i<height;i++) {
            for(j=0;j<width-step;j++) {
				tempdata1=*(banddata+i*width+j);
				tempdata2=*(banddata+i*width+j+step);
                if(tempdata1<=0||tempdata2<=0)
                    continue;
				di=(int32_t)((tempdata1-minvalue)/temprange);
				dj=(int32_t)((tempdata2-minvalue)/temprange);
				(*(tempcount+di*gsp+dj))++;
				count++;
			}
		}
		break;
	case 45:
        for(i=step;i<height;i++) {
            for(j=0;j<width-step;j++) {
				tempdata1=*(banddata+i*width+j);
				tempdata2=*(banddata+(i-step)*width+j+step);
                if(tempdata1<=0||tempdata2<=0)
                    continue;
				di=(int32_t)((tempdata1-minvalue)/temprange);
				dj=(int32_t)((tempdata2-minvalue)/temprange);
				(*(tempcount+di*gsp+dj))++;
				count++;
			}
		}
		break;
	case 90:
        for(i=step;i<height;i++) {
            for(j=0;j<width;j++) {
				tempdata1=*(banddata+i*width+j);
				tempdata2=*(banddata+(i-step)*width+j);
                if(tempdata1<=0||tempdata2<=0)
                    continue;
				di=(int32_t)((tempdata1-minvalue)/temprange);
				dj=(int32_t)((tempdata2-minvalue)/temprange);
				(*(tempcount+di*gsp+dj))++;
				count++;
			}
		}
		break;
	case 135:
        for(i=step;i<height;i++) {
            for(j=step;j<width;j++) {
				tempdata1=*(banddata+i*width+j);
				tempdata2=*(banddata+(i-step)*width+j-step);
                if(tempdata1<=0||tempdata2<=0)
                    continue;
				di=(int32_t)((tempdata1-minvalue)/temprange);
				dj=(int32_t)((tempdata2-minvalue)/temprange);
				(*(tempcount+di*gsp+dj))++;
				count++;
			}
		}
		break;
	default:
        for(i=0;i<height;i++) {
            for(j=0;j<width-step;j++) {
				tempdata1=*(banddata+i*width+j);
				tempdata2=*(banddata+i*width+j+step);
                if(tempdata1<=0||tempdata2<=0)
                    continue;
				di=(int32_t)((tempdata1-minvalue)/temprange);
				dj=(int32_t)((tempdata2-minvalue)/temprange);
				(*(tempcount+di*gsp+dj))++;
				count++;
			}
		}
		break;
	}
	for(i=0;i<gsp;i++){
		for(j=0;j<gsp;j++){
			*(GLCMresult+i*gsp+j)=(float)(*(tempcount+i*gsp+j))*1.0f/count;
		}
	}

	//释放内存
	CPLFree(tempcount);
	tempcount=NULL;

	return 1;
}

//ContrastRatio-影像对比度
int32_t ContrastRatio(char* filepath,char* logfilepath, vector<double>& contrastresult) {
	//定义数据集，打开文件
    GDALDataset *poDataset = NULL;
	GDALAllRegister();
	poDataset=(GDALDataset *)GDALOpen(filepath,GA_ReadOnly);
    if( poDataset == NULL ) {
		printf("Image file open error!\n");
		WriteMsg(logfilepath,-1,"Image file open error!");
		return -1; 
    } else {
		printf("ContrastRatio algorithm is executing!\n");
		WriteMsg(logfilepath,0,"ContrastRatio algorithm is executing!");
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
		double sum=0.0;
		float tempnum=0;

		//定义并初始化灰度共生矩阵的参数
		int32_t orient=0;
		int32_t step=1;
		int32_t gsp=16;

		//为灰度共生矩阵申请内存并初始化(为0)
		float* GLCMresult=(float *)CPLMalloc(sizeof(float)*gsp*gsp);
		for(i=0;i<gsp;i++)	{
			for(j=0;j<gsp;j++)	{
				*(GLCMresult+i*gsp+j)=0;
			}
		}

		//调用函数，生成灰度共生矩阵
        GLCM(banddata,width,height,step,orient,gsp,GLCMresult);
		
		//统计灰度共生矩阵的概率
        for(i=0;i<gsp;i++) {
            for(j=0;j<gsp;j++) {
				tempnum=*(GLCMresult+i*gsp+j);
                if(i!=j&&tempnum>0) {
					sum=sum+(i-j)*(i-j)*tempnum;
				}
			}
		}
		//计算对比度
		contrastresult.push_back(sum);
		//释放内存
		CPLFree(GLCMresult);
		GLCMresult=NULL;
		//关闭数据集
		GDALClose(pband);
		pband=NULL;

		//Writing the process and status of this Algorithm.
		int32_t temp = (int32_t)(100.0*(n+1)/bandnum);
		temp = (temp>99) ? 99:temp;
		WriteMsg(logfilepath,temp,"ContrastRatio algorithm is executing!");
	}

	//释放内存，关闭数据集
	CPLFree(banddata);
	banddata=NULL;

	GDALClose(poDataset);
	poDataset=NULL;

	return 1;
}

//主函数
bool mainContrastRatio(string& filepath, char* logfilepath, vector<double>& m_qRes) {
    m_qRes.clear();
    int32_t res = ContrastRatio(const_cast<char*>(filepath.c_str()),logfilepath, m_qRes);
    if(res != 1) {
        printf("Algorithm executing error!\n");
        WriteMsg(logfilepath,-1,"Algorithm executing error!");
        return false;
    }
    return true;
}
