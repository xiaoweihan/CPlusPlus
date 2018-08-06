#include "stdafx.h"
#include "DataTypeFactory.h"
#include <boost/variant.hpp>
#include "IntDataType.h"
#include "FloatDataType.h"
#include "StrDataType.h"
#include "CompoundDataType.h"

/*********************************************************
FunctionName:CreateDataType
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
CAbstractDataType* DataTypeFactory::CreateDataType(InnerType::DATA_TYPE eumDataType, unsigned int nDataByte, bool bSign)
{

	CAbstractDataType* pType = nullptr;
	switch (eumDataType)
	{
	case InnerType::DATA_TYPE::INT_TYPE:
		pType = new CIntDataType(nDataByte,bSign);
		break;
	case InnerType::DATA_TYPE::FLOAT_TYPE:
		pType = new CFloatDataType(nDataByte);
		break;
	case InnerType::DATA_TYPE::STR_TYPE:
		pType = new CStrDataType(nDataByte);
		break;
	case InnerType::DATA_TYPE::COMPOUND_TYPE:
		break;
	default:
		break;
	}

	return pType;
}

/*********************************************************
FunctionName:CreateDataType
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
CAbstractDataType* DataTypeFactory::CreateDataType(const InnerType::DATA_HEADER& DataHeader)
{
	CAbstractDataType* pType = nullptr;


	//进行闭包操作
	auto pYiledIntType = [&]()->CAbstractDataType*
	{
		auto p = boost::get<InnerType::INT_DATA_HEADER>(&DataHeader.AdditionalDataType);

		if (nullptr == p)
		{
			return nullptr;
		}

		return new CIntDataType(DataHeader.nDataByte, p->bSign);
	};

	auto pYiledCompoundType = [&]()->CAbstractDataType*
	{
		auto p = boost::get<InnerType::COMPOUND_TYPE_HEADER>(&DataHeader.AdditionalDataType);

		if (nullptr == p)
		{
			return nullptr;
		}

		return new CCompoundDataType(DataHeader.nDataByte,*p);
	};

	switch (DataHeader.eumDataType)
	{
	case InnerType::DATA_TYPE::INT_TYPE:
		pType = pYiledIntType();
		break;
	case InnerType::DATA_TYPE::FLOAT_TYPE:
		pType = new CFloatDataType(DataHeader.nDataByte);
		break;
	case InnerType::DATA_TYPE::STR_TYPE:
		pType = new CStrDataType(DataHeader.nDataByte);
		break;
	case InnerType::DATA_TYPE::COMPOUND_TYPE:
		pType = pYiledCompoundType();
		break;
	default:
		break;
	}

	return pType;
}
