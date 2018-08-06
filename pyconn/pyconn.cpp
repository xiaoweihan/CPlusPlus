// pyconn.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define BOOST_PYTHON_STATIC_LIB 
#include <boost/python.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/smart_ptr.hpp>
#include <string>
using namespace boost::interprocess;
using namespace boost::python;

class PyConn
{
public:
	PyConn(const std::string strKeyName):
		m_strKeyName(strKeyName)

	{
		if (!m_pSem)
		{
			m_pSem = boost::make_shared<named_semaphore>(open_or_create, strKeyName.c_str(),0);
		}

	}
	~PyConn()
	{
		named_semaphore::remove(m_strKeyName.c_str());
	}

public:
	bool Wait()
	{
		//创建一个

		bool bResult = true;
		try
		{
			m_pSem->wait();
		}
		catch (interprocess_exception &ex)
		{
			boost::ignore_unused(ex);
			bResult = false;
		}

		return bResult;
	}

	bool Notify()
	{
		//创建一个

		bool bResult = true;
		try
		{
			m_pSem->post();
		}
		catch (interprocess_exception &ex)
		{
			boost::ignore_unused(ex);
			bResult = false;
		}

		return bResult;
	}
private:
	std::string m_strKeyName;
	//具名条件变量
	boost::shared_ptr<named_semaphore> m_pSem;
};

BOOST_PYTHON_MODULE(pyconn)
{
	class_<PyConn>("PyConn",init<std::string>())
		.def("wait", &PyConn::Wait)
		.def("notify", &PyConn::Notify)
		;
}

