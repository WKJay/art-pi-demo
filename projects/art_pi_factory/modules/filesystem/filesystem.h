#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

int filesystem_init(void);
int root_filesystem_is_ready(void);
char *fs_read_file(const char *path);
int fs_create_dir(const char *path);
char* fs_normalize_path(char* fullpath);

#endif /*__FILESYSTEM_H*/
