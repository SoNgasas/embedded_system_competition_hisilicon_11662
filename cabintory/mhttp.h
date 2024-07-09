#ifndef __M_HTTP_H__
#define __M_HTTP_H__

#include <hi_types_base.h>
void extract_date(const char *headers, char *date_buf, size_t buf_size);
unsigned int http_clienti_get(void);
void time_get_task(void);

extern char date_buf[];

#endif