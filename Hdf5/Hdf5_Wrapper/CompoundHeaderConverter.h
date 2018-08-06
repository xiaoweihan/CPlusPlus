#pragma once
#include "AbstractHeaderConverter.h"
class CCompoundHeaderConverter :
	public CAbstractHeaderConverter
{
public:
	CCompoundHeaderConverter();
	~CCompoundHeaderConverter();

public:
	int ConvertDataHeader(const OuterType::DATA_HEADER& SrcDataHeader, InnerType::DATA_HEADER& DstDataHeader) override;
	int ConvertDataHeader(const InnerType::DATA_HEADER& SrcDataHeader, OuterType::DATA_HEADER& DstDataHeader) override;
};

