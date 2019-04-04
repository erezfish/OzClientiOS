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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/* Converts a hex character to its integer value */
static char FromHex(char ch)
{
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
static char ToHex(char code)
{
  static char hex[] = "0123456789ABCDEF";
  return hex[code & 15];
}

unsigned int UrlEncodeOutSize(char *str)
{
	if(str == NULL) return 0;
	return (strlen(str) * 3 + 1);
}
/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char* UrlEncode(char *str)
{
  char *pstr = str;
  char *buf = (char*)malloc(strlen(str) * 3 + 1);
  char *pbuf = buf;

  while (*pstr)
  {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
      *pbuf++ = *pstr;
    else if (*pstr == ' ')
      *pbuf++ = '+';
    else
      *pbuf++ = '%', *pbuf++ = ToHex(*pstr >> 4), *pbuf++ = ToHex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';

  return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char* UrlDecode(char *str)
{
  if(str == NULL ) return NULL;

  char *pstr = str, *buf = (char*)malloc(strlen(str) + 1);
  char *pbuf = buf;

  while (*pstr)
  {
    if (*pstr == '%')
    {
      if (pstr[1] && pstr[2])
      {
        *pbuf++ = FromHex(pstr[1]) << 4 | FromHex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+')
    {
      *pbuf++ = ' ';
    }
    else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';

  return buf;
}

#ifdef __cplusplus
}
#endif
