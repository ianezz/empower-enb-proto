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

/*    UE REPORT MESSAGE
 *
 * This message provides the request/reply for/from eNB capabilities, which are
 * the possible operation which can be done within the base station.
 */

#ifndef __EMAGE_UE_REPORT_H
#define __EMAGE_UE_REPORT_H

#include <endian.h>
#include <stdint.h>

#include "eppri.h"
#include "epTLV.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 *
 * UE identity report information:
 *
 */

/* UE identity informations */
typedef struct __ep_ue_report_identity {
	rnti_id_t rnti;  /* Radio Network Temporary Identifier */
	plmn_id_t plmn;  /* Public Land Mobile Network identifier */
	imsi_id_t imsi;  /* International Mobile Subscriber Identity */
	tmsi_id_t tmsi;  /* Temporary Mobile Subscriber Identity */
	uint8_t   state; /* State of the UE */
}__attribute__((packed)) ep_uerep_id;

/* UE identity informations as TLV token */
typedef struct __ep_ue_report_identity_TLV {
	ep_TLV      header;
	ep_uerep_id body;
}__attribute__((packed)) ep_uerep_id_TLV;

/* UE report request carry no data and just enable cell mechanism */
typedef struct __ep_ue_report_request {
	uint8_t dummy;
}__attribute__((packed)) ep_uerep_req;

/******************************************************************************
 * Opaque structures for message formatting/parsing                           *
 ******************************************************************************/

typedef enum __ep_ue_report_status {
	UE_STATUS_CONNECTED          = 0, /* UE is considered connected */
	UE_STATUS_DISCONNECTED       = 1, /* UE is considered disconnected */
	UE_STATUS_MAX
} UE_REPORT_STATUS;

typedef struct __ep_ue_details {
	plmn_id_t plmn;  /* Public Land Mobile Network identifier */
	rnti_id_t rnti;  /* Radio Network Temporary Identifier */
	imsi_id_t imsi;  /* International Mobile Subscriber Identity */
	tmsi_id_t tmsi;  /* Temporary Mobile Subscriber Identity */
	uint8_t   state; /* State of the UE */
} ep_ue_details;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

/******************************************************************************
 * Operation on trigger-event messages                                        *
 ******************************************************************************/

/* Format an UE report reply failure.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_uerep_rep_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/* Format an UE report reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id,
	uint32_t        nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues);

/* Parse an UE report reply looking for the desired fields */
int epp_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues);

/* Format an UE report request.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_uerep_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	ep_op_type   op);

/* Parse an UE report request for the desired fields */
int epp_trigger_uerep_req(char * buf, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_UE_REPORT_H */
