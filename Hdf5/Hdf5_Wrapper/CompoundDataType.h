#pragma once
#include "AbstractDataType.h"
#include "Type.h"
class CCompoundDataType : public CAbstractDataType
{
public:
	CCompoundDataType(unsigned int nDataByte,const InnerType::COMPOUND_TYPE_HEADER& TypeInfo);
	~CCompoundDataType();
public:
	std::string GetDataTypeName() override;

	H5::DataType* CreateProperType() override;
private:
	InnerType::COMPOUND_TYPE_HEADER m_TypeInfo;
	//���Ͻṹ��ռ�õ��ֽڳ���
	unsigned int m_nDataByte = 0;
};

