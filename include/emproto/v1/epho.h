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

/*    HANDOVER MESSAGE
 *
 * This message provides the request/reply for Handover procedures.
 */

#ifndef __EMAGE_HANDOVER_H
#define __EMAGE_HANDOVER_H

#include <endian.h>
#include <stdint.h>

#include "eppri.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum __ep_handover_cause {
	EP_HO_CAUSE_CRITICAL     = 1, /* Critical handover */
	EP_HO_CAUSE_OPTIMIZATION = 2, /* Optimization handover */
} ep_ho_cause;

typedef struct __ep_handover_request {
	uint16_t rnti;       /* Target RNTI */
	enb_id_t target_eNB; /* Target eNB id */
	uint16_t target_pci; /* Target physical cell id */
	uint8_t  cause;      /* Cause of the hand-over */
}__attribute__((packed)) ep_ho_req;

typedef struct __ep_handover_reply {
	enb_id_t origin_eNB;  /* Original eNB  */
	uint16_t origin_pci;  /* Original PCI  */
	uint16_t origin_rnti; /* Original RNTI */
	uint16_t target_rnti; /* Target RNTI   */
}__attribute__((packed)) ep_ho_rep;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/* Format an handover negative reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ho_rep_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	enb_id_t     origin_eNB,
	uint16_t     origin_pci,
	uint16_t     origin_rnti,
	uint16_t     target_rnti);

/* Format an handover "not-supported" reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ho_rep_ns(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	enb_id_t     origin_eNB,
	uint16_t     origin_pci,
	uint16_t     origin_rnti,
	uint16_t     target_rnti);

/* Format an handover positive reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ho_rep(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	enb_id_t     origin_eNB,
	uint16_t     origin_pci,
	uint16_t     origin_rnti,
	uint16_t     target_rnti);

/* Parse an handover reply looking for the desired fields */
int epp_single_ho_rep(
	char *       buf,
	unsigned int size,
	enb_id_t *   origin_eNB,
	uint16_t *   origin_pci,
	uint16_t *   origin_rnti,
	uint16_t *   target_rnti);

/* Format an handover request.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ho_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint16_t     rnti,
	enb_id_t     enb,
	uint16_t     pci,
	uint8_t      cause);

/* Parse an handover request looking for the desired fields */
int epp_single_ho_req(
	char *       buf,
	unsigned int size,
	uint16_t *   rnti,
	enb_id_t *   enb,
	uint16_t *   pci,
	uint8_t *    cause);

/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

/******************************************************************************
 * Operation on trigger-event messages                                        *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_HANDOVER_H */
