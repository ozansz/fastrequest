#pragma once

#include <Python.h>

#undef FR_DEBUG

#define FR_HTTP_DEFAULT_ENCODING "utf-8"
#define FR_HTTP_URLENCODED  1
#define FR_HTTP_JSON    2

#if PY_MAJOR_VERSION < 3
    #error This library is compatible for Python version >= 3 only!
#endif

#ifdef __GNUC__
    #ifdef _WIN32
        #error This library is compatible for Linux-based platforms only!
    #endif
#endif