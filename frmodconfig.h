#pragma once

// Because we're going to use this library in a Python extension,
// explicit printf calls are inappropriate.
#define DEBUG 0

#define EC_OUT_OF_MEMORY  "Out of memory (Segmentation fault)"
#define EC_CURL_ERROR     "Generic CURL error"
#define EC_ARG_UAGENT_TOO_BIG "Argument \"user_agent\" is too big"
#define EC_ARG_URL_IS_NULL  "Argument \"url\" is None (NULL) or has zero size"
