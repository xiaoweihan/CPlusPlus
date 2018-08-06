/*********************************************************
Copyright(C):
FileName:AbstractH5Strategy.h
Descripton:抽象策略模式
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#pragma once
#include <string>
#include <vector>
#include "Type.h"
class CH5AbstractNode;
class CAbstractH5Strategy
{
public:
	CAbstractH5Strategy(const std::string& strFilePath);
	virtual ~CAbstractH5Strategy();
public:
	//读取H5
	virtual int ReadH5(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams,std::vector<InnerType::HDF5_READ_DATA>& Results) = 0;
	//写入h5
	virtual int WriteH5(std::vector<InnerType::HDF5_WRITE_DATA>& DataArray) = 0;
	//快速读取H5
	virtual int ReadH5_Quick(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results) = 0;
	//快读读取subset信息
	virtual int ReadH5_PartialData(const std::vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results) = 0;

protected:
	//递归创建相关节点
	CH5AbstractNode* RecursiveCreateNode(const InnerType::LP_HDF5_WRITE_DATA pData);

protected:
	//HDF5文件所在的路径
	std::string m_strFilePath;
};

