/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/http/http.h>
#include <mupnp/util/log.h>

static const char MUPNP_HTTP_REASON_PHRASE_100[] = { MUPNP_HTTP_REASON_100 };
static const char MUPNP_HTTP_REASON_PHRASE_101[] = { MUPNP_HTTP_REASON_101 };
static const char MUPNP_HTTP_REASON_PHRASE_200[] = { MUPNP_HTTP_REASON_200 };
static const char MUPNP_HTTP_REASON_PHRASE_201[] = { MUPNP_HTTP_REASON_201 };
static const char MUPNP_HTTP_REASON_PHRASE_202[] = { MUPNP_HTTP_REASON_202 };
static const char MUPNP_HTTP_REASON_PHRASE_203[] = { MUPNP_HTTP_REASON_203 };
static const char MUPNP_HTTP_REASON_PHRASE_204[] = { MUPNP_HTTP_REASON_204 };
static const char MUPNP_HTTP_REASON_PHRASE_205[] = { MUPNP_HTTP_REASON_205 };
static const char MUPNP_HTTP_REASON_PHRASE_206[] = { MUPNP_HTTP_REASON_206 };
static const char MUPNP_HTTP_REASON_PHRASE_300[] = { MUPNP_HTTP_REASON_300 };
static const char MUPNP_HTTP_REASON_PHRASE_301[] = { MUPNP_HTTP_REASON_301 };
static const char MUPNP_HTTP_REASON_PHRASE_302[] = { MUPNP_HTTP_REASON_302 };
static const char MUPNP_HTTP_REASON_PHRASE_303[] = { MUPNP_HTTP_REASON_303 };
static const char MUPNP_HTTP_REASON_PHRASE_304[] = { MUPNP_HTTP_REASON_304 };
static const char MUPNP_HTTP_REASON_PHRASE_305[] = { MUPNP_HTTP_REASON_305 };
static const char MUPNP_HTTP_REASON_PHRASE_307[] = { MUPNP_HTTP_REASON_307 };
static const char MUPNP_HTTP_REASON_PHRASE_400[] = { MUPNP_HTTP_REASON_400 };
static const char MUPNP_HTTP_REASON_PHRASE_401[] = { MUPNP_HTTP_REASON_401 };
static const char MUPNP_HTTP_REASON_PHRASE_402[] = { MUPNP_HTTP_REASON_402 };
static const char MUPNP_HTTP_REASON_PHRASE_403[] = { MUPNP_HTTP_REASON_403 };
static const char MUPNP_HTTP_REASON_PHRASE_404[] = { MUPNP_HTTP_REASON_404 };
static const char MUPNP_HTTP_REASON_PHRASE_405[] = { MUPNP_HTTP_REASON_405 };
static const char MUPNP_HTTP_REASON_PHRASE_406[] = { MUPNP_HTTP_REASON_406 };
static const char MUPNP_HTTP_REASON_PHRASE_407[] = { MUPNP_HTTP_REASON_407 };
static const char MUPNP_HTTP_REASON_PHRASE_408[] = { MUPNP_HTTP_REASON_408 };
static const char MUPNP_HTTP_REASON_PHRASE_409[] = { MUPNP_HTTP_REASON_409 };
static const char MUPNP_HTTP_REASON_PHRASE_410[] = { MUPNP_HTTP_REASON_410 };
static const char MUPNP_HTTP_REASON_PHRASE_411[] = { MUPNP_HTTP_REASON_411 };
static const char MUPNP_HTTP_REASON_PHRASE_412[] = { MUPNP_HTTP_REASON_412 };
static const char MUPNP_HTTP_REASON_PHRASE_413[] = { MUPNP_HTTP_REASON_413 };
static const char MUPNP_HTTP_REASON_PHRASE_414[] = { MUPNP_HTTP_REASON_414 };
static const char MUPNP_HTTP_REASON_PHRASE_415[] = { MUPNP_HTTP_REASON_415 };
static const char MUPNP_HTTP_REASON_PHRASE_416[] = { MUPNP_HTTP_REASON_416 };
static const char MUPNP_HTTP_REASON_PHRASE_417[] = { MUPNP_HTTP_REASON_417 };
static const char MUPNP_HTTP_REASON_PHRASE_500[] = { MUPNP_HTTP_REASON_500 };
static const char MUPNP_HTTP_REASON_PHRASE_501[] = { MUPNP_HTTP_REASON_501 };
static const char MUPNP_HTTP_REASON_PHRASE_502[] = { MUPNP_HTTP_REASON_502 };
static const char MUPNP_HTTP_REASON_PHRASE_503[] = { MUPNP_HTTP_REASON_503 };
static const char MUPNP_HTTP_REASON_PHRASE_504[] = { MUPNP_HTTP_REASON_504 };
static const char MUPNP_HTTP_REASON_PHRASE_505[] = { MUPNP_HTTP_REASON_505 };
static const char MUPNP_HTTP_REASON_PHRASE_UNKNOWN[] = { "Unknown" };

/**
 * Convert an HTTP status code to a reason phrase (ex. 200 -> "OK")
 *
 * @param code The HTTP status code to convert
 * @return The reason phrase (do not modify the pointer's contents)
 */
const char* mupnp_http_statuscode2reasonphrase(const int code)
{
  mupnp_log_debug_l4("Entering...\n");

  switch (code) {
  case 100:
    return MUPNP_HTTP_REASON_PHRASE_100;
  case 101:
    return MUPNP_HTTP_REASON_PHRASE_101;
  case 200:
    return MUPNP_HTTP_REASON_PHRASE_200;
  case 201:
    return MUPNP_HTTP_REASON_PHRASE_201;
  case 202:
    return MUPNP_HTTP_REASON_PHRASE_202;
  case 203:
    return MUPNP_HTTP_REASON_PHRASE_203;
  case 204:
    return MUPNP_HTTP_REASON_PHRASE_204;
  case 205:
    return MUPNP_HTTP_REASON_PHRASE_205;
  case 206:
    return MUPNP_HTTP_REASON_PHRASE_206;
  case 300:
    return MUPNP_HTTP_REASON_PHRASE_300;
  case 301:
    return MUPNP_HTTP_REASON_PHRASE_301;
  case 302:
    return MUPNP_HTTP_REASON_PHRASE_302;
  case 303:
    return MUPNP_HTTP_REASON_PHRASE_303;
  case 304:
    return MUPNP_HTTP_REASON_PHRASE_304;
  case 305:
    return MUPNP_HTTP_REASON_PHRASE_305;
  case 307:
    return MUPNP_HTTP_REASON_PHRASE_307;
  case 400:
    return MUPNP_HTTP_REASON_PHRASE_400;
  case 401:
    return MUPNP_HTTP_REASON_PHRASE_401;
  case 402:
    return MUPNP_HTTP_REASON_PHRASE_402;
  case 403:
    return MUPNP_HTTP_REASON_PHRASE_403;
  case 404:
    return MUPNP_HTTP_REASON_PHRASE_404;
  case 405:
    return MUPNP_HTTP_REASON_PHRASE_405;
  case 406:
    return MUPNP_HTTP_REASON_PHRASE_406;
  case 407:
    return MUPNP_HTTP_REASON_PHRASE_407;
  case 408:
    return MUPNP_HTTP_REASON_PHRASE_408;
  case 409:
    return MUPNP_HTTP_REASON_PHRASE_409;
  case 410:
    return MUPNP_HTTP_REASON_PHRASE_410;
  case 411:
    return MUPNP_HTTP_REASON_PHRASE_411;
  case 412:
    return MUPNP_HTTP_REASON_PHRASE_412;
  case 413:
    return MUPNP_HTTP_REASON_PHRASE_413;
  case 414:
    return MUPNP_HTTP_REASON_PHRASE_414;
  case 415:
    return MUPNP_HTTP_REASON_PHRASE_415;
  case 416:
    return MUPNP_HTTP_REASON_PHRASE_416;
  case 417:
    return MUPNP_HTTP_REASON_PHRASE_417;
  case 500:
    return MUPNP_HTTP_REASON_PHRASE_500;
  case 501:
    return MUPNP_HTTP_REASON_PHRASE_501;
  case 502:
    return MUPNP_HTTP_REASON_PHRASE_502;
  case 503:
    return MUPNP_HTTP_REASON_PHRASE_503;
  case 504:
    return MUPNP_HTTP_REASON_PHRASE_504;
  case 505:
    return MUPNP_HTTP_REASON_PHRASE_505;
  default:
    return MUPNP_HTTP_REASON_PHRASE_UNKNOWN;
  }

  mupnp_log_debug_l4("Leaving...\n");
}
