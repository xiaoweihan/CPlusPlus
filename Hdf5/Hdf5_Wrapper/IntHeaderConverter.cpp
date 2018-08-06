#include "stdafx.h"
#include "IntHeaderConverter.h"
#include <boost/variant.hpp>
#include "ErrorCode.h"

CIntHeaderConverter::CIntHeaderConverter()
{
}


CIntHeaderConverter::~CIntHeaderConverter()
{
}

int CIntHeaderConverter::ConvertDataHeader(const OuterType::DATA_HEADER& SrcDataHeader, InnerType::DATA_HEADER& DstDataHeader)
{
	int nResult = ERROR_NO_ERROR;
	if (ERROR_NO_ERROR != (nResult = CAbstractHeaderConverter::ConvertDataHeader(SrcDataHeader, DstDataHeader)))
	{
		return nResult;
	}

	InnerType::INT_DATA_HEADER IntTypeHeader;
	auto pHeader = boost::get<OuterType::INT_DATA_HEADER>(&SrcDataHeader.ExternDataType);
	if (nullptr != pHeader)
	{
		IntTypeHeader.bSign = pHeader->bSign;
		DstDataHeader.AdditionalDataType = IntTypeHeader;
		return nResult;
	}
	return ERROR_CONVERT_INT_TYPE_HEADER_FAILED;
}

int CIntHeaderConverter::ConvertDataHeader(const InnerType::DATA_HEADER& SrcDataHeader, OuterType::DATA_HEADER& DstDataHeader)
{
	int nResult = ERROR_NO_ERROR;
	if (ERROR_NO_ERROR != (nResult = CAbstractHeaderConverter::ConvertDataHeader(SrcDataHeader, DstDataHeader)))
	{
		return nResult;
	}

	auto pHeader = boost::get<InnerType::INT_DATA_HEADER>(&SrcDataHeader.AdditionalDataType);

	if (nullptr == pHeader)
	{
		return ERROR_CONVERT_INT_TYPE_HEADER_FAILED;
	}

	OuterType::INT_DATA_HEADER IntTypeHeader;
	IntTypeHeader.bSign = pHeader->bSign;
	DstDataHeader.ExternDataType = IntTypeHeader;
	return ERROR_NO_ERROR;
}
