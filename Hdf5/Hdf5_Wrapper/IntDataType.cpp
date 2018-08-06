#include "stdafx.h"
#include "IntDataType.h"

CIntDataType::CIntDataType(unsigned int nDataBytes, bool bSign):
	m_nDataBytes(nDataBytes),
	m_bSign(bSign)
{

}

CIntDataType::~CIntDataType()
{
}

std::string CIntDataType::GetDataTypeName()
{
	return "IntType";
}

H5::DataType* CIntDataType::CreateProperType()
{
	using namespace H5;

	IntType* pDataType = nullptr;

	if (m_bSign)
	{
		switch (m_nDataBytes)
		{
			//1个字节
		case 1:
			pDataType = new IntType(PredType::NATIVE_CHAR);
			break;
			//2个字节
		case 2:
			pDataType = new IntType(PredType::NATIVE_SHORT);
			break;
			//4个字节
		case 4:
			pDataType = new IntType(PredType::NATIVE_INT);
			break;
			//8个字节
		case 8:
			pDataType = new IntType(PredType::NATIVE_LLONG);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (m_nDataBytes)
		{
			//1个字节
		case 1:
			pDataType = new IntType(PredType::NATIVE_UCHAR);
			break;
			//2个字节
		case 2:
			pDataType = new IntType(PredType::NATIVE_USHORT);
			break;
			//4个字节
		case 4:
			pDataType = new IntType(PredType::NATIVE_UINT);
			break;
			//8个字节
		case 8:
			pDataType = new IntType(PredType::NATIVE_LLONG);
			break;
		default:
			break;
		}

		if (nullptr != pDataType)
		{
			pDataType->setSign(H5T_SGN_NONE);
		}
	}

	return pDataType;
}
