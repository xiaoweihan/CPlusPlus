#pragma once
#include "H5AbstractNode.h"
class CStrDataSet : public CH5AbstractNode
{
public:
	CStrDataSet(const std::string& strDataSetKeyName, InnerType::LP_DATA_HEADER pHeader, void* pData, int nCompressLevel = -1);
	CStrDataSet() = default;
	~CStrDataSet();

public:
	//��ȡ�ڵ����
	unsigned int GetChildNum(void) override;

	//��������
	int CreateSelfToFile(H5::Group* pGroupNode) override;

	//��������
	int ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData) override;


	//��ȡ����������
	int ReadPartialData(H5::DataSet* pDataset, const InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM& QueryParam, InnerType::HDF5_READ_DATA& DstData) override;
private:
	//����������Ϣ
	InnerType::LP_DATA_HEADER m_pHeaderInfo = nullptr;
	//����
	void* m_pDataSetData = nullptr;
	//ѹ��ϵ��
	int m_nCompressLevel = -1;

};

