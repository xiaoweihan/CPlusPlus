/*********************************************************
Copyright(C):
FileName:AbstractNodeVistor.cpp
Descripton:节点抽象访问者
Author:xiaowei.han
Data:2018/02/05
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "AbstractNodeVistor.h"
#include <H5Cpp.h>
#include <memory>
#include "AttrFactory.h"
#include "Utility.h"
#include "AbstractAttr.h"
CAbstractNodeVistor::CAbstractNodeVistor(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParamVec)
{

	for (auto& Element : QueryParamVec)
	{
		m_QueryParams.emplace(Element.strQueryKeyName, Element.bQueryAttr);
	}

}


CAbstractNodeVistor::CAbstractNodeVistor(const boost::unordered_map<std::string, bool>& QueryParams)
{
	m_QueryParams = QueryParams;
}

CAbstractNodeVistor::~CAbstractNodeVistor()
{
}

void CAbstractNodeVistor::VisitAttr(H5::Group* pGroup, InnerType::HDF5_READ_DATA& NodeData, bool bReadAttr)
{
	using namespace H5;
	if (nullptr == pGroup)
	{
		return;
	}

	//如果需要查询属性
	if (bReadAttr)
	{
		//获取属性的个数
		int nAttrNum = (int)pGroup->getNumAttrs();
		//处理属性信息
		for (int i = 0; i < nAttrNum; ++i)
		{
			auto Attr = pGroup->openAttribute(i);
			auto pHandler = AttrFactory::CreateAttr(Attr.getTypeClass());
			if (nullptr != pHandler)
			{
				InnerType::DATA_ATTRIBUTE TempAttr;
				pHandler->ReadSelf(&Attr, TempAttr);
				NodeData.AttributeArray.push_back(std::move(TempAttr));
			}
			Attr.close();
		}
	}
}

void CAbstractNodeVistor::VisitAttr(H5::DataSet* pDataSet, InnerType::HDF5_READ_DATA& NodeData, bool bReadAttr)
{
	using namespace H5;
	if (nullptr == pDataSet)
	{
		return;
	}

	//如果需要查询属性
	if (bReadAttr)
	{
		//获取属性的个数
		int nAttrNum = (int)pDataSet->getNumAttrs();
		//处理属性信息
		for (int i = 0; i < nAttrNum; ++i)
		{
			auto Attr = pDataSet->openAttribute(i);
			auto pHandler = AttrFactory::CreateAttr(Attr.getTypeClass());
			if (nullptr != pHandler)
			{
				InnerType::DATA_ATTRIBUTE TempAttr;
				pHandler->ReadSelf(&Attr, TempAttr);
				NodeData.AttributeArray.push_back(std::move(TempAttr));
			}
			Attr.close();
		}
	}
}
