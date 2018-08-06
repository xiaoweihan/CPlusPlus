#include "stdafx.h"
#include "NodeFactory.h"
#include <boost/variant.hpp>
#include "IntDataSet.h"
#include "FloatDataSet.h"
#include "StrDataSet.h"
#include "CompoundDataSet.h"
CH5AbstractNode* NodeFactory::CreateNode(const InnerType::LP_HDF5_WRITE_DATA pData)
{
	using namespace InnerType;
	if (nullptr == pData)
	{
		return nullptr;
	}

	CH5AbstractNode* pResult = nullptr;
	switch (pData->Header.eumDataType)
	{
		//ÕûÐÍµÄDataSet
	case InnerType::DATA_TYPE::INT_TYPE:
		pResult = new CIntDataSet(pData->strKeyName, &pData->Header, pData->pData, pData->nCompressLevel);
		break;
	case InnerType::DATA_TYPE::STR_TYPE:
		pResult = new CStrDataSet(pData->strKeyName, &pData->Header, pData->pData, pData->nCompressLevel);
		break;
	case InnerType::DATA_TYPE::FLOAT_TYPE:
		pResult = new CFloatDataSet(pData->strKeyName, &pData->Header, pData->pData, pData->nCompressLevel);
		break;
		case InnerType::DATA_TYPE::COMPOUND_TYPE:
		pResult = new CCompoundDataSet(pData->strKeyName, &pData->Header, pData->pData, pData->nCompressLevel);
		break;
	default:
		break;

	}

	return pResult;
}
