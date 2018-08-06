/*********************************************************
Copyright(C):
FileName:H5Root.cpp
Descripton:组合模式基类
Author:xiaowei.han
Data:2018/01/26
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "H5AbstractNode.h"
#include <algorithm>
#include "AbstractAttr.h"
#include "ErrorCode.h"
#include "Utility.h"

CH5AbstractNode::CH5AbstractNode(const std::string& strName):
	m_strName(strName)
{

}

CH5AbstractNode::~CH5AbstractNode()
{
	for (auto& pAttr : m_AttrList)
	{
		DELETE_POINTER(pAttr);
	}

	for (auto& pNode : m_ChildList)
	{
		DELETE_POINTER(pNode);
	}
}


/*********************************************************
FunctionName:GetName
FunctionDesc:获取节点名字
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
std::string CH5AbstractNode::GetName(void)
{
	return m_strName;
}

/*********************************************************
FunctionName:SetName
FunctionDesc:设置节点名字
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
void CH5AbstractNode::SetName(const std::string& strName)
{
	m_strName = strName;
}

/*********************************************************
FunctionName:GetAbsoluteName
FunctionDesc:获取绝对路径
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
std::string CH5AbstractNode::GetAbsoluteName(void)
{
	using namespace std;
	//如果没有父节点
	if (nullptr == m_pParent)
	{
		return "/" + m_strName;
	}

	return m_pParent->GetAbsoluteName() + "/" + m_strName;
}


void CH5AbstractNode::AppendAttr(CAbstractAttr* pAttr)
{
	if (nullptr == pAttr)
	{
		return;
	}

	auto Iter = std::find(m_AttrList.begin(), m_AttrList.end(), pAttr);

	if (Iter == m_AttrList.end())
	{
		m_AttrList.push_back(pAttr);
	}
}


void CH5AbstractNode::RemoveAttr(CAbstractAttr* pAttr)
{
	if (nullptr == pAttr)
	{
		return;
	}

	auto Iter = std::find(m_AttrList.begin(), m_AttrList.end(), pAttr);

	if (Iter != m_AttrList.end())
	{
		m_AttrList.erase(Iter);
	}
}

/*********************************************************
FunctionName:
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
unsigned int CH5AbstractNode::GetAttrNum()
{

	return static_cast<unsigned int>(m_AttrList.size());
}

int CH5AbstractNode::ReadPartialData(H5::DataSet* pDataset, const InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM& QueryParam, InnerType::HDF5_READ_DATA& DstData)
{
	return ERROR_NO_ERROR;
}

