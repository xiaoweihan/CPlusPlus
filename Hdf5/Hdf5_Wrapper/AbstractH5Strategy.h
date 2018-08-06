/*********************************************************
Copyright(C):
FileName:AbstractH5Strategy.h
Descripton:�������ģʽ
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
	//��ȡH5
	virtual int ReadH5(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams,std::vector<InnerType::HDF5_READ_DATA>& Results) = 0;
	//д��h5
	virtual int WriteH5(std::vector<InnerType::HDF5_WRITE_DATA>& DataArray) = 0;
	//���ٶ�ȡH5
	virtual int ReadH5_Quick(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results) = 0;
	//�����ȡsubset��Ϣ
	virtual int ReadH5_PartialData(const std::vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results) = 0;

protected:
	//�ݹ鴴����ؽڵ�
	CH5AbstractNode* RecursiveCreateNode(const InnerType::LP_HDF5_WRITE_DATA pData);

protected:
	//HDF5�ļ����ڵ�·��
	std::string m_strFilePath;
};

