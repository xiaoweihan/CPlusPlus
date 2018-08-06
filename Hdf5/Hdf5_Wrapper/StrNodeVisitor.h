/*********************************************************
Copyright(C):
FileName:StrNodeVisitor.h
Descripton:字符类型节点访问者
Author:xiaowei.han
Data:2018/02/05
Others:
History:
Version:1.0
*********************************************************/
#pragma once
#include "AbstractNodeVistor.h"
class CStrNodeVisitor :
	public CAbstractNodeVistor
{
	using CAbstractNodeVistor::CAbstractNodeVistor;

public:
	bool Visit(H5::Group* pGroup, std::vector<InnerType::HDF5_READ_DATA>& Results) override;
	bool Visit(H5::DataSet* pDataSet, std::vector<InnerType::HDF5_READ_DATA>& Results) override;
};

