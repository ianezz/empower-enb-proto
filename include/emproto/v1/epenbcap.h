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

/*    ENB CAPABILITIES MESSAGE
 *
 * This message provides the request/reply for/from eNB capabilities, which are
 * the possible operation which can be done within the base station.
 */

#ifndef __EMAGE_ENB_CAPABILITIES_H
#define __EMAGE_ENB_CAPABILITIES_H

#include <stdint.h>

#include "eppri.h"
#include "epcellcap.h"
#include "epRAN.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * eNB capabilities message
 */

/******************************************************************************
 * Opaque structures for message formatting/parsing                           *
 ******************************************************************************/

/* Maximum number of expected cells in an eNB capabilities message */
#define EP_ECAP_CELL_MAX	8

typedef struct __ep_enb_details {
	/* Array of the valid cells */
	ep_cell_det cells[EP_ECAP_CELL_MAX];
	/* Number of valid cells */
	uint32_t    nof_cells;
	/* RAN slicing details */
	ep_ran_det  ran[EP_ECAP_CELL_MAX];
	/* Number of valid RAN slicing details */
	uint32_t    nof_ran;
} ep_enb_det;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/* Format an eNB capabilities negative reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ecap_rep_fail(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id);

/* Format an eNB capabilities reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id,
	ep_enb_det *  det);

/* Parse an eNB capabilities reply looking for the desired fields */
int epp_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	ep_enb_det *  det);

/* Format an eNB capabilities request.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ecap_req(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id);

/* Parse an eNB capabilities request for the desired fields */
int epp_single_ecap_req(char * buf, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_ENB_CAPABILITIES_H */
