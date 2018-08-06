/*********************************************************
Copyright(C):
FileName:Type.h
Descripton:内部类型定义
Author:xiaowei.han
Data:2018/01/26
Others:
History:
Version:1.0
*********************************************************/
#ifndef TYPE_H
#define TYPE_H
#include <vector>
#include <boost/variant.hpp>
#include "Utility.h"
namespace InnerType
{

	//要存储的数据类型
	enum class DATA_TYPE
	{
		NO_TYPE = 0,
		//浮点类型
		FLOAT_TYPE = 1,
		//整形
		INT_TYPE,
		//字符串类型
		STR_TYPE,
		//复合类型
		COMPOUND_TYPE
	};

	enum class NODE_TYPE
	{
		NODE_NO_TYPE = 0,
		NODE_BRANCH,
		NODE_LEAF
	};


	//整型的数据头定义
	struct _int_type_header
	{
		//是否带符号
		bool bSign;

		_int_type_header()
		{
			bSign = true;
		}
	};

	using INT_DATA_HEADER = _int_type_header;
	using LP_INT_DATA_HEADER = _int_type_header*;

	//浮点型的数据头定义
	struct _float_type_header
	{
		_float_type_header()
		{

		}
	};

	using FLOAT_DATA_HEADER = _float_type_header;
	using LP_FLOAT_DATA_HEADER = _float_type_header*;


	//字符串的数据头定义
	struct _str_type_header
	{
		_str_type_header()
		{
		}
	};
	using STR_DATA_HEADER = _str_type_header;
	using LP_STR_DATA_HEADER = _str_type_header*;

	struct _compound_type_elment
	{
		std::string strElementName;
		unsigned int nElementoffset;
		//数据类型
		DATA_TYPE eumDataType;
		//数据类型占用的字节数
		unsigned int nDataByte;
		//类型补充
		boost::variant<InnerType::INT_DATA_HEADER, InnerType::FLOAT_DATA_HEADER, InnerType::STR_DATA_HEADER> AdditionalDataType;
		_compound_type_elment()
		{
			nDataByte = 0;
			eumDataType = DATA_TYPE::NO_TYPE;
			nElementoffset = 0;
		}
	};

	using COMPOUND_TYPE_ELEMENT = _compound_type_elment;
	using LP_COMPOUND_TYPE_ELEMENT = _compound_type_elment*;

	//符合型的数据头定义
	struct _compound_type_header
	{
		//信息
		std::vector<COMPOUND_TYPE_ELEMENT> ElementArray;
		_compound_type_header()
		{
			ElementArray.clear();
		}
	};
	using COMPOUND_TYPE_HEADER = _compound_type_header;
	using LP_COMPOUND_TYPE_HEADER = _compound_type_header*;


	//数据描述头
	struct _data_header
	{
		//维数
		int nRank;
		//维度信息
		int* pDimensionArray;
		//数据类型
		DATA_TYPE eumDataType;
		//数据类型占用的字节数
		unsigned int nDataByte;
		//类型补充
		boost::variant<InnerType::INT_DATA_HEADER, InnerType::COMPOUND_TYPE_HEADER> AdditionalDataType;

		_data_header()
		{
			nDataByte = 0;
			nRank = 0;
			pDimensionArray = nullptr;
			eumDataType = DATA_TYPE::NO_TYPE;
		}

		//移动构造函数
		_data_header(_data_header&& CopyValue)
		{
			nRank = CopyValue.nRank;
			pDimensionArray = CopyValue.pDimensionArray;
			eumDataType = CopyValue.eumDataType;
			nDataByte = CopyValue.nDataByte;
			CopyValue.pDimensionArray = nullptr;
			AdditionalDataType = CopyValue.AdditionalDataType;
		}
		_data_header& operator=(_data_header&& CopyValue)
		{
			if (this == &CopyValue)
			{
				return *this;
			}
			nRank = CopyValue.nRank;
			pDimensionArray = CopyValue.pDimensionArray;
			eumDataType = CopyValue.eumDataType;
			nDataByte = CopyValue.nDataByte;
			AdditionalDataType = CopyValue.AdditionalDataType;
			CopyValue.pDimensionArray = nullptr;
			return *this;
		}

		~_data_header()
		{
			DELETE_ARRAY_POINTER(pDimensionArray);
		}

	};

	using DATA_HEADER = _data_header;
	using LP_DATA_HEADER = _data_header*;

	//数据属性
	struct _data_attribute
	{
		std::string strKeyAttrName;
		//数据头
		DATA_HEADER Header;
		//数据
		void* pData;

		_data_attribute()
		{
			pData = nullptr;
		}

		_data_attribute(_data_attribute&& CopyValue)
		{
			strKeyAttrName = CopyValue.strKeyAttrName;
			Header = std::move(CopyValue.Header);
			pData = CopyValue.pData;
			CopyValue.pData = nullptr;
		}

		_data_attribute& operator=(_data_attribute&& CopyValue)
		{
			if (this == &CopyValue)
			{
				return *this;
			}
			strKeyAttrName = CopyValue.strKeyAttrName;
			Header = std::move(CopyValue.Header);
			pData = CopyValue.pData;
			CopyValue.pData = nullptr;
			return *this;
		}
		~_data_attribute()
		{
			DELETE_ARRAY_POINTER(pData);
		}

	};
	using DATA_ATTRIBUTE = _data_attribute;
	using LP_DATA_ATTRIBUTE = _data_attribute;


	//数据定义
	struct _hdf5_write_data
	{
		NODE_TYPE eumNodeType;
		//hash表中的key
		std::string strKeyName;
		//数据属性
		std::vector<DATA_ATTRIBUTE> AttributeArray;
		//数据头
		DATA_HEADER Header;
		//数据信息
		void* pData;
		//压缩比
		int nCompressLevel;
		//子信息
		std::vector<_hdf5_write_data*> SubDataArray;

		_hdf5_write_data()
		{
			pData = nullptr;
			SubDataArray.clear();
			nCompressLevel = -1;
			AttributeArray.clear();
			eumNodeType = NODE_TYPE::NODE_NO_TYPE;
		}

		_hdf5_write_data(_hdf5_write_data&& CopyValue)
		{
			strKeyName = CopyValue.strKeyName;
			AttributeArray = std::move(CopyValue.AttributeArray);
			Header = std::move(CopyValue.Header);
			pData = CopyValue.pData;
			CopyValue.pData = nullptr;
			nCompressLevel = CopyValue.nCompressLevel;
			SubDataArray.swap(CopyValue.SubDataArray);
			eumNodeType = CopyValue.eumNodeType;
		}

		_hdf5_write_data& operator=(_hdf5_write_data&& CopyValue)
		{
			if (this == &CopyValue)
			{
				return *this;
			}

			strKeyName = CopyValue.strKeyName;
			AttributeArray = std::move(CopyValue.AttributeArray);
			Header = std::move(CopyValue.Header);
			pData = CopyValue.pData;
			CopyValue.pData = nullptr;
			nCompressLevel = CopyValue.nCompressLevel;
			SubDataArray.swap(CopyValue.SubDataArray);
			eumNodeType = CopyValue.eumNodeType;
			return *this;
		}

		~_hdf5_write_data()
		{
			DELETE_ARRAY_POINTER(pData);

			for (auto& SubElement : SubDataArray)
			{
				DELETE_POINTER(SubElement);
			}
		}

	};

	using HDF5_WRITE_DATA = _hdf5_write_data;
	using LP_HDF5_WRITE_DATA = _hdf5_write_data*;

	struct _hdf5_read_data
	{
		//节点类型
		NODE_TYPE eumNodeType;
		//变量名称
		std::string strKeyName;
		//数据属性
		std::vector<DATA_ATTRIBUTE> AttributeArray;
		//数据头
		DATA_HEADER Header;
		//数据信息
		void* pData;

		_hdf5_read_data()
		{
			AttributeArray.clear();
			pData = nullptr;
			eumNodeType = NODE_TYPE::NODE_NO_TYPE;
		}

		_hdf5_read_data(_hdf5_read_data&& CopyValue)
		{
			strKeyName = CopyValue.strKeyName;
			AttributeArray = std::move(CopyValue.AttributeArray);
			Header = std::move(CopyValue.Header);
			pData = CopyValue.pData;
			CopyValue.pData = nullptr;
			eumNodeType = CopyValue.eumNodeType;
		}
		_hdf5_read_data& operator= (_hdf5_read_data&& CopyValue)
		{
			if (this == &CopyValue)
			{
				return *this;
			}
			strKeyName = CopyValue.strKeyName;
			AttributeArray = std::move(CopyValue.AttributeArray);
			Header = std::move(CopyValue.Header);
			pData = CopyValue.pData;
			CopyValue.pData = nullptr;
			eumNodeType = CopyValue.eumNodeType;
			return *this;
		}
		~_hdf5_read_data()
		{
			DELETE_ARRAY_POINTER(pData);
		}
	};
	using HDF5_READ_DATA = _hdf5_read_data;
	using LP_HDF5_READ_DATA = _hdf5_read_data*;

	struct _hdf5_query_param
	{
		std::string strQueryKeyName;
		bool bQueryAttr;

		_hdf5_query_param()
		{
			bQueryAttr = false;
		}
		_hdf5_query_param(const std::string& strKeyName, bool bQueryAttr)
		{
			this->strQueryKeyName = strKeyName;
			this->bQueryAttr = bQueryAttr;
		}
	};

	using HDF5_QUERY_PARAM = _hdf5_query_param;
	using LP_HDF5_QUERY_PARAM = _hdf5_query_param*;



	//用于查询矩阵信息
	struct _hdf5_query_partial_data_param
	{
		std::string strQueryKeyName;
		
		//查询开始的索引
		int nIndex;

		//表示索引是基于行索引还是基于列索引 true:基于列索引 false:基于行索引
		bool bColumn;

		_hdf5_query_partial_data_param()
		{
			nIndex = -1;
			bColumn = false;
		}

		_hdf5_query_partial_data_param(const std::string& strKeyName, int nIndex, bool bColumn):
			strQueryKeyName(strKeyName),
			nIndex(nIndex),
			bColumn(bColumn)
		{

		}

	};

	using HDF5_QUERY_PARTIAL_DATA_PARAM = _hdf5_query_partial_data_param;
	using LP_HDF5_QUERY_PARTIAL_DATA_PARAM = _hdf5_query_partial_data_param*;
}

#endif

