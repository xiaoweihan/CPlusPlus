/*********************************************************
Copyright(C):
FileName:CompoundDataSet.h
Descripton:用于复合类型的dataset的读写
Author:xiaowei.han
Data:2018/02/07
Others:
History:
Version:1.0
*********************************************************/
#pragma once
#include <vector>
#include "H5AbstractNode.h"
#include "Type.h"
class CCompoundDataSet : public CH5AbstractNode
{
public:
	CCompoundDataSet(const std::string& strDataSetKeyName, InnerType::LP_DATA_HEADER pHeader, void* pData, int nCompressLevel = -1);
	CCompoundDataSet() = default;
	~CCompoundDataSet();
public:
	//获取节点个数
	unsigned int GetChildNum(void) override;

	//创建自身
	int CreateSelfToFile(H5::Group* pGroupNode) override;

	//解析自身
	int ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData) override;
private:
	//解析复合类型
	bool ParseComType(H5::CompType& CompoundDataType, InnerType::HDF5_READ_DATA& DstData);
private:
	//数据描述信息
	InnerType::LP_DATA_HEADER m_pHeaderInfo = nullptr;
	void* m_pDataSetData = nullptr;
	//压缩系数
	int m_nCompressLevel = -1;
};

