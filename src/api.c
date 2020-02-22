#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <curl/curl.h>

#include "stringbuffer.h"
#include "api.h"
#include "utils.h"
#include "errors.h"

static int_fast64_t curl_write_callback_stringbuffer_err = 0;
static int_fast64_t curl_read_callback_stringbuffer_err = 0;

size_t _FastRequestAPI_LibcurlWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    StringBuffer *buf = (StringBuffer *) userdata;
    int_fast64_t strbuf_push_ret;

    //FastRequest_FuncDebug("_FastRequestAPI_LibcurlWriteCallback", "==> Function enter");

    if ((strbuf_push_ret = StringBuffer_PushSequence(buf, ptr, nmemb)) < 0)
        curl_write_callback_stringbuffer_err = strbuf_push_ret;

    //FastRequest_FuncDebug("_FastRequestAPI_LibcurlWriteCallback", "<== Function exit");

    return size * nmemb;
}

size_t _FastRequestAPI_LibcurlReadCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    StringBuffer *buf = (StringBuffer *) userdata;
    int_fast64_t size_read;
    char *pptr;

    pptr = StringBuffer_GetSequenceRef(buf, size * nmemb, &size_read);

    if (pptr == NULL) {
        curl_read_callback_stringbuffer_err = -FR_ERR_MEMORY;
        return 0;
    }

    memcpy(ptr, pptr, size_read);

    return size_read;
}

StringBuffer *FastRequestAPI_LibcurlHttpGet(char *url, PyObject *headers) {
    char err_string[FR_GENERIC_ERRSTR_SIZE];

    CURL *curl_handle;
    CURLcode res;

    struct curl_slist *header_chunk = NULL;

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpGet", "==> Function enter");

    StringBuffer *strbuf = StringBuffer_Create();
    
    if (url == NULL || strlen(url) == 0) {
        snprintf(err_string, FR_GENERIC_ERRSTR_SIZE - 1, "Argument 'url' is None or have zero length.");
        PyErr_SetString(PyExc_RuntimeError, err_string);
        return NULL;
    }

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpGet", "Initializing CURL");

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpGet", "Setting CURL opts");

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, _FastRequestAPI_LibcurlWriteCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) strbuf);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, FR_LIBCURL_DEFAULT_UAGENT);

    if (headers != NULL) {
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        
        char hdr[FR_LIBCURL_HEADER_ITEM_SIZE];

        FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpGet", "<ARG> Given headers");

        while (PyDict_Next(headers, &pos, &key, &value)) {
            if (value == Py_None)
                snprintf(hdr, FR_LIBCURL_HEADER_ITEM_SIZE, "%s:", PyUnicode_AsUTF8(key));
            else
                snprintf(hdr, FR_LIBCURL_HEADER_ITEM_SIZE, "%s: %s", PyUnicode_AsUTF8(key), PyUnicode_AsUTF8(value));

            FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpGet", hdr);

            header_chunk = curl_slist_append(header_chunk, hdr);
        }

        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, header_chunk);

        // There is no need to call Py_DECREF for key and value as they are borrowed references.
    } else {
        FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpGet", "<ARG> Headers null");
    }

    curl_write_callback_stringbuffer_err = 0;
    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        snprintf(err_string, FR_GENERIC_ERRSTR_SIZE - 1, "Generic CURL error: %s", curl_easy_strerror(res));
        PyErr_SetString(PyExc_RuntimeError, err_string);
        return NULL;
    }

    if (curl_write_callback_stringbuffer_err < 0) {
        snprintf(err_string, FR_GENERIC_ERRSTR_SIZE - 1, "String buffer error in CURL write callback (%lld)", curl_write_callback_stringbuffer_err);
        PyErr_SetString(PyExc_RuntimeError, err_string);
        return NULL;
    }

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpGet", "Cleaning up CURL context");

    curl_easy_cleanup(curl_handle);

    // Cleanup custom headers
    if (header_chunk)
        curl_slist_free_all(header_chunk);

    curl_global_cleanup();

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpGet", "<== Function exit");

    return strbuf;
}

StringBuffer *FastRequestAPI_LibcurlHttpPost(char *url, const char *payload, PyObject *headers) {
    char err_string[FR_GENERIC_ERRSTR_SIZE];

    CURL *curl_handle;
    CURLcode res;

    struct curl_slist *header_chunk = NULL;

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", "==> Function enter");
    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", "==== PAYLOAD ====");
    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", payload);
 
    StringBuffer *strbuf = StringBuffer_Create();
    StringBuffer *readbuf = StringBuffer_FromString(payload);

    StringBuffer_Dump(readbuf);
    
    if (url == NULL || strlen(url) == 0) {
        snprintf(err_string, FR_GENERIC_ERRSTR_SIZE - 1, "Argument 'url' is None or have zero length.");
        PyErr_SetString(PyExc_RuntimeError, err_string);
        return NULL;
    }

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", "Initializing CURL");

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", "Setting CURL opts");

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, _FastRequestAPI_LibcurlWriteCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) strbuf);
    curl_easy_setopt(curl_handle, CURLOPT_READDATA, (void *) readbuf);
    curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, _FastRequestAPI_LibcurlReadCallback);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, readbuf->index + 1);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, FR_LIBCURL_DEFAULT_UAGENT);

    if (headers != NULL) {
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        
        char hdr[FR_LIBCURL_HEADER_ITEM_SIZE];

        FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", "<ARG> Given headers");

        while (PyDict_Next(headers, &pos, &key, &value)) {
            if (value == Py_None)
                snprintf(hdr, FR_LIBCURL_HEADER_ITEM_SIZE, "%s:", PyUnicode_AsUTF8(key));
            else
                snprintf(hdr, FR_LIBCURL_HEADER_ITEM_SIZE, "%s: %s", PyUnicode_AsUTF8(key), PyUnicode_AsUTF8(value));

            FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", hdr);

            header_chunk = curl_slist_append(header_chunk, hdr);
        }

        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, header_chunk);

        // There is no need to call Py_DECREF for key and value as they are borrowed references.
    } else {
        FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", "<ARG> Headers null");
    }

    curl_write_callback_stringbuffer_err = 0;
    curl_read_callback_stringbuffer_err = 0;

    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        snprintf(err_string, FR_GENERIC_ERRSTR_SIZE - 1, "Generic CURL error: %s", curl_easy_strerror(res));
        PyErr_SetString(PyExc_RuntimeError, err_string);
        return NULL;
    }

    if (curl_write_callback_stringbuffer_err < 0) {
        snprintf(err_string, FR_GENERIC_ERRSTR_SIZE - 1, "String buffer error in CURL write callback (%lld)", curl_write_callback_stringbuffer_err);
        PyErr_SetString(PyExc_RuntimeError, err_string);
        return NULL;
    }

    if (curl_read_callback_stringbuffer_err < 0) {
        snprintf(err_string, FR_GENERIC_ERRSTR_SIZE - 1, "String buffer error in CURL read callback (%lld)", curl_read_callback_stringbuffer_err);
        PyErr_SetString(PyExc_RuntimeError, err_string);
        return NULL;
    }

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", "Cleaning up CURL context");

    curl_easy_cleanup(curl_handle);

    // Cleanup custom headers
    if (header_chunk)
        curl_slist_free_all(header_chunk);

    curl_global_cleanup();
    StringBuffer_Free(readbuf);

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpPost", "<== Function exit");

    return strbuf;
}