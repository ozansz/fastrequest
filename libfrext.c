#include "libfrext.h"
#include "frmodconfig.h"

#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <curl/curl.h>

#define DEFAULT_UAGENT "Mozilla/5.0 (Windows NT 6.1; WOW64) " \
                       "AppleWebKit/537.36 (KHTML, like Gecko) " \
                       "Chrome/28.0.1500.52 Safari/537.36 OPR/15.0.1147.100"

void __debug(const char *msg)
{
  if (DEBUG) {
    printf("\e[1;94m[DEBUG]\e[0m %s\n", msg);
  }
}

size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  __debug("[CALL] WriteMemoryCallback");

  size_t realsize = size * nmemb;
  ChunkMem *mem = (ChunkMem *)userp;

  __debug("[PRECALL] PyMem_Realloc");
  mem->memory = PyMem_Realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    // Because we're going to use this library in a Python extension,
    // explicit printf calls are inappropriate.
    //printf("[!] Not enough memory (code: %d)\n", OUT_OF_MEMORY);
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

int_fast8_t
FastGet_libcurl(const char *url, const char *user_agent, ChunkMem *mem)
{
  __debug("[CALL] FastGet");

  CURL *curl_handle;
  CURLcode res;

  char uagent[UAGENT_MAX_SIZE];
  char curl_err_string[EXCEPTION_MSG_MAX_SIZE];

  __debug("Checking if arg:url is NULL");
  if (url == NULL || strlen(url) == 0) {
    return -ARG_URL_IS_NULL;
  }

  bzero((char *) &uagent, UAGENT_MAX_SIZE);

  __debug("Checking if arg:user_agent is NULL or empty");
  if ((user_agent == NULL) || (user_agent == "") || (strlen(user_agent) == 0)) {
    __debug("arg:user_agent is NULL or empty, using DEFAULT_UAGENT instead");
    strcpy(uagent, DEFAULT_UAGENT);
  }
  else {
    __debug("Checking if arg:user_agent is too big");
    if (strlen(user_agent) >= UAGENT_MAX_SIZE) {
      return -ARG_UAGENT_IS_TOO_BIG;
    }
    __debug("arg:user_agent is not NULL nor empty");
    strcpy(uagent, user_agent);
  }

  __debug("CURL init");
  curl_global_init(CURL_GLOBAL_ALL);

  curl_handle = curl_easy_init();

  __debug("Setting CURL opts");
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) mem);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, uagent);

  __debug("CURL hnd process is starting");
  res = curl_easy_perform(curl_handle);

  __debug("Checking if CURL has returned any error");
  if(res != CURLE_OK) {
    sprintf(curl_err_string, "Generic CURL error: %s", curl_easy_strerror(res));
    __debug("[PRECALL] PyErr_SetString");
    PyErr_SetString(PyExc_RuntimeError, curl_err_string);
    return -CURL_ERROR;
  }

  __debug("CURL cleanup");
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  return RETURN_SUCCESS;
}

int_fast32_t
FastGet_libcurl_format_exception(char *str, int_fast8_t err_code)
{
  __debug("[CALL] FastGet_libcurl_format_exception");

  char err_str[ERR_STR_MAX_SIZE] = {0};

  switch (err_code) {
    case -OUT_OF_MEMORY:
      strcpy(err_str, EC_OUT_OF_MEMORY);
      break;
    case -CURL_ERROR:
      strcpy(err_str, EC_CURL_ERROR);
      break;
    case -ARG_URL_IS_NULL:
      strcpy(err_str, EC_ARG_URL_IS_NULL);
      break;
    case -ARG_UAGENT_IS_TOO_BIG:
      strcpy(err_str, EC_ARG_UAGENT_TOO_BIG);
      break;
    default:
      strcpy(err_str, "Unknown error");
  }

  __debug("[PRECALL] sprintf");
  return (int_fast32_t) sprintf(str, \
    "FastGet_libcurl returned with code: %d\n%s", err_code, err_str);
}
