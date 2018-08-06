/*********************************************************
Copyright(C):
FileName:StrAttr.h
Descripton:�ַ������ԵĴ���
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#pragma once
#include "AbstractAttr.h"
class CStrAttr :
	public CAbstractAttr
{
public:
	CStrAttr(const std::string& strAttrKeyName,unsigned int nDataByte, void* pData);
	CStrAttr() = default;
	~CStrAttr();

public:
	//������
	void WriteSelf(H5::DataSet* pElementDataSet) override;

	//д����
	void WriteSelf(H5::Group* pGroupNode) override;

	//��ȡDataSet����
	void ReadSelf(H5::Attribute* pAttr, InnerType::DATA_ATTRIBUTE& DataAttr) override;
private:
	//��������ռ�õ��ֽ���
	unsigned int m_nDataByte = 0;
	//����
	void* m_pAttrData = nullptr;
};

