#pragma once

#include <stdlib.h>
#include <stdint.h>

#define ERR_STR_MAX_SIZE 1024
#define EXCEPTION_MSG_MAX_SIZE 2048
#define UAGENT_MAX_SIZE 255
#define URL_MAX_SIZE 255

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  RETURN_SUCCESS = 0,
  GENERIC_ERROR = 1,
  FATAL_ERROR,
  OUT_OF_MEMORY,
  CURL_ERROR,
  ARG_URL_IS_NULL,
  ARG_UAGENT_IS_TOO_BIG
} FAST_GET_LIBCURL_RETURN_CODE;

typedef struct {
  char *memory;
  size_t size;
} ChunkMem;

size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

int_fast8_t
FastGet_libcurl(const char *url, const char *user_agent, ChunkMem *mem);

int_fast32_t
FastGet_libcurl_format_exception(char *str, int_fast8_t err_code);

void __debug(const char *msg);

#ifdef __cplusplus
}
#endif
