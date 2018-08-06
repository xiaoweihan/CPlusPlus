#include "stdafx.h"
#include "AttrFactory.h"
#include <boost/variant.hpp>
#include "IntAttr.h"
#include "FloatAttr.h"
#include "StrAttr.h"
CAbstractAttr* AttrFactory::CreateAttr(const InnerType::DATA_ATTRIBUTE& Attr)
{
	CAbstractAttr* pResult = nullptr;

	switch (Attr.Header.eumDataType)
	{
	case InnerType::DATA_TYPE::INT_TYPE:
	{
		auto pExternDataType = boost::get<InnerType::INT_DATA_HEADER>(&(Attr.Header.AdditionalDataType));
		
		if (nullptr != pExternDataType)
		{
			pResult = new CIntAttr(Attr.strKeyAttrName, Attr.Header.nRank, Attr.Header.pDimensionArray, Attr.Header.nDataByte, pExternDataType->bSign, Attr.pData);

		}
		
	}	
	break;
	case InnerType::DATA_TYPE::FLOAT_TYPE:
		pResult = new CFloatAttr(Attr.strKeyAttrName, Attr.Header.nRank, Attr.Header.pDimensionArray, Attr.Header.nDataByte,Attr.pData);
		break;
	case InnerType::DATA_TYPE::STR_TYPE:
		pResult = new CStrAttr(Attr.strKeyAttrName,Attr.Header.nDataByte,Attr.pData);
		break;
	case InnerType::DATA_TYPE::COMPOUND_TYPE:
		break;

	default:
		break;
	}

	return pResult;
}

CAbstractAttr* AttrFactory::CreateAttr(H5T_class_t eumClassType)
{
	using namespace H5;

	CAbstractAttr* pHandler = nullptr;

	switch (eumClassType)
	{
	case H5T_INTEGER:
		pHandler = new CIntAttr;
		break;
	case H5T_FLOAT:
		pHandler = new CFloatAttr;
		break;
	case H5T_STRING:
		pHandler = new CStrAttr;
		break;
	default:
		break;
	}
	return pHandler;
}
