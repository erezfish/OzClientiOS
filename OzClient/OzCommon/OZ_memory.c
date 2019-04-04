/* Description */

/* Include */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "OZ_memory.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Define */
/* External reference */
/* Enum & type declarations */


/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
void* COM_memory_get(unsigned int size, const char* module, int line)
{
	void* mem;
	mem = malloc(size);
	if(mem == NULL)
	{
		//COM_log_printf(__FILE__, COM_LOG_ERR, __LINE__, "COM_memory fail malloc %d bytes",size);
		return NULL;
	}
	memset(mem, 0, size);

	return mem;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
void COM_memory_put(void* p_buffer, const char* module, int line)
{
	if(p_buffer != NULL){
		free(p_buffer);
	}
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
#define COM_ALIGN 64

void* COM_memory_get_aligned(unsigned int size, const char* module, int line)
{
    void *mem = malloc(size+COM_ALIGN+sizeof(void*));
    if(mem == NULL)
    {
    	//COM_log_printf(__FILE__, COM_LOG_ERR, __LINE__, NULL);
    	return NULL;
    }

    void **ptr = (void**)((long)(mem+COM_ALIGN+sizeof(void*)) & ~(COM_ALIGN-1));
    ptr[-1] = mem;

    memset(mem, 0, size);
    //printf(" %d MALLOC 0x%x %d  -  %s %d\n", malloc_free, (unsigned int)mem, size, module, line);

    return ptr;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
void COM_memory_put_aligned(void *p_buffer, const char* module, int line)
{

	if(p_buffer != NULL){
		free(((void**)p_buffer)[-1]);
	}
}

/* for P2P module */
/***********************************************************
  Description: Called only by cJSON module

  Parameters :

  Return     :
 **********************************************************/
void* COM_cJSON_memory_get(unsigned int size)
{
   return (COM_memory_get(size, __FILE__, __LINE__));
}

/***********************************************************
  Description: Called only by cJSON module

  Parameters :

  Return     :
 **********************************************************/
void COM_cJSON_memory_put(void* p_buffer)
{
    COM_memory_put(p_buffer,  __FILE__, __LINE__);
}


char *COM_memory_strdup(const char *s)
{
	void *p_new = COM_memory_get((strlen(s) + 1),__FILE__,__LINE__);

	if (p_new == NULL)
	{
		return NULL;
	}

	return strcpy(p_new, s);
}

char *COM_memory_strndup(const char *s, size_t n)
{
	int len = strnlen(s, n);
	char *p_new = COM_memory_get((len+1),__FILE__,__LINE__);

	if (p_new == NULL)
	{
		return NULL;
	}

	p_new[len] = '\0';

	return strncpy(p_new, s, len);
}

#ifdef __cplusplus
}
#endif


