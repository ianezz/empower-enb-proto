/* Copyright (c) 2019 FBK
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include <emproto.h>

volatile int ep_dbg_ready = 0;
int          ep_dbg_std   = 0;
FILE *       ep_dbg_fd    = 0;

/* The debugging module tries first to open the standard FD; if it's not
 * possible, it then dump everything on the standard output.
 */
void ep_dbg_init()
{
        char lp[256] = {0};

        /* Unique log per process */
        sprintf(lp, "./emproto.%d.log", getpid());
	ep_dbg_fd = fopen(lp, "w");

	if (!ep_dbg_fd) {
		ep_dbg_std = 1;
	}

	ep_dbg_ready = 1;
}

void ep_dbg(char * prologue, char * buf, int size)
{
	int i;

	if(!ep_dbg_ready) {
		ep_dbg_init();
	}

	ep_dbg_log(prologue);

	for(i = 0; i < size; i++) {
		ep_dbg_log("%02x ", (unsigned char)buf[i]);
	}

	ep_dbg_log("\n");
}

/* Just dump the messages somewhere...
 */
void ep_dbg_log(char * msg, ...)
{
	FILE *  fd;
	va_list vl;

	va_start(vl, msg);

	if (ep_dbg_std) {
		fd = stdout;
	} else {
		fd = ep_dbg_fd;
	}

	/* Prints and flush the file; slow, but accurate in case of error */
	vfprintf(fd, msg, vl);
	fflush(fd);

	va_end(vl);

	return;
}
