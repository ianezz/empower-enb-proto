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

#include <arpa/inet.h>

#include <emproto.h>

int epf_single(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	ep_op_type   op)
{
	ep_s_hdr * h = (ep_s_hdr *)(buf);

	if(size < sizeof(ep_s_hdr)) {
		ep_dbg_log(EP_DBG_1"F - SING: Not enough space!\n");
		return -1;
	}

	h->type = htons(type);
	h->op   = (uint8_t)op;

	ep_dbg_dump(EP_DBG_1"F - SING: ", buf, sizeof(ep_s_hdr));

	return sizeof(ep_s_hdr);
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

ep_act_type epp_single_type(char * buf, unsigned int size)
{
	ep_s_hdr * h = (ep_s_hdr *)(buf + sizeof(ep_hdr));

	if(size < sizeof(ep_hdr) + sizeof(ep_s_hdr)) {
		ep_dbg_log(EP_DBG_0"P - Single Type: Not enough space!\n");
		return EP_ACT_INVALID;
	}

	ep_dbg_dump(
		EP_DBG_1"P - SING: ",
		buf + sizeof(ep_hdr),
		sizeof(ep_s_hdr));

	return ntohs(h->type);
}

ep_op_type epp_single_op(char * buf, unsigned int size)
{
	ep_s_hdr * h = (ep_s_hdr *)(buf + sizeof(ep_hdr));

	if(size < sizeof(ep_hdr) + sizeof(ep_s_hdr)) {
		ep_dbg_log(EP_DBG_0"P - Single Op: Not enough space!\n");
		return EP_ERROR;
	}

	return (ep_op_type)h->op;
}
