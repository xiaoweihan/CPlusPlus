/*********************************************************
Copyright(C):
FileName:CPPH5Strategy.h
Descripton:C++ģʽ����h5�㷨
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#pragma once
#include <vector>
#include <string>
#include "Type.h"
#include "AbstractH5Strategy.h"
class CCppH5Strategy :
	public CAbstractH5Strategy
{

	using CAbstractH5Strategy::CAbstractH5Strategy;

public:
	//��ȡH5
	int ReadH5(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results) override;
	//д��h5
	int WriteH5(std::vector<InnerType::HDF5_WRITE_DATA>& DataArray) override;
	//���ٶ�ȡH5
	int ReadH5_Quick(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results) override;
	//���ٶ�ȡH5��������
	int ReadH5_PartialData(const std::vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results) override;
};

