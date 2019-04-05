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

/*    MASTER HEADER
 *
 * Modifying the master header will cause a break in the protocol; in case where
 * it is necessary to add/remove some elements, dispatch a new version. Adding
 * new types does not break the protocol, and allows to maintain backward
 * compatibility.
 */

#ifndef __EMAGE_PROTOCOLS_HEADER_H
#define __EMAGE_PROTOCOLS_HEADER_H

#include <stdint.h>

#include "eppri.h"
#include "eptype.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Size of the header */
#define EP_HEADER_SIZE         sizeof(ep_hdr)

/*
 * Values of the 'flags' field in the header:
 *
 * Bit 1/16: */
#define EP_HDR_FLAG_DIR		1 /* Which bit is dedicated to  */
#define EP_HDR_FLAG_DIR_REQ	0 /* Set to 0 marks a request */
#define EP_HDR_FLAG_DIR_REP	1 /* Set to 1 marks a reply */

typedef struct __ep_header_id {
	enb_id_t  enb_id;        /* Base station identifier */
	cell_id_t cell_id;       /* Physical cell id */
	mod_id_t  mod_id;        /* Module id */
}__attribute__((packed)) ep_hdrid;

typedef struct __ep_header {
	uint8_t  type;
	uint8_t  vers;
	ep_hdrid id;
	uint16_t flags;
	uint32_t seq;            /* Sequence number */
	/* Length of the whole packet (headers + data) */
	uint16_t length;
}__attribute__((packed)) ep_hdr;

/* Format a master header with the desired fields.
 * Returns the size of the message, or a negative error number.
 */
int epf_head(
	char *       buf,
	unsigned int size,
	ep_msg_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint16_t     flags);

/* Parse a master header extracting the valuable fields.
 * Returns EP_SUCCESS, or an error code on failure.
 */
int epp_head(
	char *        buf,
	unsigned int  size,
	ep_msg_type * type,
	enb_id_t *    enb_id,
	cell_id_t *   cell_id,
	mod_id_t *    mod_id,
	uint16_t *    flags);

/* Extracts the type from an Empower message */
ep_msg_type epp_msg_type(char * buf, unsigned int size);

/* Parses the direction of this header, either request or reply */
int         epp_dir(char * buf, unsigned int size);

/* Extracts the sequence number from the message */
uint32_t    epp_seq(char * buf, unsigned int size);

/* Extracts the message length in the header. */
uint16_t    epp_msg_length(char * buf, unsigned int size);

/* Inject a sequence number in the header. */
int         epf_seq(char * buf, unsigned int size, uint32_t seq);

/* Inject the message length in the header. */
int         epf_msg_length(char * buf, unsigned int size, uint16_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PROTOCOLS_HEADER_H */
