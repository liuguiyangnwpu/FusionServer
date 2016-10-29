/*
主要是用来设计海量图像检索的RPC调用接口
*/

module RPCWiseFuse {
    struct FusionInf{
	//数据库写入信息
	double	cnttimeuse;				//所用时间
	string	producetime;			//产品生成时间
	double	ulcoorvalidLongitude;	//有效融合区域左上角经度坐标
	double	ulcoorvalidLatitude ;	//有效融合区域左上角纬度坐标
	double	brcoorvalidLongitude;	//有效融合区域右下角经度坐标
	double	brcoorvalidLatitude ;	//有效融合区域右下角纬度坐标
	double	ulcoorwholeLongitude;	//全局左上角经度坐标
	double	ulcoorwholeLatitude ;	//全局左上角纬度坐标 
	double	brcoorwholeLongitude;	//全局右下角经度坐标
	double	brcoorwholeLatitude ;	//全局右下角纬度坐标
	float	resolution;				//融合后产品分辨率
	string	productFormat;			//产品格式
	string	projectiontype;			//投影类型
	string	datumname;				//投影椭球体
	string	projectioncode;			//投影编码
	string	projectionunits;		//投影单位
	float	projcentralmeridian;	//投影带中央经线 
	int		status;					//任务执行状态	
};


    dictionary<string, string> DirArgs;

    interface WiseFusionInf {
    	//提交同步任务，一直等待结果
    	//--------geneThumbImg----
    	//inPath	:输入图像的服务器绝对路径，包括文件名
    	//outPath	:输出图像的服务器绝对路径，包括文件名
    	//bandlist	:波段	波段编号1|波段编号1|波段编号1
    	bool geneThumbImg(string inPath, string outPath, string bandlist);
    	
    	//--------fuseSyn----mapArgs-----
    	//k:id	v:		//任务唯一编号
    	//k:panurl	v:	//图像1地址
    	//k:msurl	v:	//图像2地址
    	//k:outurl	v:	//输出图像地址
    	//k:algname	v:	//算法名称 "数据的name"+"_"+"数据库的version"
    	//k:band	v:	//波段	波段编号1|波段编号1|波段编号1
    	//k:intername	v:	//插值算名称 "数据的name"+"_"+"数据库的version"
    	FusionInf fuseSyn(DirArgs mapArgs); 
    	
    	//提交异步任务
    	//--------fuseAsyn----mapArgs-----
    	//k:id	v:		//任务唯一编号
    	//k:panurl	v:	//图像1地址
    	//k:msurl	v:	//图像2地址
    	//k:outurl	v:	//输出图像地址
    	//k:algname	v:	//算法名称 "数据的name"+"_"+"数据库的version"
    	//k:band	v:	//波段	波段编号1|波段编号1|波段编号1
    	//k:intername	v:	//插值算名称 "数据的name"+"_"+"数据库的version"
        int fuseAsyn(DirArgs mapArgs);//提交任务数据
        //--------fuseAsyn----mapArgs-----
    	//k:id	v:		//任务唯一编号
        string askProcess(DirArgs mapArgs);//询问处理进度
        //--------fuseAsyn----mapArgs-----
    	//k:id	v:		//任务唯一编号
        FusionInf fetchFuseRes(DirArgs mapArg);//取任务处理结果
    };
};
