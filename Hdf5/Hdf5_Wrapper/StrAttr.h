/*********************************************************
Copyright(C):
FileName:StrAttr.h
Descripton:字符串属性的处理
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
	//读属性
	void WriteSelf(H5::DataSet* pElementDataSet) override;

	//写属性
	void WriteSelf(H5::Group* pGroupNode) override;

	//读取DataSet属性
	void ReadSelf(H5::Attribute* pAttr, InnerType::DATA_ATTRIBUTE& DataAttr) override;
private:
	//数据类型占用的字节数
	unsigned int m_nDataByte = 0;
	//数据
	void* m_pAttrData = nullptr;
};

