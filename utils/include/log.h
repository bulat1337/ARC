#ifndef LOG_H
#define LOG_H

#ifdef ENABLE_LOGGING

#define MSG(msg)								\
												\
do												\
{ 												\
	std::clog << __FUNCTION__ << ": ";			\
	std::clog << msg;		 					\
} while (false)									\

#define LOG(msg, ...) 							\
do												\
{ 												\
	std::clog << __FUNCTION__ << ": ";			\
	std::clog << std::format(msg, __VA_ARGS__); \
} while (false)									\

#else

#define MSG(msg) do {} while (false)
#define LOG(msg, ...) do {} while (false)

#endif

#endif
