#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "http.h"
#include "stringbuffer.h"
#include "api.h"
#include "utils.h"
#include "models.h"

#include "structmember.h"

static PyObject *json_module = NULL;

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

    return PyUnicode_FromEncodedObject(self->response_data, "utf-8", NULL);
}

static PyObject *HTTPResponse_json(HTTPResponseObject *self, PyObject *Py_UNUSED(ignored)) {
    PyObject *json_loads, *call_ret, *call_args;

    if (self->response_data == NULL) {
        PyErr_SetString(PyExc_AttributeError, "data");
        return NULL;
    }

    if (json_module == NULL) {
        PyErr_SetString(PyExc_ImportError, "JSON module was not imported");
        return NULL;
    }

    json_loads = PyObject_GetAttrString(json_module, "loads");

    if (json_loads == NULL) {
        PyErr_SetString(PyExc_AttributeError, "json.loads");
        return NULL;
    }

    call_args = PyTuple_Pack(1, self->response_data);
    call_ret = PyObject_Call(json_loads, call_args, NULL);

    Py_DECREF(call_args);
    Py_DECREF(json_loads);

    return call_ret;
}

static PyObject *HTTPResponse_get_default_encoding(HTTPResponseObject *self, PyObject *Py_UNUSED(ignored)) {
    return PyUnicode_FromString("utf-8");
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
    {"json", (PyCFunction) HTTPResponse_json, METH_NOARGS,
     "Get the string value of the HTTP response"
    },
    {"get_default_encoding", (PyCFunction) HTTPResponse_get_default_encoding,
    METH_CLASS | METH_NOARGS, "Get the default encoding type"
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

    PyObject *headers = NULL;
    PyObject *response_args, *response, *res_data, *res_size, *req_url;

    if (!PyArg_ParseTuple(args, "s|O", &url, &headers)) {
        return NULL;
    }

    if (headers != NULL) {
        FastRequest_FuncDebug("FastRequest_HttpGet", "<ARG> Headers is non-null");

        if (!PyDict_Check(headers)) {
            PyErr_SetString(PyExc_TypeError, "\'headers\' argument must be type of dict.");
            return NULL;
        }

        Py_INCREF(headers);
    } else {
        FastRequest_FuncDebug("FastRequest_HttpGet", "<ARG> Headers is null");
    }

    strbuf = FastRequestAPI_LibcurlHttpGet(url, headers);

    if (strbuf == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "FastRequestAPI_LibcurlHttpGet response strbuf got NULL");
        return NULL;
    }

    StringBuffer_PushChar(strbuf, '\0');

    res_data = PyBytes_FromString(strbuf->buf);    
    res_size = PyLong_FromLongLong((long long)(strbuf->index));
    req_url = PyUnicode_FromString(url);

    if (res_data == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "res_data PyObject got NULL");
        return NULL;
    }

    if (res_size == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "res_size PyObject got NULL");
        return NULL;
    }

    if (req_url == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "req_url PyObject got NULL");
        return NULL;
    }

    response_args = PyTuple_Pack(3, res_data, res_size, req_url);

    if (response_args == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "response_args PyObject got NULL");
        return NULL;
    }

    response = PyObject_CallObject((PyObject *) &HTTPResponseType, response_args);

    Py_DECREF(response_args);
    Py_DECREF(res_data);
    Py_DECREF(res_size);
    Py_DECREF(req_url);

    StringBuffer_Free(strbuf);

    if (response == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "response PyObject got NULL");
        return NULL;
    }

    return response;
}

PyObject *FastRequest_HttpPost(PyObject *self, PyObject *args) {
    char *url;
    const char *payload_str;

    int_fast8_t payload_encoding = FR_HTTP_POST_URLENCODED;

    StringBuffer *strbuf;

    PyObject *headers = NULL, *payload = NULL, *payload_json_dump, *json_dumps;
    PyObject *json_dumps_call_args, *payload_repr;
    PyObject *response_args, *response, *res_data, *res_size, *req_url;

    FastRequest_FuncDebug("FastRequest_HttpPost", ">> Initializing arguments");

    if (!PyArg_ParseTuple(args, "s|OO", &url, &payload, &headers)) {
        return NULL;
    }

    if (payload != NULL) {
        FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is given");

        if (PyDict_Check(payload)) {
            FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is dictionary");

            json_dumps = PyObject_GetAttrString(json_module, "dumps");

            if (json_dumps == NULL) {
                PyErr_SetString(PyExc_AttributeError, "json.dumps");
                return NULL;
            }

            json_dumps_call_args = PyTuple_Pack(1, payload);
            payload_json_dump = PyObject_Call(json_dumps, json_dumps_call_args, NULL);

            payload_str = PyUnicode_AsUTF8(payload_json_dump);
            payload_encoding = FR_HTTP_POST_JSON;

            Py_DECREF(json_dumps_call_args);
            Py_DECREF(json_dumps);
            Py_DECREF(payload_json_dump);
        } else if (PyUnicode_Check(payload)) {
            FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is string");

            payload_str = PyUnicode_AsUTF8(payload);
        } else {
            FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is not dictionary nor string, will use repr");

            payload_repr = PyObject_Repr(payload);
            payload_str = PyUnicode_AsUTF8(payload_repr);

            Py_DECREF(payload_repr);
        }
    } else {
        FastRequest_FuncDebug("FastRequest_HttpPost", ">> Payload is NULL");

        payload_str = NULL;
    }

    if (headers != NULL) {
        FastRequest_FuncDebug("FastRequest_HttpPost", ">> Headers is given");

        if (!PyDict_Check(headers)) {
            PyErr_SetString(PyExc_TypeError, "\'headers\' argument must be type of dict.");
            return NULL;
        }

        Py_INCREF(headers);
    } else {
        FastRequest_FuncDebug("FastRequest_HttpPost", ">> Headers is NULL");
    }

    FastRequest_FuncDebug("FastRequest_HttpPost", ">> Making the POST request");
    strbuf = FastRequestAPI_LibcurlHttpPost(url, payload_str, payload_encoding, headers);

    if (strbuf == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "FastRequestAPI_LibcurlHttpGet response strbuf got NULL");
        return NULL;
    }

    StringBuffer_PushChar(strbuf, '\0');

    res_data = PyBytes_FromString(strbuf->buf);    
    res_size = PyLong_FromLongLong((long long)(strbuf->index));
    req_url = PyUnicode_FromString(url);

    if (res_data == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "res_data PyObject got NULL");
        return NULL;
    }

    if (res_size == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "res_size PyObject got NULL");
        return NULL;
    }

    if (req_url == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "req_url PyObject got NULL");
        return NULL;
    }

    response_args = PyTuple_Pack(3, res_data, res_size, req_url);

    if (response_args == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "response_args PyObject got NULL");
        return NULL;
    }

    FastRequest_FuncDebug("FastRequest_HttpPost", ">> Creating response object");
    response = PyObject_CallObject((PyObject *) &HTTPResponseType, response_args);

    Py_DECREF(response_args);
    Py_DECREF(res_data);
    Py_DECREF(res_size);
    Py_DECREF(req_url);

    StringBuffer_Free(strbuf);

    if (response == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "response PyObject got NULL");
        return NULL;
    }

    return response;
}

static PyMethodDef FastRequestHTTPMethods[] = {
    {"get", FastRequest_HttpGet, METH_VARARGS, "Fast GET request using libcurl4 library."},
    {"post", FastRequest_HttpPost, METH_VARARGS, "Fast POST request using libcurl4 library."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef fastrequesthttpmodule = {
    PyModuleDef_HEAD_INIT,
    "fastrequest",
    "A Python C extension module for fast HTTP requests with SSL/TLS support",
    -1,
    FastRequestHTTPMethods
};

PyMODINIT_FUNC PyInit_http(void) {
    PyObject *m;

    if (PyType_Ready(&HTTPResponseType) < 0)
        return NULL;

    m = PyModule_Create(&fastrequesthttpmodule);
    
    if (m == NULL)
        return NULL;

    Py_INCREF(&HTTPResponseType);
    PyModule_AddObject(m, "HTTPResponse", (PyObject *) &HTTPResponseType);

    json_module = PyImport_ImportModule("json");

    if (json_module == NULL)
        return NULL;

    Py_INCREF(json_module);

    return m;
}
