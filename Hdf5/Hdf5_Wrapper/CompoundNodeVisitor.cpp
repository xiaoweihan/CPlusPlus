/*********************************************************
Copyright(C):
FileName:CompoundNodeVisitor.cpp
Descripton:复合类型节点访问者
Author:xiaowei.han
Data:2018/02/05
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "CompoundNodeVisitor.h"
#include <H5Cpp.h>
#include "CompoundDataSet.h"
bool CCompoundNodeVisitor::Visit(H5::Group* pGroup, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	if (nullptr == pGroup)
	{
		return false;
	}

	return true;
}

bool CCompoundNodeVisitor::Visit(H5::DataSet* pDataSet, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	if (nullptr == pDataSet || m_QueryParams.empty())
	{
		return false;
	}

	//判断查询的变量名是否存在
	auto strDataSetName = pDataSet->getObjName();
	auto Iter = m_QueryParams.find(strDataSetName);
	//不存在则返回了
	if (Iter == m_QueryParams.end())
	{
		return false;
	}
	//解析属性
	InnerType::HDF5_READ_DATA DataSetData;
	DataSetData.strKeyName = strDataSetName;
	DataSetData.eumNodeType = InnerType::NODE_TYPE::NODE_LEAF;
	VisitAttr(pDataSet, DataSetData, Iter->second);
	m_QueryParams.erase(Iter);
	//查询Dataset数据
	CCompoundDataSet* pDatasetReader = new CCompoundDataSet;

	if (nullptr != pDatasetReader)
	{
		if (!pDatasetReader->ParseSelf(pDataSet, DataSetData))
		{
			//放到结果集中
			Results.push_back(std::move(DataSetData));
		}
	}
	DELETE_POINTER(pDatasetReader);
	return true;
}