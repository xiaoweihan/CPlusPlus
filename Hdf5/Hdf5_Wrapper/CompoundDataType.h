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
	//符合结构体占用的字节长度
	unsigned int m_nDataByte = 0;
};

