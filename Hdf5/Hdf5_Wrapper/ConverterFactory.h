#pragma once
#include "Type.h"
#include "Hdf5_Wrapper.h"

class CAbstractHeaderConverter;
namespace ConverterFactory
{

	CAbstractHeaderConverter* CreateConverter(OuterType::DATA_TYPE eumDataType);

	CAbstractHeaderConverter* CreateConverter(InnerType::DATA_TYPE eumDataType);

}