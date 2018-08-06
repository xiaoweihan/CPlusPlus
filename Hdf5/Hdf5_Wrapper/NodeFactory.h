#pragma once
#include "Type.h"
class CH5AbstractNode;

namespace NodeFactory
{

	CH5AbstractNode* CreateNode(const InnerType::LP_HDF5_WRITE_DATA pData);



}
