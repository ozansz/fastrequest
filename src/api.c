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

StringBuffer *FastRequestAPI_LibcurlHttpGet(char *url) {
    char err_string[FR_GENERIC_ERRSTR_SIZE];

    CURL *curl_handle;
    CURLcode res;

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
    curl_global_cleanup();

    FastRequest_FuncDebug("FastRequestAPI_LibcurlHttpGet", "<== Function exit");

    return strbuf;
}

size_t _FastRequestAPI_LibcurlWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    StringBuffer *buf = (StringBuffer *) userdata;
    int_fast64_t strbuf_push_ret;

    FastRequest_FuncDebug("_FastRequestAPI_LibcurlWriteCallback", "==> Function enter");

    if ((strbuf_push_ret = StringBuffer_PushSequence(buf, ptr, nmemb)) < 0)
        curl_write_callback_stringbuffer_err = strbuf_push_ret;

    FastRequest_FuncDebug("_FastRequestAPI_LibcurlWriteCallback", "<== Function exit");

    return size * nmemb;
}