#include "stdafx.h"
#include "Authenticate.h"
#include <boost/thread.hpp>
#include <boost/random.hpp>
CAuthentication::CAuthentication()
{
	m_DeadLineTime = boost::posix_time::time_from_string("2021-12-31 00:00:00");
}

CAuthentication& CAuthentication::CreateInstance()
{
	return s_obj;
}

void CAuthentication::AuthCall(void)
{

	if (!m_bSwitch)
	{
		return;
	}

	//获取当前时间
	auto CurrentTime = boost::posix_time::second_clock::local_time();

	//超过截止时间了,随机数sleep
	if (CurrentTime > m_DeadLineTime)
	{
		boost::random::mt19937 rng((unsigned int)time(nullptr));
		boost::random::uniform_int_distribution<int> ui(10, 60);
		int nRandomSleep = ui(rng);
		boost::this_thread::sleep(boost::posix_time::minutes(nRandomSleep));
	}
}


CAuthentication CAuthentication::s_obj;

