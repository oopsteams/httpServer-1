
#ifndef UTILS_TIMEUTILS_H_
#define UTILS_TIMEUTILS_H_

#include <stdint.h>
#include <time.h>

namespace utils
{

class TimeUtils
{
public:
	TimeUtils();
	virtual ~TimeUtils();

	static void sleepSec(int seconds);

    static void sleepMs(int milliseconds);

	static long getCurrentTime();
};

} /* namespace utils */

#endif /* UTILS_TIMEUTILS_H_ */