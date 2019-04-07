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

#include <netinet/in.h>

#include <emproto.h>

int epf_ecap_rep(
	char *        buf,
	unsigned int  size,
	ep_enb_det *  det)
{
	char * c   = buf;
	int    i   = 0;

	ep_ccap_TLV *     ctlv;
	ep_ran_caps_TLV * rtlv;

	ep_dbg_dump(EP_DBG_2"F - ECAP Rep: ", buf, 0);

	/* Cell capabilities */
	for(i = 0; i < det->nof_cells && i < EP_ECAP_CELL_MAX; i++) {
		if(c + sizeof(ep_ccap_TLV) > buf + size) {
			ep_dbg_log(EP_DBG_3"F - ECAP Rep: Not enough space!\n");
			return -1;
		}

		ctlv = (ep_ccap_TLV *)c;

		ctlv->header.type   = htons(EP_TLV_CELL_CAP);
		ctlv->header.length = htons(sizeof(ep_ccap_det));

		ctlv->body.pci      = htons(det->cells[i].pci);
		ctlv->body.feat     = htonl(det->cells[i].feat);
		ctlv->body.DL_earfcn= htons(det->cells[i].DL_earfcn);
		ctlv->body.DL_prbs  = det->cells[i].DL_prbs;
		ctlv->body.UL_earfcn= htons(det->cells[i].UL_earfcn);
		ctlv->body.UL_prbs  = det->cells[i].UL_prbs;
		ctlv->body.max_ues  = htons(det->cells[i].max_ues);

		ep_dbg_dump(EP_DBG_3"F - CCAP TLV: ", c, sizeof(ep_ccap_TLV));

		/* Point to the next token */
		c += sizeof(ep_ccap_TLV);
	}

	/* RAN slicing capabilities for each cell */
	for(i = 0; i < det->nof_ran && i < EP_ECAP_CELL_MAX; i++) {
		if(c + sizeof(ep_ran_caps_TLV) > buf + size) {
			ep_dbg_log(EP_DBG_3"F - ECAP Rep: Not enough space!\n");
			return -1;
		}

		rtlv = (ep_ran_caps_TLV *)c;

		rtlv->header.type    = htons(EP_TLV_RAN_CAP);
		rtlv->header.length  = htons(sizeof(ep_ran_caps));

		rtlv->body.pci       = htons(det->ran[i].pci);
		rtlv->body.l1_caps   = htonl(det->ran[i].l1_mask);
		rtlv->body.l2_caps   = htonl(det->ran[i].l2_mask);
		rtlv->body.l3_caps   = htonl(det->ran[i].l3_mask);
		rtlv->body.mac_sched = htonl(det->ran[i].l2.mac.slice_sched);
		rtlv->body.max_slices= htons(det->ran[i].max_slices);

		ep_dbg_dump(
			EP_DBG_3"F - RCAP TLV: ", c, sizeof(ep_ran_caps_TLV));

		/* Point to the next token */
		c += sizeof(ep_ran_caps_TLV);
	}

	return c - buf;
}

/* Parse a single TLV field.
 *
 * It assumes that the checks over the size have already been done by the
 * caller, and the area of memory is fine to access.
 */
int epp_ecap_single_TLV(char * buf, ep_enb_det * det)
{
	ep_TLV *      tlv = (ep_TLV *)buf;

	ep_ccap_det * ccap;
	ep_ran_caps * rcap;

	/* Decide what to do depending on the TLV type */
	switch(ntohs(tlv->type)) {
	case EP_TLV_CELL_CAP:
		/* No more cell than this */
		if(det->nof_cells >= EP_ECAP_CELL_MAX) {
			ep_dbg_log(EP_DBG_3"P - CCAP TLV: "
				"Hitting max cells limit!\n");
			break;
		}

		/* Points to the TLV body */
		ccap = (ep_ccap_det *)(buf + sizeof(ep_TLV));

		det->cells[det->nof_cells].pci       = ntohs(ccap->pci);
		det->cells[det->nof_cells].feat      = ntohl(ccap->feat);
		det->cells[det->nof_cells].DL_earfcn = ntohs(ccap->DL_earfcn);
		det->cells[det->nof_cells].DL_prbs   = ccap->DL_prbs;
		det->cells[det->nof_cells].UL_earfcn = ntohs(ccap->UL_earfcn);
		det->cells[det->nof_cells].UL_prbs   = ccap->UL_prbs;
		det->cells[det->nof_cells].max_ues   = ntohs(ccap->max_ues);

		/* Increase the value to use as index and counter */
		det->nof_cells++;

		ep_dbg_dump(EP_DBG_3"P - CCAP TLV: ", buf, sizeof(ep_ccap_TLV));

		break;
	case EP_TLV_RAN_CAP:
		/* No more RAN slicing info than this */
		if(det->nof_ran >= EP_ECAP_CELL_MAX) {
			ep_dbg_log(EP_DBG_3"P - RCAP TLV: "
				"Hitting max cells limit!\n");
			break;
		}

		rcap = (ep_ran_caps *)(buf + sizeof(ep_TLV));

		det->ran[det->nof_ran].pci        = ntohs(rcap->pci);
		det->ran[det->nof_ran].l1_mask    = ntohl(rcap->l1_caps);
		det->ran[det->nof_ran].l2_mask    = ntohl(rcap->l2_caps);
		det->ran[det->nof_ran].l3_mask    = ntohl(rcap->l3_caps);
		det->ran[det->nof_ran].max_slices = ntohl(rcap->max_slices);

		det->ran[det->nof_ran].l2.mac.slice_sched =
			ntohl(rcap->mac_sched);

		det->nof_ran++;

		ep_dbg_dump(EP_DBG_3"P - CCAP TLV: ", buf, sizeof(ep_ccap_TLV));

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - ECAP Rep: Unexpected token %d!\n",
			tlv->type);
		break;
	}

	return EP_SUCCESS;
}

int epp_ecap_rep(
	char *        buf,
	unsigned int  size,
	ep_enb_det *  det)
{
	char *        c   = buf;
	//ep_ecap_rep * rep = (ep_ecap_rep *)buf;
	ep_TLV *      tlv;

	if(!det) {
		ep_dbg_log(EP_DBG_2"P - ECAP Rep: Invalid pointer!\n");
		return EP_ERROR;
	}

	ep_dbg_dump(EP_DBG_2"P - ECAP Rep: ", buf, 0);

	/* We need these set to a correct value */
	det->nof_cells = 0;
	det->nof_ran   = 0;

	/* Continue until the end of the given array */
	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) >= buf + size) {
			ep_dbg_log(EP_DBG_3"P - ECAP Rep: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + tlv->length,
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_ecap_single_TLV(c, det);

		/* To the next tag */
		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

int epf_ecap_req(char * buf, unsigned int size)
{
#if 0
	ep_ecap_req * rep = (ep_ecap_req *)buf;

	if(size < sizeof(ep_ecap_req)) {
		ep_dbg_log(EP_DBG_2"F - ECAP Req: Not enough space!\n");
		return -1;
	}

	rep->dummy = 0;
#endif
	ep_dbg_dump(EP_DBG_2"F - ECAP Req: ", buf, 0);

	return 0;
}

int epp_ecap_req(char * buf, unsigned int size)
{
#if 0
	if(size < sizeof(ep_ecap_req)) {
		ep_dbg_log(EP_DBG_2"P - ECAP Rep: Not enough space!\n");
		return -1;
	}
#endif
	ep_dbg_dump(EP_DBG_2"P - ECAP Req: ", buf, 0);

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ecap_rep_fail(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single ECAP Fail: Invalid buffer!\n");
		return EP_ERROR;
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
		EP_ACT_ECAP,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ecap_rep(buf + ret, size - ret, 0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id,
	ep_enb_det *  det)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single ECAP Rep: Invalid buffer!\n");
		return EP_ERROR;
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
		EP_ACT_ECAP,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ecap_rep(buf + ret, size - ret, det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	ep_enb_det *  det)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Single ECAP Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ecap_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		det);
}

int epf_single_ecap_req(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single ECAP Req: Invalid buffer!\n");
		return EP_ERROR;
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
		EP_ACT_ECAP,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ecap_req(buf + ret, size - ret);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ecap_req(char * buf, unsigned int size)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Single ECAP Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ecap_req(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size);
}
