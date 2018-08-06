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

	//创建所需的数据类型
	shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(*m_pHeaderInfo));

	if (!pDataTypeGenerator)
	{
		ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_ALLOCATE_FAILED;
	}
	//生产出合适的数据类型
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
			//获取维度信息
			for (int i = 0; i < m_pHeaderInfo->nRank; ++i)
			{
				pDims[i] = m_pHeaderInfo->pDimensionArray[i];
			}

			//创建数据空间
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

				//采用压缩
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

					//写入属性
					for (auto& AttrElement : m_AttrList)
					{
						if (AttrElement != nullptr)
						{
							AttrElement->WriteSelf(pDataSet);
						}
					}
				}
			//不采用压缩
				else
				{
					//创建DataSet
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

					//写入属性
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
	//闭包
	auto pScalarHandler = [](H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DatasetData)->int
	{
		if (nullptr == pDataset)
		{
			return ERROR_PARAM_INVALID;
		}

		//获取占用的字节数
		auto nDatasetValueLen = pDataset->getInMemDataSize();
		//获取DataSpace
		auto DatasetDataSpace = pDataset->getSpace();
		//数据类型
		auto DatasetDataType = pDataset->getStrType();
		//申请内存
		char* pData = new char[nDatasetValueLen];
		if (nullptr != pData)
		{
			//数值类型
			DatasetData.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
			//数值类型占用的字节数
			DatasetData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
			//获取维度信息
			DatasetData.Header.nRank = 1;
			int* pDimensionArray = new int[1]{ 1 };
			DatasetData.Header.pDimensionArray = pDimensionArray;
			//创建所需的数据类型
			std::shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(DatasetData.Header));
			if (!pDataTypeGenerator)
			{
				ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
				return ERROR_ALLOCATE_FAILED;
			}
			//生产出合适的数据类型
			std::shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());
			if (!pSuitableDataType)
			{
				ERROR_LOG("call [%s] failed,allocate DataType failed.", BOOST_CURRENT_FUNCTION);
				return ERROR_ALLOCATE_FAILED;
			}
			//进行数据读取
			pDataset->read(pData, *pSuitableDataType);
			//属性数值
			DatasetData.pData = pData;
			return ERROR_NO_ERROR;
		}

		return ERROR_ALLOCATE_FAILED;
	};

	//闭包
	auto pSimpleHandler = [](H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DatasetData)->int
	{
		if (nullptr == pDataset)
		{
			return ERROR_PARAM_INVALID;
		}

		//获取占用的字节数
		auto nDatasetValueLen = pDataset->getInMemDataSize();
		//获取DataSpace
		auto DatasetDataSpace = pDataset->getSpace();
		//数据类型
		auto DatasetDataType = pDataset->getStrType();
		//申请内存
		char* pData = new char[nDatasetValueLen];
		if (nullptr != pData)
		{
			//获取维度信息
			int nRank = DatasetDataSpace.getSimpleExtentNdims();
			//申请内存
			boost::scoped_array<hsize_t> pDimsArray(new hsize_t[nRank]);
			if (pDimsArray)
			{
				//获取具体的维度信息
				DatasetDataSpace.getSimpleExtentDims(pDimsArray.get(), NULL);
				//保存维度信息
				int* pDimensionArray = new int[nRank];
				if (nullptr != pDimensionArray)
				{
					for (int i = 0; i < nRank; ++i)
					{
						pDimensionArray[i] = (int)pDimsArray[i];
					}
					//维数
					DatasetData.Header.nRank = nRank;
					//数值类型
					DatasetData.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
					//数值类型占用的字节数
					DatasetData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
					//设置维度信息
					DatasetData.Header.pDimensionArray = pDimensionArray;
					//创建所需的数据类型
					std::shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(DatasetData.Header));
					if (!pDataTypeGenerator)
					{
						ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
						return ERROR_ALLOCATE_FAILED;
					}
					//生产出合适的数据类型
					std::shared_ptr<DataType> pSuitableDataType(pDataTypeGenerator->CreateProperType());
					if (!pSuitableDataType)
					{
						ERROR_LOG("call [%s] failed,allocate DataType failed.", BOOST_CURRENT_FUNCTION);
						return ERROR_ALLOCATE_FAILED;
					}
					//进行数据读取
					pDataset->read(pData, *pSuitableDataType);
					//指针赋值
					DatasetData.pData = pData;
					return ERROR_NO_ERROR;
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

		return ERROR_ALLOCATE_FAILED;
	};

	//获取Dataset的Dataspace
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

	//参数合法性判断
	if (nullptr == pDataset)
	{
		return ERROR_PARAM_INVALID;
	}

	//获取DataSpace
	auto DatasetDataSpace = pDataset->getSpace();
	//数据类型
	auto DatasetDataType = pDataset->getIntType();
	//获取维度信息
	int nRank = DatasetDataSpace.getSimpleExtentNdims();
	//获取每个数据占用的字节数
	int nDataByte = (int)DatasetDataType.getSize();

	hsize_t DimsArray[MATRIX_RANK] = { 0 };

	//获取具体的维度信息
	DatasetDataSpace.getSimpleExtentDims(DimsArray, NULL);

	//获取总的行数和列数
	int nRowNum = (int)DimsArray[0];
	int nColumnNum = (int)DimsArray[1];

	//数据不是2个维度的
	if (2 != nRank)
	{
		return ERROR_RANK_NOT_SUPPORT;
	}

	//保存维度信息
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
	//如果是行
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
	//如果是列
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


	//维数
	DstData.Header.nRank = 1;
	//数值类型
	DstData.Header.eumDataType = InnerType::DATA_TYPE::STR_TYPE;
	//数值类型占用的字节数
	DstData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
	//设置维度信息
	DstData.Header.pDimensionArray = pDimensionArray;

	//创建所需的数据类型
	std::shared_ptr<CAbstractDataType> pDataTypeGenerator(DataTypeFactory::CreateDataType(DstData.Header));
	if (!pDataTypeGenerator)
	{
		ERROR_LOG("call [%s] failed,allocate CAbstractDataType failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_ALLOCATE_FAILED;
	}
	//生产出合适的数据类型
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

	//选择超块
	DatasetDataSpace.selectHyperslab(H5S_SELECT_SET, CountArray, OffsetArray, StrideArray, BlockArray);

	//定义一个内存空间
	DataSpace memspace(1, MemDataSpaceDimenArray, NULL);

	//进行读取
	pDataset->read(pBuffer, *pSuitableDataType);

	DstData.pData = pBuffer;

	return ERROR_NO_ERROR;
}
