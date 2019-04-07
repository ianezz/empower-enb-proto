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

#ifndef __EMAGE_CELL_REPORTS_H
#define __EMAGE_CELL_REPORTS_H

#include <stdint.h>

#include "eppri.h"
#include "epTLV.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * Cell MAC report messages:
 */

#if 0
typedef struct __ep_cell_mac_report_reply {
	uint8_t  DL_prbs_total;
	uint32_t DL_prbs_used;
	uint8_t  UL_prbs_total;
	uint32_t UL_prbs_used;
}__attribute__((packed)) ep_macrep_rep;

typedef struct __ep_cell_mac_report_request {
	uint16_t interval;
}__attribute__((packed)) ep_macrep_req;
#endif

/*
 *
 * Cell Physical Resources measurement report:
 *
 */

typedef struct __ep_cell_PRB_rep {
	uint8_t         DL_res;
	uint32_t        DL_used;
	uint8_t         UL_res;
	uint32_t        UL_used;
} __attribute__((packed)) ep_cell_PRB_rep;

typedef struct __ep_cell_PRB_rep_TLV {
	ep_TLV          header;
	ep_cell_PRB_rep body;
} __attribute__((packed)) ep_cell_PRB_rep_TLV;

/*
 *
 * Cell Physical Resources measurement request:
 *
 */

typedef struct __ep_cell_PRB_meas_TLV {
	ep_TLV           header;
	/* No body right now... */
} __attribute__((packed)) ep_cell_PRB_meas_TLV;

/******************************************************************************
 * Opaque structures                                                          *
 ******************************************************************************/

/* Report of the PRB in the cell */
typedef struct __ep_cell_prb_rep_info {
	uint8_t  DL_prbs;
	uint32_t DL_prbs_used;
	uint8_t  UL_prbs;
	uint32_t UL_prbs_used;
} ep_cell_prb_info;

/* Cumulative reports of the cell */
typedef struct __ep_cell_reports {
	ep_cell_prb_info prb;
} ep_cell_rep;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

/* Format a MAC report negative reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_sched_cell_meas_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/* Format a MAC report not-supported reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_sched_cell_meas_ns(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/* Format a MAC report reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_sched_cell_meas_rep(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id,
	uint32_t        interval,
	ep_cell_rep *   rep);

/* Parse a MAC report reply looking for the desired fields */
int epp_sched_cell_meas_rep(
	char *          buf,
	unsigned int    size,
	ep_cell_rep *   rep);

/* Format a MAC report request.
 * Returns the size of the message, or a negative error number.
 */
int epf_sched_cell_meas_req(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id,
	uint32_t        interval);

/* Parse a MAC report request looking for the desired fields */
int epp_sched_cell_meas_req(
	char *          buf,
	unsigned int    size,
	uint32_t *      interval);

/******************************************************************************
 * Operation on trigger-event messages                                        *
 ******************************************************************************/

/* Format a MAC report negative reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_cell_meas_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/* Format a MAC report not-supported reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_cell_meas_ns(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/* Format a MAC report reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_cell_meas_rep(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id,
	ep_cell_rep *   rep);

/* Parse a MAC report reply looking for the desired fields */
int epp_trigger_cell_meas_rep(
	char *          buf,
	unsigned int    size,
	ep_cell_rep *   rep);

/* Format a MAC report request.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_cell_meas_req(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id);

/* Parse a MAC report request looking for the desired fields */
int epp_trigger_cell_meas_req(
	char *          buf,
	unsigned int    size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_CELL_REPORTS_H */
