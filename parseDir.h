#include <stdio.h>

#ifndef _PARSEDIR_H_
#define _PARSEDIR_H_

typedef struct fileCtx {
    char *fn;
    FILE *fp;
} fileCtx_t;

fileCtx_t *open_lock_oldest_unlocked_file ( const char *folder );
char *get_file_content ( fileCtx_t *fileCtx );
int close_unlock_file ( fileCtx_t *fileCtx );
int close_unlink_file ( fileCtx_t *fileCtx );

#endif
