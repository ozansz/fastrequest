#pragma once

#include <Python.h>

#define FR_DEBUG

#if PY_MAJOR_VERSION < 3
    #error This library is compatible for Python version >= 3 only!
#endif

#ifdef __GNUC__
    #ifdef _WIN32
        #error This library is compatible for Linux-based platforms only!
    #endif
#endif

PyObject *FastRequest_HttpGet(PyObject *self, PyObject *args);
PyObject *FastRequest_HttpPost(PyObject *self, PyObject *args);