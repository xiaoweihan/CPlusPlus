/*******************************************************************************
FileName:AbstractDataAdapter.h
FileDescription:数据转换抽象类
CopyRight:
Author:xiaowei_han
Date:2018/02/03
Histrory:V0.1
*******************************************************************************/
#pragma once
#include <vector>
#include "Type.h"
#include "Hdf5_Wrapper.h"
class CAbstractDataAdapter
{
public:
	CAbstractDataAdapter();
	virtual ~CAbstractDataAdapter();

public:
	bool AdapterData(const Hdf5_Wrapper::LP_HDF5_DATA pSrcData, InnerType::LP_HDF5_WRITE_DATA pDstData);
protected:
	virtual bool AdapterDataHeader(const Hdf5_Wrapper::DATA_HEADER& SrcDataHeader, InnerType::DATA_HEADER& DstDataHeader) = 0;
protected:
	//转换数据类型
	bool AdapterDataType(Hdf5_Wrapper::DATA_TYPE eumSrcDataType, InnerType::DATA_TYPE& eumDstDataType);
	//转换数据属性
	void AdapterAttr(const std::vector<Hdf5_Wrapper::DATA_ATTRIBUTE>& SrcArray, std::vector<InnerType::DATA_ATTRIBUTE>& DstArray);
	//转换节点属性
	bool AdapterNodeType(Hdf5_Wrapper::NODE_TYPE eumSrcNodeType, InnerType::NODE_TYPE& eumDstNodeType);
};

