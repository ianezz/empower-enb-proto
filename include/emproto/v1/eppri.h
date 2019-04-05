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

/*
 * Contains some primitives types to be used anywhere in the protocol.
 */

#ifndef __EMAGE_PRIMITIES_H
#define __EMAGE_PRIMITIES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef uint64_t enb_id_t;      /* Definition of the enb-id */
typedef uint16_t cell_id_t;     /* Definition of the cell id */
typedef uint32_t mod_id_t;      /* Definition of the module id */

typedef uint16_t rnti_id_t;     /* Definition of the RNTI id */
typedef uint32_t plmn_id_t;     /* Definition of the PLMN id */
typedef uint64_t imsi_id_t;     /* Definition of the IMSI id */
typedef uint32_t tmsi_id_t;     /* Definition of the TMSI id */
typedef uint64_t imei_id_t;     /* Definition of the IMEI id */

typedef uint32_t sched_id_t;    /* Definition of the scheduler id */

typedef uint16_t  meas_id_t;    /* Definition of the measurement id */

/*
 * RAN types
 */

typedef uint64_t slice_id_t;    /* Definition of the slice id */

/*
 * TLV types
 */

typedef uint16_t tlv_type_t;    /* Definition of TLV type field */
typedef uint16_t tlv_length_t;  /* Definition of TLV length field */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PRIMITIES_H */
