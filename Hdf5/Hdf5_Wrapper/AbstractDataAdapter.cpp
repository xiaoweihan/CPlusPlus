/*******************************************************************************
FileName:AbstractDataAdapter.cpp
FileDescription:数据转换抽象类
CopyRight:
Author:xiaowei_han
Date:2018/02/03
Histrory:V0.1
*******************************************************************************/
#include "stdafx.h"
#include "AbstractDataAdapter.h"


CAbstractDataAdapter::CAbstractDataAdapter()
{
}


CAbstractDataAdapter::~CAbstractDataAdapter()
{
}

/********************************************************************************
funciton name:AdapterData
function description:
input param:
output param:
return value:
author:xiaowei_han
date:2018/02/03
********************************************************************************/
bool CAbstractDataAdapter::AdapterData(const Hdf5_Wrapper::LP_HDF5_DATA pSrcData, InnerType::LP_HDF5_WRITE_DATA pDstData)
{
	//参数合法性判断
	if (nullptr == pSrcData || nullptr == pDstData)
	{
		return false;
	}

	//数据名称进行转换


}

/********************************************************************************
funciton name:AdapterDataType
function description:
input param:
output param:
return value:
author:xiaowei_han
date:2018/02/03
********************************************************************************/
bool CAbstractDataAdapter::AdapterDataType(Hdf5_Wrapper::DATA_TYPE eumSrcDataType, InnerType::DATA_TYPE& eumDstDataType)
{
	bool bResult = true;

	switch (eumSrcDataType)
	{
	case Hdf5_Wrapper::INT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::INT_TYPE;
		break;
	case Hdf5_Wrapper::FLOAT_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::FLOAT_TYPE;
		break;
	case Hdf5_Wrapper::STR_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::STR_TYPE;
		break;
	case Hdf5_Wrapper::COMPOUND_TYPE:
		eumDstDataType = InnerType::DATA_TYPE::COMPOUND_TYPE;
		break;
		//超出范围
	default:
		bResult = false;
		break;
	}

	return bResult;
}

void CAbstractDataAdapter::AdapterAttr(const std::vector<Hdf5_Wrapper::DATA_ATTRIBUTE>& SrcArray, std::vector<InnerType::DATA_ATTRIBUTE>& DstArray)
{
	DstArray.clear();
	if (SrcArray.empty())
	{
		return;
	}

	for (auto& Element : SrcArray)
	{
		InnerType::DATA_ATTRIBUTE TempAttr;
		TempAttr.strKeyAttrName = Element.strKeyAttrName;
		InnerType::DATA_HEADER TempAttrHeader;
		//转换头失败
		if (!AdapterDataHeader(Element.Header, TempAttrHeader))
		{
			ERROR_LOG("call [%s] failed,AdapterDataHeader failed.", BOOST_CURRENT_FUNCTION);
			continue;
		}
		//进行数据头赋值
		TempAttr.Header = std::move(TempAttrHeader);
		if (Element.Header.nTotalDataLen > 0)
		{
			//进行数据拷贝
			TempAttr.pData = new char[Element.Header.nTotalDataLen];
			if (nullptr != TempAttr.pData)
			{
				//进行数据拷贝
				memcpy(TempAttr.pData, Element.pData, Element.Header.nTotalDataLen);
				//调用移动构造函数
				DstArray.push_back(std::move(TempAttr));
			}
		}
	}
}

/********************************************************************************
funciton name:AdapterNodeType
function description:
input param:
output param:
return value:
author:xiaowei_han
date:2018/02/03
********************************************************************************/
bool CAbstractDataAdapter::AdapterNodeType(Hdf5_Wrapper::NODE_TYPE eumSrcNodeType, InnerType::NODE_TYPE& eumDstNodeType)
{
	bool bResult = true;

	switch (eumSrcNodeType)
	{
	case Hdf5_Wrapper::NODE_BRANCH:
		eumDstNodeType = InnerType::NODE_TYPE::NODE_BRANCH;
		break;
	case Hdf5_Wrapper::NODE_LEAF:
		eumDstNodeType = InnerType::NODE_TYPE::NODE_LEAF;
		break;
	default:
		bResult = false;
		break;
	}

	return bResult;
}
