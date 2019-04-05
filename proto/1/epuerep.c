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

#include <netinet/in.h>

#include <emproto.h>

int epf_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	int                  i;
	int                  s = 0;
	char *               c = buf;

	ep_uerep_id_TLV *    ueid;

	/* Process one UE per time */
	for(i = 0; i < nof_ues && i < max_ues; i++) {
		/* Do not count invalid RNTIs */
		if(ues[i].rnti == 0) {
			continue; /* Next UE */
		}

		if(c + sizeof(ep_uerep_id) > buf + size) {
			ep_dbg_log(
				EP_DBG_3"F - UERep TLV: Not enough space!\n");

			return -1;
		}

		ueid = (ep_uerep_id_TLV *)c;

		ueid->header.type   = htons(EP_TLV_UE_REP_ID);
		ueid->header.length = htons(sizeof(ep_uerep_id));
		ueid->body.rnti     = htons(ues[i].rnti);
		ueid->body.plmn     = htonl(ues[i].plmn);
		ueid->body.imsi     = htobe64(ues[i].imsi);
		ueid->body.tmsi     = htonl(ues[i].tmsi);
		ueid->body.state    = ues[i].state;

		ep_dbg_dump(EP_DBG_3
			"F - UERep ID TLV: ", c, sizeof(ep_uerep_id_TLV));

		/* Advance with buffer and size*/
		s += sizeof(ep_uerep_id_TLV);
		c += sizeof(ep_uerep_id_TLV);
	}

	return s;
}

int epp_uerep_rep_TLV(
	char *          buf,
	ep_ue_details * ues,
	uint32_t *      nof_ues,
	uint32_t        max_ues)
{
	int                  i;
	int                  j   = -1;
	ep_TLV *             tlv;
	ep_uerep_id_TLV *    ueid;

	tlv      = (ep_TLV *)buf;
	*nof_ues = 0;

	switch(ntohs(tlv->type)) {
	case EP_TLV_UE_REP_ID:
		ueid = (ep_uerep_id_TLV *)buf;

		/* Select the right UE slot where to save data */
		for(i = 0; i < max_ues; i++) {
			/* Select a free slot */
			if(j < 0 && ues[i].rnti == 0) {
				j = i;
				*nof_ues = *nof_ues + 1;
			}

			if(ues[i].rnti == ntohs(ueid->body.rnti)) {
				j = i;
				break; /* for */
			}
		}

		ep_dbg_dump(EP_DBG_3"P - UERep ID TLV: ",
			buf, sizeof(ep_uerep_id_TLV));

		ues[j].rnti = ntohs(ueid->body.rnti);
		ues[j].plmn = ntohl(ueid->body.plmn);
		ues[j].imsi = be64toh(ueid->body.imsi);
		ues[j].tmsi = ntohl(ueid->body.tmsi);

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - UERep TLV: Unexpected token %d!\n",
			tlv->type);

		break;
	}

	return EP_SUCCESS;
}

int epp_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	char *               c = buf;

	ep_TLV *             tlv;

	if(!buf || !nof_ues || !ues) {
		return EP_ERROR;
	}

	ep_dbg_dump(EP_DBG_2"P - UERep Rep: ", buf, 0);

	/* Continue until exhaustion of given data */
	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) > buf + size) {
			ep_dbg_log(EP_DBG_3"P - UERep Rep: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + ntohs(tlv->length),
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_uerep_rep_TLV(c, ues, nof_ues, max_ues);

		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

int epf_uerep_req(char * buf, unsigned int size)
{
	ep_uerep_req * req = (ep_uerep_req *)buf;

	if(size < sizeof(ep_uerep_req)) {
		ep_dbg_log(EP_DBG_2"F - UEREP Req: Not enough space!\n");
		return -1;
	}

	req->dummy = 0;

	ep_dbg_dump(EP_DBG_2"F - UREP Req: ", buf, sizeof(ep_uerep_req));

	return sizeof(ep_uerep_req);
}

int epp_uerep_req(char * buf, unsigned int size)
{
	if(size < sizeof(ep_uerep_req)) {
		ep_dbg_log(EP_DBG_2"P - UEREP Req: Not enough space!\n");
		return EP_ERROR;
	}

	ep_dbg_dump(EP_DBG_2"P - UREP Req: ", buf, 0);

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_trigger_uerep_rep_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Trigger UEREP Fail: Invalid buffer!\n");
		return -1;
	}

	ms += epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_trigger(
		buf + ret,
		size - ret,
		EP_ACT_UE_REPORT,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uerep_rep(buf + ret, size - ret, 0, 0, 0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id,
	uint32_t        nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Trigger UEREP Rep: Invalid buffer!\n");
		return -1;
	}

	if(nof_ues > 0 && !ues) {
		ep_dbg_log(EP_DBG_0"F - Trigger UEREP Rep: Invalid UEs!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_trigger(
		buf + ret,
		size - ret,
		EP_ACT_UE_REPORT,
		EP_OPERATION_SUCCESS);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uerep_rep(buf + ret, size - ret, nof_ues, max_ues, ues);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Trigger UEREP Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_uerep_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		nof_ues,
		max_ues,
		ues);
}

int epf_trigger_uerep_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	ep_op_type   op)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Trigger UEREP Req: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REQ);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_trigger(
		buf + ret,
		size - ret,
		EP_ACT_UE_REPORT,
		op);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uerep_req(buf + ret, size - ret);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_uerep_req(char * buf, unsigned int size)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Trigger UEREP Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_uerep_req(buf, size);
}
