#ifndef SHA256_H_
#define SHA256_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SHA256_OUT_SIZE  64

int SHA256_digest(char* input, char* output);

#ifdef __cplusplus
}
#endif
#endif /* SHA256_H_ */
