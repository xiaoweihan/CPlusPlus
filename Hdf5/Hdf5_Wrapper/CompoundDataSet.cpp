/*********************************************************
Copyright(C):
FileName:CompoundDataSet.cpp
Descripton:用于复合类型的dataset的读写
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
FunctionDesc:获取子节点的个数
InputParam:
OutputParam:
Return:0:成功 非0:错误码
Author:xiaowei.han
*********************************************************/
unsigned int CCompoundDataSet::GetChildNum(void)
{
	return 0;
}

/*********************************************************
FunctionName:CreateSelfToFile
FunctionDesc:把自身节点数据写进文件
InputParam:
OutputParam:
Return:0:成功 非0:错误码
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
			}BOOST_SCOPE_EXIT_END;

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

/*********************************************************
FunctionName:ParseSelf
FunctionDesc:解析自身节点
InputParam:
OutputParam:
Return:0:成功 非0:错误码
Author:xiaowei.han
*********************************************************/
int CCompoundDataSet::ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData)
{
	if (nullptr == pDataset)
	{
		return ERROR_PARAM_INVALID;
	}

	using namespace H5;
	//闭包
	auto pScalarHandler = [this](H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DatasetData)->int
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
		auto DatasetDataType = pDataset->getCompType();
		//申请内存
		char* pData = new char[nDatasetValueLen];
		if (nullptr != pData)
		{
			//数值类型
			DatasetData.Header.eumDataType = InnerType::DATA_TYPE::COMPOUND_TYPE;
			//数值类型占用的字节数
			DatasetData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
			//获取维度信息
			DatasetData.Header.nRank = 1;
			int* pDimensionArray = new int[1]{ 1 };
			DatasetData.Header.pDimensionArray = pDimensionArray;

			//设置compound的子类型
			if (!ParseComType(DatasetDataType, DatasetData))
			{
				return ERROR_PARSE_COMPOUND_TYPE_FAILED;
			}
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
	auto pSimpleHandler = [this](H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DatasetData)->int
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
		auto DatasetDataType = pDataset->getCompType();
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
					DatasetData.Header.eumDataType = InnerType::DATA_TYPE::COMPOUND_TYPE;
					//数值类型占用的字节数
					DatasetData.Header.nDataByte = (unsigned int)DatasetDataType.getSize();
					DatasetData.Header.pDimensionArray = pDimensionArray;
					//设置compound的子类型
					if (!ParseComType(DatasetDataType, DatasetData))
					{
						return ERROR_PARSE_COMPOUND_TYPE_FAILED;
					}
					
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

/*********************************************************
FunctionName:ParseComType
FunctionDesc:解析复合类型
InputParam:
OutputParam:
Return:0:成功 非0:错误码
Author:xiaowei.han
*********************************************************/
bool CCompoundDataSet::ParseComType(H5::CompType& CompoundDataType, InnerType::HDF5_READ_DATA& DstData)
{
	//获取成员的个数
	int nMemNum = CompoundDataType.getNmembers();

	if (nMemNum <= 0)
	{
		return false;
	}

	InnerType::COMPOUND_TYPE_HEADER ComTypeHeader;

	for (int i = 0; i < nMemNum; ++i)
	{
		InnerType::COMPOUND_TYPE_ELEMENT ComTypeElement;
		//获取名称
		ComTypeElement.strElementName = CompoundDataType.getMemberName(i);
		//获取偏移量
		ComTypeElement.nElementoffset = (unsigned int)CompoundDataType.getMemberOffset(i);
		//获取类型
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
