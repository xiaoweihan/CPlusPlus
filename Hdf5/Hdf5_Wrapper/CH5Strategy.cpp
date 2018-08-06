/*********************************************************
Copyright(C):
FileName:CH5Strategy.cpp
Descripton:C模式操作h5算法
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "CH5Strategy.h"
#include "ErrorCode.h"

/*********************************************************
FunctionName:ReadH5
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CCH5Strategy::ReadH5(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	if (QueryParams.empty())
	{
		return ERROR_PARAM_INVALID;
	}
	return ERROR_NO_ERROR;
}

/*********************************************************
FunctionName:WriteH5
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CCH5Strategy::WriteH5(std::vector<InnerType::HDF5_WRITE_DATA>& DataArray)
{
	if (DataArray.empty())
	{
		return ERROR_PARAM_INVALID;
	}

	return ERROR_NO_ERROR;
}

int CCH5Strategy::ReadH5_Quick(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	return ERROR_NO_ERROR;
}

int CCH5Strategy::ReadH5_PartialData(const std::vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	return ERROR_NO_ERROR;
}
