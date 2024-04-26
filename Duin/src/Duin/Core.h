#pragma once

#ifdef DN_PLATFORM_WINDOWS
	#ifdef DN_BUILD_DLL
		#define DUIN_API __declspec(dllexport)
	#else
		#define DUIN_API __declspec(dllimport)
	#endif
	#else
		#error Duin only supports Windows.
#endif