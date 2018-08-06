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
	//�����Ϸ����ж�
	if (SrcDataHeader.DimensionArray.empty() || SrcDataHeader.nRank <= 0)
	{
		return ERROR_DATA_HEADER_INVALID;
	}

	DstDataHeader.nRank = SrcDataHeader.nRank;
	//��������ռ�õ��ֽ���
	DstDataHeader.nDataByte = SrcDataHeader.nDataByte;
	//ת������ʧ��
	if (ERROR_NO_ERROR != (nResult = ConvertDataType(SrcDataHeader.eumDataType, DstDataHeader.eumDataType)))
	{
		return nResult;
	}
	int nDimenLen = (int)SrcDataHeader.DimensionArray.size();
	if (nDimenLen != SrcDataHeader.nRank)
	{
		return ERROR_DATA_HEADER_INVALID;
	}

	//�����ڴ�
	DstDataHeader.pDimensionArray = new int[nDimenLen];

	//�����ڴ�ʧ��
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
	//�����Ϸ����ж�
	if (nullptr == SrcDataHeader.pDimensionArray || SrcDataHeader.nRank <= 0)
	{
		return ERROR_DATA_HEADER_INVALID;
	}
	int nResult = ERROR_NO_ERROR;
	DstDataHeader.nRank = SrcDataHeader.nRank;
	DstDataHeader.nDataByte = SrcDataHeader.nDataByte;
	//ת������ʧ��
	if (0 != (nResult = ConvertDataType(SrcDataHeader.eumDataType, DstDataHeader.eumDataType)))
	{
		return nResult;
	}
	unsigned int nTotalLen = 1;
	//ת��ά����Ϣ
	for (int i = 0; i < SrcDataHeader.nRank; ++i)
	{
		DstDataHeader.DimensionArray.push_back(SrcDataHeader.pDimensionArray[i]);
		nTotalLen *= SrcDataHeader.pDimensionArray[i];
	}
	//�������ݵ��ڴ�����峤��
	DstDataHeader.nTotalDataLen = nTotalLen * SrcDataHeader.nDataByte;

	return nResult;
}

//��->��ת��
int CAbstractHeaderConverter::ConvertDataType(OuterType::DATA_TYPE eumSrcDataType, InnerType::DATA_TYPE& eumDstDataType)
{
	
	int nResult = ERROR_NO_ERROR;

	switch (eumSrcDataType)
	{
		//��������
	case OuterType::INT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::INT_TYPE;
		break;
		//��������
	case OuterType::FLOAT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::FLOAT_TYPE;
		break;
		//�ַ�������
	case OuterType::STR_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::STR_TYPE;
		break;
		//��������
	case OuterType::COMPOUND_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::COMPOUND_TYPE;
		break;
	default:
		nResult = ERROR_DATA_TYPE_FAILED;
		break;
	}

	return nResult;
}

//��->��ת��
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
