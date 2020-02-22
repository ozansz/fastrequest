#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "fastrequest.h"
#include "stringbuffer.h"
#include "api.h"
#include "utils.h"
#include "models.h"

#include "structmember.h"

static void HTTPResponse_dealloc(HTTPResponseObject *self) {
    Py_XDECREF(self->response_data);
    Py_XDECREF(self->response_size);
    Py_XDECREF(self->request_url);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *HTTPResponse_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    HTTPResponseObject *self;
    self = (HTTPResponseObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->response_data = PyBytes_FromString("");

        if (self->response_data == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        
        self->response_size = PyLong_FromLong(0);
        
        if (self->response_size == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        
        self->request_url = PyUnicode_FromString("");
        
        if (self->request_url == NULL) {
            Py_DECREF(self);
            return NULL;
        }
    }

    return (PyObject *) self;
}

static int HTTPResponse_init(HTTPResponseObject *self, PyObject *args, PyObject *kwds) {
    PyObject *data = NULL, *size = NULL, *url = NULL, *tmp;

    if (!PyArg_ParseTuple(args, "OO|O", &data, &size, &url))
        return -1;

    if (data) {
        tmp = self->response_data;
        Py_INCREF(data);
        self->response_data = data;
        Py_XDECREF(tmp);
    }
    
    if (size) {
        tmp = self->response_size;
        Py_INCREF(size);
        self->response_size = size;
        Py_XDECREF(tmp);
    }

    if (url) {
        tmp = self->request_url;
        Py_INCREF(url);
        self->request_url = url;
        Py_XDECREF(tmp);
    }

    return 0;
}

static PyObject *HTTPResponse_text(HTTPResponseObject *self, PyObject *Py_UNUSED(ignored)) {
    if (self->response_data == NULL) {
        PyErr_SetString(PyExc_AttributeError, "data");
        return NULL;
    }

    return PyUnicode_FromEncodedObject(self->response_data, NULL, NULL);
}

static PyObject *HTTPResponse_repr(HTTPResponseObject *self) {
    return PyUnicode_FromFormat("<fastrequest.HTTPResponse (%S)>", self->request_url);
}

static PyObject *HTTPResponse_str(HTTPResponseObject *self) {
    return HTTPResponse_text(self, NULL);
}

static PyMethodDef HTTPResponse_methods[] = {
    {"text", (PyCFunction) HTTPResponse_text, METH_NOARGS,
     "Get the string value of the HTTP response"
    },
    {NULL}  /* Sentinel */
};

static PyMemberDef HTTPResponse_members[] = {
    {"size", T_OBJECT_EX, offsetof(HTTPResponseObject, response_size), 0,
     "response size"},
    {"data", T_OBJECT_EX, offsetof(HTTPResponseObject, response_data), 0,
     "response data"},
    {"url", T_OBJECT_EX, offsetof(HTTPResponseObject, request_url), 0,
     "request url"},
    {NULL}  /* Sentinel */
};

static PyTypeObject HTTPResponseType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "fastrequest.HTTPResponse",
    .tp_doc = "HTTP response object",
    .tp_basicsize = sizeof(HTTPResponseObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = HTTPResponse_new,
    .tp_init = (initproc) HTTPResponse_init,
    .tp_dealloc = (destructor) HTTPResponse_dealloc,
    .tp_members = HTTPResponse_members,
    .tp_methods = HTTPResponse_methods,
    .tp_repr = (reprfunc) HTTPResponse_repr,
    .tp_str = (reprfunc) HTTPResponse_str
};

PyObject *FastRequest_HttpGet(PyObject *self, PyObject *args) {
    char *url;

    StringBuffer *strbuf;

    PyObject *response_args, *response, *res_data, *res_size, *req_url;

    if (!PyArg_ParseTuple(args, "s", &url)) {
        return NULL;
    }

    strbuf = FastRequestAPI_LibcurlHttpGet(url);

    if (strbuf == NULL)
        return NULL;

    StringBuffer_PushChar(strbuf, '\0');

    res_data = PyBytes_FromString(strbuf->buf);
    res_size = PyLong_FromLongLong((long long)(strbuf->index));
    req_url = PyUnicode_FromString(url);

    response_args = PyTuple_Pack(3, res_data, res_size, req_url);
    response = PyObject_CallObject((PyObject *) &HTTPResponseType, response_args);

    Py_DECREF(response_args);
    Py_DECREF(res_data);
    Py_DECREF(res_size);
    Py_DECREF(req_url);

    StringBuffer_Free(strbuf);

    return response;
}

static PyMethodDef FastRequestMethods[] = {
    {"http_get", FastRequest_HttpGet, METH_VARARGS, "Fast GET request using libcurl4 library."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef fastrequestmodule = {
    PyModuleDef_HEAD_INIT,
    "fastrequest",
    "A Python C extension module for fast HTTP requests with SSL/TLS support",
    -1,
    FastRequestMethods
};

PyMODINIT_FUNC PyInit_fastrequest(void) {
    //return PyModule_Create(&fastrequestmodule);

    PyObject *m;

    if (PyType_Ready(&HTTPResponseType) < 0)
        return NULL;

    m = PyModule_Create(&fastrequestmodule);
    
    if (m == NULL)
        return NULL;

    Py_INCREF(&HTTPResponseType);
    PyModule_AddObject(m, "HTTPResponse", (PyObject *) &HTTPResponseType);

    return m;
}
