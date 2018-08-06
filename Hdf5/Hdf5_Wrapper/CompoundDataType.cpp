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
		//遍历所有成员的原子类型
		for (auto& ElementDataType : m_TypeInfo.ElementArray)
		{
			shared_ptr<CAbstractDataType> pDataTypeGenerator;
			//如果是整型
			if (InnerType::DATA_TYPE::INT_TYPE == ElementDataType.eumDataType)
			{
				auto p = boost::get<InnerType::INT_DATA_HEADER>(&ElementDataType.AdditionalDataType);
				if (nullptr != p)
				{
					//获取子变量的类型
					pDataTypeGenerator.reset(DataTypeFactory::CreateDataType(ElementDataType.eumDataType, ElementDataType.nDataByte, p->bSign));
				}
			}
			else
			{
				//获取子变量的类型
				pDataTypeGenerator.reset(DataTypeFactory::CreateDataType(ElementDataType.eumDataType, ElementDataType.nDataByte, true));
			}
			if (pDataTypeGenerator)
			{
				//生产出合适的数据类型
				shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());
				if (pSuitableDataType)
				{
					//开始组装数据类型
					pCompoundType->insertMember(ElementDataType.strElementName,ElementDataType.nElementoffset,*pSuitableDataType);
				}
			}
		}
	}
	pDataType = pCompoundType;
	return pDataType;
}
