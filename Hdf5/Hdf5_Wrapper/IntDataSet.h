/*********************************************************
Copyright(C):
FileName:H5DataSetNode.h
Descripton:组合模式叶子节点的类
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#ifndef INT_DATA_SET_H
#define INT_DATA_SET_H
#include "H5AbstractNode.h"
class CIntDataSet:
	public CH5AbstractNode
{
public:
	CIntDataSet(const std::string& strDataSetKeyName,InnerType::LP_DATA_HEADER pHeader,void* pData,int nCompressLevel = -1);
	CIntDataSet() = default;
	~CIntDataSet();

public:
	//获取节点个数
	unsigned int GetChildNum(void) override;
	//创建自身
	int CreateSelfToFile(H5::Group* pGroupNode) override;
	//解析自身
	int ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData) override;
	//读取自身超块数据
	int ReadPartialData(H5::DataSet* pDataset, const InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM& QueryParam, InnerType::HDF5_READ_DATA& DstData) override;
private:
	//数据描述信息
	InnerType::LP_DATA_HEADER m_pHeaderInfo = nullptr;
	//压缩系数
	int m_nCompressLevel = -1;
	//数据
	void* m_pData = nullptr;
};

#endif