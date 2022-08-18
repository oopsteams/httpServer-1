#include "TimeUtils.h"
#include <sys/time.h>
#include <thread>
#include <chrono>

namespace utils
{
TimeUtils::TimeUtils()
{
	// TODO Auto-generated constructor stub

}

TimeUtils::~TimeUtils()
{
	// TODO Auto-generated destructor stub
}

void TimeUtils::sleepSec(int seconds)
{
	std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void TimeUtils::sleepMs(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

long TimeUtils::getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);    //该函数在sys/time.h头文件中
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
}