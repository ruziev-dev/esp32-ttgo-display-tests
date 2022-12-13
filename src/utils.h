#include <Arduino.h>

#ifndef utils_h
#define utils_h
#endif

#ifdef ESP32
#include "mbedtls/md.h"
#else
#include "Hash.h"
#endif

String sha1(String payloadStr);
