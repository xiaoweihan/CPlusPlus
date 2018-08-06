/*********************************************************
Copyright(C):
FileName:AbstractAttr.h
Descripton:brief
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#pragma once
#include <H5Cpp.h>
#include <string>
#include "Type.h"
class CAbstractAttr
{
public:
	CAbstractAttr(const std::string& strAttrKeyName);
	CAbstractAttr() = default;
	virtual ~CAbstractAttr();


public:
	//写属性
	virtual void WriteSelf(H5::DataSet* pElementDataSet) = 0;

	//写属性
	virtual void WriteSelf(H5::Group* pGroupNode) = 0;

	//读取Group属性
	virtual void ReadSelf(H5::Attribute* pAttr,InnerType::DATA_ATTRIBUTE& DataAttr) = 0;


protected:
	std::string m_strAttrKeyName;
};

