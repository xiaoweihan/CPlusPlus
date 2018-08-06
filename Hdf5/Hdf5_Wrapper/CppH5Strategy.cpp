/*********************************************************
Copyright(C):
FileName:CppH5Strategy.cpp
Descripton:C++模式操作h5算法
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "CppH5Strategy.h"
#include <H5Cpp.h>
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>
#include "ErrorCode.h"
#include "Log.h"
#include "Utility.h"
#include "H5AbstractNode.h"
#include "GroupVisitor.h"
#include "IntDataSet.h"
#include "FloatDataSet.h"
#include "CompoundDataSet.h"
#include "StrDataSet.h"
/*********************************************************
FunctionName:ReadH5
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CCppH5Strategy::ReadH5(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	using namespace H5;
	if (QueryParams.empty())
	{
		return ERROR_PARAM_INVALID;
	}
	//判断文件是否是Hdf5格式的
	if (!H5File::isHdf5(m_strFilePath.c_str()))
	{
		ERROR_LOG("the file [%s] is not hdf5 format.", m_strFilePath.c_str());
		return ERROR_FILE_FORMAT_INVALID;
	}

	try
	{
		Exception::dontPrint();
		H5File Reader(m_strFilePath, H5F_ACC_RDONLY);
		BOOST_SCOPE_EXIT(&Reader)
		{
			Reader.close();
		}BOOST_SCOPE_EXIT_END;

		//定义一个组访问器
		auto pGroupVisitor = std::make_shared<CGroupVisitor>(QueryParams);

		pGroupVisitor->Visit(&Reader,Results);

	}


	catch (const FileIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a FileIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const GroupIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a GroupIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const DataSetIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a DataSetIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const DataSpaceIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a DataSpaceIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const PropListIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a PropListIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const AttributeIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a AttributeIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}


	catch (const Exception& e)
	{
		ERROR_LOG("ReadHdf5File raise a exception,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	return ERROR_NO_ERROR;
}

/*********************************************************
FunctionName:WriteH5
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CCppH5Strategy::WriteH5(std::vector<InnerType::HDF5_WRITE_DATA>& DataArray)
{
	using namespace H5;
	using namespace std;
	int nResult = ERROR_NO_ERROR;
	if (DataArray.empty())
	{
		return ERROR_PARAM_INVALID;
	}

	boost::ptr_vector<CH5AbstractNode> NodeTree;
	//根据数据组装节点
	for (auto& Element : DataArray)
	{
		auto pNode = RecursiveCreateNode(&Element);
		if (pNode)
		{
			NodeTree.push_back(pNode);
		}
	}
	//开始生成H5文件
	try
	{
		Exception::dontPrint();
		H5File Writer(m_strFilePath, H5F_ACC_TRUNC);
		BOOST_SCOPE_EXIT(&Writer)
		{
			Writer.close();
		}BOOST_SCOPE_EXIT_END;

		//逐级生成
		for (int i = 0; i < (int)NodeTree.size(); ++i)
		{
			NodeTree[i].CreateSelfToFile(&Writer);
		}
		Writer.flush(H5F_SCOPE_LOCAL);
	}

	catch (const FileIException& e)
	{
		ERROR_LOG("WriteHdf5File raise a FileIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_WRITE_HDF5_FAILED;
	}

	catch (const GroupIException& e)
	{
		ERROR_LOG("WriteHdf5File raise a GroupIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_WRITE_HDF5_FAILED;
	}

	catch (const DataSetIException& e)
	{
		ERROR_LOG("WriteHdf5File raise a DataSetIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_WRITE_HDF5_FAILED;
	}

	catch (const DataSpaceIException& e)
	{
		ERROR_LOG("WriteHdf5File raise a DataSpaceIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_WRITE_HDF5_FAILED;
	}

	catch (const PropListIException& e)
	{
		ERROR_LOG("WriteHdf5File raise a PropListIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_WRITE_HDF5_FAILED;
	}

	catch (const AttributeIException& e)
	{
		ERROR_LOG("WriteHdf5File raise a AttributeIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_WRITE_HDF5_FAILED;
	}

	catch (const Exception& e)
	{
		ERROR_LOG("WriteHdf5File raise a exception,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_WRITE_HDF5_FAILED;
	}

	return nResult;

}

/*********************************************************
FunctionName:ReadH5_Quick
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CCppH5Strategy::ReadH5_Quick(const std::vector<InnerType::HDF5_QUERY_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	using namespace H5;
	if (QueryParams.empty())
	{
		return ERROR_PARAM_INVALID;
	}
	//判断文件是否是Hdf5格式的
	if (!H5File::isHdf5(m_strFilePath.c_str()))
	{
		ERROR_LOG("the file [%s] is not hdf5 format.", m_strFilePath.c_str());
		return ERROR_FILE_FORMAT_INVALID;
	}

	try
	{
		Exception::dontPrint();
		H5File Reader(m_strFilePath, H5F_ACC_RDONLY);
		BOOST_SCOPE_EXIT(&Reader)
		{
			Reader.close();
		}BOOST_SCOPE_EXIT_END;


		for (auto& QueryElement: QueryParams)
		{
			//获取dataset数据类型
			auto pSubDataset = new DataSet(Reader.openDataSet(QueryElement.strQueryKeyName));

			if (nullptr != pSubDataset)
			{
				//获取数据类型
				auto SubDatasetDataType = pSubDataset->getTypeClass();
				CH5AbstractNode* pVisitor = nullptr;
				switch (SubDatasetDataType)
				{
					//整数类型
				case H5T_INTEGER:
					pVisitor = new CIntDataSet;
					break;
					//浮点数类型
				case H5T_FLOAT:
					pVisitor = new CFloatDataSet;
					break;
					//字符串类型
				case H5T_STRING:
					pVisitor = new CStrDataSet;
					break;
					//复合类型
				case H5T_COMPOUND:
					pVisitor = new CCompoundDataSet;
					break;
				default:
					break;
				}
				if (nullptr != pVisitor)
				{
					InnerType::HDF5_READ_DATA Result;
					Result.eumNodeType = InnerType::NODE_TYPE::NODE_LEAF;
					Result.strKeyName = QueryElement.strQueryKeyName;
					if (!pVisitor->ParseSelf(pSubDataset, Result))
					{
						//放到结果集中
						Results.push_back(std::move(Result));
					}
					DELETE_POINTER(pVisitor);
				}
				DELETE_POINTER(pSubDataset);
			}
		}
	}



	catch (const FileIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a FileIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const GroupIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a GroupIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const DataSetIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a DataSetIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const DataSpaceIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a DataSpaceIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const PropListIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a PropListIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const AttributeIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a AttributeIException,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	catch (const Exception& e)
	{
		ERROR_LOG("ReadHdf5File raise a exception,the exception is [%s].", e.getDetailMsg().c_str());
		return ERROR_READ_HDF5_FAILED;
	}

	return ERROR_NO_ERROR;
}


/*********************************************************
FunctionName:ReadH5_PartialData读取部分数据
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CCppH5Strategy::ReadH5_PartialData(const std::vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	using namespace H5;
	//数据合法性判断
	if (QueryParams.empty())
	{
		return ERROR_PARAM_INVALID;
	}
	//判断文件是否是Hdf5格式的
	if (!H5File::isHdf5(m_strFilePath.c_str()))
	{
		ERROR_LOG("the file [%s] is not hdf5 format.", m_strFilePath.c_str());
		return ERROR_FILE_FORMAT_INVALID;
	}

	int nResult = ERROR_NO_ERROR;

	try
	{
		//关闭异常输出
		Exception::dontPrint();
		H5File Reader(m_strFilePath, H5F_ACC_RDONLY);
		BOOST_SCOPE_EXIT(&Reader)
		{
			Reader.close();
		}BOOST_SCOPE_EXIT_END;


		//开始遍历查询的元素

		for (auto& Element : QueryParams)
		{

			//获取dataset数据类型
			auto pSubDataset = new DataSet(Reader.openDataSet(Element.strQueryKeyName));

			//内存申请成功
			if (nullptr != pSubDataset)
			{

				//获取数据类型
				auto SubDatasetDataType = pSubDataset->getTypeClass();
				CH5AbstractNode* pVisitor = nullptr;
				switch (SubDatasetDataType)
				{
					//整数类型
				case H5T_INTEGER:
					pVisitor = new CIntDataSet;
					break;
					//浮点数类型
				case H5T_FLOAT:
					pVisitor = new CFloatDataSet;
					break;
					//字符串类型
				case H5T_STRING:
					pVisitor = new CStrDataSet;
					break;
					//复合类型暂时不支持
				case H5T_COMPOUND:
					nResult = ERROR_NOT_SUPPORT_DATA_TYPE;
					break;
				default:
					break;
				}
				if (nullptr != pVisitor)
				{
					InnerType::HDF5_READ_DATA Result;
					Result.eumNodeType = InnerType::NODE_TYPE::NODE_LEAF;
					Result.strKeyName = Element.strQueryKeyName;
					if (!pVisitor->ReadPartialData(pSubDataset,Element,Result))
					{
						//放到结果集中
						Results.push_back(std::move(Result));
					}
					DELETE_POINTER(pVisitor);
				}
				DELETE_POINTER(pSubDataset);
			}
		}
	}
	catch (const FileIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a FileIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_READ_HDF5_FAILED;
	}

	catch (const GroupIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a GroupIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_READ_HDF5_FAILED;
	}

	catch (const DataSetIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a DataSetIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_READ_HDF5_FAILED;
	}

	catch (const DataSpaceIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a DataSpaceIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_READ_HDF5_FAILED;
	}

	catch (const PropListIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a PropListIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_READ_HDF5_FAILED;
	}

	catch (const AttributeIException& e)
	{
		ERROR_LOG("ReadHdf5File raise a AttributeIException,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_READ_HDF5_FAILED;
	}

	catch (const Exception& e)
	{
		ERROR_LOG("ReadHdf5File raise a exception,the exception is [%s].", e.getDetailMsg().c_str());
		nResult = ERROR_READ_HDF5_FAILED;
	}

	return nResult;
}
