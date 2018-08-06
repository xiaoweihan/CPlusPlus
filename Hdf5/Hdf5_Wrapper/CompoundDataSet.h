/*********************************************************
Copyright(C):
FileName:CompoundDataSet.h
Descripton:���ڸ������͵�dataset�Ķ�д
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
	//��ȡ�ڵ����
	unsigned int GetChildNum(void) override;

	//��������
	int CreateSelfToFile(H5::Group* pGroupNode) override;

	//��������
	int ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData) override;
private:
	//������������
	bool ParseComType(H5::CompType& CompoundDataType, InnerType::HDF5_READ_DATA& DstData);
private:
	//����������Ϣ
	InnerType::LP_DATA_HEADER m_pHeaderInfo = nullptr;
	void* m_pDataSetData = nullptr;
	//ѹ��ϵ��
	int m_nCompressLevel = -1;
};

