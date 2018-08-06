#ifndef __AUTHENTICATE_H__
#define __AUTHENTICATE_H__
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
class CAuthentication
{

private:
	CAuthentication();
	~CAuthentication() = default;

public:
	static CAuthentication& CreateInstance();

	//进行验证
	void AuthCall(void);

	void On()
	{
		m_bSwitch = true;
	}
	void Off()
	{
		m_bSwitch = false;
	}

private:
	bool m_bSwitch = true;
private:
	static CAuthentication s_obj;
	//运行截止日期
	boost::posix_time::ptime m_DeadLineTime;
};


#endif
