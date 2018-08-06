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

	//�жϲ�ѯ�ı������Ƿ����
	auto strGroupName = pGroup->getObjName();
	auto Iter = m_QueryParams.find(strGroupName);
	if (Iter != m_QueryParams.end())
	{
		InnerType::HDF5_READ_DATA GroupData;
		GroupData.eumNodeType = InnerType::NODE_TYPE::NODE_BRANCH;
		GroupData.strKeyName = strGroupName;
		//��ѯ���Գɹ�
		VisitAttr(pGroup, GroupData, Iter->second);
		//�ŵ��������
		Results.push_back(std::move(GroupData));
		//�ҵ�֮��ɾ���ýڵ���Ϣ����ֹ�ظ�����
		m_QueryParams.erase(Iter);
	}

	//�Ż��ݹ��ٶȣ�������ҵı����Ѿ�Ϊ���ˣ���û�б�Ҫ���ʺ����Ľڵ�
	if (m_QueryParams.empty())
	{
		return true;
	}

	//�ݹ�����ӽڵ�
	int nObjNum = (int)pGroup->getNumObjs();
	//����������Ϣ
	for (int j = 0; j < nObjNum; ++j)
	{
		//��ȡ�ڵ������
		auto ChildObjType = pGroup->getObjTypeByIdx(j);
		if (H5G_GROUP == ChildObjType)
		{
			//���������
			auto pSubGroup = new Group(pGroup->openGroup(pGroup->getObjnameByIdx(j)));
			if (nullptr != pSubGroup)
			{
				Visit(pSubGroup, Results);
				DELETE_POINTER(pSubGroup);
			}
		}
		//�����DataSet
		else if (H5G_DATASET == ChildObjType)
		{
			//��ȡdataset��������
			auto pSubDataset = new DataSet(pGroup->openDataSet(pGroup->getObjnameByIdx(j)));

			if (nullptr != pSubDataset)
			{
				//��ȡ��������
				auto SubDatasetDataType = pSubDataset->getTypeClass();
				CAbstractNodeVistor* pVisitor = nullptr;
				switch (SubDatasetDataType)
				{
					//��������
				case H5T_INTEGER:
					pVisitor = new CIntNodeVisitor(m_QueryParams);
					break;
					//����������
				case H5T_FLOAT:
					pVisitor = new CFloatNodeVisitor(m_QueryParams);
					break;
					//�ַ�������
				case H5T_STRING:
					pVisitor = new CStrNodeVisitor(m_QueryParams);
					break;
					//��������
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

