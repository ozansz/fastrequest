#pragma once

#include <stdint.h>

#define STRINGBUFFER_INITIAL_SIZE 32
#define STRINGBUFFER_EXTEND_SIZE 16

typedef struct {
    char *buf;
    int_fast64_t head_index;
    int_fast64_t index;
    int_fast64_t size;
} StringBuffer;

StringBuffer *StringBuffer_Create(void);
void StringBuffer_Free(StringBuffer *buf);
int_fast64_t StringBuffer_Extend(StringBuffer *buf);
int_fast64_t StringBuffer_PushChar(StringBuffer *buf, char ch);
int_fast64_t StringBuffer_PushSequence(StringBuffer *buf, char *seq, size_t size);
char *StringBuffer_GetSequenceRef(StringBuffer *buf, size_t size);
