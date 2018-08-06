/*********************************************************
Copyright(C):
FileName:H5Contex.h
Descripton:用于解析h5文件的执行上下文
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#ifndef H5_CONTEX_H
#define H5_CONTEX_H
#include <vector>
#include "Type.h"
class CAbstractH5Strategy;
class CH5Contex
{
public:
	CH5Contex() = default;
	~CH5Contex() = default;

public:
	//设置执行策略
	void SetH5Strategy(CAbstractH5Strategy* pHandler);

	//读取H5
	int ReadH5(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results);

	//写入H5
	int WriteH5(std::vector<InnerType::HDF5_WRITE_DATA>& DataArray);

	//快速读取H5
	int ReadH5_Quick(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results);

	//快速读取局部数据
	int ReadH5_PartialData(const std::vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results);

private:
	CAbstractH5Strategy* m_pStrategyHandler = nullptr;

};
#endif
