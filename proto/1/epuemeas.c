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
#include <string.h>

#include <emproto.h>

int epf_uemeas_rep(
	char *          buf,
	unsigned int    size,
	ep_ue_report *  rep)
{
	int    i;
	int    s = 0;
	char * c = buf;

	ep_ue_RRC_rep_TLV * rrc;

	for(i = 0; i < rep->nof_rrc && i < EP_UE_RRC_MEAS_MAX; i++) {
		if(c + sizeof(ep_ue_RRC_rep_TLV) > buf + size) {
			ep_dbg_log(
				EP_DBG_3"F - RRCRep TLV: Not enough space!\n");

			return -1;
		}

		rrc = (ep_ue_RRC_rep_TLV *)c;

		rrc->header.type   = htons(EP_TLV_UE_RRC_REPORT);
		rrc->header.length = htons(sizeof(ep_ue_RRC_rep));

		rrc->body.meas_id  = htons(rep->rrc[i].meas_id);
		rrc->body.pci      = htons(rep->rrc[i].pci);
		rrc->body.rsrq     = htons(rep->rrc[i].rsrq);
		rrc->body.rsrp     = htons(rep->rrc[i].rsrp);

		ep_dbg_dump(EP_DBG_3
			"F - RRCRep TLV: ", c, sizeof(ep_ue_RRC_rep_TLV));

		s += sizeof(ep_ue_RRC_rep_TLV);
		c += sizeof(ep_ue_RRC_rep_TLV);
	}

	return s;
}

int epp_uemeas_rep_TLV(char * buf, ep_ue_report * rep)
{
	ep_TLV *            tlv = (ep_TLV *)buf;
	ep_ue_RRC_rep_TLV * rrc;

	switch(ntohs(tlv->type)) {
	case EP_TLV_UE_RRC_REPORT:
		rrc = (ep_ue_RRC_rep_TLV *)buf;

		if(rep->nof_rrc >= EP_UE_RRC_MEAS_MAX) {
			ep_dbg_log(EP_DBG_3"P - RRCRep TLV: No more slots\n");
			return EP_ERROR;
		}

		ep_dbg_dump(EP_DBG_3"P - RRCRep TLV: ",
			buf, sizeof(ep_ue_RRC_rep_TLV));

		rep->rrc[rep->nof_rrc].meas_id = ntohs(rrc->body.meas_id);
		rep->rrc[rep->nof_rrc].pci     = ntohs(rrc->body.pci);
		rep->rrc[rep->nof_rrc].rsrp    = ntohs(rrc->body.rsrp);
		rep->rrc[rep->nof_rrc].rsrq    = ntohs(rrc->body.rsrq);

		rep->nof_rrc++;

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - UEMeas TLV: Unexpected token %d!\n",
			tlv->type);

		break;
	}

	return EP_SUCCESS;
}

int epp_uemeas_rep(
	char *         buf,
	unsigned int   size,
	ep_ue_report * rep)
{
	char *   c = buf;
	ep_TLV * tlv;

	if(!buf || !rep) {
		return EP_ERROR;
	}

	ep_dbg_dump(EP_DBG_2"P - RRCMeas Rep: ", buf, 0);

	memset(rep, 0, sizeof(ep_ue_report));

	/* Continue until exhaustion of given data */
	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) > buf + size) {
			ep_dbg_log(EP_DBG_3"P - UEMeas Rep: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + ntohs(tlv->length),
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_uemeas_rep_TLV(c, rep);

		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

int epf_uemeas_req(
	char *        buf,
	unsigned int  size,
	ep_ue_meas *  meas)
{
	int    i;
	int    s = 0;
	char * c = buf;

	ep_ue_RRC_meas_TLV * rrc;

	for(i = 0; i < meas->nof_rrc && i < EP_UE_RRC_MEAS_MAX; i++) {
		if(c + sizeof(ep_ue_RRC_meas_TLV) > buf + size) {
			ep_dbg_log(
				EP_DBG_3"F - RRCMeas TLV: Not enough space!\n");

			return -1;
		}

		rrc = (ep_ue_RRC_meas_TLV *)c;

		rrc->header.type   = htons(EP_TLV_UE_RRC_MEAS);
		rrc->header.length = htons(sizeof(ep_ue_RRC_measure));

		rrc->body.rnti     = htons(meas->rrc[i].rnti);
		rrc->body.meas_id  = htons(meas->rrc[i].meas_id);
		rrc->body.earfcn   = htons(meas->rrc[i].earfcn);
		rrc->body.max_cells= htons(meas->rrc[i].max_cells);
		rrc->body.max_meas = htons(meas->rrc[i].max_meas);

		ep_dbg_dump(EP_DBG_3
			"F - RRCMeas TLV: ", c, sizeof(ep_ue_RRC_meas_TLV));

		s += sizeof(ep_ue_RRC_meas_TLV);
		c += sizeof(ep_ue_RRC_meas_TLV);
	}

	return s;
}

int epp_uemeas_req_TLV(char * buf, ep_ue_meas * meas)
{
	ep_TLV *             tlv = (ep_TLV *)buf;
	ep_ue_RRC_meas_TLV * rrc;

	switch(ntohs(tlv->type)) {
	case EP_TLV_UE_RRC_MEAS:
		rrc = (ep_ue_RRC_meas_TLV *)buf;

		if(meas->nof_rrc >= EP_UE_RRC_MEAS_MAX) {
			ep_dbg_log(EP_DBG_3"P - RRCMeas TLV: No more slots\n");
			return EP_ERROR;
		}

		ep_dbg_dump(EP_DBG_3"P - RRCMeas TLV: ",
			buf, sizeof(ep_ue_RRC_meas_TLV));

		meas->rrc[meas->nof_rrc].meas_id  = ntohs(rrc->body.meas_id);
		meas->rrc[meas->nof_rrc].rnti     = ntohs(rrc->body.rnti);
		meas->rrc[meas->nof_rrc].earfcn   = ntohs(rrc->body.earfcn);
		meas->rrc[meas->nof_rrc].interval = ntohs(rrc->body.interval);
		meas->rrc[meas->nof_rrc].max_cells= ntohs(rrc->body.max_cells);
		meas->rrc[meas->nof_rrc].max_meas = ntohs(rrc->body.max_meas);

		meas->nof_rrc++;

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - UEMeas TLV: Unexpected token %d!\n",
			tlv->type);

		break;
	}

	return EP_SUCCESS;
}

int epp_uemeas_req(char * buf, unsigned int size, ep_ue_meas * meas)
{
	char *   c = buf;
	ep_TLV * tlv;

	if(!buf || !meas) {
		return EP_ERROR;
	}

	ep_dbg_dump(EP_DBG_2"P - RRCMeas Req: ", buf, 0);

	memset(meas, 0, sizeof(ep_ue_meas));

	/* Continue until exhaustion of given data */
	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) > buf + size) {
			ep_dbg_log(EP_DBG_3"P - UEMeas Req: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + ntohs(tlv->length),
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_uemeas_req_TLV(c, meas);

		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_trigger_uemeas_rep_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Trigger UMEA Fail: Invalid buffer!\n");
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
		EP_ACT_UE_MEASURE,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
#if 0
	ms   = epf_uemeas_rep(buf + ret, size - ret, 0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
#endif
	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_trigger_uemeas_rep(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id,
	ep_ue_report *  rep)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !rep) {
		ep_dbg_log(EP_DBG_0"F - Trigger UMEA Rep: Invalid pointers!\n");
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
		EP_ACT_UE_MEASURE,
		EP_OPERATION_SUCCESS);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uemeas_rep(buf + ret, size - ret, rep);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_uemeas_rep(
	char *          buf,
	unsigned int    size,
	ep_ue_report *  rep)
{
	if(!buf || !rep) {
		ep_dbg_log(EP_DBG_0"P - Trigger UMEA Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_uemeas_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_t_hdr)),
		rep);
}

int epf_trigger_uemeas_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	ep_op_type   op,
	ep_ue_meas * meas)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Trigger UMEA Req: Invalid buffer!\n");
		return -1;
	}

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_head(
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
		EP_ACT_UE_MEASURE,
		op);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_uemeas_req(buf + ret, size - ret, meas);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_uemeas_req(
	char *       buf,
	unsigned int size,
	ep_ue_meas * meas)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Trigger UMEA Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_uemeas_req(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_t_hdr)),
		meas);
}
