#pragma once
#include "AbstractNodeVistor.h"
class CGroupVisitor :
	public CAbstractNodeVistor
{
	using CAbstractNodeVistor::CAbstractNodeVistor;
public:
	bool Visit(H5::Group* pGroup, std::vector<InnerType::HDF5_READ_DATA>& Results) override;
	bool Visit(H5::DataSet* pDataSet, std::vector<InnerType::HDF5_READ_DATA>& Results) override;


};

