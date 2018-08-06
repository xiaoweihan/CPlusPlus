/*********************************************************
Copyright(C):
FileName:CppH5Strategy.cpp
Descripton:C++ģʽ����h5�㷨
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
	//�ж��ļ��Ƿ���Hdf5��ʽ��
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

		//����һ���������
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
	//����������װ�ڵ�
	for (auto& Element : DataArray)
	{
		auto pNode = RecursiveCreateNode(&Element);
		if (pNode)
		{
			NodeTree.push_back(pNode);
		}
	}
	//��ʼ����H5�ļ�
	try
	{
		Exception::dontPrint();
		H5File Writer(m_strFilePath, H5F_ACC_TRUNC);
		BOOST_SCOPE_EXIT(&Writer)
		{
			Writer.close();
		}BOOST_SCOPE_EXIT_END;

		//������
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
	//�ж��ļ��Ƿ���Hdf5��ʽ��
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
			//��ȡdataset��������
			auto pSubDataset = new DataSet(Reader.openDataSet(QueryElement.strQueryKeyName));

			if (nullptr != pSubDataset)
			{
				//��ȡ��������
				auto SubDatasetDataType = pSubDataset->getTypeClass();
				CH5AbstractNode* pVisitor = nullptr;
				switch (SubDatasetDataType)
				{
					//��������
				case H5T_INTEGER:
					pVisitor = new CIntDataSet;
					break;
					//����������
				case H5T_FLOAT:
					pVisitor = new CFloatDataSet;
					break;
					//�ַ�������
				case H5T_STRING:
					pVisitor = new CStrDataSet;
					break;
					//��������
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
						//�ŵ��������
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
FunctionName:ReadH5_PartialData��ȡ��������
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CCppH5Strategy::ReadH5_PartialData(const std::vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM>& QueryParams, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	using namespace H5;
	//���ݺϷ����ж�
	if (QueryParams.empty())
	{
		return ERROR_PARAM_INVALID;
	}
	//�ж��ļ��Ƿ���Hdf5��ʽ��
	if (!H5File::isHdf5(m_strFilePath.c_str()))
	{
		ERROR_LOG("the file [%s] is not hdf5 format.", m_strFilePath.c_str());
		return ERROR_FILE_FORMAT_INVALID;
	}

	int nResult = ERROR_NO_ERROR;

	try
	{
		//�ر��쳣���
		Exception::dontPrint();
		H5File Reader(m_strFilePath, H5F_ACC_RDONLY);
		BOOST_SCOPE_EXIT(&Reader)
		{
			Reader.close();
		}BOOST_SCOPE_EXIT_END;


		//��ʼ������ѯ��Ԫ��

		for (auto& Element : QueryParams)
		{

			//��ȡdataset��������
			auto pSubDataset = new DataSet(Reader.openDataSet(Element.strQueryKeyName));

			//�ڴ�����ɹ�
			if (nullptr != pSubDataset)
			{

				//��ȡ��������
				auto SubDatasetDataType = pSubDataset->getTypeClass();
				CH5AbstractNode* pVisitor = nullptr;
				switch (SubDatasetDataType)
				{
					//��������
				case H5T_INTEGER:
					pVisitor = new CIntDataSet;
					break;
					//����������
				case H5T_FLOAT:
					pVisitor = new CFloatDataSet;
					break;
					//�ַ�������
				case H5T_STRING:
					pVisitor = new CStrDataSet;
					break;
					//����������ʱ��֧��
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
						//�ŵ��������
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
