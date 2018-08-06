/*********************************************************
Copyright(C):
FileName:H5AbstractNode.h
Descripton:组合模式基类
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
	//获取名称
	std::string GetName(void);

	//设置名称
	void SetName(const std::string& strName);

	//获取绝对名字
	std::string GetAbsoluteName(void);

	//设置父节点
	void SetParentNode(CH5AbstractNode* pParentNode)
	{
		m_pParent = pParentNode;
	}

	CH5AbstractNode* GetParentNode(void)
	{
		return m_pParent;
	}

	//增加属性
	void AppendAttr(CAbstractAttr* pAttr);

	//删除属性
	void RemoveAttr(CAbstractAttr* pAttr);

	//获取属性个数
	unsigned int GetAttrNum();

	//添加子节点
	virtual void AppendChild(CH5AbstractNode* pChildNode){}

	//移除子节点
	virtual void RemoveChild(CH5AbstractNode* pChildNode){}

	//获取节点个数
	virtual unsigned int GetChildNum(void) = 0;

	//创建自身
	virtual int CreateSelfToFile(H5::Group* pGroupNode) = 0;

	//从自身读取数据
	virtual int ParseSelf(H5::DataSet* pDataset, InnerType::HDF5_READ_DATA& DstData) = 0;

	//读取部分数据
	virtual int ReadPartialData(H5::DataSet* pDataset, const InnerType::HDF5_QUERY_PARTIAL_DATA_PARAM& QueryParam, InnerType::HDF5_READ_DATA& DstData);
protected:
	//节点名称
	std::string m_strName;
	//子节点
	std::list<CH5AbstractNode*> m_ChildList;
	//父节点信息
	CH5AbstractNode* m_pParent = nullptr;
	//属性信息
	std::list<CAbstractAttr*> m_AttrList;
};

#endif