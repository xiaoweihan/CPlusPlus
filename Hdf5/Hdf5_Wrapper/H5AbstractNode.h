/*********************************************************
Copyright(C):
FileName:H5AbstractNode.h
Descripton:���ģʽ����
Author:xiaowei.han
Data:2018/01/26
Others:
History:
Version:1.0
*********************************************************/
#ifndef H5_ABSTRACT_NODE_H
#define H5_ABSTRACT_NODE_H
#include <string>
#include <list>
#include <H5Cpp.h>
#include "Type.h"
class CAbstractAttr;
class CH5AbstractNode
{
public:
	CH5AbstractNode(const std::string& strName);
	CH5AbstractNode() = default;
	virtual ~CH5AbstractNode();
public:
	//��ȡ����
	std::string GetName(void);

	//��������
	void SetName(const std::string& strName);

	//��ȡ��������
	std::string GetAbsoluteName(void);

	//���ø��ڵ�
	void SetParentNode(CH5AbstractNode* pParentNode)
	{
		m_pParent = pParentNode;
	}

	CH5AbstractNode* GetParentNode(void)
	{
		return m_pParent;
	}

	//��������
	void AppendAttr(CAbstractAttr* pAttr);

	//ɾ������
	void RemoveAttr(CAbstractAttr* pAttr);

	//��ȡ���Ը���
	unsigned int GetAttrNum();

	//����ӽڵ�
	virtual void AppendChild(CH5AbstractNode* pChildNode){}

	//�Ƴ��ӽڵ�
	virtual void RemoveChild(CH5AbstractNode* pChildNode){}

	//��ȡ�ڵ����
	virtual unsigned int GetChildNum(void) = 0;

	//��������
	virtual int CreateSelfToFile(H5::Group* pGroupNode) = 0;

	//�������ȡ����
	virtual int ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData) = 0;

	//��ȡ��������
	virtual int ReadPartialData(H5::DataSet* pDataset, const InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM& QueryParam, InnerType::HDF5_READ_DATA& DstData);
protected:
	//�ڵ�����
	std::string m_strName;
	//�ӽڵ�
	std::list<CH5AbstractNode*> m_ChildList;
	//���ڵ���Ϣ
	CH5AbstractNode* m_pParent = nullptr;
	//������Ϣ
	std::list<CAbstractAttr*> m_AttrList;
};

#endif