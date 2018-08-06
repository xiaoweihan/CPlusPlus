#include "stdafx.h"
#include "CompoundDataType.h"
#include <memory>
#include <boost/variant.hpp>
#include "DataTypeFactory.h"
#include "AbstractDataType.h"
#include "Utility.h"
#include "Log.h"
#include "ErrorCode.h"
CCompoundDataType::CCompoundDataType(unsigned int nDataByte, const InnerType::COMPOUND_TYPE_HEADER& TypeInfo):
	m_TypeInfo(TypeInfo),
	m_nDataByte(nDataByte)
{

}

CCompoundDataType::~CCompoundDataType()
{
}

std::string CCompoundDataType::GetDataTypeName()
{
	return "Compound";
}

/*********************************************************
FunctionName:CreateProperType
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
H5::DataType* CCompoundDataType::CreateProperType()
{
	using namespace H5;
	using namespace std;
	DataType* pDataType = nullptr;
	CompType* pCompoundType = new CompType((size_t)m_nDataByte);
	if (nullptr != pCompoundType)
	{
		//�������г�Ա��ԭ������
		for (auto& ElementDataType : m_TypeInfo.ElementArray)
		{
			shared_ptr<CAbstractDataType> pDataTypeGenerator;
			//���������
			if (InnerType::DATA_TYPE::INT_TYPE == ElementDataType.eumDataType)
			{
				auto p = boost::get<InnerType::INT_DATA_HEADER>(&ElementDataType.AdditionalDataType);
				if (nullptr != p)
				{
					//��ȡ�ӱ���������
					pDataTypeGenerator.reset(DataTypeFactory::CreateDataType(ElementDataType.eumDataType, ElementDataType.nDataByte, p->bSign));
				}
			}
			else
			{
				//��ȡ�ӱ���������
				pDataTypeGenerator.reset(DataTypeFactory::CreateDataType(ElementDataType.eumDataType, ElementDataType.nDataByte, true));
			}
			if (pDataTypeGenerator)
			{
				//���������ʵ���������
				shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());
				if (pSuitableDataType)
				{
					//��ʼ��װ��������
					pCompoundType->insertMember(ElementDataType.strElementName,ElementDataType.nElementoffset,*pSuitableDataType);
				}
			}
		}
	}
	pDataType = pCompoundType;
	return pDataType;
}
