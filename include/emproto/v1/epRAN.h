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
 *
 * RADIO ACCESS NETWORK SLICING MESSAGE
 *
 *
 */

#ifndef __EMAGE_RAN_H
#define __EMAGE_RAN_H

#include <stdint.h>

#include "eppri.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*****************************************************************************
 *                                                                           *
 *                          RAN Slice capabilities                           *
 *                                                                           *
 *****************************************************************************/

/* Bitmask values to use in the RAN setup capabilities bitmask */
typedef enum __ep_ran_layer1_caps {
	EP_RAN_LAYER1_CAP_NOTHING = 0,
} RAN_l1_caps;

/* Bitmask values to use in the RAN setup capabilities bitmask */
typedef enum __ep_ran_layer2_caps {
	EP_RAN_LAYER2_CAP_NOTHING     = 0,
	EP_RAN_LAYER2_CAP_RBG_SLICING = 1,
	EP_RAN_LAYER2_CAP_PRB_SLICING = 2,
} RAN_l2_caps;

/* Bitmask values to use in the RAN setup capabilities bitmask */
typedef enum __ep_ran_layer3_caps {
	EP_RAN_LAYER3_CAP_NOTHING = 0,
} RAN_l3_caps;

/* RAN slicing capabilities message */
typedef struct __ep_ran_slicing_capabilities {
	uint16_t pci;           /* Physical Cell ID */
	uint32_t l1_caps;       /* Mask of capabilities at layer 1 */
	uint32_t l2_caps;       /* Mask of capabilities at layer 2 */
	uint32_t l3_caps;       /* Mask of capabilities at layer 3 */
	uint32_t mac_sched;     /* MAC slice scheduler ID */
	uint16_t max_slices;    /* Maximum amount of supported slices */
}__attribute__((packed)) ep_ran_caps;

/* Contains information to describe the slice capabilities in TLV style */
typedef struct __ep_ran_slicing_capabilities_TLC {
	ep_TLV      header;
	ep_ran_caps body;
} __attribute__((packed)) ep_ran_caps_TLV;

/*****************************************************************************
 *                                                                           *
 *                          RAN Slicing Setup                                *
 *                                                                           *
 *****************************************************************************/

/* Contains information to describe the slice scheduler */
typedef struct __ep_ran_mac_sched {
	sched_id_t slice_sched;
} __attribute__((packed)) ep_ran_mac_sched;

/* Contains information to describe the slice scheduler in TLV style */
typedef struct __ep_ran_mac_sched_TLV {
	ep_TLV           header;
	ep_ran_mac_sched body;
} __attribute__((packed)) ep_ran_mac_sched_TLV;

 /* RAN Setup reply message */
typedef struct __ep_ran_setup {
	uint32_t layer1_cap;	/* Mask of capabilities at layer 1 */
	uint32_t layer2_cap;	/* Mask of capabilities at layer 2 */
	uint32_t layer3_cap;	/* Mask of capabilities at layer 3 */
}__attribute__((packed)) ep_ran_setup;

/*****************************************************************************
 *                                                                           *
 *                         RAN Slicing Slice Setup                           *
 *                                                                           *
 *****************************************************************************/

/* RAN slice scheduler information */
typedef struct __ep_ran_slice_sched {
	sched_id_t  user_sched; /* Id of the user scheduler associated */
}__attribute__((packed)) ep_ran_ssch;

/* RAN slice scheduler information in TLV style */
typedef struct __ep_ran_slice_sched_TLV {
	ep_TLV      header;
	ep_ran_ssch body;
}__attribute__((packed)) ep_ran_ssch_TLV;

/* RAN slice resources information */
typedef struct __ep_ran_slice_res {
	uint16_t    rbgs;       /* PRBs assigned to the slice */
}__attribute__((packed)) ep_ran_sres;

/* RAN slice resources information in TLV style */
typedef struct __ep_ran_slice_res_TLV {
	ep_TLV      header;
	ep_ran_sres body;
}__attribute__((packed)) ep_ran_sres_TLV;

 /* RAN Slice info message */
typedef struct __ep_ran_slice_inf {
	slice_id_t  id;	         /* ID of the Slice */
}__attribute__((packed)) ep_ran_sinf;

 /*****************************************************************************
  *                                                                           *
  * Opaque structures                                                         *
  *                                                                           *
  *****************************************************************************/

#define EP_RAN_USERS_MAX	32

typedef struct __ep_ran_slice_l2_details {
	sched_id_t       usched; /* User scheduler ID */
	uint16_t         rbgs;   /* PRBs assigned to the slice */
} ep_ran_slice_l2d;

typedef struct __ep_ran_slice_details {
	/* Users of this slice */
	uint32_t         nof_users;
	rnti_id_t        users[EP_RAN_USERS_MAX];
	ep_ran_slice_l2d l2;     /* ID of the active User scheduler */
} ep_ran_slice_det;

/* Invalid id for a scheduler */
#define EP_RAN_SCHED_INVALID	0

typedef struct __rp_ran_mac_details {
	sched_id_t slice_sched; /* Id of the slice scheduler */
} ep_ran_mac_det;

typedef struct __rp_ran_l2_details {
	ep_ran_mac_det mac;     /* Details at MAC layer */
} ep_ran_l2_det;

typedef struct __ep_ran_setup_details {
	cell_id_t     pci;       /* Physical cell ID */
	uint32_t      l1_mask;   /* Layer 1 capabilities */
	uint32_t      l2_mask;   /* Layer 2 capabilities */
	uint32_t      l3_mask;   /* Layer 3 capabilities */
	ep_ran_l2_det l2;        /* Layer 2 details*/
	uint16_t      max_slices;/* Max amount of supported slices */
} ep_ran_det;

/******************************************************************************
 *                                                                            *
 * Operation on single-event messages                                         *
 *                                                                            *
 ******************************************************************************/

/* Format a RAN setup operation success message.
 * Returns the message size or -1 on error.
 */
 #define epf_single_ran_setup_success(b, s, e, c, m)	\
	epf_single_ran_rep_success(b, s, EP_ACT_RAN_SETUP, e, c, m)

/* Format a RAN Slice operation success message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_slice_success(b, s, e, c, m)	\
	epf_single_ran_rep_success(b, s, EP_ACT_RAN_SLICE, e, c, m)

/* Format a RAN operation success message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_rep_success(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/* Format a RAN setup operation failed message.
 * Returns the message size or -1 on error.
 */
 #define epf_single_ran_setup_fail(b, s, e, c, m, i)    \
	epf_single_ran_rep_fail(b, s, EP_ACT_RAN_SETUP, e, c, m, i)

/* Format a RAN Slice operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_slice_fail(b, s, e, c, m, i)     \
	epf_single_ran_rep_fail(b, s, EP_ACT_RAN_SLICE, e, c, m, i)

/* Format a RAN operation failed message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_rep_fail(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	slice_id_t   slice_id);

/* Format a RAN setup operation failed message.
 * Returns the message size or -1 on error.
 */
 #define epf_single_ran_setup_ns(b, s, e, c, m, i) \
	epf_single_ran_rep_ns(b, s, EP_ACT_RAN_SETUP, e, c, m, i)

/* Format a RAN Slice operation failed message.
 * Returns the message size or -1 on error.
 */
#define epf_single_ran_slice_ns(b, s, e, c, m, i)  \
	epf_single_ran_rep_ns(b, s, EP_ACT_RAN_SLICE, e, c, m, i)

/* Format a RAN operation not supported message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_rep_ns(
	char *       buf,
	unsigned int size,
	ep_act_type  type,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	slice_id_t   slice_id);

/******************************************************************************/

/* Formats a RAN setup request message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_setup_req(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id);

/* Parses a RAN setup request message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_setup_req(
	char *       buf,
	unsigned int size);

/* Formats a RAN setup reply message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	enb_id_t     enb_id,
	cell_id_t    cell_id,
	mod_id_t     mod_id,
	ep_ran_det * ran);

/* Parses a RAN setup reply message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_setup_rep(
	char *       buf,
	unsigned int size,
	ep_ran_det * ran);

/******************************************************************************/

/* Formats a RAN Slice request message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_slice_req(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id);

/* Parses a RAN Slice request message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_slice_req(
	char *       buf,
	unsigned int size,
	slice_id_t * slice_id);

/* Formats a RAN Slice reply message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_slice_rep(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det);

/* Parses a RAN Slice reply message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_slice_rep(
	char *             buf,
	unsigned int       size,
	slice_id_t *       slice_id,
	ep_ran_slice_det * det);

/* Formats a RAN Slice add message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_slice_add(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det);

/* Parses a RAN Slice add message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_slice_add(
	char *             buf,
	unsigned int       size,
	slice_id_t *       slice_id,
	ep_ran_slice_det * det);

/* Formats a RAN Slice remove message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_slice_rem(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id);

/* Parses a RAN Slice remove message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_slice_rem(
	char *             buf,
	unsigned int       size,
	slice_id_t *       id);

/* Formats a RAN Slice set message.
 * Returns the message size or -1 on error.
 */
int epf_single_ran_slice_set(
	char *             buf,
	unsigned int       size,
	enb_id_t           enb_id,
	cell_id_t          cell_id,
	mod_id_t           mod_id,
	slice_id_t         slice_id,
	ep_ran_slice_det * det);

/* Parses a RAN Slice set message.
 * Returns EP_SUCCESS on success, otherwise a negative error code.
 */
int epp_single_ran_slice_set(
	char *             buf,
	unsigned int       size,
	slice_id_t *       slice_id,
	ep_ran_slice_det * det);

/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

/******************************************************************************
 * Operation on trigger-event messages                                        *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_RAN_H */
