/*********************************************************
Copyright(C):
FileName:H5GroupNode.cpp
Descripton:��ڵ���
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "H5GroupNode.h"
#include <algorithm>
#include <boost/scope_exit.hpp>
#include "ErrorCode.h"
#include "Utility.h"
#include "AbstractAttr.h"

/*********************************************************
FunctionName:AppendChild
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
void CH5GroupNode::AppendChild(CH5AbstractNode* pChildNode)
{
	if (nullptr == pChildNode)
	{
		return;
	}

	auto Iter = std::find(m_ChildList.begin(), m_ChildList.end(), pChildNode);

	if (Iter == m_ChildList.end())
	{
		m_ChildList.push_back(pChildNode);
		pChildNode->SetParentNode(this);
	}

}

/*********************************************************
FunctionName:RemoveChild
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
void CH5GroupNode::RemoveChild(CH5AbstractNode* pChildNode)
{
	if (nullptr == pChildNode)
	{
		return;
	}

	auto Iter = std::find(m_ChildList.begin(), m_ChildList.end(), pChildNode);

	if (Iter != m_ChildList.end())
	{
		m_ChildList.erase(Iter);
	}

}

/*********************************************************
FunctionName:GetChildNum
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
unsigned int CH5GroupNode::GetChildNum(void)
{

	unsigned int nResult = 0;
	//��ȡ���������ӽڵ�ĸ���
	for (auto& ChildNode : m_ChildList)
	{
		if (nullptr != ChildNode)
		{
			nResult += ChildNode->GetChildNum();
		}	
	}
	return nResult;
}

/*********************************************************
FunctionName:CreateSelfToFile
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CH5GroupNode::CreateSelfToFile(H5::Group* pGroupNode)
{
	using namespace std;
	using namespace H5;
	if (nullptr == pGroupNode)
	{
		return ERROR_PARAM_INVALID;
	}

	//�����������ڵ�
	Group* pGroup = new Group(pGroupNode->createGroup(m_strName));

	//�����ڴ�ʧ��
	if (nullptr == pGroup)
	{
		return ERROR_ALLOCATE_FAILED;
	}

	BOOST_SCOPE_EXIT(&pGroup)
	{
		//�ͷ��ڴ�
		DELETE_POINTER(pGroup);
	}BOOST_SCOPE_EXIT_END

	//д������
	for (auto& AttrElement : m_AttrList)
	{
		if (AttrElement != nullptr)
		{
			AttrElement->WriteSelf(pGroup);
		}
	}

	//д���ӽڵ�
	for (auto& SubNodeElement : m_ChildList)
	{
		if (SubNodeElement != nullptr)
		{
			SubNodeElement->CreateSelfToFile(pGroup);
		}
	}
	return ERROR_NO_ERROR;
}

int CH5GroupNode::ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData)
{
	if (nullptr == pDataset)
	{
		return ERROR_PARAM_INVALID;
	}
	return ERROR_NO_ERROR;
}
