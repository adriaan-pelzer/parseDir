#ifndef _PARSEDIR_H_
#define _PARSEDIR_H_

#include <stdio.h>

FILE *open_lock_oldest_unlocked_file ( const char *folder );
int close_unlock_file ( FILE *fd );
int close_unlink_file ( FILE *fd );

#endif
