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

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_schedule(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	ep_op_type   op,
	uint32_t     interval)
{
	ep_c_hdr * h = (ep_c_hdr *)(buf);

	if(size < sizeof(ep_hdr) + sizeof(ep_c_hdr)) {
		ep_dbg_log(EP_DBG_1"F - SCHED: Not enough space!\n");
		return -1;
	}

	h->type     = htons(type);
	h->op       = (uint8_t)op;
	h->interval = htonl(interval);

	ep_dbg_dump(EP_DBG_1"F - SCHE: ", buf, sizeof(ep_c_hdr));

	return sizeof(ep_c_hdr);
}

uint32_t epp_sched_interval(char * buf, unsigned int size)
{
	ep_c_hdr * h = (ep_c_hdr *)(buf + sizeof(ep_hdr));

	if(size < sizeof(ep_hdr) + sizeof(ep_c_hdr)) {
		ep_dbg_log(EP_DBG_0"P - SCHED Int: Not enough space!\n");
		return 0;
	}

	return ntohl(h->interval);
}

ep_act_type epp_schedule_type(char * buf, unsigned int size)
{
	ep_c_hdr * h = (ep_c_hdr *)(buf + sizeof(ep_hdr));

	if(size < sizeof(ep_hdr) + sizeof(ep_c_hdr)) {
		ep_dbg_log(EP_DBG_0"P - SCHED Type: Not enough space!\n");
		return EP_ACT_INVALID;
	}

	ep_dbg_dump(
		EP_DBG_1"P - SCHE: ",
		buf + sizeof(ep_hdr),
		sizeof(ep_c_hdr));

	return ntohs(h->type);
}
