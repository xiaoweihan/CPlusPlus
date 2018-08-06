/*********************************************************
Copyright(C):
FileName:FloatAttr.h
Descripton:�ַ������ԵĴ���
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#pragma once
#include "AbstractAttr.h"
class CFloatAttr :
	public CAbstractAttr
{
public:
	CFloatAttr(const std::string& strAttrKeyName, int nRank, int* pDimensionArray, unsigned int nDataByte, void* pData);
	CFloatAttr() = default;
	~CFloatAttr();

public:
	//������
	void WriteSelf(H5::DataSet* pElementDataSet) override;

	//д����
	void WriteSelf(H5::Group* pGroupNode) override;

	//��ȡGroup����
	void ReadSelf(H5::Attribute* pAttr, InnerType::DATA_ATTRIBUTE& DataAttr) override;

private:
	//ά��
	int m_nRank = 0;
	//ά����Ϣ
	int* m_pDimensionArray = nullptr;
	//��������ռ�õ��ֽ���
	unsigned int m_nDataByte = 0;
	//����
	void* m_pAttrData = nullptr;

};

