/*********************************************************
Copyright(C):
FileName:CompoundDataSet.cpp
Descripton:���ڸ������͵�dataset�Ķ�д
Author:xiaowei.han
Data:2018/02/07
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "CompoundDataSet.h"
#include <H5Cpp.h>
#include <boost/current_function.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/scope_exit.hpp>
#include "ErrorCode.h"
#include "AbstractAttr.h"
#include "DataTypeFactory.h"
#include "AbstractDataType.h"
#include "Utility.h"
#include "Log.h"

CCompoundDataSet::CCompoundDataSet(const std::string& strDataSetKeyName, InnerType::LP_DATA_HEADER pHeader, void* pData, int nCompressLevel /*= -1*/):
	CH5AbstractNode(strDataSetKeyName),
	m_pHeaderInfo(pHeader),
	m_pDataSetData(pData),
	m_nCompressLevel(nCompressLevel)
{

}

CCompoundDataSet::~CCompoundDataSet()
{
}

/*********************************************************
FunctionName:GetChildNum
FunctionDesc:��ȡ�ӽڵ�ĸ���
InputParam:
OutputParam:
Return:0:�ɹ� ��0:������
Author:xiaowei.han
*********************************************************/
unsigned int CCompoundDataSet::GetChildNum(void)
{
	return 0;
}

/*********************************************************
FunctionName:CreateSelfToFile
FunctionDesc:������ڵ�����д���ļ�
InputParam:
OutputParam:
Return:0:�ɹ� ��0:������
Author:xiaowei.han
*********************************************************/
int CCompoundDataSet::CreateSelfToFile(H5::Group* pGroupNode)
{
	using namespace std;
	using namespace H5;
	if (nullptr == pGroupNode || nullptr == m_pHeaderInfo)
	{
		ERROR_LOG("call [%s] failed,the param is invalid.", BOOST_CURRENT_FUNCTION);
		return ERROR_PARAM_INVALID;
	}

	//�����������������
	shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(*m_pHeaderInfo));

	if (!pDataTypeGenerator)
	{
		ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_ALLOCATE_FAILED;
	}
	//���������ʵ���������
	shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());

	if (!pSuitableDataType)
	{
		ERROR_LOG("call [%s] failed,allocate DataType failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_ALLOCATE_FAILED;
	}

	if (m_pHeaderInfo->nRank > 0)
	{
		boost::scoped_array<hsize_t> pDims(new hsize_t[m_pHeaderInfo->nRank]);
		if (pDims)
		{
			//��ȡά����Ϣ
			for (int i = 0; i < m_pHeaderInfo->nRank; ++i)
			{
				pDims[i] = m_pHeaderInfo->pDimensionArray[i];
			}

			//�������ݿռ�
			DataSpace* pDataSpace = new DataSpace(m_pHeaderInfo->nRank, pDims.get());

			if (nullptr == pDataSpace)
			{
				ERROR_LOG("call [%s] failed,allocate DataSpace failed.", BOOST_CURRENT_FUNCTION);
				return ERROR_ALLOCATE_FAILED;
			}
			BOOST_SCOPE_EXIT(&pDataSpace)
			{
				DELETE_POINTER(pDataSpace);
			}BOOST_SCOPE_EXIT_END;

			//����ѹ��
			if (m_nCompressLevel >= 0)
			{
				DSetCreatPropList* pPropList = new DSetCreatPropList;

				if (nullptr == pPropList)
				{
					ERROR_LOG("call [%s] failed,allocate DSetCreatPropList failed.", BOOST_CURRENT_FUNCTION);
					return ERROR_ALLOCATE_FAILED;
				}
				BOOST_SCOPE_EXIT(&pPropList)
				{
					DELETE_POINTER(pPropList);
				}BOOST_SCOPE_EXIT_END;

				pPropList->setChunk(m_pHeaderInfo->nRank, pDims.get());
				pPropList->setDeflate(m_nCompressLevel);

				DataSet* pDataSet = new DataSet(pGroupNode->createDataSet(m_strName, *pSuitableDataType, *pDataSpace ,*pPropList));

				if (nullptr == pDataSet)
				{
					ERROR_LOG("call [%s] failed,allocate createDataSet failed.", BOOST_CURRENT_FUNCTION);
					return ERROR_ALLOCATE_FAILED;
				}

				BOOST_SCOPE_EXIT(&pDataSet)
				{
					DELETE_POINTER(pDataSet);
				}BOOST_SCOPE_EXIT_END;
				pDataSet->write(m_pDataSetData, *pSuitableDataType);

				//д������
				for (auto& AttrElement : m_AttrList)
				{
					if (AttrElement != nullptr)
					{
						AttrElement->WriteSelf(pDataSet);
					}
				}
			}
			//������ѹ��
			else
			{
				//����DataSet
				DataSet* pDataSet = new DataSet(pGroupNode->createDataSet(m_strName, *pSuitableDataType, *pDataSpace));
				if (nullptr == pDataSet)
				{
					ERROR_LOG("call [%s] failed,allocate createDataSet failed.", BOOST_CURRENT_FUNCTION);
					return ERROR_ALLOCATE_FAILED;
				}
				BOOST_SCOPE_EXIT(&pDataSet)
				{
					DELETE_POINTER(pDataSet);
				}BOOST_SCOPE_EXIT_END;
				pDataSet->write(m_pDataSetData, *pSuitableDataType);

				//д������
				for (auto& AttrElement : m_AttrList)
				{
					if (AttrElement != nullptr)
					{
						AttrElement->WriteSelf(pDataSet);
					}
				}
			}
		}
		return ERROR_NO_ERROR;
	}
	ERROR_LOG("call [%s] failed, the rank is [%d].", BOOST_CURRENT_FUNCTION, m_pHeaderInfo->nRank);
	return ERROR_PARAM_INVALID;
}

/*********************************************************
FunctionName:ParseSelf
FunctionDesc:��������ڵ�
InputParam:
OutputParam:
Return:0:�ɹ� ��0:������
Author:xiaowei.han
*********************************************************/
int CCompoundDataSet::ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData)
{
	if (nullptr == pDataset)
	{
		return ERROR_PARAM_INVALID;
	}

	using namespace H5;
	//�հ�
	auto pScalarHandler = [this](H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DatasetData)->int
	{
		if (nullptr == pDataset)
		{
			return ERROR_PARAM_INVALID;
		}

		//��ȡռ�õ��ֽ���
		auto nDatasetValueLen = pDataset->getInMemDataSize();
		//��ȡDataSpace
		auto DatasetDataSpace = pDataset->getSpace();
		//��������
		auto DatasetDataType = pDataset->getCompType();
		//�����ڴ�
		char* pData = new char[nDatasetValueLen];
		if (nullptr != pData)
		{
			//��ֵ����
			DatasetData.Header.eumDataType = InnerType::DATA_TYPE::COMPOUND_TYPE;
			//��ֵ����ռ�õ��ֽ���
			DatasetData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
			//��ȡά����Ϣ
			DatasetData.Header.nRank = 1;
			int* pDimensionArray = new int[1]{ 1 };
			DatasetData.Header.pDimensionArray = pDimensionArray;

			//����compound��������
			if (!ParseComType(DatasetDataType, DatasetData))
			{
				return ERROR_PARSE_COMPOUND_TYPE_FAILED;
			}
			//�����������������
			std::shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(DatasetData.Header));
			if (!pDataTypeGenerator)
			{
				ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
				return ERROR_ALLOCATE_FAILED;
			}
			//���������ʵ���������
			std::shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());
			if (!pSuitableDataType)
			{
				ERROR_LOG("call [%s] failed,allocate DataType failed.", BOOST_CURRENT_FUNCTION);
				return ERROR_ALLOCATE_FAILED;
			}
			//�������ݶ�ȡ
			pDataset->read(pData, *pSuitableDataType);
			//������ֵ
			DatasetData.pData = pData;
			return ERROR_NO_ERROR;
		}

		return ERROR_ALLOCATE_FAILED;
	};

	//�հ�
	auto pSimpleHandler = [this](H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DatasetData)->int
	{
		if (nullptr == pDataset)
		{
			return ERROR_PARAM_INVALID;
		}

		//��ȡռ�õ��ֽ���
		auto nDatasetValueLen = pDataset->getInMemDataSize();
		//��ȡDataSpace
		auto DatasetDataSpace = pDataset->getSpace();
		//��������
		auto DatasetDataType = pDataset->getCompType();
		//�����ڴ�
		char* pData = new char[nDatasetValueLen];
		if (nullptr != pData)
		{
			//��ȡά����Ϣ
			int nRank = DatasetDataSpace.getSimpleExtentNdims();
			//�����ڴ�
			boost::scoped_array<hsize_t> pDimsArray(new hsize_t[nRank]);
			if (pDimsArray)
			{
				//��ȡ�����ά����Ϣ
				DatasetDataSpace.getSimpleExtentDims(pDimsArray.get(), NULL);
				//����ά����Ϣ
				int* pDimensionArray = new int[nRank];
				if (nullptr != pDimensionArray)
				{
					for (int i = 0; i < nRank; ++i)
					{
						pDimensionArray[i] = (int)pDimsArray[i];
					}
					//ά��
					DatasetData.Header.nRank = nRank;
					//��ֵ����
					DatasetData.Header.eumDataType = InnerType::DATA_TYPE::COMPOUND_TYPE;
					//��ֵ����ռ�õ��ֽ���
					DatasetData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
					DatasetData.Header.pDimensionArray = pDimensionArray;
					//����compound��������
					if (!ParseComType(DatasetDataType, DatasetData))
					{
						return ERROR_PARSE_COMPOUND_TYPE_FAILED;
					}
					
					//�����������������
					std::shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(DatasetData.Header));
					if (!pDataTypeGenerator)
					{
						ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
						return ERROR_ALLOCATE_FAILED;
					}
					//���������ʵ���������
					std::shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());
					if (!pSuitableDataType)
					{
						ERROR_LOG("call [%s] failed,allocate DataType failed.", BOOST_CURRENT_FUNCTION);
						return ERROR_ALLOCATE_FAILED;
					}
					//�������ݶ�ȡ
					pDataset->read(pData, *pSuitableDataType);
					//ָ�븳ֵ
					DatasetData.pData = pData;
					return ERROR_NO_ERROR;
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

		return ERROR_ALLOCATE_FAILED;
	};

	//��ȡDataset��Dataspace
	auto DatasetDataSpace = pDataset->getSpace();
	auto DatasetExtentType = DatasetDataSpace.getSimpleExtentType();
	int nResult = ERROR_NO_ERROR;
	switch (DatasetExtentType)
	{
	case H5S_SCALAR:
		nResult = pScalarHandler(pDataset, DstData);
		break;
	case H5S_SIMPLE:
		nResult = pSimpleHandler(pDataset, DstData);
		break;
	default:
		break;
	}
	return nResult;
}

/*********************************************************
FunctionName:ParseComType
FunctionDesc:������������
InputParam:
OutputParam:
Return:0:�ɹ� ��0:������
Author:xiaowei.han
*********************************************************/
bool CCompoundDataSet::ParseComType(H5::CompType& CompoundDataType, InnerType::HDF5_READ_DATA& DstData)
{
	//��ȡ��Ա�ĸ���
	int nMemNum = CompoundDataType.getNmembers();

	if (nMemNum <= 0)
	{
		return false;
	}

	InnerType::COMPOUND_TYPE_HEADER ComTypeHeader;

	for (int i = 0; i < nMemNum; ++i)
	{
		InnerType::COMPOUND_TYPE_ELEMENT ComTypeElement;
		//��ȡ����
		ComTypeElement.strElementName = CompoundDataType.getMemberName(i);
		//��ȡƫ����
		ComTypeElement.nElementoffset = (unsigned int)CompoundDataType.getMemberOffset(i);
		//��ȡ����
		auto ElementType = CompoundDataType.getMemberDataType(i).getClass();

		switch (ElementType)
		{
		case H5T_INTEGER:
		{
			ComTypeElement.eumDataType = InnerType::DATA_TYPE::INT_TYPE;
			auto TempIntType = CompoundDataType.getMemberIntType(i);
			InnerType::INT_DATA_HEADER TempIntHeader;
			if (TempIntType.getSign() == H5T_SGN_NONE)
			{
				TempIntHeader.bSign = false;
			}
			else
			{
				TempIntHeader.bSign = true;
			}
			ComTypeElement.AdditionalDataType = TempIntHeader;
			ComTypeElement.nDataByte = (unsigned int)TempIntType.getSize();
		}
			break;
		case H5T_FLOAT:
		{
			ComTypeElement.eumDataType = InnerType::DATA_TYPE::FLOAT_TYPE;
			auto TempFloatType = CompoundDataType.getMemberFloatType(i);
			ComTypeElement.nDataByte = (unsigned int)TempFloatType.getSize();
		}
			
			break;
		case H5T_STRING:
		{
			ComTypeElement.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
			auto TempStrType = CompoundDataType.getMemberStrType(i);
			ComTypeElement.nDataByte = (unsigned int)TempStrType.getSize();
		}
			
			break;
		default:
			ERROR_LOG("in compound type detect unknow atom type.");
			break;
		}

		ComTypeHeader.ElementArray.push_back(ComTypeElement);
	}

	DstData.Header.AdditionalDataType = ComTypeHeader;
	return true;
}
