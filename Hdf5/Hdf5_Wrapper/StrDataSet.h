#pragma once
#include "H5AbstractNode.h"
class CStrDataSet : public CH5AbstractNode
{
public:
	CStrDataSet(const std::string& strDataSetKeyName, InnerType::LP_DATA_HEADER pHeader, void* pData, int nCompressLevel = -1);
	CStrDataSet() = default;
	~CStrDataSet();

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
	//数据
	void* m_pDataSetData = nullptr;
	//压缩系数
	int m_nCompressLevel = -1;

};

