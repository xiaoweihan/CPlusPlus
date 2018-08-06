// Hdf5_Wrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Hdf5_Wrapper.h"
#include <memory>
#include <boost/current_function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <memory>
#include "Log.h"
#include "Authenticate.h"
#include "ErrorCode.h"
#include "H5Contex.h"
#include "CppH5Strategy.h"
#include "Utility.h"
#include "ConverterFactory.h"
#include "AbstractHeaderConverter.h"
//�̰߳�ȫ��
static boost::mutex s_Lock;

/********************************************************************************
funciton name:AdapterNodeType
function description:���ⲿ����Ľڵ���������Ϊ�ڲ��Ľڵ�����
input param:
output param:
return value:true:�ɹ� false:ʧ��
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static int AdapterNodeType(OuterType::NODE_TYPE eumSrcNodeType, InnerType::NODE_TYPE& eumDstNodeType)
{
	int nResult = ERROR_NO_ERROR;

	switch (eumSrcNodeType)
	{
		//��ڵ�
	case OuterType::NODE_BRANCH:
		eumDstNodeType = InnerType::NODE_TYPE::NODE_BRANCH;
		break;
		//Ҷ�ӽڵ�
	case OuterType::NODE_LEAF:
		eumDstNodeType = InnerType::NODE_TYPE::NODE_LEAF;
		break;
	default:
		nResult = ERROR_CONVERT_NODE_TYPE_FAILED;
		break;
	}

	return nResult;
}

/********************************************************************************
funciton name:AdapterNodeType
function description:��->��
input param:
output param:
return value:true:�ɹ� false:ʧ��
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static int AdapterNodeType(InnerType::NODE_TYPE eumSrcNodeType, OuterType::NODE_TYPE& eumDstNodeType)
{
	int nResult = ERROR_NO_ERROR;

	switch (eumSrcNodeType)
	{
		//��ڵ�
	case InnerType::NODE_TYPE::NODE_BRANCH:
		eumDstNodeType = OuterType::NODE_TYPE::NODE_BRANCH;
		break;
		//Ҷ�ӽڵ�
	case InnerType::NODE_TYPE::NODE_LEAF:
		eumDstNodeType = OuterType::NODE_TYPE::NODE_LEAF;
		break;
	default:
		nResult = ERROR_CONVERT_NODE_TYPE_FAILED;
		break;
	}

	return nResult;
}

/********************************************************************************
funciton name:AdapterAttr
function description:�������������ϢΪ�ڲ�������Ϣ
input param:
output param:
return value:
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static int AdapterAttr(const std::vector<OuterType::DATA_ATTRIBUTE>& SrcArray, std::vector<InnerType::DATA_ATTRIBUTE>& DstArray)
{
	int nResult = ERROR_NO_ERROR;
	DstArray.clear();
	for (auto& SrcElement : SrcArray)
	{
		if (SrcElement.pData == nullptr || SrcElement.Header.nTotalDataLen <= 0)
		{
			continue;
		}

		InnerType::DATA_ATTRIBUTE DstElement;
		DstElement.strKeyAttrName = SrcElement.strKeyAttrName;
		//��������ͷת��
		std::shared_ptr<CAbstractHeaderConverter> pHandler(ConverterFactory::CreateConverter(SrcElement.Header.eumDataType));
		if (pHandler)
		{
			//ת��ʧ��
			if (ERROR_NO_ERROR != (nResult = pHandler->ConvertDataHeader(SrcElement.Header, DstElement.Header)))
			{
				break;
			}
		}
		else
		{
			nResult = ERROR_ALLOCATE_FAILED;
			break;
		}
		//������Ч
		{
			//�������ݿ���
			DstElement.pData = new char[SrcElement.Header.nTotalDataLen];
			if (nullptr != DstElement.pData)
			{
				//�������ݿ���
				memcpy(DstElement.pData, SrcElement.pData, SrcElement.Header.nTotalDataLen);
				//�����ƶ����캯��
				DstArray.push_back(std::move(DstElement));
			}
		}
	}

	return nResult;
}

/********************************************************************************
funciton name:RecursiveAdapterNode
function description:�ݹ�����ڵ� ��->��
input param:
output param:
return value:
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static int RecursiveAdapterNode(const OuterType::HDF5_DATA* pSrcData, InnerType::LP_HDF5_WRITE_DATA pDstData)
{
	int nResult = ERROR_NO_ERROR;
	//�����Ϸ����ж�
	if (nullptr == pSrcData || nullptr == pDstData)
	{
		return ERROR_PARAM_INVALID;
	}

	//�����ƽ��и�ֵ
	pDstData->strKeyName = pSrcData->strKeyName;
	//����ע��
	AdapterAttr(pSrcData->AttributeArray, pDstData->AttributeArray);
	//����ڵ�����
	if (ERROR_NO_ERROR != AdapterNodeType(pSrcData->eumNodeType, pDstData->eumNodeType))
	{
		ERROR_LOG("call [%s] failed,AdapterNodeType failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_CONVERT_NODE_TYPE_FAILED;
	}
	//Ҷ�ӽڵ�
	if (OuterType::NODE_LEAF == pSrcData->eumNodeType)
	{

		//���ݺϷ����ж�
		if (pSrcData->pData == nullptr || pSrcData->Header.nTotalDataLen <= 0)
		{
			return ERROR_PARAM_INVALID;
		}

		//ѹ���ȸ�ֵ
		pDstData->nCompressLevel = pSrcData->nCompressLevel;
		//����ͷ��Ϣ

		//��������ͷת��
		std::shared_ptr<CAbstractHeaderConverter> pHandler(ConverterFactory::CreateConverter(pSrcData->Header.eumDataType));
		if (pHandler)
		{
			if (ERROR_NO_ERROR != (nResult = pHandler->ConvertDataHeader(pSrcData->Header, pDstData->Header)))
			{
				return nResult;
			}
		}
		else
		{
			return ERROR_ALLOCATE_FAILED;
		}
		unsigned int nLen = pSrcData->Header.nTotalDataLen;
		//����������ݣ�������һ������
		{
			char* pBuffer = new char[nLen];
			if (nullptr != pBuffer)
			{
				memcpy(pBuffer, pSrcData->pData, nLen);
				pDstData->pData = pBuffer;
				return ERROR_NO_ERROR;
			}
			ERROR_LOG("call [%s] failed,Allocate failed.", BOOST_CURRENT_FUNCTION);
			return ERROR_ALLOCATE_FAILED;
		}
		ERROR_LOG("call [%s] failed,the dataset data is empty.", BOOST_CURRENT_FUNCTION);
		return ERROR_PARAM_INVALID;	
	}

	//�����ӽڵ�
	for (auto& pElement : pSrcData->SubDataArray)
	{
		if (nullptr == pElement)
		{
			continue;
		}
		//�����ڴ�
		InnerType::LP_HDF5_WRITE_DATA pSubData = new InnerType::HDF5_WRITE_DATA;
		if (nullptr != pSubData)
		{
			if (ERROR_NO_ERROR == RecursiveAdapterNode(pElement, pSubData))
			{
				pDstData->SubDataArray.push_back(pSubData);
			}
			else
			{
				DELETE_POINTER(pSubData);
			}
		}
	}
	return ERROR_NO_ERROR;
}

/********************************************************************************
funciton name:AdapterData
function description:�������� ��->��
input param:
output param:
return value:
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static int AdapterData(const std::vector<OuterType::HDF5_DATA>& SrcArray, std::vector<InnerType::HDF5_WRITE_DATA>& DstArray)
{
	int nResult = ERROR_NO_ERROR;
	DstArray.clear();
	//��������
	for (auto& Element : SrcArray)
	{
		InnerType::HDF5_WRITE_DATA DstElement;
		//��ʼת���ӽڵ���Ϣ
		if (ERROR_NO_ERROR == RecursiveAdapterNode(&Element, &DstElement))
		{
			DstArray.push_back(std::move(DstElement));
		}
	}
	return nResult;
}

/*********************************************************
FunctionName:AdapterAttr
FunctionDesc:�������� ��->��
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
static int AdapterAttr(const std::vector<InnerType::DATA_ATTRIBUTE>& SrcAttrArray,std::vector<OuterType::DATA_RESULT_ATTRIBUTE>& DstAttrArray)
{
	int nResult = ERROR_NO_ERROR;
	DstAttrArray.clear();
	//�����ڲ�������
	for (auto& SrcAttrElement : SrcAttrArray)
	{
		OuterType::DATA_RESULT_ATTRIBUTE DstAttrElment;
		DstAttrElment.strKeyAttrName = SrcAttrElement.strKeyAttrName;

		//��������ͷת��
		std::shared_ptr<CAbstractHeaderConverter> pHandler(ConverterFactory::CreateConverter(SrcAttrElement.Header.eumDataType));
		if (pHandler)
		{
			nResult = pHandler->ConvertDataHeader(SrcAttrElement.Header, DstAttrElment.Header);
			if (ERROR_NO_ERROR != nResult)
			{
				break;
			}
		}
		else
		{
			nResult = ERROR_ALLOCATE_FAILED;
			break;
		}
		//��������
		DstAttrElment.ResultArray.resize(DstAttrElment.Header.nTotalDataLen);
		char* pBuffer = (char*)SrcAttrElement.pData;
		std::copy(pBuffer, pBuffer + DstAttrElment.Header.nTotalDataLen, DstAttrElment.ResultArray.begin());
		DstAttrArray.push_back(DstAttrElment);
	}
	return nResult;
}


/*********************************************************
FunctionName:AdapterData
FunctionDesc:�������� ��->��
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
static int AdapterData(const std::vector<InnerType::HDF5_READ_DATA>& SrcData, std::vector<OuterType::HDF5_RESULT>& DstData)
{
	int nResult = ERROR_NO_ERROR;

	for (auto& SrcElement : SrcData)
	{
		OuterType::HDF5_RESULT DstElement;
		//��������
		DstElement.strKeyName = SrcElement.strKeyName;
		//ת������
		if ((nResult = AdapterAttr(SrcElement.AttributeArray, DstElement.AttributeArray)) != 0)
		{
			break;
		}
		//ת���ڵ�����
		if (ERROR_NO_ERROR != (nResult = AdapterNodeType(SrcElement.eumNodeType, DstElement.eumNodeType)))
		{
			break;
		}
		
		//�����Ҷ�ӽڵ�Ž���ת��
		if (InnerType::NODE_TYPE::NODE_LEAF == SrcElement.eumNodeType)
		{
			//��������ͷת��
			std::shared_ptr<CAbstractHeaderConverter> pHandler(ConverterFactory::CreateConverter(SrcElement.Header.eumDataType));
			if (pHandler)
			{
				nResult = pHandler->ConvertDataHeader(SrcElement.Header, DstElement.Header);
				if (ERROR_NO_ERROR != nResult)
				{
					break;
				}
			}
			else
			{
				nResult = ERROR_ALLOCATE_FAILED;
				break;
			}
			char* pTempData = (char*)SrcElement.pData;
			DstElement.ResultArray.resize(DstElement.Header.nTotalDataLen);
			std::copy(pTempData, pTempData + DstElement.Header.nTotalDataLen, DstElement.ResultArray.begin());
			
		}
		DstData.push_back(DstElement);
	}
	return nResult;
}


/*********************************************************
FunctionName:Data2Hdf5
FunctionDesc:
InputParam:
OutputParam:
Return:0:�ɹ� ��0:������
Author:xiaowei.han
*********************************************************/
int OuterType::Data2Hdf5(const char* szFilePath, std::vector<HDF5_DATA>& DataArray)
{
	using namespace std;
	//����ǩ����֤
	CAuthentication::CreateInstance().AuthCall();
	//�����Ϸ����ж�
	if (nullptr == szFilePath || DataArray.empty())
	{
		ERROR_LOG("the param is invalid.");
		return ERROR_PARAM_INVALID;
	}
	vector<InnerType::HDF5_WRITE_DATA> DstArray;
	//���б�Ҫ������ת��
	AdapterData(DataArray, DstArray);
	//ʹ��CPP����h5������
	CH5Contex Contex;
	auto pHandler = std::make_shared<CCppH5Strategy>(szFilePath);
	if (!pHandler)
	{
		ERROR_LOG("call [%s] failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_ALLOCATE_FAILED;
	}
	//����ִ�в���
	Contex.SetH5Strategy(pHandler.get());
	int nResult = ERROR_NO_ERROR;
	{
		//����
		boost::lock_guard<boost::mutex> Locker(s_Lock);
		nResult = Contex.WriteH5(DstArray);
	}
	return nResult;
}

/*********************************************************
FunctionName:Hdf52Data
FunctionDesc:��ȡHDF5���ݵ��ڴ�
InputParam:
OutputParam:
Return:0:�ɹ� ��0:������
Author:xiaowei.han
*********************************************************/
int OuterType::Hdf52Data(const char* szFilePath, const std::vector<HDF5_QUERY_PARAM>& QueryParamVec, std::vector<HDF5_RESULT>& ResultVec)
{
	using namespace std;
	CAuthentication::CreateInstance().AuthCall();
	//�����Ϸ����ж�
	if (nullptr == szFilePath || QueryParamVec.empty())
	{
		ERROR_LOG("call [%s] failed,the param is invalid.", BOOST_CURRENT_FUNCTION);
		return ERROR_PARAM_INVALID;
	}

	//�ж��ļ��Ƿ����
	if (!Utility::IsExist(szFilePath))
	{
		ERROR_LOG("call [%s] failed,the file is not exist.", BOOST_CURRENT_FUNCTION);
		return ERROR_FILE_NOT_EXIST;
	}

	ResultVec.clear();

	//��������ת��
	vector<InnerType::HDF5_QUERY_PARAM> QueryParams;
	for (auto& Element : QueryParamVec)
	{
		QueryParams.emplace_back(Element.strQueryKeyName, Element.bQueryAttr);
	}

	CH5Contex Contex;
	auto pHandler = std::make_shared<CCppH5Strategy>(szFilePath);
	if (!pHandler)
	{
		ERROR_LOG("call [%s] failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_ALLOCATE_FAILED;
	}
	Contex.SetH5Strategy(pHandler.get());

	vector<InnerType::HDF5_READ_DATA> Results;
	int nResult = ERROR_NO_ERROR;
	{
		//����
		boost::lock_guard<boost::mutex> Locker(s_Lock);
		nResult = Contex.ReadH5(QueryParams,Results);
	}

	//��������ת��
	nResult = AdapterData(Results, ResultVec);

	return nResult;
}

/*********************************************************
FunctionName:Hdf52Data_Quick
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int OuterType::Hdf52Data_Quick(const char* szFilePath, const std::vector<HDF5_QUERY_PARAM>& QueryParamVec, std::vector<HDF5_RESULT>& ResultVec)
{
	using namespace std;
	CAuthentication::CreateInstance().AuthCall();
	//�����Ϸ����ж�
	if (nullptr == szFilePath || QueryParamVec.empty())
	{
		ERROR_LOG("call [%s] failed,the param is invalid.", BOOST_CURRENT_FUNCTION);
		return ERROR_PARAM_INVALID;
	}

	//�ж��ļ��Ƿ����
	if (!Utility::IsExist(szFilePath))
	{
		ERROR_LOG("call [%s] failed,the file is not exist.", BOOST_CURRENT_FUNCTION);
		return ERROR_FILE_NOT_EXIST;
	}

	ResultVec.clear();

	//��������ת��
	vector<InnerType::HDF5_QUERY_PARAM> QueryParams;
	for (auto& Element : QueryParamVec)
	{
		QueryParams.emplace_back(Element.strQueryKeyName, Element.bQueryAttr);
	}

	CH5Contex Contex;
	auto pHandler = std::make_shared<CCppH5Strategy>(szFilePath);
	if (!pHandler)
	{
		ERROR_LOG("call [%s] failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_ALLOCATE_FAILED;
	}
	Contex.SetH5Strategy(pHandler.get());

	vector<InnerType::HDF5_READ_DATA> Results;
	int nResult = ERROR_NO_ERROR;
	{
		//����
		boost::lock_guard<boost::mutex> Locker(s_Lock);
		nResult = Contex.ReadH5_Quick(QueryParams, Results);
	}

	//��������ת��
	nResult = AdapterData(Results, ResultVec);

	return nResult;
}

/*********************************************************
FunctionName:ReadHdf5MatrixData
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int OuterType::ReadHdf5MatrixData(const char* szFilePath, const std::vector<HDF5_QUERY_MATRIX_PARAM>& QueryParamVec, std::vector<HDF5_RESULT>& ResultVec)
{
	using namespace std;
	CAuthentication::CreateInstance().AuthCall();
	//�����Ϸ����ж�
	if (nullptr == szFilePath || QueryParamVec.empty())
	{
		ERROR_LOG("call [%s] failed,the param is invalid.", BOOST_CURRENT_FUNCTION);
		return ERROR_PARAM_INVALID;
	}
	//�ж��ļ��Ƿ����
	if (!Utility::IsExist(szFilePath))
	{
		ERROR_LOG("call [%s] failed,the file is not exist.", BOOST_CURRENT_FUNCTION);
		return ERROR_FILE_NOT_EXIST;
	}

	ResultVec.clear();

	//��������ת��
	vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM> QueryParams;
	for (auto& Element : QueryParamVec)
	{
		//�������ݹ���
		if (Element.strQueryKeyName.empty() || Element.nIndex < 0)
		{
			continue;
		}

		QueryParams.emplace_back(Element.strQueryKeyName,Element.nIndex,Element.bColumn);
	}


	CH5Contex Contex;
	auto pHandler = std::make_shared<CCppH5Strategy>(szFilePath);
	if (!pHandler)
	{
		ERROR_LOG("call [%s] failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_ALLOCATE_FAILED;
	}
	Contex.SetH5Strategy(pHandler.get());

	vector<InnerType::HDF5_READ_DATA> Results;
	int nResult = ERROR_NO_ERROR;
	{
		//����
		boost::lock_guard<boost::mutex> Locker(s_Lock);
		nResult = Contex.ReadH5_PartialData(QueryParams, Results);
	}

	//��������ת��
	nResult = AdapterData(Results, ResultVec);

	return nResult;
}


#if 0
//������������
static bool AdapterDataType(InnerType::DATA_TYPE eumSrcDataType, Hdf5_Wrapper::DATA_TYPE& eumDstDataType)
{
	bool bResult = true;
	switch (eumSrcDataType)
	{
	case InnerType::DATA_TYPE::INT_TYPE:
		eumDstDataType = Hdf5_Wrapper::DATA_TYPE::INT_TYPE;
		break;
	case InnerType::DATA_TYPE::FLOAT_TYPE:
		eumDstDataType = Hdf5_Wrapper::DATA_TYPE::FLOAT_TYPE;
		break;
	case InnerType::DATA_TYPE::STR_TYPE:
		eumDstDataType = Hdf5_Wrapper::DATA_TYPE::STR_TYPE;
		break;
	case InnerType::DATA_TYPE::COMPOUND_TYPE:
		eumDstDataType = Hdf5_Wrapper::DATA_TYPE::COMPOUND_TYPE;
		break;
	default:
		bResult = false;
		break;

	}

	return bResult;
}


//��������ͷ
static int AdapterDataHeader(const InnerType::DATA_HEADER& SrcDataHeader, Hdf5_Wrapper::DATA_HEADER& DstDataHeader)
{
	if (nullptr == SrcDataHeader.pDimensionArray || SrcDataHeader.nRank <= 0)
	{
		return ERROR_DATA_HEADER_INVALID;

	}
	int nResult = ERROR_NO_ERROR;
	DstDataHeader.nRank = SrcDataHeader.nRank;
	DstDataHeader.nDataByte = SrcDataHeader.nDataByte;
	//ת������ʧ��
	if (!AdapterDataType(SrcDataHeader.eumDataType, DstDataHeader.eumDataType))
	{
		return ERROR_DATA_TYPE_ADAPT_FAILED;
	}

	unsigned int nTotalLen = 1;
	//ת��ά����Ϣ
	for (int i = 0; i < SrcDataHeader.nRank; ++i)
	{
		DstDataHeader.DimensionArray.push_back(SrcDataHeader.pDimensionArray[i]);
		nTotalLen *= SrcDataHeader.pDimensionArray[i];
	}

	//�������ݵ��ڴ�����峤��
	DstDataHeader.nTotalDataLen = nTotalLen * SrcDataHeader.nDataByte;


	//���������������������
	if (InnerType::DATA_TYPE::INT_TYPE == SrcDataHeader.eumDataType)
	{
		auto p = boost::get<InnerType::INT_DATA_HEADER>(&SrcDataHeader.AdditionalDataType);

		if (nullptr == p)
		{
			return ERROR_CONVERT_INT_TYPE_HEADER_FAILED;
		}

		Hdf5_Wrapper::INT_DATA_HEADER TempIntHeader;
		TempIntHeader.bSign = p->bSign;
		DstDataHeader.ExternDataType = TempIntHeader;
		return ERROR_NO_ERROR;
	}

	if (InnerType::DATA_TYPE::COMPOUND_TYPE == SrcDataHeader.eumDataType)
	{

		auto p = boost::get<InnerType::COMPOUND_TYPE_HEADER>(&SrcDataHeader.AdditionalDataType);

		if (nullptr == p)
		{
			return ERROR_CONVERT_COMPOUND_TYPE_HEADER_FAILED;
		}

		Hdf5_Wrapper::COMPOUND_TYPE_HEADER TempCompoundHeader;
		for (auto& SubElement : p->ElementArray)
		{
			Hdf5_Wrapper::COMPOUND_TYPE_ELEMENT TempElement;
			TempElement.nDataByte = SubElement.nDataByte;
			TempElement.strElementName = SubElement.strElementName;
			TempElement.nElementoffset = SubElement.nElementoffset;

			//ת������ͷ
			if (!AdapterDataType(SubElement.eumDataType, TempElement.eumDataType))
			{
				return ERROR_DATA_TYPE_ADAPT_FAILED;
			}

			if (Hdf5_Wrapper::DATA_TYPE::INT_TYPE == TempElement.eumDataType)
			{
				auto p = boost::get<InnerType::INT_DATA_HEADER>(&SubElement.AdditionalDataType);
				if (nullptr == p)
				{
					return ERROR_CONVERT_INT_TYPE_HEADER_FAILED;
				}

				Hdf5_Wrapper::INT_DATA_HEADER TempIntHeader;
				TempIntHeader.bSign = p->bSign;
				TempElement.AdditionalDataType = TempIntHeader;
			}

			TempCompoundHeader.ElementArray.push_back(TempElement);
		}

		DstDataHeader.ExternDataType = TempCompoundHeader;

		return ERROR_NO_ERROR;

	}

	return ERROR_NO_ERROR;
}

/********************************************************************************
funciton name:AdapterDataType
function description:����紫�����������ת��Ϊ�ڲ���Ҫ����������
input param:
output param:
return value:true:�ɹ� false:ʧ��
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static bool AdapterDataType(Hdf5_Wrapper::DATA_TYPE eumSrcDataType, InnerType::DATA_TYPE& eumDstDataType)
{
	bool bResult = true;

	switch (eumSrcDataType)
	{
		//��������
	case Hdf5_Wrapper::INT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::INT_TYPE;
		break;
		//��������
	case Hdf5_Wrapper::FLOAT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::FLOAT_TYPE;
		break;
		//�ַ�������
	case Hdf5_Wrapper::STR_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::STR_TYPE;
		break;
		//��������
	case Hdf5_Wrapper::COMPOUND_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::COMPOUND_TYPE;
		break;
	default:
		bResult = false;
		break;
	}

	return bResult;
}

/********************************************************************************
funciton name:AdapterDataHeader
function description:��������ͷ
input param:
output param:
return value:true:�ɹ� false:ʧ��
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static bool AdapterDataHeader(const Hdf5_Wrapper::DATA_HEADER& SrcDataHeader, InnerType::DATA_HEADER& DstDataHeader)
{

	//ת������ʧ��
	if (!AdapterDataType(SrcDataHeader.eumDataType, DstDataHeader.eumDataType))
	{
		ERROR_LOG("call [%s] failed,AdapterDataType failed.", BOOST_CURRENT_FUNCTION);
		return false;
	}

	//�Ϸ��Լ��
	if (SrcDataHeader.nDataByte <= 0)
	{
		ERROR_LOG("call [%s] failed,the DataByte is [%d]", BOOST_CURRENT_FUNCTION, SrcDataHeader.nDataByte);
		return false;
	}

	//��������ռ�õ��ֽ���
	DstDataHeader.nDataByte = SrcDataHeader.nDataByte;

	if (SrcDataHeader.nRank <= 0)
	{
		ERROR_LOG("call [%s] failed,the rank is [%d]", BOOST_CURRENT_FUNCTION, SrcDataHeader.nRank);
		return false;
	}

	//����ά����Ϣ
	DstDataHeader.nRank = SrcDataHeader.nRank;

	int nDimenLen = (int)SrcDataHeader.DimensionArray.size();

	if (nDimenLen > 0)
	{
		//�����ڴ�
		DstDataHeader.pDimensionArray = new int[nDimenLen];

		if (nullptr != DstDataHeader.pDimensionArray)
		{
			for (int i = 0; i < nDimenLen; ++i)
			{
				DstDataHeader.pDimensionArray[i] = SrcDataHeader.DimensionArray[i];
			}
		}
	}

	//�������������
	if (InnerType::DATA_TYPE::INT_TYPE == DstDataHeader.eumDataType)
	{
		InnerType::INT_DATA_HEADER TempHeader;
		auto p = boost::get<Hdf5_Wrapper::INT_DATA_HEADER>(&SrcDataHeader.ExternDataType);
		if (nullptr != p)
		{
			TempHeader.bSign = p->bSign;
			DstDataHeader.AdditionalDataType = TempHeader;
			return true;
		}
		ERROR_LOG("call [%s] failed,pConvertIntDataHeader failed.", BOOST_CURRENT_FUNCTION);
		return false;

	}
	//����Ǹ�������
	if (InnerType::DATA_TYPE::COMPOUND_TYPE == DstDataHeader.eumDataType)
	{
		auto p = boost::get<Hdf5_Wrapper::COMPOUND_TYPE_HEADER>(&SrcDataHeader.ExternDataType);
		if (nullptr != p)
		{
			bool bBreak = false;
			InnerType::COMPOUND_TYPE_HEADER TempHeader;
			for (auto& Element : p->ElementArray)
			{
				InnerType::COMPOUND_TYPE_ELEMENT TempElement;
				TempElement.strElementName = Element.strElementName;
				TempElement.nElementoffset = Element.nElementoffset;
				TempElement.nDataByte = Element.nDataByte;
				//��������ʧ��
				if (!AdapterDataType(Element.eumDataType, TempElement.eumDataType))
				{
					bBreak = true;
					break;
				}

				//�������������
				if (Hdf5_Wrapper::DATA_TYPE::INT_TYPE == Element.eumDataType)
				{
					InnerType::INT_DATA_HEADER TempIntHeader;
					auto p = boost::get<Hdf5_Wrapper::INT_DATA_HEADER>(&Element.AdditionalDataType);
					if (nullptr == p)
					{
						bBreak = true;
						break;
					}
					TempIntHeader.bSign = p->bSign;
					TempElement.AdditionalDataType = TempIntHeader;
				}
				TempHeader.ElementArray.push_back(std::move(TempElement));
			}

			if (bBreak)
			{
				return false;
			}
			//��������ö������
			DstDataHeader.AdditionalDataType = TempHeader;
			return true;
		}
		return false;
	}
	return true;
}
#endif