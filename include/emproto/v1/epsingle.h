/* Copyright (c) 2019 FBK
 * Designed by Roberto Riggio
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

/*    SINGLE-EVENT MESSAGES
 *
 * These types of messages defines an event which occurs only once, with a
 * request/reply couple or even just one interaction. It's a fire-and-forget
 * message style.
 */

#ifndef __EMAGE_PROTOCOLS_SINGLE_H
#define __EMAGE_PROTOCOLS_SINGLE_H

#include <stdint.h>

#include "epop.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * Single-event messages header
 */

typedef uint16_t single_type_t;

typedef struct __ep_single_header {
	single_type_t type;
	uint8_t       op;    /* Operation type, see epop.h */
}__attribute__((packed)) ep_s_hdr;

/* Format a single-event message */
int epf_single(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	ep_op_type   op);

/* Extracts the type from an Empower single message */
ep_act_type epp_single_type(char * buf, unsigned int size);

/* Extracts the operation from an Empower single message */
ep_op_type epp_single_op(char * buf, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PROTOCOLS_SINGLE_H */
