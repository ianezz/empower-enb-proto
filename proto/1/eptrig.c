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

int epf_trigger(
	char * buf, unsigned int size,
	ep_act_type type,
	ep_op_type  op)
{
	ep_t_hdr * h = (ep_t_hdr *)(buf);

	if(size < sizeof(ep_t_hdr)) {
		ep_dbg_log(EP_DBG_1"F - TRIG: Not enough space!\n");
		return -1;
	}

	h->type = htons(type);
	h->op   = (uint8_t)op;

	ep_dbg_dump(EP_DBG_1"F - TRIG: ", buf, sizeof(ep_t_hdr));

	return sizeof(ep_t_hdr);
}
/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

ep_op_type epp_trigger_op(char * buf, unsigned int size)
{
	ep_t_hdr * h = (ep_t_hdr *)(buf + sizeof(ep_hdr));

	if(size < sizeof(ep_hdr) + sizeof(ep_t_hdr)) {
		ep_dbg_log(EP_DBG_0"F - TRIG Op: Not enough space!\n");
		return EP_TYPE_INVALID_MSG;
	}

	return (ep_act_type)h->op;
}

ep_act_type epp_trigger_type(char * buf, unsigned int size)
{
	ep_t_hdr * h = (ep_t_hdr *)(buf + sizeof(ep_hdr));

	if(size < sizeof(ep_hdr) + sizeof(ep_t_hdr)) {
		ep_dbg_log(EP_DBG_0"F - TRIG Type: Not enough space!\n");
		return EP_ACT_INVALID;
	}

	ep_dbg_dump(
		EP_DBG_1"P - TRIG: ",
		buf + sizeof(ep_hdr),
		sizeof(ep_t_hdr));

	return ntohs(h->type);
}
