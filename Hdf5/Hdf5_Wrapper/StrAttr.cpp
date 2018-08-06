/*********************************************************
Copyright(C):
FileName:StrAttr.cpp
Descripton:�ַ������ԵĴ���
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
	//�����������������
	shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(InnerType::DATA_TYPE::STR_TYPE, m_nDataByte, false));

	if (!pDataTypeGenerator)
	{
		ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
		return;
	}
	//���������ʵ���������
	shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());

	if (!pSuitableDataType)
	{
		ERROR_LOG("call [%s] failed,allocate DataType failed.", BOOST_CURRENT_FUNCTION);
		return;
	}
	//�����ռ�
	DataSpace AttrSpace(H5S_SCALAR);
	Attribute Attr = pElementDataSet->createAttribute(m_strAttrKeyName, *pSuitableDataType, AttrSpace);
	//���ݲ�Ϊ�ղŽ���д��
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
	//�����������������
	shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(InnerType::DATA_TYPE::STR_TYPE, m_nDataByte, false));

	if (!pDataTypeGenerator)
	{
		ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
		return;
	}
	//���������ʵ���������
	shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());

	if (!pSuitableDataType)
	{
		ERROR_LOG("call [%s] failed,allocate DataType failed.", BOOST_CURRENT_FUNCTION);
		return;
	}


	//�����ռ�
	DataSpace AttrSpace(H5S_SCALAR);
	Attribute Attr = pGroupNode->createAttribute(m_strAttrKeyName, *pSuitableDataType, AttrSpace);

	//���ݲ�Ϊ�ղŽ���д��
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
	//�հ�
	auto pScalarHandler = [](H5::Attribute& Attr, InnerType::DATA_ATTRIBUTE& DataAttr)
	{
		//��ȡռ�õ��ֽ���
		auto nAttrValueLen = Attr.getInMemDataSize();
		//��ȡDataSpace
		auto AttrDataSpace = Attr.getSpace();
		//��������
		auto AttrDataType = Attr.getStrType();
		//�����ڴ�
		char* pData = new char[nAttrValueLen];
		if (nullptr != pData)
		{
			Attr.read(AttrDataType, pData);
			//��������
			DataAttr.strKeyAttrName = Attr.getName();
			//������ֵ
			DataAttr.pData = pData;
			//��ֵ����
			DataAttr.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
			//��ֵ����ռ�õ��ֽ���
			DataAttr.Header.nDataByte = (unsigned int)AttrDataType.getSize();
			//��ȡά����Ϣ
			DataAttr.Header.nRank = 1;
			int* pDimensionArray = new int[1]{ 1 };
			DataAttr.Header.pDimensionArray = pDimensionArray;
		}
	};

	//�հ�
	auto pSimpleHandler = [](H5::Attribute& Attr, InnerType::DATA_ATTRIBUTE& DataAttr)
	{
		//��ȡռ�õ��ֽ���
		auto nAttrValueLen = Attr.getInMemDataSize();
		//��ȡDataSpace
		auto AttrDataSpace = Attr.getSpace();
		//��������
		auto AttrDataType = Attr.getStrType();
		//�����ڴ�
		char* pData = new char[nAttrValueLen];
		if (nullptr != pData)
		{

			//��ȡά����Ϣ
			int nRank = AttrDataSpace.getSimpleExtentNdims();
			//�����ڴ�
			boost::scoped_array<hsize_t> pDimsArray(new hsize_t[nRank]);
			if (pDimsArray)
			{
				//��ȡ�����ά����Ϣ
				AttrDataSpace.getSimpleExtentDims(pDimsArray.get(), NULL);
				//����ά����Ϣ
				int* pDimensionArray = new int[nRank];
				if (nullptr != pDimensionArray)
				{
					for (int i = 0; i < nRank; ++i)
					{
						pDimensionArray[i] = (int)pDimsArray[i];
					}

					//ά��
					DataAttr.Header.nRank = nRank;
					//��������
					DataAttr.strKeyAttrName = Attr.getName();
					//��ֵ����
					DataAttr.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
					//��ֵ����ռ�õ��ֽ���
					DataAttr.Header.nDataByte = (unsigned int)AttrDataType.getSize();
					//����ά����Ϣ
					DataAttr.Header.pDimensionArray = pDimensionArray;
					//�������ݶ�ȡ
					Attr.read(AttrDataType, pData);
					//ָ�븳ֵ
					DataAttr.pData = pData;
				}
				else
				{
					//�ͷ��ڴ�
					DELETE_ARRAY_POINTER(pData);
				}
			}
			else
			{
				//�ͷ��ڴ�
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


