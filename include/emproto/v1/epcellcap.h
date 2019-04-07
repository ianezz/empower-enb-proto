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

/*    CELL CAPABILITIES MESSAGE
 *
 * This message provides the request/reply for/from cell capabilities.
 */

#ifndef __EMAGE_CELL_CAPABILITIES_H
#define __EMAGE_CELL_CAPABILITIES_H

#include <stdint.h>

#include "eppri.h"
#include "epTLV.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Capabilities for a single Cell */
typedef enum __ep_cell_capabilities_types {
	/* Can't do anything except present himself */
	EP_CCAP_NOTHING      = 0,
	/* Cell support UE reporting */
	EP_CCAP_UE_REPORT    = 1,
	/* Cell support UE measurements */
	EP_CCAP_UE_MEASURE   = 2,
	/* Cell supports measurements over its mechanism */
	EP_CCAP_CELL_MEASURE = 4,
	/* Cell can perform X2 handover */
	EP_CCAP_X2_HANDOVER  = 8,
} ep_ccap_type;

/*
 *
 * Cell capabilities details
 *
 */

typedef struct __ep_cell_capabilities_details {
	uint16_t pci;       /* Physical cell id */
	uint32_t feat;      /* Cell features */
	uint16_t DL_earfcn; /* Frequency at which the cell operates in DL */
	uint8_t  DL_prbs;   /* Physical Resource Block available in the DL */
	uint16_t UL_earfcn; /* Frequency at which the cell operates in UL */
	uint8_t  UL_prbs;   /* Physical Resource Block available in the UL */
	uint16_t max_ues;   /* Maximum number of supported UEs */
}__attribute__((packed)) ep_ccap_det;

/* TLV-style descriptor of Cell Capabilities information */
typedef struct __ep_ccap_TLV {
	ep_TLV      header;  /* Header of the TLV token */
	ep_ccap_det body;    /* Body of the TLV token */
} __attribute__((packed)) ep_ccap_TLV;

/*
 *
 * Cell capabilities request
 *
 */

typedef struct __ep_cell_capabilities_request {
	uint8_t dummy;
}__attribute__((packed)) ep_ccap_req;

/******************************************************************************
 * Opaque structures for message formatting/parsing                           *
 ******************************************************************************/

typedef struct __ep_cell_details {
	uint16_t pci;
	uint32_t feat;
	uint16_t DL_earfcn;
	uint16_t UL_earfcn;
	uint8_t  DL_prbs;
	uint8_t  UL_prbs;
	uint16_t max_ues;
} ep_cell_det;
#if 0
/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/* Format a cell capabilities negative reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ccap_rep_fail(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id);

/* Format a cell capabilities reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ccap_rep(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id,
	ep_cell_det * cell);

/* Parse a cell capabilities reply looking for the desired fields */
int epp_single_ccap_rep(
	char *        buf,
	unsigned int  size,
	ep_cell_det * cell);

/* Format a cell capabilities request.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ccap_req(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id);

/* Parse a cell capabilities request for the desired fields */
int epp_single_ccap_req(char * buf, unsigned int size);
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_CELL_CAPABILITIES_H */
