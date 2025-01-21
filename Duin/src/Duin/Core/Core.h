#ifndef CORE_H
#define CORE_H

#ifdef DN_PLATFORM_WINDOWS
	#ifdef DN_BUILD_STATIC
		#define DAPI
	#else
        #ifdef DN_BUILD_DLL
            #define DAPI __declspec(dllexport)
        #else
            #define DAPI __declspec(dllimport)
        #endif
	#endif

#else
    #error Duin only supports Windows.
#endif

#endif /* CORE_H */
