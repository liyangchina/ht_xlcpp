#ifndef B64
#include <stdio.h>
#include <string.h>
#ifdef  __cplusplus
extern "C" {
#endif

extern int  lws_b64_encode_string(const char *in, int in_len,  char *out,  int out_size);

extern int
    lws_b64_decode_string(const char *in, 
                          char *out, 
                          int out_size);

#ifdef  __cplusplus
}
#endif
#endif
