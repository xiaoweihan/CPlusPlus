#include "stdafx.h"
#include "GroupVisitor.h"
#include <H5Cpp.h>
#include <memory>
#include "AttrFactory.h"
#include "Utility.h"
#include "AbstractAttr.h"
#include "IntNodeVisitor.h"
#include "FloatNodeVisitor.h"
#include "StrNodeVisitor.h"
#include "CompoundNodeVisitor.h"
bool CGroupVisitor::Visit(H5::Group* pGroup, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	using namespace H5;
	if (nullptr == pGroup || m_QueryParams.empty())
	{
		return false;
	}

	//判断查询的变量名是否存在
	auto strGroupName = pGroup->getObjName();
	auto Iter = m_QueryParams.find(strGroupName);
	if (Iter != m_QueryParams.end())
	{
		InnerType::HDF5_READ_DATA GroupData;
		GroupData.eumNodeType = InnerType::NODE_TYPE::NODE_BRANCH;
		GroupData.strKeyName = strGroupName;
		//查询属性成功
		VisitAttr(pGroup, GroupData, Iter->second);
		//放到结果集中
		Results.push_back(std::move(GroupData));
		//找到之后删除该节点信息，防止重复查找
		m_QueryParams.erase(Iter);
	}

	//优化递归速度，如果查找的变量已经为空了，则没有必要访问后续的节点
	if (m_QueryParams.empty())
	{
		return true;
	}

	//递归访问子节点
	int nObjNum = (int)pGroup->getNumObjs();
	//处理子组信息
	for (int j = 0; j < nObjNum; ++j)
	{
		//获取节点的类型
		auto ChildObjType = pGroup->getObjTypeByIdx(j);
		if (H5G_GROUP == ChildObjType)
		{
			//申请访问者
			auto pSubGroup = new Group(pGroup->openGroup(pGroup->getObjnameByIdx(j)));
			if (nullptr != pSubGroup)
			{
				Visit(pSubGroup, Results);
				DELETE_POINTER(pSubGroup);
			}
		}
		//如果是DataSet
		else if (H5G_DATASET == ChildObjType)
		{
			//获取dataset数据类型
			auto pSubDataset = new DataSet(pGroup->openDataSet(pGroup->getObjnameByIdx(j)));

			if (nullptr != pSubDataset)
			{
				//获取数据类型
				auto SubDatasetDataType = pSubDataset->getTypeClass();
				CAbstractNodeVistor* pVisitor = nullptr;
				switch (SubDatasetDataType)
				{
					//整数类型
				case H5T_INTEGER:
					pVisitor = new CIntNodeVisitor(m_QueryParams);
					break;
					//浮点数类型
				case H5T_FLOAT:
					pVisitor = new CFloatNodeVisitor(m_QueryParams);
					break;
					//字符串类型
				case H5T_STRING:
					pVisitor = new CStrNodeVisitor(m_QueryParams);
					break;
					//复合类型
				case H5T_COMPOUND:
					pVisitor = new CCompoundNodeVisitor(m_QueryParams);
					break;
				default:
					break;
				}
				if (nullptr != pVisitor)
				{
					pVisitor->Visit(pSubDataset, Results);

					DELETE_POINTER(pVisitor);
				}

				DELETE_POINTER(pSubDataset);
			}
		}
		else
		{

		}
	}
	return true;
}

bool CGroupVisitor::Visit(H5::DataSet* pDataSet, std::vector<InnerType::HDF5_READ_DATA>& Results)
{
	if (nullptr == pDataSet)
	{
		return false;
	}

	return true;
}

