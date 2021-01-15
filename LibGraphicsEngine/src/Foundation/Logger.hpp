#ifndef FOUNDATION_LOGGER_HPP
#define FOUNDATION_LOGGER_HPP

#include "Core/AppConfig.hpp"
#include "Foundation/RTTI.hpp"
//#include <cstdarg>
#include <cstdio>

//int _print(const char* format, ...)
//{
//	va_list arg;
//	int done;
//	va_start(arg, format);
//	done = __vfprintf_internal(stdout, format, arg, 0);
//	va_end(arg);
//	return done;
//}

#ifdef ENABLE_LOG
	#define LOG_ERROR(args, ...) do { printf(args, ##__VA_ARGS__); printf("\nFILE: %s, FUNC: %s, LINE: %ld \n\n", __FILE__, __FUNCTION__, __LINE__); fflush(stdout); } while (0);
	#define LOG_WARNING(args, ...) do { printf(args, ##__VA_ARGS__); printf("\nFILE: %s, FUNC: %s, LINE: %ld \n\n", __FILE__, __FUNCTION__, __LINE__); } while (0);
	#define LOG_DEBUG(args, ...) do { printf(args, ##__VA_ARGS__); printf("\nFILE: %s, FUNC: %s, LINE: %ld \n\n", __FILE__, __FUNCTION__, __LINE__); } while (0);
	#define LOG_INFO(args, ...) do { printf(args, ##__VA_ARGS__); printf("\n\n"); } while (0);
#else
	#define	LOG_ERROR(...) do{}while(0);
	#define	LOG_WARNING(...) do{}while(0);
	#define	LOG_DEBUG(...) do{}while(0);
	#define LOG_INFO(...) do{}while(0);
#endif // ENABLE_LOG

#endif /* FOUNDATION_LOGGER_HPP */