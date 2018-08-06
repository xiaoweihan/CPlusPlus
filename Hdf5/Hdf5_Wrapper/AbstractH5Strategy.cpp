/*********************************************************
Copyright(C):
FileName:AbstractH5Strategy.cpp
Descripton:�������ģʽ
Author:xiaowei.han
Data:2018/01/29
Others:
History:
Version:1.0
*********************************************************/
#include "stdafx.h"
#include "AbstractH5Strategy.h"
#include "NodeFactory.h"
#include "H5GroupNode.h"
#include "AttrFactory.h"
CAbstractH5Strategy::CAbstractH5Strategy(const std::string& strFilePath):
	m_strFilePath(strFilePath)
{

}

CAbstractH5Strategy::~CAbstractH5Strategy()
{
}

CH5AbstractNode* CAbstractH5Strategy::RecursiveCreateNode(const InnerType::LP_HDF5_WRITE_DATA pData)
{
	if (nullptr == pData)
	{
		return nullptr;
	}

	CH5AbstractNode* pResult = nullptr;
	//�������һ��Group
	if (InnerType::NODE_TYPE::NODE_LEAF == pData->eumNodeType)
	{
		//�����dataset��������Ч����
		if (nullptr != pData->pData)
		{
			auto pDataSet = NodeFactory::CreateNode(pData);

			//��֤������Ч
			if (nullptr != pDataSet)
			{
				//�������
				for (auto& Attr : pData->AttributeArray)
				{
					if (nullptr != Attr.pData)
					{
						auto pAttr = AttrFactory::CreateAttr(Attr);

						if (nullptr != pAttr)
						{
							pDataSet->AppendAttr(pAttr);
						}
					}
				}
			}
			pResult = pDataSet;
		}
	}
	//�����һ��Group
	else if (InnerType::NODE_TYPE::NODE_BRANCH == pData->eumNodeType)
	{
		auto pGroup = new CH5GroupNode(pData->strKeyName);

		if (nullptr != pGroup)
		{
			//�������
			for (auto& Attr : pData->AttributeArray)
			{
				//��֤������Ч
				if (nullptr != Attr.pData)
				{
					auto pAttr = AttrFactory::CreateAttr(Attr);

					if (nullptr != pAttr)
					{
						pGroup->AppendAttr(pAttr);
					}
				}
			}

			for (auto& pSubNode : pData->SubDataArray)
			{
				auto pTempNode = RecursiveCreateNode(pSubNode);

				if (nullptr != pTempNode)
				{
					pGroup->AppendChild(pTempNode);
				}
			}
		}
		pResult = pGroup;
	}
	else
	{

	}
	return pResult;
}
