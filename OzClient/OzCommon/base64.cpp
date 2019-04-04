#ifdef WIN32
#define OSN_API __stdcall
#ifdef OSNCLIENT_EXPORTS
#define OSN_DECLSPEC __declspec(dllexport)
#else
#define OSN_DECLSPEC __declspec(dllimport)
#endif
#else
#define OSN_API
#define OSN_DECLSPEC
#endif
#ifdef __cplusplus
#define OSN_EXTERN extern "C" OSN_DECLSPEC
#else
#define OSN_EXTERN extern OSN_DECLSPEC
#endif


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef NULL
	#define NULL ( (void *) 0)
#endif


#if 0
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

static char *decoding_table = NULL;
//static int mod_table[] = {0, 2, 1};
//
static void build_decoding_table()
{
    int i;

	decoding_table = malloc(256);

    for (i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}
//
void base64_cleanup()
{
    free(decoding_table);
}
#endif
//
int base64encode(const void* data_buf, unsigned int dataLength, char* result, unsigned int resultSize)
{
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   unsigned int resultIndex = 0;
   unsigned int x;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   /* increment over the length of the string, three characters at a time */
   for (x = 0; x < dataLength; x += 3)
   {
      /* these three 8-bit (ASCII) characters become one 24-bit number */
      n = data[x] << 16;

      if((x+1) < dataLength)
         n += data[x+1] << 8;

      if((x+2) < dataLength)
         n += data[x+2];

      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;
      /*
       * if we have one byte available, then its encoding is spread
       * out over two characters
       */
      if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n1];
      /*
       * if we have only two bytes available, then their encoding is
       * spread out over three chars
       */
      if((x+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n2];
      }

      /*
       * if we have all three bytes available, then their encoding is spread
       * out over four characters
       */
      if((x+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n3];
      }
   }
   /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
   if (padCount > 0)
   {
      for (; padCount < 3; padCount++)
      {
         if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
         result[resultIndex++] = '=';
      }
   }
   if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
   result[resultIndex] = 0;

   return resultIndex;   /* indicate success */
}

char *create_basic_authrization(char *input) {
    int len = strlen(input), padding = (len % 3)?3 - (len % 3):0, n, xlen = 0;
    size_t bitlen = len * 8;
    uint8_t *x = (uint8_t*)malloc(((int)(bitlen / 24) * 4) + ((padding)?4:0)), i, o;
    for (n = 0; n < len; n++) {
        switch (n % 3) {
            case 0:
                i = (uint8_t)(input[n] >> 2);
                x[xlen++] = i;
                break;
            case 1:
                i = (uint8_t)((input[n-1] & 3) << 4);
                o = (uint8_t)((input[n] & 240) >> 4);
                x[xlen++] = i | o;
                break;
            case 2:
                i = (uint8_t)((input[n-1] & 15) << 2);
                o = (uint8_t)((input[n] & 192) >> 6);
                x[xlen++] = i | o;
                i = (uint8_t)(input[n] & 63);
                x[xlen++] = i;
                break;
        }
        if ((len - n) == ((padding==2)?2:3) && padding > 0) {
            n++;
            break;
        }
    }
    if (padding == 2) {
        i = (uint8_t)(input[n] >> 2);
        x[xlen++] = i;
        i = (uint8_t)((input[n] & 3) << 4);
        x[xlen++] = i;
    } else if (padding == 1) {
        i = (uint8_t)(input[n] >> 2);
        x[xlen++] = i;
        i = (uint8_t)((input[n] & 3) << 4);
        o = (uint8_t)((input[n+1] & 240) >> 4);
        x[xlen++] = i | o;
        i = (uint8_t)((input[n+1] & 15) << 2);
        x[xlen++] = i;
    }
    const char *index = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *returnString = (char*)malloc((int)((bitlen / 24) + padding) * 4 + 1);
    for (n = 0; n < ((int)(bitlen / 24) * 4) + ((padding)?4:0) - padding; n++) {
        returnString[n] = index[x[n]];
    }
    free(x);
    for (;padding > 0; padding--) {
        returnString[n++] = '=';
    }
    returnString[n] = '\0';
    return returnString;
}
//
#if 0
unsigned char *base64_decode(const char *data,
							 unsigned int input_length,
							 unsigned int *output_length)
{
	int i, j;
	uint32_t sextet_a, sextet_b, sextet_c, sextet_d;

	if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (i = 0, j = 0; i < input_length; )
    {

        sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}
#endif

#ifdef __cplusplus
}
#endif

