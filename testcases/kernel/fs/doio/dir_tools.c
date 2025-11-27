#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "dir_tools.h"

int mkdir_path(const char *path, mode_t mode)
{
	int saved_errno;
	char *full;
	size_t len;
	char *buf;
	size_t bi = 0;
	size_t i = 0;

	if (!path) {
		errno = EINVAL;
		return -1;
	}
	if (*path == '\0')
		return 0;

	full = strdup(path);
	if (!full) {
		errno = ENOMEM;
		return -1;
	}

	len = strlen(full);
	if (len > 1) {
		while (len > 1 && full[len - 1] == '/')
			full[--len] = '\0';
	}

	/* special case: root path */
	if (strcmp(full, "/") == 0) {
		free(full);
		return 0;
	}

	buf = malloc(len + 2);
	if (!buf) {
		saved_errno = errno;
		free(full);
		errno = saved_errno ? saved_errno : ENOMEM;
		return -1;
	}

	if (full[0] == '/') {
		buf[bi++] = '/';
		i = 1;
	}

	while (i < len) {
		/* skip duplicated separators */
		while (i < len && full[i] == '/')
			i++;
		/* get segment */
		{
			size_t start = i;

			while (i < len && full[i] != '/')
				i++;
			size_t slen = i - start;

			if (slen == 0)
				continue;
			memcpy(buf + bi, full + start, slen);
			bi += slen;
			buf[bi] = '\0';
		}

		if (mkdir(buf, mode) == -1) {
			if (errno == EEXIST) {
				struct stat st;

				if (stat(buf, &st) == -1) {
					saved_errno = errno;
					free(buf);
					free(full);
					errno = saved_errno;
					return -1;
				}
				if (!S_ISDIR(st.st_mode)) {
					free(buf);
					free(full);
					errno = ENOTDIR;
					return -1;
				}
				/* existing directory, continue */
			} else {
				saved_errno = errno;
				free(buf);
				free(full);
				errno = saved_errno;
				return -1;
			}
		}

		if (i < len)
			buf[bi++] = '/';
	}

	free(buf);
	free(full);
	return 0;
}
