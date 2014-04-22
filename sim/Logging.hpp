#pragma once

#ifdef ANDROID
#include <sstream>
#include <android/log.h>


#define LOG_ERROR( X ) \
	do { \
		std::ostringstream log_stream; \
		log_stream << X << std::endl; \
		__android_log_print(ANDROID_LOG_ERROR, "rainbow", "%s", log_stream.str().c_str()); \
	} while (0);
#define LOG_INFO( X ) \
	do { \
		std::ostringstream log_stream; \
		log_stream << X << std::endl; \
		__android_log_print(ANDROID_LOG_INFO, "rainbow", "%s", log_stream.str().c_str()); \
	} while (0);

#else

#include <iostream>

#define LOG_ERROR( X ) std::cerr << X << std::endl
#define LOG_INFO( X ) std::cout << X << std::endl

#endif
