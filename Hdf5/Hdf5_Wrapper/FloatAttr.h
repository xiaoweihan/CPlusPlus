/*********************************************************
Copyright(C):
FileName:FloatAttr.h
Descripton:字符串属性的处理
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
	//读属性
	void WriteSelf(H5::DataSet* pElementDataSet) override;

	//写属性
	void WriteSelf(H5::Group* pGroupNode) override;

	//读取Group属性
	void ReadSelf(H5::Attribute* pAttr, InnerType::DATA_ATTRIBUTE& DataAttr) override;

private:
	//维数
	int m_nRank = 0;
	//维度信息
	int* m_pDimensionArray = nullptr;
	//数据类型占用的字节数
	unsigned int m_nDataByte = 0;
	//数据
	void* m_pAttrData = nullptr;

};

