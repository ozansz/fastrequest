#pragma once

#include <Python.h>

#define PY_SSIZE_T_CLEAN

typedef struct {
    PyObject_HEAD
    PyObject *response_size;
    PyObject *response_data;
    PyObject *request_url;
} HTTPResponseObject;