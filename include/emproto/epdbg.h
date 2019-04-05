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

/*
 *    EMPOWER AGENT PROTOCOLS DEBUGGING
 */

#ifndef __EMAGE_PROTOCOLS_DEBUG_H
#define __EMAGE_PROTOCOLS_DEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Utilities included only in the case the software has been built with
 * Debugging profile ON.
 */
#ifdef EBUG
extern volatile int ep_dbg_ready;

/* Initialize the debugging subsystem */
void ep_dbg_init();
/* Debug a message into the logging file */
void ep_dbg(char * prologue, char * buf, int size);
/* Log something into the debugging subsystem */
void ep_dbg_log(char * msg, ...);

#define ep_dbg_dump(p, m, s)	ep_dbg(p, m, s)
#else
//#define ep_dbg_prologue()	/* ... into nothing */
#define ep_dbg_log(m, ...)      /* ... into nothing */
#define ep_dbg_dump(p, m, s)    /* ... into nothing */
#endif

/* Formatting spacing for nested message categories */

#define EP_DBG_0 ""
#define EP_DBG_1 "    "
#define EP_DBG_2 "        "
#define EP_DBG_3 "            "

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PROTOCOLS_DEBUG_H */
