#include "stdafx.h"
#include "AbstractHeaderConverter.h"
#include <boost/variant.hpp>
#include "ErrorCode.h"

CAbstractHeaderConverter::CAbstractHeaderConverter()
{
}


CAbstractHeaderConverter::~CAbstractHeaderConverter()
{
}

int CAbstractHeaderConverter::ConvertDataHeader(const OuterType::DATA_HEADER& SrcDataHeader, InnerType::DATA_HEADER& DstDataHeader)
{
	int nResult = ERROR_NO_ERROR;
	//参数合法性判断
	if (SrcDataHeader.DimensionArray.empty() || SrcDataHeader.nRank <= 0)
	{
		return ERROR_DATA_HEADER_INVALID;
	}

	DstDataHeader.nRank = SrcDataHeader.nRank;
	//数据类型占用的字节数
	DstDataHeader.nDataByte = SrcDataHeader.nDataByte;
	//转换类型失败
	if (ERROR_NO_ERROR != (nResult = ConvertDataType(SrcDataHeader.eumDataType, DstDataHeader.eumDataType)))
	{
		return nResult;
	}
	int nDimenLen = (int)SrcDataHeader.DimensionArray.size();
	if (nDimenLen != SrcDataHeader.nRank)
	{
		return ERROR_DATA_HEADER_INVALID;
	}

	//申请内存
	DstDataHeader.pDimensionArray = new int[nDimenLen];

	//申请内存失败
	if (nullptr == DstDataHeader.pDimensionArray)
	{
		return ERROR_ALLOCATE_FAILED;
	}
	for (int i = 0; i < nDimenLen; ++i)
	{
		DstDataHeader.pDimensionArray[i] = SrcDataHeader.DimensionArray[i];
	}

	return nResult;
}

int CAbstractHeaderConverter::ConvertDataHeader(const InnerType::DATA_HEADER& SrcDataHeader, OuterType::DATA_HEADER& DstDataHeader)
{
	//参数合法性判断
	if (nullptr == SrcDataHeader.pDimensionArray || SrcDataHeader.nRank <= 0)
	{
		return ERROR_DATA_HEADER_INVALID;
	}
	int nResult = ERROR_NO_ERROR;
	DstDataHeader.nRank = SrcDataHeader.nRank;
	DstDataHeader.nDataByte = SrcDataHeader.nDataByte;
	//转换类型失败
	if (0 != (nResult = ConvertDataType(SrcDataHeader.eumDataType, DstDataHeader.eumDataType)))
	{
		return nResult;
	}
	unsigned int nTotalLen = 1;
	//转换维度信息
	for (int i = 0; i < SrcDataHeader.nRank; ++i)
	{
		DstDataHeader.DimensionArray.push_back(SrcDataHeader.pDimensionArray[i]);
		nTotalLen *= SrcDataHeader.pDimensionArray[i];
	}
	//描述数据的内存块整体长度
	DstDataHeader.nTotalDataLen = nTotalLen * SrcDataHeader.nDataByte;

	return nResult;
}

//外->内转换
int CAbstractHeaderConverter::ConvertDataType(OuterType::DATA_TYPE eumSrcDataType, InnerType::DATA_TYPE& eumDstDataType)
{
	
	int nResult = ERROR_NO_ERROR;

	switch (eumSrcDataType)
	{
		//整数类型
	case OuterType::INT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::INT_TYPE;
		break;
		//浮点类型
	case OuterType::FLOAT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::FLOAT_TYPE;
		break;
		//字符串类型
	case OuterType::STR_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::STR_TYPE;
		break;
		//复合类型
	case OuterType::COMPOUND_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::COMPOUND_TYPE;
		break;
	default:
		nResult = ERROR_DATA_TYPE_FAILED;
		break;
	}

	return nResult;
}

//内->外转换
int CAbstractHeaderConverter::ConvertDataType(InnerType::DATA_TYPE eumSrcDataType, OuterType::DATA_TYPE& eumDstDataType)
{
	int nResult = ERROR_NO_ERROR;
	switch (eumSrcDataType)
	{
	case InnerType::DATA_TYPE::INT_TYPE:
		eumDstDataType = OuterType::DATA_TYPE::INT_TYPE;
		break;
	case InnerType::DATA_TYPE::FLOAT_TYPE:
		eumDstDataType = OuterType::DATA_TYPE::FLOAT_TYPE;
		break;
	case InnerType::DATA_TYPE::STR_TYPE:
		eumDstDataType = OuterType::DATA_TYPE::STR_TYPE;
		break;
	case InnerType::DATA_TYPE::COMPOUND_TYPE:
		eumDstDataType = OuterType::DATA_TYPE::COMPOUND_TYPE;
		break;
	default:
		nResult = ERROR_DATA_TYPE_FAILED;
		break;

	}

	return nResult;
}
