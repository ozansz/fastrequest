#pragma once

#include <stdlib.h>
#include <stdint.h>

#define ERR_STR_MAX_SIZE 1024
#define EXCEPTION_MSG_MAX_SIZE 2048
#define UAGENT_MAX_SIZE 255
#define URL_MAX_SIZE 255
#define SOCK_GET_MSG_MAX_SIZE 2048

#define CHUNK_SIZE 1024

#define DEFAULT_HTTP_PORT 80
#define DEFAULT_UAGENT "Mozilla/5.0 (Windows NT 6.1; WOW64) " \
                       "AppleWebKit/537.36 (KHTML, like Gecko) " \
                       "Chrome/28.0.1500.52 Safari/537.36 OPR/15.0.1147.100"

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
  ARG_UAGENT_IS_TOO_BIG,
  SYSCALL_GETHOSTBYNAME_FAILED,
  SYSCALL_SETSOCKOPT_FAILED,
  SYSCALL_CONNECT_FAILED,
  SYSCALL_WRITE_FAILED
} LIBFREXT_FUNC_RETURN_CODE;


typedef struct {
  char *memory;
  size_t size;
} ChunkMem;

void __debug(const char *msg);

size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

int_fast8_t
FastGet_libcurl(const char *url, const char *user_agent, ChunkMem *mem);

int_fast32_t
FastGet_libcurl_format_exception(char *str, int_fast8_t err_code);

int_fast8_t FastReq_SockConnect(int_fast16_t *sock, const char *host);

size_t FastReq_FillChunk(void *contents, size_t size, void *chunk);

int_fast32_t FastGet_sock(const char *host, const char *path, ChunkMem *mem);


#ifdef __cplusplus
}
#endif
