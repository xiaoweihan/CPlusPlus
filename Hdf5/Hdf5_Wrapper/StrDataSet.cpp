#include "stdafx.h"
#include "StrDataSet.h"
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

const int MATRIX_RANK = 2;

CStrDataSet::CStrDataSet(const std::string& strDataSetKeyName, InnerType::LP_DATA_HEADER pHeader, void* pData, int nCompressLevel /*= -1*/):
	CH5AbstractNode(strDataSetKeyName),
	m_pHeaderInfo(pHeader),
	m_pDataSetData(pData),
	m_nCompressLevel(nCompressLevel)
{

}

CStrDataSet::~CStrDataSet()
{
}

unsigned int CStrDataSet::GetChildNum(void)
{
	return 0;
}

int CStrDataSet::CreateSelfToFile(H5::Group* pGroupNode)
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
			}BOOST_SCOPE_EXIT_END

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

					DataSet* pDataSet = new DataSet(pGroupNode->createDataSet(m_strName, *pSuitableDataType, *pDataSpace, *pPropList));

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

int CStrDataSet::ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData)
{
	if (nullptr == pDataset)
	{
		return ERROR_PARAM_INVALID;
	}

	using namespace H5;
	//�հ�
	auto pScalarHandler = [](H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DatasetData)->int
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
		auto DatasetDataType = pDataset->getStrType();
		//�����ڴ�
		char* pData = new char[nDatasetValueLen];
		if (nullptr != pData)
		{
			//��ֵ����
			DatasetData.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
			//��ֵ����ռ�õ��ֽ���
			DatasetData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
			//��ȡά����Ϣ
			DatasetData.Header.nRank = 1;
			int* pDimensionArray = new int[1]{ 1 };
			DatasetData.Header.pDimensionArray = pDimensionArray;
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
	auto pSimpleHandler = [](H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DatasetData)->int
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
		auto DatasetDataType = pDataset->getStrType();
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
					DatasetData.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
					//��ֵ����ռ�õ��ֽ���
					DatasetData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
					//����ά����Ϣ
					DatasetData.Header.pDimensionArray = pDimensionArray;
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

int CStrDataSet::ReadPartialData(H5::DataSet* pDataset, const InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM& QueryParam, InnerType::HDF5_READ_DATA& DstData)
{
	using namespace H5;

	//�����Ϸ����ж�
	if (nullptr == pDataset)
	{
		return ERROR_PARAM_INVALID;
	}

	//��ȡDataSpace
	auto DatasetDataSpace = pDataset->getSpace();
	//��������
	auto DatasetDataType = pDataset->getIntType();
	//��ȡά����Ϣ
	int nRank = DatasetDataSpace.getSimpleExtentNdims();
	//��ȡÿ������ռ�õ��ֽ���
	int nDataByte = (int)DatasetDataType.getSize();

	hsize_t DimsArray[MATRIX_RANK] = { 0 };

	//��ȡ�����ά����Ϣ
	DatasetDataSpace.getSimpleExtentDims(DimsArray, NULL);

	//��ȡ�ܵ�����������
	int nRowNum = (int)DimsArray[0];
	int nColumnNum = (int)DimsArray[1];

	//���ݲ���2��ά�ȵ�
	if (2 != nRank)
	{
		return ERROR_RANK_NOT_SUPPORT;
	}

	//����ά����Ϣ
	int* pDimensionArray = new int[1];

	if (nullptr == pDimensionArray)
	{
		return ERROR_ALLOCATE_FAILED;
	}

	hsize_t OffsetArray[MATRIX_RANK] = { 0 };
	hsize_t CountArray[MATRIX_RANK] = { 0 };
	hsize_t StrideArray[MATRIX_RANK] = { 0 };
	hsize_t BlockArray[MATRIX_RANK] = { 0 };
	hsize_t MemDataSpaceDimenArray[1] = { 0 };
	//�������
	if (QueryParam.bColumn)
	{
		if (QueryParam.nIndex >= nColumnNum)
		{
			DELETE_ARRAY_POINTER(pDimensionArray);
			return ERROR_INDEX_OUT_OF_RANGE;
		}

		OffsetArray[0] = 0;
		OffsetArray[1] = QueryParam.nIndex;

		CountArray[0] = nRowNum;
		CountArray[1] = 1;

		StrideArray[0] = 1;
		StrideArray[1] = 1;

		BlockArray[0] = 1;
		BlockArray[1] = 1;

		pDimensionArray[0] = nRowNum;

		MemDataSpaceDimenArray[0] = nRowNum;
	}
	//�������
	else
	{
		if (QueryParam.nIndex >= nRowNum)
		{
			DELETE_ARRAY_POINTER(pDimensionArray);
			return ERROR_INDEX_OUT_OF_RANGE;
		}

		OffsetArray[0] = QueryParam.nIndex;
		OffsetArray[1] = 0;

		CountArray[0] = 1;
		CountArray[1] = nColumnNum;

		StrideArray[0] = 1;
		StrideArray[1] = 1;

		BlockArray[0] = 1;
		BlockArray[1] = 1;

		pDimensionArray[0] = nColumnNum;

		MemDataSpaceDimenArray[0] = nColumnNum;
	}


	//ά��
	DstData.Header.nRank = 1;
	//��ֵ����
	DstData.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
	//��ֵ����ռ�õ��ֽ���
	DstData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
	//����ά����Ϣ
	DstData.Header.pDimensionArray = pDimensionArray;

	//�����������������
	std::shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(DstData.Header));
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
	char* pBuffer = new char[pDimensionArray[0] * nDataByte];
	if (nullptr == pBuffer)
	{
		return ERROR_ALLOCATE_FAILED;
	}

	//ѡ�񳬿�
	DatasetDataSpace.selectHyperslab(H5S_SELECT_SET, CountArray, OffsetArray, StrideArray, BlockArray);

	//����һ���ڴ�ռ�
	DataSpace memspace(1, MemDataSpaceDimenArray, NULL);

	//���ж�ȡ
	pDataset->read(pBuffer, *pSuitableDataType);

	DstData.pData = pBuffer;

	return ERROR_NO_ERROR;
}
