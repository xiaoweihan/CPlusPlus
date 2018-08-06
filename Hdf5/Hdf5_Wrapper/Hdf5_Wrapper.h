/*********************************************************
Copyright(C):
FileName:Hdf5_Wrapper.h
Descripton:hdf5文件封装头
Author:xiaowei.han
Data:2018/01/30
Others:
History:
Version:1.0
*********************************************************/
#ifndef HDF5_WRAPPER_H
#define HDF5_WRAPPER_H
#include <vector>
#include <string>
#include <boost/variant.hpp>
namespace OuterType
{
	//要存储的数据类型
	enum DATA_TYPE
	{
		NO_TYPE = 0,
		//浮点数
		FLOAT_TYPE = 1,
		//整数
		INT_TYPE,
		//字符串
		STR_TYPE,
		//复合类型
		COMPOUND_TYPE,
		DATA_TYPE_TOTAL,
		DATA_TYPE_MIN = FLOAT_TYPE,
		DATA_TYPE_MAX = DATA_TYPE_TOTAL - 1,
	};
	enum NODE_TYPE
	{
		NODE_NO_TYPE = 0,
		//组节点
		NODE_BRANCH,
		//叶子节点
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
		_float_type_header() = default;
	};
	using FLOAT_DATA_HEADER = _float_type_header;
	using LP_FLOAT_DATA_HEADER = _float_type_header*;


	//字符串的数据头定义
	struct _str_type_header
	{
		_str_type_header() = default;
	};
	using STR_DATA_HEADER = _str_type_header;
	using LP_STR_DATA_HEADER = _str_type_header*;

	struct _compound_type_elment
	{
		//每个字段的名称
		std::string strElementName;
		//对应的偏移量
		unsigned int nElementoffset;
		//数据类型
		DATA_TYPE eumDataType;
		//数据类型占用的字节数
		unsigned int nDataByte;
		//类型补充
		boost::variant<OuterType::INT_DATA_HEADER, OuterType::FLOAT_DATA_HEADER, OuterType::STR_DATA_HEADER> AdditionalDataType;
		_compound_type_elment()
		{
			nDataByte = 0;
			eumDataType = DATA_TYPE::NO_TYPE;
			nElementoffset = 0;
		}
	};
	using COMPOUND_TYPE_ELEMENT = _compound_type_elment;
	using LP_COMPOUND_TYPE_ELEMENT = _compound_type_elment*;


	//复合型的数据头定义
	struct _compound_type_header
	{
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
		std::vector<int> DimensionArray;
		//数据类型占用的字节数
		unsigned int nDataByte;
		//描述数据的内存块整体长度
		unsigned int nTotalDataLen;
		//数据头
		DATA_TYPE eumDataType;
		//根据数据头决定固定数据类型
		boost::variant<INT_DATA_HEADER, COMPOUND_TYPE_HEADER> ExternDataType;
		_data_header()
		{
			eumDataType = NO_TYPE;
			nRank = 0;
			DimensionArray.clear();
			nDataByte = 0;
			nTotalDataLen = 0;
		}

		void Reset()
		{
			eumDataType = NO_TYPE;
			nRank = 0;
			DimensionArray.clear();
			nDataByte = 0;
			nTotalDataLen = 0;
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

		void Reset()
		{
			strKeyAttrName.clear();
			pData = nullptr;
			Header.Reset();
		}
	};
	using DATA_ATTRIBUTE = _data_attribute;
	using LP_DATA_ATTRIBUTE = _data_attribute*;


	//数据定义
	struct _hdf5_data
	{
		//节点类型
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
		std::vector<_hdf5_data*> SubDataArray;

		_hdf5_data()
		{
			eumNodeType = NODE_NO_TYPE;
			pData = nullptr;
			nCompressLevel = -1;
			SubDataArray.clear();				
		}

		void Reset()
		{
			pData = nullptr;
			SubDataArray.clear();
			AttributeArray.clear();
			strKeyName.clear();
			nCompressLevel = -1;
			eumNodeType = NODE_NO_TYPE;
			Header.Reset();
		}

	};
	using HDF5_DATA = _hdf5_data;
	using LP_HDF5_DATA = _hdf5_data*;



	//数据属性
	struct _data_result_attribute
	{
		std::string strKeyAttrName;
		//数据头
		DATA_HEADER Header;
		//数据信息
		std::vector<unsigned char> ResultArray;
		_data_result_attribute()
		{
			ResultArray.clear();
		}

	};
	using DATA_RESULT_ATTRIBUTE = _data_result_attribute;
	using LP_DATA_RESULT_ATTRIBUTE = _data_result_attribute*;

	//数据读取结果集
	struct _hdf5_result
	{
		//节点类型
		NODE_TYPE eumNodeType;
		//hash表中的key
		std::string strKeyName;
		//数据属性
		std::vector<DATA_RESULT_ATTRIBUTE> AttributeArray;
		//数据头
		DATA_HEADER Header;
		//数据信息
		std::vector<unsigned char> ResultArray;
		_hdf5_result()
		{
			eumNodeType = NODE_NO_TYPE;
			AttributeArray.clear();
			ResultArray.clear();
		}

	};
	using HDF5_RESULT = _hdf5_result;
	using LP_HDF5_RESULT = _hdf5_result*;


	struct _hdf5_query_param
	{
		std::string strQueryKeyName;
		bool bQueryAttr;

		_hdf5_query_param()
		{
			bQueryAttr = false;
		}
	};

	using HDF5_QUERY_PARAM = _hdf5_query_param;
	using LP_HDF5_QUERY_PARAM = _hdf5_query_param*;


	struct _hdf5_query_matrix_param
	{
		std::string strQueryKeyName;

		//查询开始的索引
		int nIndex;

		//表示索引是基于行索引还是基于列索引 false:基于行索引 true:基于列索引
		bool bColumn;

		_hdf5_query_matrix_param()
		{
			nIndex = -1;
			bColumn = true;
		}
	};

	using HDF5_QUERY_MATRIX_PARAM = _hdf5_query_matrix_param;
	using LP_HDF5_QUERY_MATRIX_PARAM = _hdf5_query_matrix_param*;

	/*********************************************************
	FunctionName:Data2Hdf5
	FunctionDesc:把内存数据转换为hdf5文件存储
	InputParam:szFilePath:要存储的hdf5文件绝对路径
			   DataArray:树形的内存数据		  
	OutputParam:None
	Return:0:succeed 非0:错误码
	Author:xiaowei.han
	*********************************************************/
	int Data2Hdf5(const char* szFilePath,std::vector<HDF5_DATA>& DataArray);

	/*********************************************************
	FunctionName:Hdf52Data
	FunctionDesc:把hdf5文件中的内容读取到内存中
	InputParam:szFilePath:要读取的hdf5文件绝对路径
	           QueryParams:要查询的hdf5节点的绝对名称
	OutputParam:ResultVec:查询到的结果数据
	Return:0:succeed 非0:错误码
	Author:xiaowei.han
	*********************************************************/
	int Hdf52Data(const char* szFilePath,const std::vector<HDF5_QUERY_PARAM>& QueryParamVec,std::vector<HDF5_RESULT>& ResultVec);

	/*********************************************************
	FunctionName:Hdf52Data_Quick
	FunctionDesc:把hdf5文件中的内容快速读取到内存中
	InputParam:szFilePath:要读取的hdf5文件绝对路径(只支持dataset的快速读取)
	QueryParams:要查询的hdf5节点的绝对名称
	OutputParam:ResultVec:查询到的结果数据
	Return:0:succeed 非0:错误码
	Author:xiaowei.han
	*********************************************************/
	int Hdf52Data_Quick(const char* szFilePath, const std::vector<HDF5_QUERY_PARAM>& QueryParamVec, std::vector<HDF5_RESULT>& ResultVec);

	/*********************************************************
	FunctionName:ReadHdf5MatrixData
	FunctionDesc:读取hdf5中dataset数据是二维的数据(只支持dataset的快速读取)
	InputParam:szFilePath:要读取的hdf5文件绝对路径
	QueryParams:要查询的hdf5节点的绝对名称
	OutputParam:ResultVec:查询到的结果数据
	Return:0:succeed 非0:错误码
	Author:xiaowei.han
	*********************************************************/
	int ReadHdf5MatrixData(const char* szFilePath, const std::vector<HDF5_QUERY_MATRIX_PARAM>& QueryParamVec, std::vector<HDF5_RESULT>& ResultVec);
}





#endif
