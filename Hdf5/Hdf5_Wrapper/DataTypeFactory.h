#pragma once
#include "Type.h"
class CAbstractDataType;
namespace DataTypeFactory
{
	//�������ʵ�����
	CAbstractDataType* CreateDataType(InnerType::DATA_TYPE eumDataType,unsigned int nDataByte,bool bSign);

	CAbstractDataType* CreateDataType(const InnerType::DATA_HEADER& DataHeader);
}
