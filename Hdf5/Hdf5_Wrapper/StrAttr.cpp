/*********************************************************
Copyright(C):
FileName:StrAttr.cpp
Descripton:字符串属性的处理
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "StrAttr.h"
#include <memory>
#include <H5Cpp.h>
#include <boost/smart_ptr.hpp>
#include <boost/current_function.hpp>
#include "DataTypeFactory.h"
#include "AbstractDataType.h"
#include "Log.h"


CStrAttr::CStrAttr(const std::string& strAttrKeyName, unsigned int nDataByte, void* pData):
	CAbstractAttr(strAttrKeyName),
	m_nDataByte(nDataByte),
	m_pAttrData(pData)
{

}

CStrAttr::~CStrAttr()
{
}

/*********************************************************
FunctionName:WriteSelf
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
void CStrAttr::WriteSelf(H5::DataSet* pElementDataSet)
{
	if (nullptr == pElementDataSet)
	{
		ERROR_LOG("call [%s] failed,the param is invalid.", BOOST_CURRENT_FUNCTION);
		return;
	}
	using namespace std;
	using namespace H5;
	//创建所需的数据类型
	shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(InnerType::DATA_TYPE::STR_TYPE, m_nDataByte, false));

	if (!pDataTypeGenerator)
	{
		ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
		return;
	}
	//生产出合适的数据类型
	shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());

	if (!pSuitableDataType)
	{
		ERROR_LOG("call [%s] failed,allocate DataType failed.", BOOST_CURRENT_FUNCTION);
		return;
	}
	//创建空间
	DataSpace AttrSpace(H5S_SCALAR);
	Attribute Attr = pElementDataSet->createAttribute(m_strAttrKeyName, *pSuitableDataType, AttrSpace);
	//数据不为空才进行写入
	if (nullptr != m_pAttrData)
	{
		Attr.write(*pSuitableDataType, m_pAttrData);
	}
	else
	{
		ERROR_LOG("call [%s] failed,the attrdata is nullptr.", BOOST_CURRENT_FUNCTION);
	}
}

/*********************************************************
FunctionName:WriteSelf
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
void CStrAttr::WriteSelf(H5::Group* pGroupNode)
{
	if (nullptr == pGroupNode)
	{
		ERROR_LOG("call [%s] failed,the param is invalid.", BOOST_CURRENT_FUNCTION);
		return;
	}
	using namespace std;
	using namespace H5;
	//创建所需的数据类型
	shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(InnerType::DATA_TYPE::STR_TYPE, m_nDataByte, false));

	if (!pDataTypeGenerator)
	{
		ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
		return;
	}
	//生产出合适的数据类型
	shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());

	if (!pSuitableDataType)
	{
		ERROR_LOG("call [%s] failed,allocate DataType failed.", BOOST_CURRENT_FUNCTION);
		return;
	}


	//创建空间
	DataSpace AttrSpace(H5S_SCALAR);
	Attribute Attr = pGroupNode->createAttribute(m_strAttrKeyName, *pSuitableDataType, AttrSpace);

	//数据不为空才进行写入
	if (nullptr != m_pAttrData)
	{
		Attr.write(*pSuitableDataType, m_pAttrData);
	}
	else
	{
		ERROR_LOG("call [%s] failed,the attrdata is nullptr.", BOOST_CURRENT_FUNCTION);
	}
}

/*********************************************************
FunctionName:ReadSelf
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
void CStrAttr::ReadSelf(H5::Attribute* pAttr, InnerType::DATA_ATTRIBUTE& DataAttr)
{
	if (nullptr == pAttr)
	{
		return;
	}
	using namespace H5;
	//闭包
	auto pScalarHandler = [](H5::Attribute& Attr, InnerType::DATA_ATTRIBUTE& DataAttr)
	{
		//获取占用的字节数
		auto nAttrValueLen = Attr.getInMemDataSize();
		//获取DataSpace
		auto AttrDataSpace = Attr.getSpace();
		//数据类型
		auto AttrDataType = Attr.getStrType();
		//申请内存
		char* pData = new char[nAttrValueLen];
		if (nullptr != pData)
		{
			Attr.read(AttrDataType, pData);
			//属性名称
			DataAttr.strKeyAttrName = Attr.getName();
			//属性数值
			DataAttr.pData = pData;
			//数值类型
			DataAttr.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
			//数值类型占用的字节数
			DataAttr.Header.nDataByte = (unsigned int)AttrDataType.getSize();
			//获取维度信息
			DataAttr.Header.nRank = 1;
			int* pDimensionArray = new int[1]{ 1 };
			DataAttr.Header.pDimensionArray = pDimensionArray;
		}
	};

	//闭包
	auto pSimpleHandler = [](H5::Attribute& Attr, InnerType::DATA_ATTRIBUTE& DataAttr)
	{
		//获取占用的字节数
		auto nAttrValueLen = Attr.getInMemDataSize();
		//获取DataSpace
		auto AttrDataSpace = Attr.getSpace();
		//数据类型
		auto AttrDataType = Attr.getStrType();
		//申请内存
		char* pData = new char[nAttrValueLen];
		if (nullptr != pData)
		{

			//获取维度信息
			int nRank = AttrDataSpace.getSimpleExtentNdims();
			//申请内存
			boost::scoped_array<hsize_t> pDimsArray(new hsize_t[nRank]);
			if (pDimsArray)
			{
				//获取具体的维度信息
				AttrDataSpace.getSimpleExtentDims(pDimsArray.get(), NULL);
				//保存维度信息
				int* pDimensionArray = new int[nRank];
				if (nullptr != pDimensionArray)
				{
					for (int i = 0; i < nRank; ++i)
					{
						pDimensionArray[i] = (int)pDimsArray[i];
					}

					//维数
					DataAttr.Header.nRank = nRank;
					//属性名称
					DataAttr.strKeyAttrName = Attr.getName();
					//数值类型
					DataAttr.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
					//数值类型占用的字节数
					DataAttr.Header.nDataByte = (unsigned int)AttrDataType.getSize();
					//设置维度信息
					DataAttr.Header.pDimensionArray = pDimensionArray;
					//进行数据读取
					Attr.read(AttrDataType, pData);
					//指针赋值
					DataAttr.pData = pData;
				}
				else
				{
					//释放内存
					DELETE_ARRAY_POINTER(pData);
				}
			}
			else
			{
				//释放内存
				DELETE_ARRAY_POINTER(pData);
			}
		}
	};


	auto AttrDataSpace = pAttr->getSpace();
	auto AttrExtentType = AttrDataSpace.getSimpleExtentType();

	switch (AttrExtentType)
	{
	case H5S_SCALAR:
		pScalarHandler(*pAttr, DataAttr);
		break;
	case H5S_SIMPLE:
		pSimpleHandler(*pAttr, DataAttr);
		break;
	default:
		break;
	}

}


