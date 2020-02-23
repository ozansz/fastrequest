#include <Python.h>

#include "httprequest.h"
#include "funcwrappers.h"

#include "structmember.h"

extern PyObject *json_module;

static void HTTPRequest_dealloc(HTTPRequestObject *self) {
    Py_XDECREF(self->request_url);
    Py_XDECREF(self->request_headers);
    Py_XDECREF(self->request_data);
    Py_XDECREF(self->request_cookies);

    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *HTTPRequest_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    HTTPRequestObject *self;
    self = (HTTPRequestObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->request_url = PyUnicode_FromString("");

        if (self->request_url == NULL) {
            Py_DECREF(self);
            return NULL;
        }

        self->request_data = PyBytes_FromString("");

        if (self->request_data == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        
        self->request_headers = PyDict_New();

        if (self->request_headers == NULL) {
            Py_DECREF(self);
            return NULL;
        }

        self->request_cookies = PyDict_New();

        if (self->request_cookies == NULL) {
            Py_DECREF(self);
            return NULL;
        }
    }

    return (PyObject *) self;
}

static int HTTPRequest_init(HTTPRequestObject *self, PyObject *args, PyObject *kwargs) {
    PyObject *url, *payload = NULL, *headers = NULL, *cookies = NULL, *tmp;

    static char *kwlist[] = {"url", "payload", "headers", "cookies", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOO", kwlist, &url, &payload, &headers, &cookies))
        return -1;

    tmp = self->request_url;
    Py_INCREF(url);
    self->request_url = url;
    Py_XDECREF(tmp);

    if (payload) {
        tmp = self->request_data;
        Py_INCREF(payload);
        self->request_data = payload;
        Py_XDECREF(tmp);
    }
    
    if (headers) {
        tmp = self->request_headers;
        Py_INCREF(headers);
        self->request_headers = headers;
        Py_XDECREF(tmp);
    }

    if (cookies) {
        tmp = self->request_cookies;
        Py_INCREF(cookies);
        self->request_cookies = cookies;
        Py_XDECREF(tmp);
    }

    return 0;
}

PyObject *HTTPRequest_get(HTTPRequestObject *self) {
    PyObject *ret, *args;
    
    Py_INCREF(self->request_url);
    Py_INCREF(self->request_headers);
    args = PyTuple_Pack(2, self->request_url, self->request_headers);

    if (args == NULL)
        return NULL;

    ret = FastRequest_HttpGet((PyObject *) self, args);

    Py_DECREF(args);

    if (ret == NULL)
        return NULL;

    return ret;
}

PyObject *HTTPRequest_post(HTTPRequestObject *self) {
    PyObject *ret, *args;
    
    Py_INCREF(self->request_url);
    Py_INCREF(self->request_data);
    Py_INCREF(self->request_headers);
    args = PyTuple_Pack(3, self->request_url, self->request_data, self->request_headers);

    if (args == NULL)
        return NULL;

    ret = FastRequest_HttpPost((PyObject *) self, args);

    Py_DECREF(args);

    if (ret == NULL)
        return NULL;

    return ret;
}

static PyObject *HTTPRequest_repr(HTTPRequestObject *self) {
    return PyUnicode_FromFormat("<fastrequest.http.HTTPRequest (%S)>", self->request_url);
}

static PyObject *HTTPRequest_str(HTTPRequestObject *self) {
    return HTTPRequest_repr(self);
}

static PyMethodDef HTTPRequest_methods[] = {
    {"get", (PyCFunction) HTTPRequest_get, METH_NOARGS,
     "Make a GET request with the instance settings"
    },
    {"post", (PyCFunction) HTTPRequest_post, METH_NOARGS,
     "Make a GET request with the instance settings"
    },
    {NULL}  /* Sentinel */
};

static PyMemberDef HTTPRequest_members[] = {
    {"url", T_OBJECT_EX, offsetof(HTTPRequestObject, request_url), 0,
     "request URI"},
    {"payload", T_OBJECT_EX, offsetof(HTTPRequestObject, request_data), 0,
     "request data"},
    {"headers", T_OBJECT_EX, offsetof(HTTPRequestObject, request_headers), 0,
     "request headers"},
    {"cookies", T_OBJECT_EX, offsetof(HTTPRequestObject, request_cookies), 0,
     "request cookies"},
    {NULL}  /* Sentinel */
};

PyTypeObject HTTPRequestType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "fastrequest.http.HTTPRequest",
    .tp_doc = "HTTP request object",
    .tp_basicsize = sizeof(HTTPRequestObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = HTTPRequest_new,
    .tp_init = (initproc) HTTPRequest_init,
    .tp_dealloc = (destructor) HTTPRequest_dealloc,
    .tp_members = HTTPRequest_members,
    .tp_methods = HTTPRequest_methods,
    .tp_repr = (reprfunc) HTTPRequest_repr,
    .tp_str = (reprfunc) HTTPRequest_str
};