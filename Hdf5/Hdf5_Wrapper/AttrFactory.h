#pragma once
#include <H5Cpp.h>
#include "Type.h"
class CAbstractAttr;
namespace AttrFactory
{

	CAbstractAttr* CreateAttr(const InnerType::DATA_ATTRIBUTE& Attr);

	CAbstractAttr* CreateAttr(H5T_class_t eumClassType);

}