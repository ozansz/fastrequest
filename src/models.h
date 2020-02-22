#pragma once

#include <Python.h>

#define PY_SSIZE_T_CLEAN

#if PY_MAJOR_VERSION < 3
    #error This library is compatible for Python version >= 3 only!
#endif

typedef struct {
    PyObject_HEAD
    PyObject *response_size;
    PyObject *response_data;
    PyObject *request_url;
} HTTPResponseObject;