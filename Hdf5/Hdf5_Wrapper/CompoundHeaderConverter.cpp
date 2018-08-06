#include "stdafx.h"
#include "CompoundHeaderConverter.h"
#include <boost/variant.hpp>
#include "ErrorCode.h"

CCompoundHeaderConverter::CCompoundHeaderConverter()
{
}


CCompoundHeaderConverter::~CCompoundHeaderConverter()
{
}

int CCompoundHeaderConverter::ConvertDataHeader(const OuterType::DATA_HEADER& SrcDataHeader, InnerType::DATA_HEADER& DstDataHeader)
{
	int nResult = ERROR_NO_ERROR;
	if (ERROR_NO_ERROR != (nResult = CAbstractHeaderConverter::ConvertDataHeader(SrcDataHeader, DstDataHeader)))
	{
		return nResult;
	}

	auto pHeader = boost::get<OuterType::COMPOUND_TYPE_HEADER>(&SrcDataHeader.ExternDataType);
	if (nullptr != pHeader)
	{
		InnerType::COMPOUND_TYPE_HEADER CompoundHeader;
		for (auto& Element : pHeader->ElementArray)
		{
			InnerType::COMPOUND_TYPE_ELEMENT CompoundHeaderElement;
			CompoundHeaderElement.strElementName = Element.strElementName;
			CompoundHeaderElement.nElementoffset = Element.nElementoffset;
			CompoundHeaderElement.nDataByte = Element.nDataByte;
			//设置类型失败
			if ((nResult = ConvertDataType(Element.eumDataType, CompoundHeaderElement.eumDataType)) != ERROR_NO_ERROR)
			{
				return nResult;
			}
			//如果是整数类型
			if (OuterType::DATA_TYPE::INT_TYPE == Element.eumDataType)
			{
				InnerType::INT_DATA_HEADER CompoundIntHeaderElement;
				auto p = boost::get<OuterType::INT_DATA_HEADER>(&Element.AdditionalDataType);
				if (nullptr == p)
				{
					return ERROR_CONVERT_INT_TYPE_HEADER_FAILED;
				}
				CompoundIntHeaderElement.bSign = p->bSign;
				CompoundHeaderElement.AdditionalDataType = CompoundIntHeaderElement;
			}
			CompoundHeader.ElementArray.push_back(std::move(CompoundHeaderElement));
		}

		//设置类似枚举属性
		DstDataHeader.AdditionalDataType = CompoundHeader;

		return ERROR_NO_ERROR;
	}

	return ERROR_CONVERT_COMPOUND_TYPE_HEADER_FAILED;
}

int CCompoundHeaderConverter::ConvertDataHeader(const InnerType::DATA_HEADER& SrcDataHeader, OuterType::DATA_HEADER& DstDataHeader)
{
	int nResult = ERROR_NO_ERROR;
	if (ERROR_NO_ERROR != (nResult = CAbstractHeaderConverter::ConvertDataHeader(SrcDataHeader, DstDataHeader)))
	{
		return nResult;
	}

	auto p = boost::get<InnerType::COMPOUND_TYPE_HEADER>(&SrcDataHeader.AdditionalDataType);
	if (nullptr == p)
	{
		return ERROR_CONVERT_COMPOUND_TYPE_HEADER_FAILED;
	}

	OuterType::COMPOUND_TYPE_HEADER CompoundHeader;
	for (auto& SubElement : p->ElementArray)
	{
		OuterType::COMPOUND_TYPE_ELEMENT CompoundHeaderElement;
		CompoundHeaderElement.nDataByte = SubElement.nDataByte;
		CompoundHeaderElement.strElementName = SubElement.strElementName;
		CompoundHeaderElement.nElementoffset = SubElement.nElementoffset;

		//转换数据头
		if ((nResult = ConvertDataType(SubElement.eumDataType, CompoundHeaderElement.eumDataType)) != ERROR_NO_ERROR)
		{
			return nResult;
		}

		if (OuterType::DATA_TYPE::INT_TYPE == CompoundHeaderElement.eumDataType)
		{
			auto p = boost::get<InnerType::INT_DATA_HEADER>(&SubElement.AdditionalDataType);
			if (nullptr == p)
			{
				return ERROR_CONVERT_INT_TYPE_HEADER_FAILED;
			}

			OuterType::INT_DATA_HEADER TempIntHeader;
			TempIntHeader.bSign = p->bSign;
			CompoundHeaderElement.AdditionalDataType = TempIntHeader;
		}
		CompoundHeader.ElementArray.push_back(CompoundHeaderElement);
	}
	DstDataHeader.ExternDataType = CompoundHeader;
	return ERROR_NO_ERROR;
}
