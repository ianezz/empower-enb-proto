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

#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>

#include <emproto.h>

#define min(a,b)	(a < b ? a : b)

/*
 *
 * Parser setup primitives for RAN:
 *
 */

/* Format EUQ, sEtup Unspecified reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_euq(char * buf, unsigned int size)
{
	/*
	 * Currently the RAN setup request has no body
	 */

	ep_dbg_dump(EP_DBG_2"F - RANS Rep: ", buf, 0);

	return 0;
}

/*
 *
 * TLV parser for RAN:
 *
 */

/* Format SUP, Slice rePly TLV tokens.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_TLV(char * buf, unsigned int size, ep_ran_slice_det * det)
{
	int               s = 0;
	char *            c = buf;

	ep_ran_sres_TLV * sres;
	ep_ran_ssch_TLV * ssch;

	/*
	 * Format the RAN Slice resource token
	 */

	sres = (ep_ran_sres_TLV *)c;

	if(c + sizeof(ep_ran_sres_TLV) > buf + size) {
		ep_dbg_log(EP_DBG_3"F - RANS TLV: Not enough space!\n");
		return -1;
	}

	sres->header.type   = htons(EP_TLV_RAN_SLICE_MAC_RES);
	sres->header.length = htons(sizeof(ep_ran_sres));
	sres->body.rbgs     = htons(det->l2.rbgs);

	ep_dbg_dump(EP_DBG_3
		"F - RANS Res TLV: ", c, sizeof(ep_ran_sres_TLV));

	s += sizeof(ep_ran_sres_TLV);
	c += sizeof(ep_ran_sres_TLV);

	/*
	 * Format the RAN Slice scheduler token
	 */

	/* Bypass this token? */
	if(det->l2.usched == 0) {
		goto usr;
	}

	ssch = (ep_ran_ssch_TLV *)c;

	if(c + sizeof(ep_ran_ssch_TLV) > buf + size) {
		ep_dbg_log(EP_DBG_3"F - RANS TLV: Not enough space!\n");
		return -1;
	}

	ssch->header.type    = htons(EP_TLV_RAN_SLICE_MAC_SCHED);
	ssch->header.length  = htons(sizeof(ep_ran_ssch));
	ssch->body.user_sched= htonl(det->l2.usched);

	ep_dbg_dump(EP_DBG_3
		"F - RANS Sched TLV: ", c, sizeof(ep_ran_ssch_TLV));

	s += sizeof(ep_ran_ssch_TLV);
	c += sizeof(ep_ran_ssch_TLV);
usr:
	/*
	 * Format the RAN Slice users token
	 */

	if(det->nof_users > 0) {
		s = epf_TLV_rnti_report(
			c, size - s, det->users, det->nof_users);

		if(s < 0) {
			return s;
		}

		c += s;
	}

	return c - buf;
}

/* Parse a single TLV field for Slice Unspecified Reply.
 *
 * It assumes that the checks over the size have already been done by the
 * caller, and the area of memory is fine to access.
 */
int epp_ran_TLV(char * buf, ep_ran_slice_det * det)
{
	ep_TLV *          tlv = (ep_TLV *)buf;
	ep_ran_sres_TLV * sres;
	ep_ran_ssch_TLV * ssch;

	/* Decide what to do depending on the TLV type */
	switch(ntohs(tlv->type)) {
	case EP_TLV_RNTI_REPORT:
		det->nof_users = EP_RAN_USERS_MAX;

		if(epp_TLV_rnti_report(
			(char*)tlv, det->users, &det->nof_users))
		{
			return EP_ERROR;
		}
		break;
	case EP_TLV_RAN_SLICE_MAC_RES:
		/* Points to the TLV body */
		sres = (ep_ran_sres_TLV *)(buf);

		det->l2.rbgs   = ntohs(sres->body.rbgs);

		ep_dbg_dump(
			EP_DBG_3"P - RANS Res TLV: ",
			buf,
			sizeof(ep_ran_sres_TLV));

		break;
	case EP_TLV_RAN_SLICE_MAC_SCHED:
		/* Points to the TLV body */
		ssch = (ep_ran_ssch_TLV *)(buf);

		det->l2.usched = ntohl(ssch->body.user_sched);

		ep_dbg_dump(
			EP_DBG_3"P - RANS Sched TLV: ",
			buf,
			sizeof(ep_ran_ssch_TLV));

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - RANS: Unexpected TLV %d!\n",
			tlv->type);
		break;
	}

	return EP_SUCCESS;
}

/*
 *
 * RAN parsers for messages:
 *
 */

/* Parse EUQ, sEtup Unspecified reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_euq(char * buf, unsigned int size)
{
	/*
	 * Currently the RAN setup request has no body
	 */

	ep_dbg_dump(EP_DBG_2"P - RANS Rep: ", buf, 0);

	return EP_SUCCESS;
}

/* Format EPQ, sEtup Unspecified rePly.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_eup(char * buf, unsigned int size, ep_ran_det * det)
{
	ep_ran_setup * s = (ep_ran_setup *)buf;

	/* Keeps the most updated current buf position */
	char *         c = buf;

	/* Possible info that can be parsed in TLV style */
	ep_ran_mac_sched_TLV * macs;

	if(size < sizeof(ep_ran_setup)) {
		ep_dbg_log(EP_DBG_2"F - RANS Rep: Not enough space!\n");
		return -1;
	}

	s->layer1_cap = htonl(det->l1_mask);
	s->layer2_cap = htonl(det->l2_mask);
	s->layer3_cap = htonl(det->l3_mask);

	c += sizeof(ep_ran_setup);

	/* TLV for MAC scheduler information */
	if(det->l2.mac.slice_sched != EP_RAN_SCHED_INVALID) {
		if(c + sizeof(ep_ran_mac_sched_TLV) > buf + size) {
			ep_dbg_log(EP_DBG_3"F - RANS Rep: Not enough space!\n");
			return -1;
		}

		macs  = (ep_ran_mac_sched_TLV *)c;

		macs->header.type      = htons(EP_TLV_RAN_MAC_SCHED);
		macs->header.length    = htons(sizeof(ep_ran_mac_sched));
		macs->body.slice_sched = htonl(det->l2.mac.slice_sched);

		c += sizeof(ep_ran_mac_sched_TLV);
	}

	ep_dbg_dump(EP_DBG_2"F - RANS Rep: ", buf, c - buf);

	return c - buf;
}

/* Parse EPQ, sEtup Unspecified rePLy, TLV entries.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_eup_TLV(char * buf, ep_ran_det * det)
{
	ep_TLV *           tlv = (ep_TLV *)buf;

	ep_ran_mac_sched * macs;

	/* Decide what to do depending on the TLV type */
	switch(ntohs(tlv->type)) {
	case EP_TLV_RAN_MAC_SCHED:
		/* Points to the TLV body */
		macs = (ep_ran_mac_sched *)(buf + sizeof(ep_TLV));
		det->l2.mac.slice_sched = ntohl(macs->slice_sched);

		ep_dbg_dump(EP_DBG_3"P - RANS TLV: ",
			buf, sizeof(ep_ran_mac_sched_TLV));

		break;
	default:
		ep_dbg_log(EP_DBG_3"P - RANS TLV: Unexpected token %d!\n", tlv->type);
		break;
	}

	return EP_SUCCESS;
}

/* Parse EPQ, sEtup Unspecified rePLy.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_eup(char * buf, unsigned int size, ep_ran_det * det)
{
	char *         c;
	ep_ran_setup * s = (ep_ran_setup *)buf;
	ep_TLV *       tlv;

	if(size < sizeof(ep_ran_setup)) {
		ep_dbg_log(EP_DBG_2"P - RANS Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(!det) {
		return EP_SUCCESS;
	}

	det->l1_mask = ntohl(s->layer1_cap);
	det->l2_mask = ntohl(s->layer2_cap);
	det->l3_mask = ntohl(s->layer3_cap);

	ep_dbg_dump(EP_DBG_2"P - RANS Rep: ", buf, sizeof(ep_ran_setup));

	c = buf + sizeof(ep_ran_setup);

	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) > buf + size) {
			ep_dbg_log(EP_DBG_3"P - RANS Rep: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + ntohs(tlv->length),
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_ran_eup_TLV(c, det);

		c  += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

/*
 *
 * Parser slice primitives for RAN:
 *
 */

/* Format SUQ, Slice Unspecified reQuest
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_suq(char * buf, unsigned int size, slice_id_t id)
{
	ep_ran_sinf * r = (ep_ran_sinf *)buf;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"F - RANS Unspec Req: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Unspec Req: ", buf, sizeof(ep_ran_sinf));

	return sizeof(ep_ran_sinf);
}

/* Parse SUQ, Slice Unspecified reQuest
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_suq(char * buf, unsigned int size, slice_id_t * id)
{
	ep_ran_sinf * r = (ep_ran_sinf *)buf;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"P - RANS Unspec Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(r->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Unspec Req: ", buf, sizeof(ep_ran_sinf));

	return EP_SUCCESS;
}

/* Format SUP, Slice Unspecified rePly.
 * This message formats a single slice with its details.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_sup(
	char * buf, unsigned int size, slice_id_t id, ep_ran_slice_det * det)
{
	int           s = sizeof(slice_id_t);
	ep_ran_sinf * r = (ep_ran_sinf *) buf;

	r->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Unspec Rep: ", buf, sizeof(ep_ran_sinf));

	/* Time to check if to create TLVs for additional options */
	if(det) {
		s += epf_ran_TLV(
			buf + sizeof(ep_ran_sinf),
			size - sizeof(ep_ran_sinf),
			det);
	}

	return s;
}

/* Parses SUP, Slice Unspecified rePly.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_sup(
	char * buf, unsigned int size, slice_id_t * id, ep_ran_slice_det * det)
{
	char *        c = buf + sizeof(ep_ran_sinf);
	ep_ran_sinf * r = (ep_ran_sinf *) buf;
	ep_TLV *      tlv;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"P - RANS Unspec Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(r->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Unspec Rep: ", buf, sizeof(ep_ran_sinf));

	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) > buf + size) {
			ep_dbg_log(
				EP_DBG_3"P - RANS Rep: TLV size %d > %d\n",
				ntohs(sizeof(ep_TLV)) + ntohs(tlv->length),
				(buf + size) - c);

			break;
		}

		/* Explore the single token */
		epp_ran_TLV(c, det);

		/* To the next tag */
		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

/* Format SAQ, Slice Add reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_saq(
	char * buf, unsigned int size, slice_id_t id, ep_ran_slice_det * det)
{
	int           s = sizeof(ep_ran_sinf);
	ep_ran_sinf * r = (ep_ran_sinf *)buf;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"F - RANS Add Req: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Add Req: ", buf, sizeof(ep_ran_sinf));

	if(det) {
		s += epf_ran_TLV(
			buf + sizeof(ep_ran_sinf),
			size - sizeof(ep_ran_sinf),
			det);
	}

	return s;
}

/* Parse SAQ, Slice Add reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_saq(
	char * buf, unsigned int size, slice_id_t * id, ep_ran_slice_det * det)
{
	char *        c = buf + sizeof(ep_ran_sinf);
	ep_ran_sinf * r = (ep_ran_sinf *)buf;
	ep_TLV *      tlv;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"P - RANS Add Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(r->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Add: ", buf, sizeof(ep_ran_sinf));

	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) > buf + size) {
			ep_dbg_log(EP_DBG_3"P - RANS Add Req: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) + ntohs(tlv->length),
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		if(epp_ran_TLV(c, det)) {
			return EP_ERROR;
		}

		/* To the next tag */
		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

/* Format SRQ, Slice Rem reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_srq(char * buf, unsigned int size, slice_id_t id)
{
	ep_ran_sinf * r = (ep_ran_sinf *)buf;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"F - RANS Rem Req: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Rem Req: ", buf, sizeof(ep_ran_sinf));

	return sizeof(ep_ran_sinf);
}

/* Parse SRQ, Slice Rem reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_srq(char * buf, unsigned int size, slice_id_t * id)
{
	ep_ran_sinf * r = (ep_ran_sinf *)buf;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"P - RANS Rem: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(r->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Rem Req: ", buf, sizeof(ep_ran_sinf));

	return EP_SUCCESS;
}

/* Format SSQ, Slice Set reQuest.
 * Returns the size in bytes of the formatted area.
 */
int epf_ran_ssq(
	char * buf, unsigned int size, slice_id_t id, ep_ran_slice_det * det)
{
	int           s = sizeof(ep_ran_sinf);
	ep_ran_sinf * r = (ep_ran_sinf *)buf;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"F - RANS Set Req: Not enough space!\n");
		return -1;
	}

	r->id = htobe64(id);

	ep_dbg_dump(EP_DBG_2"F - RANS Set Req: ", buf, sizeof(ep_ran_sinf));

	if(det) {
		s+= epf_ran_TLV(
			buf + sizeof(ep_ran_sinf),
			size - sizeof(ep_ran_sinf),
			det);
	}

	return s;
}

/* Parse SRQ, Slice Set reQuest.
 * Returns the SUCCESS/FAILED error codes.
 */
int epp_ran_ssq(
	char * buf, unsigned int size, slice_id_t * id, ep_ran_slice_det * det)
{
	char *        c   = buf + sizeof(ep_ran_sinf);
	ep_ran_sinf * r   = (ep_ran_sinf *)buf;
	ep_TLV *      tlv;

	if(size < sizeof(ep_ran_sinf)) {
		ep_dbg_log(EP_DBG_2"P - RANS Set Req: Not enough space!\n");
		return EP_ERROR;
	}

	if(id) {
		*id = be64toh(r->id);
	}

	ep_dbg_dump(EP_DBG_2"P - RANS Set Req: ", buf, sizeof(ep_ran_sinf));

	/* Continue until the end of the given array */
	while(c < buf + size) {
		tlv = (ep_TLV *)c;

		/* Reading next TLV token will overflow the buffer? */
		if(c + sizeof(ep_TLV) + ntohs(tlv->length) > buf + size) {
			ep_dbg_log(
				EP_DBG_3"P - RANS Set Size: TLV %d > %d\n",
				ntohs(sizeof(ep_TLV)) +  ntohs(tlv->length),
				(buf + size) - c);
			break;
		}

		/* Explore the single token */
		epp_ran_TLV(c, det);

		/* To the next tag */
		c += sizeof(ep_TLV) + ntohs(tlv->length);
	}

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ran_rep_success(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;
	/* Check of given buffer size here */

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RAN Fail: Invalid buffer!\n");
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
		type,
		EP_OPERATION_SUCCESS);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_single_ran_rep_fail(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	slice_id_t   slice_id)
{
	int          ms = 0;
	int          ret= 0;

	slice_id_t * s;
	/* Check of given buffer size here */

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RAN Fail: Invalid buffer!\n");
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
		type,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	if(ret + sizeof(uint64_t) > size) {
		ep_dbg_log(EP_DBG_2"F - Single RAN Fail: No space!\n");
		return -1;
	}

	s = (slice_id_t *)(buf + ret);
	*s = htobe64(slice_id);

	ret += sizeof(uint64_t);

	ep_dbg_log(EP_DBG_2"F - Single RAN Fail: % " PRIu64 "\n", slice_id);

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_single_ran_rep_ns(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	slice_id_t   slice_id)
{
	int ms = 0;
	int ret= 0 ;
	/* Check of given buffer size here */

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RAN NS: Invalid buffer!\n");
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
		type,
		EP_OPERATION_NOT_SUPPORTED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

/*
 *
 * Parser public procedures for RAN Setup messages
 *
 */

int epf_single_ran_setup_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0 ;

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RANS Req: Invalid buffer!\n");
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
		EP_ACT_RAN_SETUP,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_euq(buf + ret, size - ret);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_setup_req(
	char *       buf,
	unsigned int size)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RAN Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_euq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)));
}

int epf_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	ep_ran_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log(EP_DBG_2"F - Single RAN Rep: Invalid buffer!\n");
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
		EP_ACT_RAN_SETUP,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_eup(
		buf + ret,
		size - ret,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	ep_ran_det * det)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RAN Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_eup(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

/*
 *
 * Parser public procedures for RAN Slicing messages
 *
 */

int epf_single_ran_slice_req(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RANT Req: Invalid buffer!\n");
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_suq(
		buf + ret,
		size - ret,
		slice_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_req(
	char *       buf,
	unsigned int size,
	slice_id_t * slice_id)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RANT Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_suq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		slice_id);
}

int epf_single_ran_slice_rep(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log(EP_DBG_2"F - Single RANT Rep: Invalid buffer!\n");
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_sup(
		buf + ret,
		size - ret,
		slice_id,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_rep(
	char *             buf,
	unsigned int       size,
	slice_id_t *       slice_id,
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RANT Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_sup(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		slice_id,
		det);
}

int epf_single_ran_slice_add(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf || !det) {
		ep_dbg_log(EP_DBG_2"F - Single RANT Add: Invalid buffer!\n");
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_ADD);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_saq(
		buf + ret,
		size - ret,
		slice_id,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_add(
	char *             buf,
	unsigned int       size,
	slice_id_t *       slice_id,
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_2"P - Single RANT Add: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_saq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		slice_id,
		det);
}

int epf_single_ran_slice_rem(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log("F - Single RANT Rem: Invalid buffer!\n");
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_REM);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_srq(
		buf + ret,
		size - ret,
		slice_id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_rem(
	char *             buf,
	unsigned int       size,
	slice_id_t *       det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANT Rem: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_srq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		det);
}

int epf_single_ran_slice_set(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_2"F - Single RANT Rem: Invalid buffer!\n");
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
		EP_ACT_RAN_SLICE,
		EP_OPERATION_SET);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ran_ssq(
		buf + ret,
		size - ret,
		slice_id,
		det);

	if(ms < 0) {
		return ms;
	}

	ret += ms;

	/* Inject the message size */
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ran_slice_set(
	char *             buf,
	unsigned int       size,
	slice_id_t *       slice_id,
	ep_ran_slice_det * det)
{
	if(!buf) {
		ep_dbg_log("P - Single RANT Rem: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ran_ssq(
		buf  +  sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size - (sizeof(ep_hdr) + sizeof(ep_s_hdr)),
		slice_id,
		det);
}
