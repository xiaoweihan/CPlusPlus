/*********************************************************
Copyright(C):
FileName:H5DataSetNode.cpp
Descripton:组合模式叶子节点的类
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "H5DataSetNode.h"
#include "ErrorCode.h"
#include "AbstractAttr.h"
#include "Utility.h"

/*********************************************************
FunctionName:GetChildNum
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
unsigned int CIntDataSetNode::GetChildNum(void)
{
	return 0;
}

/*********************************************************
FunctionName:CreateSelfToFile
FunctionDesc:
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
int CIntDataSetNode::CreateSelfToFile(H5::Group* pGroupNode)
{
	if (nullptr == pGroupNode)
	{
		return ERROR_PARAM_INVALID;
	}
	using namespace std;
	using namespace H5;

	return ERROR_NO_ERROR;
}
