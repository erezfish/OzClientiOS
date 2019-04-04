#ifndef OZ_MEMORY_H_
#define OZ_MEMORY_H_

typedef enum {MEM_REC = 0, MEM_NOTIF, MEM_UP, MEM_MNG, MEM_ADAPT, MEM_P2P, MEM_WD, MEM_MODULE_LAST} COM_mem_module_E;

int   COM_memory_init(int small, int medium, int large, COM_mem_module_E module);
void* COM_memory_get(unsigned int size, const char* module, int line);
void* COM_memory_get_round(unsigned int *size);
void  COM_memory_put(void* p_buffer, const char* module, int line);

void* COM_memory_get_aligned(unsigned int size, const char* module, int line);
void  COM_memory_put_aligned(void* p_buffer, const char* module, int line);

void* COM_cJSON_memory_get(unsigned int size);
void COM_cJSON_memory_put(void* p_buffer);

char *COM_memory_strdup(const char *s);
char *COM_memory_strndup(const char *s, size_t n);

#endif /* COM_MEMORY_H_ */
