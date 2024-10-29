#ifndef MATHS_CORE_H
#define MATHS_CORE_H

#ifdef DN_PLATFORM_WINDOWS
	#ifdef DN_BUILD_STATIC
		#define DMAPI
	#else
        #ifdef DN_BUILD_DLL
            #define DMAPI __declspec(dllexport)
        #else
            #define DMAPI __declspec(dllimport)
        #endif
	#endif

#else
	#error Duin only supports Windows.
#endif

#endif /* MATHS_CORE_H */
