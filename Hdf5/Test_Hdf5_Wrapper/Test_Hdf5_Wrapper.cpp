#include "stdafx.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <memory>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/timer.hpp>
#include "Hdf5_Wrapper.h"
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"Hdf5_Wrapperd.lib")
#else
#pragma comment(lib,"Hdf5_Wrapper.lib")
#endif
#endif
using rd_iterator = boost::filesystem::recursive_directory_iterator;

struct _read_file_info
{
	double dCostTime;
	std::string strHdf5FilePath;

	_read_file_info(double dCostTime, const std::string& strFilePath)
	{
		this->dCostTime = dCostTime;
		this->strHdf5FilePath = strFilePath;
	}
};

using READ_FILE_INFO = _read_file_info;
using LP_READ_FILE_INFO = _read_file_info*;

void TestCompare()
{
	using namespace OuterType;
	using namespace std;
	vector<HDF5_QUERY_MATRIX_PARAM> QueryArray;

	for (int i = 0; i < 299; ++i)
	{
		HDF5_QUERY_MATRIX_PARAM Param;
		Param.strQueryKeyName = "/SimResults/TDC";
		Param.bColumn = true;
		Param.nIndex = i;
		QueryArray.push_back(Param);
	}

	vector<HDF5_RESULT> ResultVec;
	ResultVec.clear();
	boost::timer T;
	if (0 == ReadHdf5MatrixData("V:\\Dep_DMP\\ZheyuanZheng\\WS1.h5", QueryArray, ResultVec))
	{
		cout << T.elapsed() << endl;
		cout << ResultVec.size() << endl;
	}
	cout << "****************************************" << endl;
	T.restart();

	for (int i = 0; i < 299; ++i)
	{
		ResultVec.clear();
		vector<HDF5_QUERY_MATRIX_PARAM> QueryArray1;
		QueryArray1.clear();
		HDF5_QUERY_MATRIX_PARAM Param1;
		Param1.strQueryKeyName = "/SimResults/TDC";
		Param1.bColumn = true;
		Param1.nIndex = i;
		QueryArray1.push_back(Param1);
		if (0 == ReadHdf5MatrixData("V:\\Dep_DMP\\ZheyuanZheng\\WS3.h5", QueryArray1, ResultVec))
		{
			
		}
	}

	cout << T.elapsed() << endl;
}

void TestReadParticalData(void)
{
	using namespace std;

	string strH5File = R"(D:\test_dat\en121_alf_test_final_b1.h5)";


	//开始访问
	//设置读取文件
	std::vector<OuterType::HDF5_QUERY_MATRIX_PARAM> VarArray;
	
	OuterType::HDF5_QUERY_MATRIX_PARAM TempParam;
	TempParam.strQueryKeyName = "/SimResults/TDC";
	TempParam.bColumn = false;
	TempParam.nIndex = 3;
	VarArray.push_back(std::move(TempParam));


	std::vector<OuterType::HDF5_RESULT> Results;
	

	int nResult = OuterType::ReadHdf5MatrixData(strH5File.c_str(), VarArray, Results);

	if (0 != nResult)
	{
		cout << nResult << endl;
	}
	else
	{

		if (!Results.empty())
		{

			char* pBuffer = new char[Results[0].ResultArray.size()];

			if (nullptr != pBuffer)
			{
				for (int i = 0; i < Results[0].ResultArray.size(); ++i)
				{
					pBuffer[i] = Results[0].ResultArray[i];
				}


				int i = 0;
				float* p = (float*)pBuffer;
				while (i < Results[0].Header.nTotalDataLen)
				{
					cout << *p << endl;
					p += 1;
					i += 4;
				}
			}
		}	
	}
}

/*********************************************************
FunctionName:TestReadHdf5Array
FunctionDesc:读取多个hdf5文件中的TDC
InputParam:
OutputParam:
Return:
Author:xiaowei.han
*********************************************************/
static void TestReadHdf5Array(const std::vector<std::string>& hdf5pathArray)
{
	using namespace OuterType;
	using namespace std;
	if (hdf5pathArray.empty())
	{
		return;
	}
	int nResult = 0;
	vector<READ_FILE_INFO> ReadInfoArray;
	vector<HDF5_RESULT> ResultVec;
	vector<HDF5_QUERY_PARAM> QueryArray;
	HDF5_QUERY_PARAM Param;
	Param.strQueryKeyName = "/SimResults/TDC";
	Param.bQueryAttr = false;
	QueryArray.push_back(Param);

	for (auto& FilePath : hdf5pathArray)
	{
		ResultVec.clear();
		boost::timer T;
		//读取成功
		if ((nResult = Hdf52Data_Quick(FilePath.c_str(), QueryArray, ResultVec)) == 0)
		{
			
			if (ResultVec.size() == 1)
			{
				double dCostTime = T.elapsed();
				ReadInfoArray.emplace_back(dCostTime, FilePath);
			}
		}
	}

	//开始输出
	cout << "<file_path>" << "\t" << "<cost_time [s]>" << endl;


	for (auto& Element : ReadInfoArray)
	{
		cout << Element.strHdf5FilePath << "\t" << Element.dCostTime << endl;
	}
}

static void TestReadHdf5Array1(const std::vector<std::string>& hdf5pathArray)
{
	using namespace OuterType;
	using namespace std;
	if (hdf5pathArray.empty())
	{
		return;
	}
	int nResult = 0;
	vector<READ_FILE_INFO> ReadInfoArray;
	vector<HDF5_RESULT> ResultVec;
	vector<HDF5_QUERY_PARAM> QueryArray;
	HDF5_QUERY_PARAM Param;
	Param.strQueryKeyName = "/SimResults/TDC";
	Param.bQueryAttr = false;
	QueryArray.push_back(Param);

	for (auto& FilePath : hdf5pathArray)
	{
		ResultVec.clear();
		boost::timer T;
		//读取成功
		if ((nResult = Hdf52Data_Quick(FilePath.c_str(), QueryArray, ResultVec)) == 0)
		{

			if (ResultVec.size() == 1)
			{
				double dCostTime = T.elapsed();
				ReadInfoArray.emplace_back(dCostTime, FilePath);
			}
		}
	}

	//开始输出
	cout << "<file_path>" << "\t" << "<cost_time [s]>" << endl;


	for (auto& Element : ReadInfoArray)
	{
		cout << Element.strHdf5FilePath << "\t" << Element.dCostTime << endl;
	}
}

int main(int nArgc, char* argv[])
{

	if (nArgc != 3)
	{
		return 0;
	}

#if 0
	if (nArgc <= 1)
	{
		return 0;
	}


	vector<string> fileArray;

	for (int i = 1; i < nArgc; ++i)
	{
		fileArray.push_back(argv[i]);
	}
	
	TestReadHdf5Array(fileArray);
#endif

	using namespace OuterType;
	using namespace std;
	
	string strPath1 = argv[1];
	string strPath2 = argv[2];

	int nResult = 0;
	//vector<READ_FILE_INFO> ReadInfoArray;
	vector<HDF5_RESULT> ResultVec;
	vector<HDF5_QUERY_PARAM> QueryArray;
	HDF5_QUERY_PARAM Param;
	Param.strQueryKeyName = "/SimResults/TDC";
	Param.bQueryAttr = false;
	QueryArray.push_back(Param);

	boost::timer T;
	if (Hdf52Data_Quick(strPath1.c_str(), QueryArray, ResultVec) == 0)
	{

		if (ResultVec.size() == 1)
		{
			cout << "read big block TDC cost: " << T.elapsed() << endl;
		}
	}

	ResultVec.clear();
	T.restart();
	if (Hdf52Data_Quick(strPath2.c_str(), QueryArray, ResultVec) == 0)
	{

		if (ResultVec.size() == 1)
		{
			cout << "read small block TDC cost: " << T.elapsed() << endl;
		}
	}
	
	system("pause");
#if 0
	int nProcessFileNum = 1000;
	using namespace std;
	vector<string> SrcVarArray;
	SrcVarArray.push_back("/.imcW_Yaw_CW");
	vector<string> H5FileArray;
	string strH5Dir = string(R"(D:\2017-11-06)");
	//遍历文件夹
	rd_iterator End;
	int nCount = 0;
	for (rd_iterator Iter(strH5Dir); (Iter != End && nCount < nProcessFileNum); ++Iter, ++nCount)
	{
		boost::filesystem::path tempPath = *Iter;
		H5FileArray.push_back(tempPath.string());
	}

	//设置读取文件
	std::vector<OuterType::HDF5_QUERY_PARAM> VarArray;
	for (auto& VarName : SrcVarArray)
	{
		OuterType::HDF5_QUERY_PARAM TempParam;
		TempParam.strQueryKeyName = VarName;
		TempParam.bQueryAttr = false;
		VarArray.push_back(std::move(TempParam));
	}



	int nSucceedNum = 0;
	std::vector<OuterType::HDF5_RESULT> Results;
	boost::timer T;
	for (auto& FileName : H5FileArray)
	{
		if (0 == OuterType::Hdf52Data_Quick(FileName.c_str(), VarArray, Results))
		{
			++nSucceedNum;
		}
	}
	cout << nSucceedNum << "," << T.elapsed() << endl;
#endif
	return 0;
}

