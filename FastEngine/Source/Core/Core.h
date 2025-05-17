#pragma once

#ifdef PLATFORM_WINDOWS
    #ifdef ENGINE_BUILD_DLL
        #define ENGINE_API
    #else
        #define ENGINE_API
    #endif
#endif