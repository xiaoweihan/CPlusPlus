#pragma once
#include "Type.h"
class CAbstractDataType;
namespace DataTypeFactory
{
	//创建合适的类型
	CAbstractDataType* CreateDataType(InnerType::DATA_TYPE eumDataType,unsigned int nDataByte,bool bSign);

	CAbstractDataType* CreateDataType(const InnerType::DATA_HEADER& DataHeader);
}
