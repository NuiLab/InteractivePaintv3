#pragma once

#ifdef RC_INVOKED
#define WORKING_DIR "./"
#define QUOTE_PATH(x) #x
#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) \
	ID		TYPE 	QUOTE_PATH( ./##LOCALPREFIX##PATH )
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <winapifamily.h>

#if defined(WINAPI_PARTITION_DESKTOP) // MSW RESOURCE
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#define QUOTE_PATH(x) #x
#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) \
			QUOTE_PATH(LOCALPREFIX##PATH),ID,#TYPE
#else // WINRT RESOURCE
#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) #PATH
#endif
#else // MSW RESOURCE
#define QUOTE_PATH(x) #x
#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) \
		QUOTE_PATH(LOCALPREFIX##PATH),ID,#TYPE
#endif
#else // MAC or iOS RESOURCE
#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) #PATH
#endif


#include <string>

const std::string appName = "Interactive Paint";
const float appHeight = 800.0f;
const float appWidth = 1.618 * appHeight;

#define APP_MAJOR_VERSION      0
#define APP_MINOR_VERSION      0
#define APP_POINT_RELEASE      1
#define APP_VERSION (( APP_MAJOR_VERSION << 16) + ( APP_MINOR_VERSION << 8))

inline std::string getAppVersion()
{
	return std::to_string(APP_MAJOR_VERSION) + "." +
		std::to_string(APP_MINOR_VERSION) + "." +
		std::to_string(APP_POINT_RELEASE);
}

inline std::string getTitle()
{
	std::string name = appName;
	std::string version = getAppVersion();
	std::string mode = "";
#ifndef NDEBUG
	mode = " debug";
#endif
	return  name + " " + version + mode;
}


//#define RES_MY_RES			CINDER_RESOURCE( ../resources/, image_name.png, 128, IMAGE )





