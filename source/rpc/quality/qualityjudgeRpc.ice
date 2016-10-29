module RPCQualityJudge {

    sequence<double> DataArray;//第0个是平均值 其后是各波段的值
    dictionary<string, DataArray> DatasMap;//key：指标名称 
    struct QualityInfo {
        int status;	// <0 任务失败;=0任务正在执行;>0总分
        DatasMap imgsquality; //各指标的平均值
    };
    
    struct ImageParameter {
        string filePath;	//图像路径	必须
		string bandIdList;	//波段id | 分割;当key为f2时有效
    };
    
    dictionary<string, ImageParameter> QualityMapArgs;//key: f1:融合前高分辨率文件 f2:融合前多光谱文件 f3:融合后文件
    
    struct QualityInputStruct {
        string id; // task id
        string algorithmkind; // quality algorithm class
        QualityMapArgs inputMap; // wait for judge parameters 
    };

    interface QualityInf {
        //提交同步任务，一直等待结果
        QualityInfo qualitySyn(QualityInputStruct inputArgs);

        //提交异步任务
        int qualityAsyn(QualityInputStruct inputArgs);//提交任务数据, <0 提交失败, >0 估计时间,单位是秒
        //k:id	v:		//任务唯一编号
        QualityInfo fetchQualityRes(string inputArgs);//取任务处理结果
    };
};

