/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#include "first.h"

/* test case and code based on https://github.com/curl/curl/issues/3927 */

#include "memdebug.h"

static int dload_progress_cb(void *a, curl_off_t b, curl_off_t c,
                             curl_off_t d, curl_off_t e)
{
  (void)a;
  (void)b;
  (void)c;
  (void)d;
  (void)e;
  return 0;
}

static size_t t1523_write_cb(char *d, size_t n, size_t l, void *p)
{
  /* take care of the data here, ignored in this example */
  (void)d;
  (void)p;
  return n*l;
}

static CURLcode run(CURL *hnd, long limit, long time)
{
  curl_easy_setopt(hnd, CURLOPT_LOW_SPEED_LIMIT, limit);
  curl_easy_setopt(hnd, CURLOPT_LOW_SPEED_TIME, time);
  return curl_easy_perform(hnd);
}

static CURLcode test_lib1523(const char *URL)
{
  CURLcode ret;
  CURL *hnd;
  char buffer[CURL_ERROR_SIZE];
  curl_global_init(CURL_GLOBAL_ALL);
  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_URL, URL);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, t1523_write_cb);
  curl_easy_setopt(hnd, CURLOPT_ERRORBUFFER, buffer);
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, dload_progress_cb);

  ret = run(hnd, 1, 2);
  if(ret)
    curl_mfprintf(stderr, "error (%d) %s\n", ret, buffer);

  ret = run(hnd, 12000, 1);
  if(ret != CURLE_OPERATION_TIMEDOUT)
    curl_mfprintf(stderr, "error (%d) %s\n", ret, buffer);
  else
    ret = CURLE_OK;

  curl_easy_cleanup(hnd);
  curl_global_cleanup();

  return ret;
}
