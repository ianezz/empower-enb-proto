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

int epf_hello_rep(
	char *       buf,
	unsigned int size,
	uint32_t     id)
{
	ep_hello_rep * hr = (ep_hello_rep *)buf;

	if(size < sizeof(ep_hello_rep)) {
		ep_dbg_log(EP_DBG_2"F - HELLO Rep: Not enough space!\n");
		return -1;
	}

	hr->id = htonl(id);

	ep_dbg_dump(EP_DBG_2"F - HELLO Rep: ", buf, sizeof(ep_hello_rep));

	return sizeof(ep_hello_rep);
}

int epp_hello_rep(
	char *       buf,
	unsigned int size,
	uint32_t *   id)
{
	ep_hello_rep * hr = (ep_hello_rep *)buf;

	if(size < sizeof(ep_hello_rep)) {
		ep_dbg_log(EP_DBG_2"P - HELLO Rep: Not enough space!\n");
		return -1;
	}

	if(id) {
		*id = ntohl(hr->id);
	}

	ep_dbg_dump(EP_DBG_2"P - HELLO Rep: ", buf, sizeof(ep_hello_rep));

	return 0;
}

int epf_hello_req(
	char *       buf,
	unsigned int size,
	uint32_t     id)
{
	ep_hello_req * hr = (ep_hello_req *)buf;

	if(size < sizeof(ep_hello_req)) {
		ep_dbg_log(EP_DBG_2"F - HELLO Req: Not enough space!\n");
		return -1;
	}

	hr->id = htonl(id);

	ep_dbg_dump(EP_DBG_2"F - HELLO Req: ", buf, sizeof(ep_hello_req));

	return sizeof(ep_hello_req);
}

int epp_hello_req(
	char *       buf,
	unsigned int size,
	uint32_t *   id)
{
	ep_hello_req * hr = (ep_hello_req *)buf;

	if(size < sizeof(ep_hello_req)) {
		ep_dbg_log(EP_DBG_2"P - HELLO Req: Not enough space!\n");
		return -1;
	}

	if(id) {
		*id = ntohl(hr->id);
	}

	ep_dbg_dump(EP_DBG_2"P - HELLO Req: ", buf, sizeof(ep_hello_req));

	return 0;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

int epf_single_hello_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint32_t     id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single HELLO Req: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf, size,
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
		EP_ACT_HELLO,
		EP_OPERATION_UNSPECIFIED);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_hello_req(buf + ret, size - ret, id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}


int epp_single_hello_req(
	char *       buf,
	unsigned int size,
	uint32_t *   id)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Single HELLO Req: Invalid buffer!\n");
		return -1;
	}

	return epp_hello_req(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		id);
}

int epf_single_hello_rep(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint32_t     id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Single HELLO Rep: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf, size,
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
		EP_ACT_HELLO,
		EP_OPERATION_SUCCESS);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_hello_rep(buf + ret, size - ret, id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_single_hello_rep(
	char *       buf,
	unsigned int size,
	uint32_t *   id)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Single HELLO Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_hello_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		id);
}
/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

int epf_sched_hello_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint32_t     interval,
	uint32_t     id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Sched HELLO Req: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf, size,
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
		EP_ACT_HELLO,
		EP_OPERATION_UNSPECIFIED,
		interval);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_hello_req(buf + ret, size - ret, id);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_sched_hello_req(
	char *       buf,
	unsigned int size,
	uint32_t *   id)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Sched HELLO Req: Invalid buffer!\n");
		return -1;
	}

	return epp_hello_req(
		buf + sizeof(ep_hdr) + sizeof(ep_c_hdr),
		size,
		id);
}

int epf_sched_hello_rep(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	uint32_t     interval,
	uint32_t     id)
{
	int ms = 0;
	int ret= 0;

	if(!buf) {
		ep_dbg_log(EP_DBG_0"F - Sched HELLO Rep: Invalid buffer!\n");
		return -1;
	}

	ms = epf_head(
		buf, size,
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
		EP_ACT_HELLO,
		EP_OPERATION_SUCCESS,
		interval);

	if(ms < 0) {
		return ms;
	}

	ret += ms;
	ms   = epf_hello_rep(buf + ret, size - ret, id);

	epf_msg_length(buf, size, ret);

	return ret;
}

int epp_sched_hello_rep(
	char *       buf,
	unsigned int size,
	uint32_t *   id)
{
	if(!buf) {
		ep_dbg_log(EP_DBG_0"P - Sched HELLO Rep: Invalid buffer!\n");
		return -1;
	}

	return epp_hello_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_c_hdr),
		size,
		id);
}
