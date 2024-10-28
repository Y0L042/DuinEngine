#ifndef CORE_H
#define CORE_H

#ifdef DN_PLATFORM_WINDOWS
	#ifdef DN_BUILD_STATIC
		#define DUIN_API
	#else
        #ifdef DN_BUILD_DLL
            #define DUIN_API __declspec(dllexport)
        #else
            #define DUIN_API __declspec(dllimport)
        #endif
	#endif

#else
    #error Duin only supports Windows.
#endif

#endif /* CORE_H */
