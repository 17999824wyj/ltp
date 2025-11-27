#ifndef _DIR_TOOLS_H_
#define _DIR_TOOLS_H_

#include <sys/types.h>

/*
 * Create a directory path with semantics equivalent to 'mkdir -p'.
 * Returns 0 on success, -1 on failure and preserves errno.
 *
 * Notes:
 * - Does not use chdir(), no global state side effects.
 * - Supports absolute and relative paths.
 * - Trims redundant trailing slashes (except single "/").
 * - When a component exists, ensures it is a directory; otherwise fails.
 */
int mkdir_path(const char *path, mode_t mode);

#endif /* _DIR_TOOLS_H_ */