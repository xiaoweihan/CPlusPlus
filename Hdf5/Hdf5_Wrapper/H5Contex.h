/*********************************************************
Copyright(C):
FileName:H5Contex.h
Descripton:���ڽ���h5�ļ���ִ��������
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
	//����ִ�в���
	void SetH5Strategy(CAbstractH5Strategy* pHandler);

	//��ȡH5
	int ReadH5(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results);

	//д��H5
	int WriteH5(std::vector<InnerType::HDF5_WRITE_DATA>& DataArray);

	//���ٶ�ȡH5
	int ReadH5_Quick(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results);

	//���ٶ�ȡ�ֲ�����
	int ReadH5_PartialData(const std::vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results);

private:
	CAbstractH5Strategy* m_pStrategyHandler = nullptr;

};
#endif
