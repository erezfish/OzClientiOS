#ifndef BASE64_H_
#define BASE64_H_

#ifdef __cplusplus
extern "C" {
#endif

void base64_cleanup();

char *base64_encode(const unsigned char *data,
                    unsigned int input_length,
                    unsigned int *output_length);

int base64encode(const void* data_buf, unsigned int dataLength, char* result, unsigned int resultSize);
char *create_basic_authrization(char *input);

unsigned char *base64_decode(const char *data,
							 unsigned int input_length,
							 unsigned int *output_length);

#ifdef __cplusplus
}
#endif
#endif /* BASE64_H_ */
