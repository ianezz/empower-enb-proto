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
#include <string.h>

#include <emproto.h>

int epf_cell_meas_rep(
	char *        buf,
	unsigned int  size,
	ep_cell_rep * rep)
{
	int    s = 0;
	char * c = buf;

	ep_cell_PRB_rep_TLV * prb;

	if(rep->prb.DL_prbs != 0 || rep->prb.UL_prbs != 0) {
		prb = (ep_cell_PRB_rep_TLV *)buf;

		prb->header.type   = htons(EP_TLV_CELL_PRB_REPORT);
		prb->header.length = htons(sizeof(ep_cell_PRB_rep));
		prb->body.DL_res   = rep->prb.DL_prbs;
		prb->body.DL_used  = htonl(rep->prb.DL_prbs_used);
		prb->body.UL_res   = rep->prb.UL_prbs;
		prb->body.UL_used  = htonl(rep->prb.UL_prbs_used);

		ep_dbg_dump(EP_DBG_3
			"F - Cell PRB TLV: ", c, sizeof(ep_cell_PRB_rep_TLV));

		c += sizeof(ep_cell_PRB_rep_TLV);
		s += sizeof(ep_cell_PRB_rep_TLV);
	}

	return s;
}

int epp_cell_meas_rep_TLV(char * buf, ep_cell_rep * rep)
{
	ep_TLV *              tlv = (ep_TLV *)buf;
	ep_cell_PRB_rep_TLV * prb;

	switch(ntohs(tlv->type)) {
	case EP_TLV_CELL_PRB_REPORT:
		prb = (ep_cell_PRB_rep_TLV *)buf;

		rep->prb.DL_prbs      = prb->body.DL_res;
		rep->prb.DL_prbs_used = ntohl(prb->body.DL_used);
		rep->prb.UL_prbs      = prb->body.UL_res;
		rep->prb.UL_prbs_used = ntohl(prb->body.UL_used);

		ep_dbg_dump(EP_DBG_3"P - Cell PRB TLV: ",
			buf, sizeof(ep_cell_PRB_rep_TLV));

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - Cell PRB TLV: Unexpected token %d!\n",
			tlv->type);

		break;
	}

	return EP_SUCCESS;
}

int epp_cell_meas_rep(
	char *        buf,
	unsigned int  size,
	ep_cell_rep * rep)
{
	char *   c = buf;
	ep_TLV * tlv;

	if(!buf || !rep) {
		return EP_ERROR;
	}

	ep_dbg_dump(EP_DBG_2"P - Cell Meas Rep: ", buf, 0);

	/* Clean-before-use policy */
	memset(rep, 0, sizeof(ep_cell_rep));

	/* Continue until exhaustion of given data */
	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) > buf + size) {
			ep_dbg_log(EP_DBG_3"P - Cell Meas Rep: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + ntohs(tlv->length),
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_cell_meas_rep_TLV(c, rep);

		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

int epf_cell_meas_req(char * buf, unsigned int size)
{
	int    s = 0;
	char * c = buf;

	ep_cell_PRB_meas_TLV * prb;

	if(size < sizeof(ep_cell_PRB_meas_TLV)) {
		ep_dbg_log(EP_DBG_2"F - Cell Meas Req: Not enough space!\n");
		return -1;
	}

	prb = (ep_cell_PRB_meas_TLV *)buf;

	prb->header.type   = htons(EP_TLV_CELL_PRB_REQ);
	prb->header.length = htons(0);	/* There is no body right now */

	ep_dbg_dump(EP_DBG_3
		"F - Cell PRB TLV: ", c, sizeof(ep_cell_PRB_meas_TLV));

	s += sizeof(ep_cell_PRB_meas_TLV);
	c += sizeof(ep_cell_PRB_meas_TLV);

	return s;
}

int epp_cell_meas_req(char * buf, unsigned int size)
{
	char *   c = buf;
	ep_TLV * tlv;

	if(!buf) {
		return EP_ERROR;
	}

	ep_dbg_dump(EP_DBG_2"P - Cell Meas Req: ", buf, 0);

	/* Continue until exhaustion of given data */
	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) > buf + size) {
			ep_dbg_log(EP_DBG_3"P - Cell Meas Req: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + ntohs(tlv->length),
				(buf + size) - c);
			break;
		}

		if(ntohs(tlv->type) != EP_TLV_CELL_PRB_REQ) {
			ep_dbg_log(EP_DBG_3"P - Cell Meas TLV: "
				"Unexpected token %d!\n",
				tlv->type);
		}

		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}


	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

/*
 *
 * Schedule event message:
 *
 */

int epf_sched_cell_meas_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Cell Meas Fail: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SCHEDULE_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_schedule(
		buf + ret,
		size - ret,
		EP_ACT_CELL_MEASURE,
		EP_OPERATION_FAIL,
		0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = 0; /* No body here. */

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_sched_cell_meas_ns(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Cell Meas NS: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SCHEDULE_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_schedule(
		buf + ret,
		size - ret,
		EP_ACT_CELL_MEASURE,
		EP_OPERATION_NOT_SUPPORTED,
		0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = 0; /* No body here. */

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_sched_cell_meas_rep(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id,
	uint32_t        interval,
	ep_cell_rep *   info)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !info) {
		ep_dbg_log(EP_DBG_0"F - Cell Meas Rep: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SCHEDULE_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REP);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_schedule(
		buf + ret,
		size - ret,
		EP_ACT_CELL_MEASURE,
		EP_OPERATION_SUCCESS,
		interval);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_cell_meas_rep(buf + ret, size - ret, info);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_sched_cell_meas_rep(
	char *          buf,
	unsigned int    size,
	ep_cell_rep *   info)
{
	if(!buf || !info) {
		ep_dbg_log(EP_DBG_0"P - Cell Meas Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_cell_meas_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_c_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_c_hdr)),
		info);
}

int epf_sched_cell_meas_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint32_t     interval)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Cell Meas Req: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf,
		size,
		EP_TYPE_SCHEDULE_MSG,
		enb_id,
		cell_id,
		mod_id,
		EP_HDR_FLAG_DIR_REQ);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_schedule(
		buf + ret,
		size - ret,
		EP_ACT_CELL_MEASURE,
		EP_OPERATION_UNSPECIFIED,
		interval);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_cell_meas_req(buf + ret, size - ret);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_sched_cell_meas_req(
	char *          buf,
	unsigned int    size,
	uint32_t *      interval)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Cell Meas Req: Invalid buffer!\n");
		return -1;
	}

	*interval = epp_sched_interval(buf, size);

	return epp_cell_meas_req(
		buf + sizeof(ep_hdr) + sizeof(ep_c_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_c_hdr)));
}

/*
 *
 * Trigger event message:
 *
 */

int epf_trigger_cell_meas_fail(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Cell Meas Fail: Invalid buffer!\n");
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
		EP_ACT_CELL_MEASURE,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = 0; /* No body here. */

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_trigger_cell_meas_ns(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Cell Meas NS: Invalid buffer!\n");
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
		EP_ACT_CELL_MEASURE,
		EP_OPERATION_NOT_SUPPORTED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = 0; /* No body here. */

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_trigger_cell_meas_rep(
	char *          buf,
	unsigned int    size,
	enb_id_t        enb_id,
	cell_id_t       cell_id,
	mod_id_t        mod_id,
	ep_cell_rep *   info)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !info) {
		ep_dbg_log(EP_DBG_0"F - Cell Meas Rep: Invalid buffer!\n");
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
		EP_ACT_CELL_MEASURE,
		EP_OPERATION_SUCCESS);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_cell_meas_rep(buf + ret, size - ret, info);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_cell_meas_rep(
	char *          buf,
	unsigned int    size,
	ep_cell_rep *   info)
{
	if(!buf || !info) {
		ep_dbg_log(EP_DBG_0"P - Cell Meas Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_cell_meas_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_t_hdr)),
		info);
}

int epf_trigger_cell_meas_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Cell Meas Req: Invalid buffer!\n");
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
		EP_ACT_CELL_MEASURE,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_cell_meas_req(buf + ret, size - ret);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_trigger_cell_meas_req(
	char *          buf,
	unsigned int    size)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Cell Meas Req: Invalid buffer!\n");
		return -1;
	}

	return epp_cell_meas_req(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_t_hdr)));
}
