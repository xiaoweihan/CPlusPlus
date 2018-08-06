/*********************************************************
Copyright(C):
FileName:H5DataSetNode.h
Descripton:���ģʽҶ�ӽڵ����
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
	//ѹ��ϵ��
	int m_nCompressLevel = -1;
	//����
	void* m_pData = nullptr;
};

#endif