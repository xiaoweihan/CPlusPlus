/*********************************************************
Copyright(C):
FileName:AbstractNodeVistor.h
Descripton:节点抽象访问者
Author:xiaowei.han
Data:2018/02/05
Others:
History:
Version:1.0
*********************************************************/
#pragma once
#include <vector>
#include <boost/unordered_map.hpp>
#include "Type.h"
//前向类声明
namespace H5
{
	class Group;
	class DataSet;
}
class CAbstractNodeVistor
{
public:
	CAbstractNodeVistor(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParamVec);
	CAbstractNodeVistor(const boost::unordered_map<std::string, bool>& QueryParams);
	virtual ~CAbstractNodeVistor();

public:
	virtual bool Visit(H5::Group* pGroup,std::vector<InnerType::HDF5_READ_DATA>& Results) = 0;
	virtual bool Visit(H5::DataSet* pDataSet,std::vector<InnerType::HDF5_READ_DATA>& Results) = 0;


protected:
	void VisitAttr(H5::Group* pGroup, InnerType::HDF5_READ_DATA& NodeData,bool bReadAttr);
	void VisitAttr(H5::DataSet* pDataSet, InnerType::HDF5_READ_DATA& NodeData, bool bReadAttr);

protected:
	//保存的属性信息
	std::vector<InnerType::DATA_ATTRIBUTE> m_AttributeArray;
	//需要保存的变量名称
	boost::unordered_map<std::string,bool> m_QueryParams;
};

