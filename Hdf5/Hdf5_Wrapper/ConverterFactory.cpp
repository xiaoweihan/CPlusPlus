#include "stdafx.h"
#include "ConverterFactory.h"
#include "StrHeaderConverter.h"
#include "IntHeaderConverter.h"
#include "FloatHeaderConverter.h"
#include "CompoundHeaderConverter.h"

CAbstractHeaderConverter* ConverterFactory::CreateConverter(OuterType::DATA_TYPE eumDataType)
{
	CAbstractHeaderConverter* pConverter = nullptr;

	switch (eumDataType)
	{
	case OuterType::FLOAT_TYPE:
		pConverter = new CFloatHeaderConverter;
		break;
	case OuterType::INT_TYPE:
		pConverter = new CIntHeaderConverter;
		break;
	case OuterType::STR_TYPE:
		pConverter = new CStrHeaderConverter;
		break;
	case OuterType::COMPOUND_TYPE:
		pConverter = new CCompoundHeaderConverter;
		break;
	default:
		break;
	}

	return pConverter;
}

CAbstractHeaderConverter* ConverterFactory::CreateConverter(InnerType::DATA_TYPE eumDataType)
{
	CAbstractHeaderConverter* pConverter = nullptr;

	switch (eumDataType)
	{
	case InnerType::DATA_TYPE::FLOAT_TYPE:
		pConverter = new CFloatHeaderConverter;
		break;
	case InnerType::DATA_TYPE::INT_TYPE:
		pConverter = new CIntHeaderConverter;
		break;
	case InnerType::DATA_TYPE::STR_TYPE:
		pConverter = new CStrHeaderConverter;
		break;
	case InnerType::DATA_TYPE::COMPOUND_TYPE:
		pConverter = new CCompoundHeaderConverter;
		break;
	default:
		break;
	}

	return pConverter;
}
