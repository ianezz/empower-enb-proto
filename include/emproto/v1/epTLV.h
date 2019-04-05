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
 * Empower Agent Protocols Type-Length-Value common components
 */

#ifndef __EMAGE_TLV_H
#define __EMAGE_TLV_H

#include <stdint.h>

#include "eppri.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* types of valid TLV token for empower protocol */
enum ep_TLV_type {
	/* Invalid TLV token */
	EP_TLV_INVALID             = 0,

	/* A generic report of bunch of RNTIs */
	EP_TLV_RNTI_REPORT         = 0x0001,

	/*
	 * Type 1 reserved to cell
	 */

	/* Token contains a cell capabilities information */
	EP_TLV_CELL_CAP            = 0x0100,
	/* Token contains a cell PRBS measurement request */
	EP_TLV_CELL_PRB_REQ        = 0x0101,
	/* Token contains a cell PRBS measurement report */
	EP_TLV_CELL_PRB_REPORT     = 0x0102,

	/*
	 * Type 2 reserved to eNB
	 */

	/*
	 * Type 3 reserved to Handover
	 */

	/*
	 * Type 4 reserved to Reports
	 */

	/*
	 * Type 5 reserved to RAN
	 */

	/* Token contains RAN MAC Slices scheduler information */
	EP_TLV_RAN_MAC_SCHED       = 0x0500,
	/* Token contains RAN Slice specific MAC resources information */
	EP_TLV_RAN_SLICE_MAC_RES   = 0x0501,
	/* Token contains RAN Slice specific MAC scheduler information */
	EP_TLV_RAN_SLICE_MAC_SCHED = 0x0502,
	/* Token contains RAN Slice Capabilities for a specific cell */
	EP_TLV_RAN_CAP             = 0x0503,

	/*
	 * Type 6 reserved to UE measurements
	 */

	/* Token contains UE RCC Measurement information */
	EP_TLV_UE_RRC_MEAS         = 0x0600,
	EP_TLV_UE_RRC_REPORT       = 0x0601,

	/*
	 * Type 7 reserved to UE reports
	 */

	/* Token contains UE identity information */
	EP_TLV_UE_REP_ID           = 0x0700,
};

/* Structure of the TLV header common to all components */
typedef struct __ep_hdr_TLV {
	tlv_type_t   type;    /* Type of the body */
	tlv_length_t length;  /* Length of the body */
	/*
	 * Here following there will be the TLV body
	 */
}__attribute__((packed)) ep_TLV;

/*
 *
 * RNTI container generic TLV token:
 *
 */

/* Format a generic RNTI report TLV token (just a bunch of RNTIs).
 * Returns the message size or -1 on error.
 */
int epf_TLV_rnti_report(
	char *       buf,
	unsigned int size,
	rnti_id_t *  rntis,
	uint32_t     nof_rntis);

/* Parses a generic RNTI report TLV token.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_TLV_rnti_report(
	char *       buf,
	rnti_id_t *  rntis,
	uint32_t  *  nof_rntis);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_TLV_H */
