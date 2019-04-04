#ifndef URLCODE_H_
#define URLCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

unsigned int UrlEncodeOutSize(char *str);
char* UrlEncode(char *str);
char* UrlDecode(char *str);

#ifdef __cplusplus
}
#endif
#endif /* URLCODE_H_ */
