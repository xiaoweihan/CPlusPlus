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
//线程安全锁
static boost::mutex s_Lock;

/********************************************************************************
funciton name:AdapterNodeType
function description:把外部传入的节点类型适配为内部的节点类型
input param:
output param:
return value:true:成功 false:失败
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static int AdapterNodeType(OuterType::NODE_TYPE eumSrcNodeType, InnerType::NODE_TYPE& eumDstNodeType)
{
	int nResult = ERROR_NO_ERROR;

	switch (eumSrcNodeType)
	{
		//组节点
	case OuterType::NODE_BRANCH:
		eumDstNodeType = InnerType::NODE_TYPE::NODE_BRANCH;
		break;
		//叶子节点
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
function description:内->外
input param:
output param:
return value:true:成功 false:失败
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static int AdapterNodeType(InnerType::NODE_TYPE eumSrcNodeType, OuterType::NODE_TYPE& eumDstNodeType)
{
	int nResult = ERROR_NO_ERROR;

	switch (eumSrcNodeType)
	{
		//组节点
	case InnerType::NODE_TYPE::NODE_BRANCH:
		eumDstNodeType = OuterType::NODE_TYPE::NODE_BRANCH;
		break;
		//叶子节点
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
function description:适配外界属性信息为内部属性信息
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
		//进行数据头转换
		std::shared_ptr<CAbstractHeaderConverter> pHandler(ConverterFactory::CreateConverter(SrcElement.Header.eumDataType));
		if (pHandler)
		{
			//转换失败
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
		//数据有效
		{
			//进行数据拷贝
			DstElement.pData = new char[SrcElement.Header.nTotalDataLen];
			if (nullptr != DstElement.pData)
			{
				//进行数据拷贝
				memcpy(DstElement.pData, SrcElement.pData, SrcElement.Header.nTotalDataLen);
				//调用移动构造函数
				DstArray.push_back(std::move(DstElement));
			}
		}
	}

	return nResult;
}

/********************************************************************************
funciton name:RecursiveAdapterNode
function description:递归适配节点 外->内
input param:
output param:
return value:
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static int RecursiveAdapterNode(const OuterType::HDF5_DATA* pSrcData, InnerType::LP_HDF5_WRITE_DATA pDstData)
{
	int nResult = ERROR_NO_ERROR;
	//参数合法性判断
	if (nullptr == pSrcData || nullptr == pDstData)
	{
		return ERROR_PARAM_INVALID;
	}

	//对名称进行赋值
	pDstData->strKeyName = pSrcData->strKeyName;
	//适配注解
	AdapterAttr(pSrcData->AttributeArray, pDstData->AttributeArray);
	//适配节点类型
	if (ERROR_NO_ERROR != AdapterNodeType(pSrcData->eumNodeType, pDstData->eumNodeType))
	{
		ERROR_LOG("call [%s] failed,AdapterNodeType failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_CONVERT_NODE_TYPE_FAILED;
	}
	//叶子节点
	if (OuterType::NODE_LEAF == pSrcData->eumNodeType)
	{

		//数据合法性判断
		if (pSrcData->pData == nullptr || pSrcData->Header.nTotalDataLen <= 0)
		{
			return ERROR_PARAM_INVALID;
		}

		//压缩比赋值
		pDstData->nCompressLevel = pSrcData->nCompressLevel;
		//数据头信息

		//进行数据头转换
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
		//如果存在数据，则申请一块数据
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

	//遍历子节点
	for (auto& pElement : pSrcData->SubDataArray)
	{
		if (nullptr == pElement)
		{
			continue;
		}
		//申请内存
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
function description:适配数据 外->内
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
	//遍历数据
	for (auto& Element : SrcArray)
	{
		InnerType::HDF5_WRITE_DATA DstElement;
		//开始转换子节点信息
		if (ERROR_NO_ERROR == RecursiveAdapterNode(&Element, &DstElement))
		{
			DstArray.push_back(std::move(DstElement));
		}
	}
	return nResult;
}

/*********************************************************
FunctionName:AdapterAttr
FunctionDesc:适配属性 内->外
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
static int AdapterAttr(const std::vector<InnerType::DATA_ATTRIBUTE>& SrcAttrArray,std::vector<OuterType::DATA_RESULT_ATTRIBUTE>& DstAttrArray)
{
	int nResult = ERROR_NO_ERROR;
	DstAttrArray.clear();
	//遍历内部的数据
	for (auto& SrcAttrElement : SrcAttrArray)
	{
		OuterType::DATA_RESULT_ATTRIBUTE DstAttrElment;
		DstAttrElment.strKeyAttrName = SrcAttrElement.strKeyAttrName;

		//进行数据头转换
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
		//拷贝数据
		DstAttrElment.ResultArray.resize(DstAttrElment.Header.nTotalDataLen);
		char* pBuffer = (char*)SrcAttrElement.pData;
		std::copy(pBuffer, pBuffer + DstAttrElment.Header.nTotalDataLen, DstAttrElment.ResultArray.begin());
		DstAttrArray.push_back(DstAttrElment);
	}
	return nResult;
}


/*********************************************************
FunctionName:AdapterData
FunctionDesc:适配数据 内->外
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
		//变量名称
		DstElement.strKeyName = SrcElement.strKeyName;
		//转换属性
		if ((nResult = AdapterAttr(SrcElement.AttributeArray, DstElement.AttributeArray)) != 0)
		{
			break;
		}
		//转换节点属性
		if (ERROR_NO_ERROR != (nResult = AdapterNodeType(SrcElement.eumNodeType, DstElement.eumNodeType)))
		{
			break;
		}
		
		//如果是叶子节点才进行转换
		if (InnerType::NODE_TYPE::NODE_LEAF == SrcElement.eumNodeType)
		{
			//进行数据头转换
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
Return:0:成功 非0:错误码
Author:xiaowei.han
*********************************************************/
int OuterType::Data2Hdf5(const char* szFilePath, std::vector<HDF5_DATA>& DataArray)
{
	using namespace std;
	//函数签名认证
	CAuthentication::CreateInstance().AuthCall();
	//参数合法性判断
	if (nullptr == szFilePath || DataArray.empty())
	{
		ERROR_LOG("the param is invalid.");
		return ERROR_PARAM_INVALID;
	}
	vector<InnerType::HDF5_WRITE_DATA> DstArray;
	//进行必要的数据转换
	AdapterData(DataArray, DstArray);
	//使用CPP进行h5的生成
	CH5Contex Contex;
	auto pHandler = std::make_shared<CCppH5Strategy>(szFilePath);
	if (!pHandler)
	{
		ERROR_LOG("call [%s] failed.", BOOST_CURRENT_FUNCTION);
		return ERROR_ALLOCATE_FAILED;
	}
	//设置执行策略
	Contex.SetH5Strategy(pHandler.get());
	int nResult = ERROR_NO_ERROR;
	{
		//加锁
		boost::lock_guard<boost::mutex> Locker(s_Lock);
		nResult = Contex.WriteH5(DstArray);
	}
	return nResult;
}

/*********************************************************
FunctionName:Hdf52Data
FunctionDesc:读取HDF5数据到内存
InputParam:
OutputParam:
Return:0:成功 非0:错误码
Author:xiaowei.han
*********************************************************/
int OuterType::Hdf52Data(const char* szFilePath, const std::vector<HDF5_QUERY_PARAM>& QueryParamVec, std::vector<HDF5_RESULT>& ResultVec)
{
	using namespace std;
	CAuthentication::CreateInstance().AuthCall();
	//参数合法性判断
	if (nullptr == szFilePath || QueryParamVec.empty())
	{
		ERROR_LOG("call [%s] failed,the param is invalid.", BOOST_CURRENT_FUNCTION);
		return ERROR_PARAM_INVALID;
	}

	//判断文件是否存在
	if (!Utility::IsExist(szFilePath))
	{
		ERROR_LOG("call [%s] failed,the file is not exist.", BOOST_CURRENT_FUNCTION);
		return ERROR_FILE_NOT_EXIST;
	}

	ResultVec.clear();

	//进行数据转换
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
		//加锁
		boost::lock_guard<boost::mutex> Locker(s_Lock);
		nResult = Contex.ReadH5(QueryParams,Results);
	}

	//进行数据转换
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
	//参数合法性判断
	if (nullptr == szFilePath || QueryParamVec.empty())
	{
		ERROR_LOG("call [%s] failed,the param is invalid.", BOOST_CURRENT_FUNCTION);
		return ERROR_PARAM_INVALID;
	}

	//判断文件是否存在
	if (!Utility::IsExist(szFilePath))
	{
		ERROR_LOG("call [%s] failed,the file is not exist.", BOOST_CURRENT_FUNCTION);
		return ERROR_FILE_NOT_EXIST;
	}

	ResultVec.clear();

	//进行数据转换
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
		//加锁
		boost::lock_guard<boost::mutex> Locker(s_Lock);
		nResult = Contex.ReadH5_Quick(QueryParams, Results);
	}

	//进行数据转换
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
	//参数合法性判断
	if (nullptr == szFilePath || QueryParamVec.empty())
	{
		ERROR_LOG("call [%s] failed,the param is invalid.", BOOST_CURRENT_FUNCTION);
		return ERROR_PARAM_INVALID;
	}
	//判断文件是否存在
	if (!Utility::IsExist(szFilePath))
	{
		ERROR_LOG("call [%s] failed,the file is not exist.", BOOST_CURRENT_FUNCTION);
		return ERROR_FILE_NOT_EXIST;
	}

	ResultVec.clear();

	//进行数据转换
	vector<InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM> QueryParams;
	for (auto& Element : QueryParamVec)
	{
		//进行数据过滤
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
		//加锁
		boost::lock_guard<boost::mutex> Locker(s_Lock);
		nResult = Contex.ReadH5_PartialData(QueryParams, Results);
	}

	//进行数据转换
	nResult = AdapterData(Results, ResultVec);

	return nResult;
}


#if 0
//适配数据类型
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


//适配数据头
static int AdapterDataHeader(const InnerType::DATA_HEADER& SrcDataHeader, Hdf5_Wrapper::DATA_HEADER& DstDataHeader)
{
	if (nullptr == SrcDataHeader.pDimensionArray || SrcDataHeader.nRank <= 0)
	{
		return ERROR_DATA_HEADER_INVALID;

	}
	int nResult = ERROR_NO_ERROR;
	DstDataHeader.nRank = SrcDataHeader.nRank;
	DstDataHeader.nDataByte = SrcDataHeader.nDataByte;
	//转换类型失败
	if (!AdapterDataType(SrcDataHeader.eumDataType, DstDataHeader.eumDataType))
	{
		return ERROR_DATA_TYPE_ADAPT_FAILED;
	}

	unsigned int nTotalLen = 1;
	//转换维度信息
	for (int i = 0; i < SrcDataHeader.nRank; ++i)
	{
		DstDataHeader.DimensionArray.push_back(SrcDataHeader.pDimensionArray[i]);
		nTotalLen *= SrcDataHeader.pDimensionArray[i];
	}

	//描述数据的内存块整体长度
	DstDataHeader.nTotalDataLen = nTotalLen * SrcDataHeader.nDataByte;


	//如果数据类型是整数类型
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

			//转换数据头
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
function description:把外界传入的数据类型转换为内部需要的数据类型
input param:
output param:
return value:true:成功 false:失败
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static bool AdapterDataType(Hdf5_Wrapper::DATA_TYPE eumSrcDataType, InnerType::DATA_TYPE& eumDstDataType)
{
	bool bResult = true;

	switch (eumSrcDataType)
	{
		//整数类型
	case Hdf5_Wrapper::INT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::INT_TYPE;
		break;
		//浮点类型
	case Hdf5_Wrapper::FLOAT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::FLOAT_TYPE;
		break;
		//字符串类型
	case Hdf5_Wrapper::STR_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::STR_TYPE;
		break;
		//复合类型
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
function description:适配数据头
input param:
output param:
return value:true:成功 false:失败
author:xiaowei_han
date:2018/02/03
********************************************************************************/
static bool AdapterDataHeader(const Hdf5_Wrapper::DATA_HEADER& SrcDataHeader, InnerType::DATA_HEADER& DstDataHeader)
{

	//转换类型失败
	if (!AdapterDataType(SrcDataHeader.eumDataType, DstDataHeader.eumDataType))
	{
		ERROR_LOG("call [%s] failed,AdapterDataType failed.", BOOST_CURRENT_FUNCTION);
		return false;
	}

	//合法性检查
	if (SrcDataHeader.nDataByte <= 0)
	{
		ERROR_LOG("call [%s] failed,the DataByte is [%d]", BOOST_CURRENT_FUNCTION, SrcDataHeader.nDataByte);
		return false;
	}

	//数据类型占用的字节数
	DstDataHeader.nDataByte = SrcDataHeader.nDataByte;

	if (SrcDataHeader.nRank <= 0)
	{
		ERROR_LOG("call [%s] failed,the rank is [%d]", BOOST_CURRENT_FUNCTION, SrcDataHeader.nRank);
		return false;
	}

	//数据维度信息
	DstDataHeader.nRank = SrcDataHeader.nRank;

	int nDimenLen = (int)SrcDataHeader.DimensionArray.size();

	if (nDimenLen > 0)
	{
		//申请内存
		DstDataHeader.pDimensionArray = new int[nDimenLen];

		if (nullptr != DstDataHeader.pDimensionArray)
		{
			for (int i = 0; i < nDimenLen; ++i)
			{
				DstDataHeader.pDimensionArray[i] = SrcDataHeader.DimensionArray[i];
			}
		}
	}

	//如果是整数类型
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
	//如果是复合类型
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
				//设置类型失败
				if (!AdapterDataType(Element.eumDataType, TempElement.eumDataType))
				{
					bBreak = true;
					break;
				}

				//如果是整数类型
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
			//设置类似枚举属性
			DstDataHeader.AdditionalDataType = TempHeader;
			return true;
		}
		return false;
	}
	return true;
}
#endif