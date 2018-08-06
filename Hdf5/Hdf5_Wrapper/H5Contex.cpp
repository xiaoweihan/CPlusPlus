/*********************************************************
Copyright(C):
FileName:H5Contex.cpp
Descripton:用于解析h5文件的执行上下文
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "H5Contex.h"
#include "AbstractH5Strategy.h"
#include "ErrorCode.h"

/*********************************************************
FunctionName:SetH5Strategy
FunctionDesc:设置执行算法
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
void CH5Contex::SetH5Strategy(CAbstractH5Strategy* pHandler)
{
	m_pStrategyHandler = pHandler;
}

/*********************************************************
FunctionName:ReadH5
FunctionDesc:读取hdf5文件
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CH5Contex::ReadH5(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	if (nullptr == m_pStrategyHandler || QueryParams.empty())
	{
		return ERROR_PARAM_INVALID;
	}

	return m_pStrategyHandler->ReadH5(QueryParams,Results);
}

/*********************************************************
FunctionName:WriteH5
FunctionDesc:写hdf5文件
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CH5Contex::WriteH5(std::vector<InnerType::HDF5_WRITE_DATA>& DataArray)
{
	if (nullptr == m_pStrategyHandler)
	{
		return ERROR_PARAM_INVALID;
	}

	return m_pStrategyHandler->WriteH5(DataArray);
}

int CH5Contex::ReadH5_Quick(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	if (nullptr == m_pStrategyHandler || QueryParams.empty())
	{
		return ERROR_PARAM_INVALID;
	}

	return m_pStrategyHandler->ReadH5_Quick(QueryParams, Results);
}

int CH5Contex::ReadH5_PartialData(const std::vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	if (nullptr == m_pStrategyHandler || QueryParams.empty())
	{
		return ERROR_PARAM_INVALID;
	}

	return m_pStrategyHandler->ReadH5_PartialData(QueryParams, Results);
}
