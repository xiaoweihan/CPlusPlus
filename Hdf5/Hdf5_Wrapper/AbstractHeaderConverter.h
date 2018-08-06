/*********************************************************
Copyright(C):
FileName:AbstractHeaderConverter.h
Descripton:³éÏó
Author:xiaowei.han
Data:2018/02/06
Others:
History:
Version:1.0
*********************************************************/
#pragma once
#include "Type.h"
#include "Hdf5_Wrapper.h"
class CAbstractHeaderConverter
{
public:
	CAbstractHeaderConverter();
	virtual ~CAbstractHeaderConverter();

public:
	virtual int ConvertDataHeader(const OuterType::DATA_HEADER& SrcDataHeader,InnerType::DATA_HEADER& DstDataHeader);
	virtual int ConvertDataHeader(const InnerType::DATA_HEADER& SrcDataHeader, OuterType::DATA_HEADER& DstDataHeader);

protected:
	int ConvertDataType(OuterType::DATA_TYPE eumSrcDataType,InnerType::DATA_TYPE& eumDstDataType);
	int ConvertDataType(InnerType::DATA_TYPE eumSrcDataType, OuterType::DATA_TYPE& eumDstDataType);
};

