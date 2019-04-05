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
#if 0
int epf_ccap_rep(
	char *        buf,
	unsigned int  size,
	ep_cell_det * cell)
{
	ep_ccap_rep * rep = (ep_ccap_rep *)buf;

	if(size < sizeof(ep_ccap_rep)) {
		ep_dbg_log(EP_DBG_2"F - CCAP Rep: Not enough space!\n");
		return -1;
	}

	if(!cell) {
		rep->cap        = 0;
		rep->DL_earfcn  = 0;
		rep->DL_prbs    = 0;
		rep->UL_earfcn  = 0;
		rep->UL_prbs    = 0;
	} else {
		rep->cap        = htonl(cell->cap);
		rep->DL_earfcn  = htons(cell->DL_earfcn);
		rep->DL_prbs    = cell->DL_prbs;
		rep->UL_earfcn  = htons(cell->UL_earfcn);
		rep->UL_prbs    = cell->UL_prbs;
	}

	ep_dbg_dump(EP_DBG_2"F - CCAP Rep: ", buf, sizeof(ep_ccap_rep));

	return sizeof(ep_ccap_rep);
}

int epp_ccap_rep(
	char *        buf,
	unsigned int  size,
	ep_cell_det * cell)
{
	ep_ccap_rep * rep = (ep_ccap_rep *)buf;

	if(size < sizeof(ep_ccap_rep)) {
		ep_dbg_log(EP_DBG_2"P - CCAP Rep: Not enough space!\n");
		return EP_ERROR;
	}

	if(cell) {
		cell->cap        = ntohl(rep->cap);
		cell->DL_earfcn  = ntohs(rep->DL_earfcn);
		cell->DL_prbs    = rep->DL_prbs;
		cell->UL_earfcn  = ntohs(rep->UL_earfcn);
		cell->UL_prbs    = rep->UL_prbs;
	}

	ep_dbg_dump(EP_DBG_2"P - CCAP Rep: ", buf, sizeof(ep_ccap_rep));

	return EP_SUCCESS;
}

int epf_ccap_req(char * buf, unsigned int size)
{
	ep_ccap_req * rep = (ep_ccap_req *)buf;

	if(size < sizeof(ep_ccap_req)) {
		ep_dbg_log(EP_DBG_2"F - CCAP Req: Not enough space!\n");
		return -1;
	}

	rep->dummy = 0;

	ep_dbg_dump(EP_DBG_2"F - CCAP Req: ", buf, sizeof(ep_ccap_req));

	return sizeof(ep_ccap_req);
}

int epp_ccap_req(char * buf, unsigned int size)
{
	if(size < sizeof(ep_ccap_req)) {
		ep_dbg_log(EP_DBG_2"P - CCAP Req: Not enough space!\n");
		return EP_ERROR;
	}

	ep_dbg_dump(EP_DBG_2"P - CCAP Req: ", buf, 0);

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ccap_rep_fail(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single CCAP Fail: Invalid buffer!\n");
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
		EP_ACT_CCAP,
		EP_OPERATION_FAIL);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ccap_rep(buf + ret, size - ret, 0);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epf_single_ccap_rep(
	char *        buf,
	unsigned int  size,
	enb_id_t      enb_id,
	cell_id_t     cell_id,
	mod_id_t      mod_id,
	ep_cell_det * cell)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single CCAP Rep: Invalid buffer!\n");
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
		EP_ACT_CCAP,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ccap_rep(buf + ret, size - ret, cell);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_ccap_rep(
	char *        buf,
	unsigned int  size,
	ep_cell_det * cell)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Single CCAP Rep: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ccap_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		cell);
}

int epf_single_ccap_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single CCAP Req: Invalid buffer!\n");
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
		EP_ACT_CCAP,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_ccap_req(buf + ret, size - ret);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ms;
}

int epp_single_ccap_req(char * buf, unsigned int size)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Single CCAP Req: Invalid buffer!\n");
		return EP_ERROR;
	}

	return epp_ccap_req(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size);
}
#endif
