/*********************************************************
Copyright(C):
FileName:Hdf5_Wrapper.h
Descripton:hdf5�ļ���װͷ
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
	//Ҫ�洢����������
	enum DATA_TYPE
	{
		NO_TYPE = 0,
		//������
		FLOAT_TYPE = 1,
		//����
		INT_TYPE,
		//�ַ���
		STR_TYPE,
		//��������
		COMPOUND_TYPE,
		DATA_TYPE_TOTAL,
		DATA_TYPE_MIN = FLOAT_TYPE,
		DATA_TYPE_MAX = DATA_TYPE_TOTAL - 1,
	};
	enum NODE_TYPE
	{
		NODE_NO_TYPE = 0,
		//��ڵ�
		NODE_BRANCH,
		//Ҷ�ӽڵ�
		NODE_LEAF
	};
	//���͵�����ͷ����
	struct _int_type_header
	{
		//�Ƿ������
		bool bSign;

		_int_type_header()
		{
			bSign = true;
		}
	};
	using INT_DATA_HEADER = _int_type_header;
	using LP_INT_DATA_HEADER = _int_type_header*;


	//�����͵�����ͷ����
	struct _float_type_header
	{
		_float_type_header() = default;
	};
	using FLOAT_DATA_HEADER = _float_type_header;
	using LP_FLOAT_DATA_HEADER = _float_type_header*;


	//�ַ���������ͷ����
	struct _str_type_header
	{
		_str_type_header() = default;
	};
	using STR_DATA_HEADER = _str_type_header;
	using LP_STR_DATA_HEADER = _str_type_header*;

	struct _compound_type_elment
	{
		//ÿ���ֶε�����
		std::string strElementName;
		//��Ӧ��ƫ����
		unsigned int nElementoffset;
		//��������
		DATA_TYPE eumDataType;
		//��������ռ�õ��ֽ���
		unsigned int nDataByte;
		//���Ͳ���
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


	//�����͵�����ͷ����
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

	//��������ͷ
	struct _data_header
	{
		//ά��
		int nRank;
		//ά����Ϣ
		std::vector<int> DimensionArray;
		//��������ռ�õ��ֽ���
		unsigned int nDataByte;
		//�������ݵ��ڴ�����峤��
		unsigned int nTotalDataLen;
		//����ͷ
		DATA_TYPE eumDataType;
		//��������ͷ�����̶���������
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


	//��������
	struct _data_attribute
	{
		std::string strKeyAttrName;
		//����ͷ
		DATA_HEADER Header;
		//����
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


	//���ݶ���
	struct _hdf5_data
	{
		//�ڵ�����
		NODE_TYPE eumNodeType;
		//hash���е�key
		std::string strKeyName;
		//��������
		std::vector<DATA_ATTRIBUTE> AttributeArray;
		//����ͷ
		DATA_HEADER Header;
		//������Ϣ
		void* pData;
		//ѹ����
		int nCompressLevel;
		//����Ϣ
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



	//��������
	struct _data_result_attribute
	{
		std::string strKeyAttrName;
		//����ͷ
		DATA_HEADER Header;
		//������Ϣ
		std::vector<unsigned char> ResultArray;
		_data_result_attribute()
		{
			ResultArray.clear();
		}

	};
	using DATA_RESULT_ATTRIBUTE = _data_result_attribute;
	using LP_DATA_RESULT_ATTRIBUTE = _data_result_attribute*;

	//���ݶ�ȡ�����
	struct _hdf5_result
	{
		//�ڵ�����
		NODE_TYPE eumNodeType;
		//hash���е�key
		std::string strKeyName;
		//��������
		std::vector<DATA_RESULT_ATTRIBUTE> AttributeArray;
		//����ͷ
		DATA_HEADER Header;
		//������Ϣ
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

		//��ѯ��ʼ������
		int nIndex;

		//��ʾ�����ǻ������������ǻ��������� false:���������� true:����������
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
	FunctionDesc:���ڴ�����ת��Ϊhdf5�ļ��洢
	InputParam:szFilePath:Ҫ�洢��hdf5�ļ�����·��
			   DataArray:���ε��ڴ�����		  
	OutputParam:None
	Return:0:succeed ��0:������
	Author:xiaowei.han
	*********************************************************/
	int Data2Hdf5(const char* szFilePath,std::vector<HDF5_DATA>& DataArray);

	/*********************************************************
	FunctionName:Hdf52Data
	FunctionDesc:��hdf5�ļ��е����ݶ�ȡ���ڴ���
	InputParam:szFilePath:Ҫ��ȡ��hdf5�ļ�����·��
	           QueryParams:Ҫ��ѯ��hdf5�ڵ�ľ�������
	OutputParam:ResultVec:��ѯ���Ľ������
	Return:0:succeed ��0:������
	Author:xiaowei.han
	*********************************************************/
	int Hdf52Data(const char* szFilePath,const std::vector<HDF5_QUERY_PARAM>& QueryParamVec,std::vector<HDF5_RESULT>& ResultVec);

	/*********************************************************
	FunctionName:Hdf52Data_Quick
	FunctionDesc:��hdf5�ļ��е����ݿ��ٶ�ȡ���ڴ���
	InputParam:szFilePath:Ҫ��ȡ��hdf5�ļ�����·��(ֻ֧��dataset�Ŀ��ٶ�ȡ)
	QueryParams:Ҫ��ѯ��hdf5�ڵ�ľ�������
	OutputParam:ResultVec:��ѯ���Ľ������
	Return:0:succeed ��0:������
	Author:xiaowei.han
	*********************************************************/
	int Hdf52Data_Quick(const char* szFilePath, const std::vector<HDF5_QUERY_PARAM>& QueryParamVec, std::vector<HDF5_RESULT>& ResultVec);

	/*********************************************************
	FunctionName:ReadHdf5MatrixData
	FunctionDesc:��ȡhdf5��dataset�����Ƕ�ά������(ֻ֧��dataset�Ŀ��ٶ�ȡ)
	InputParam:szFilePath:Ҫ��ȡ��hdf5�ļ�����·��
	QueryParams:Ҫ��ѯ��hdf5�ڵ�ľ�������
	OutputParam:ResultVec:��ѯ���Ľ������
	Return:0:succeed ��0:������
	Author:xiaowei.han
	*********************************************************/
	int ReadHdf5MatrixData(const char* szFilePath, const std::vector<HDF5_QUERY_MATRIX_PARAM>& QueryParamVec, std::vector<HDF5_RESULT>& ResultVec);
}





#endif
