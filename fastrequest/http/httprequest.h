#pragma once

#include <Python.h>

#define PY_SSIZE_T_CLEAN

typedef struct {
    PyObject_HEAD
    PyObject *request_url;
    PyObject *request_data;
    PyObject *request_headers;
    PyObject *request_cookies;
} HTTPRequestObject;