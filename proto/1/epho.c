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

#define _DEFAULT_SOURCE
#include <endian.h>
#include <netinet/in.h>

#include <emproto.h>

int epf_ho_rep(
	char *       buf,
	unsigned int size,
	enb_id_t     origin_eNB,
	uint16_t     origin_pci,
	uint16_t     origin_rnti,
	uint16_t     target_rnti)
{
	ep_ho_rep * rep = (ep_ho_rep *)buf;

	if(size < sizeof(ep_ho_rep)) {
		ep_dbg_log(EP_DBG_2"F - HO Rep: Not enough space!\n");
		return -1;
	}

	rep->origin_eNB  = htobe64(origin_eNB);
	rep->origin_pci  = htons(origin_pci);
	rep->origin_rnti = htons(origin_rnti);
	rep->target_rnti = htons(target_rnti);

	ep_dbg_dump(EP_DBG_2"F - HO Rep:   ", buf, sizeof(ep_ho_rep));

	return sizeof(ep_ho_rep);
}

int epp_ho_rep(
	char *       buf,
	unsigned int size,
	enb_id_t *   origin_eNB,
	uint16_t *   origin_pci,
	uint16_t *   origin_rnti,
	uint16_t *   target_rnti)
{
	ep_ho_rep * rep = (ep_ho_rep *)buf;

	if(size < sizeof(ep_ho_rep)) {
		ep_dbg_log(EP_DBG_2"P - HO Rep: Not enough space!\n");
		return -1;
	}

	if(origin_eNB) {
		*origin_eNB  = be64toh(rep->origin_eNB);
	}

	if(origin_pci) {
		*origin_pci  = ntohs(rep->origin_pci);
	}

	if(origin_rnti) {
		*origin_rnti = ntohs(rep->origin_rnti);
	}

	if(target_rnti) {
		*target_rnti = ntohs(rep->target_rnti);
	}

	ep_dbg_dump(EP_DBG_2"P - HO Rep:   ", buf, sizeof(ep_ho_rep));

	return EP_SUCCESS;
}

int epf_ho_req(
	char *       buf,
	unsigned int size,
	uint16_t     rnti,
	enb_id_t     enb,
	uint16_t     pci,
	uint8_t      cause)
{
	ep_ho_req * req = (ep_ho_req *)buf;

	if(size < sizeof(ep_ho_req)) {
		ep_dbg_log(EP_DBG_2"F - HO Req: Not enough space!\n");
		return -1;
	}

	req->rnti       = htons(rnti);
	req->target_eNB = htobe64(enb);
	req->target_pci = htons(pci);
	req->cause      = cause;

	ep_dbg_dump(EP_DBG_2"F - HO Req:   ", buf, sizeof(ep_ho_req));

	return sizeof(ep_ho_req);
}

int epp_ho_req(
	char *       buf,
	unsigned int size,
	uint16_t *   rnti,
	enb_id_t *   enb,
	uint16_t *   pci,
	uint8_t *    cause)
{
	ep_ho_req * req = (ep_ho_req *)buf;

	if(size < sizeof(ep_ho_req)) {
		ep_dbg_log(EP_DBG_2"P - HO Req: Not enough space!\n");
		return -1;
	}

	if(rnti) {
		*rnti  = ntohs(req->rnti);
	}

	if(enb) {
		*enb   = be64toh(req->target_eNB);
	}

	if(pci) {
		*pci   = ntohs(req->target_pci);
	}

	if(cause) {
		*cause = req->cause;
	}

	ep_dbg_dump(EP_DBG_2"P - HO Req:   ", buf, sizeof(ep_ho_req));

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ho_rep_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	enb_id_t     origin_eNB,
	uint16_t     origin_pci,
	uint16_t     origin_rnti,
	uint16_t     target_rnti)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single HO Fail: Invalid buffer!\n");
		return -1;
	}

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_HANDOVER,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ho_rep(
		buf + ret,
		size - ret,
		origin_eNB,
		origin_pci,
		origin_rnti,
		target_rnti);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_single_ho_rep_ns(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	enb_id_t     origin_eNB,
	uint16_t     origin_pci,
	uint16_t     origin_rnti,
	uint16_t     target_rnti)
{
	int ms = 0;
	int ret= 0 ;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single HO NS: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_HANDOVER,
		EP_OPERATION_NOT_SUPPORTED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ho_rep(
		buf + ret,
		size - ret,
		origin_eNB,
		origin_pci,
		origin_rnti,
		target_rnti);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_single_ho_rep(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	enb_id_t     origin_eNB,
	uint16_t     origin_pci,
	uint16_t     origin_rnti,
	uint16_t     target_rnti)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single HO Rep: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_HANDOVER,
		EP_OPERATION_SUCCESS);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ho_rep(
		buf + ret,
		size - ret,
		origin_eNB,
		origin_pci,
		origin_rnti,
		target_rnti);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ho_rep(
	char *       buf,
	unsigned int size,
	enb_id_t *   origin_eNB,
	uint16_t *   origin_pci,
	uint16_t *   origin_rnti,
	uint16_t *   target_rnti)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Single HO Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_ho_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		origin_eNB,
		origin_pci,
		origin_rnti,
		target_rnti);
}

int epf_single_ho_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint16_t     rnti,
	enb_id_t     enb,
	uint16_t     pci,
	uint8_t      cause)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single HO Req: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REQ);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_single(
		buf + ret,
		size - ret,
		EP_ACT_HANDOVER,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ho_req(buf + ret, size - ret, rnti, enb, pci, cause);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ho_req(
	char *       buf,
	unsigned int size,
	uint16_t *   rnti,
	enb_id_t *   enb,
	uint16_t *   pci,
	uint8_t *    cause)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Single HO Req: Invalid buffer!\n");
		return -1;
	}

	return epp_ho_req(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		rnti,
		enb,
		pci,
		cause);
}
