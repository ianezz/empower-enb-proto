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

#ifndef __EMAGE_PROTOCOLS_OPERATION_TYPE_H
#define __EMAGE_PROTOCOLS_OPERATION_TYPE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum __ep_operation_type {
	EP_OPERATION_UNSPECIFIED   = 0, /* Do not care of this field */
	EP_OPERATION_SUCCESS       = 1, /* Success                   */
	EP_OPERATION_FAIL          = 2, /* Failure                   */
	EP_OPERATION_NOT_SUPPORTED = 3, /* Not supported             */
	EP_OPERATION_ADD           = 4, /* Add something             */
	EP_OPERATION_REM           = 5, /* Remove something          */
	EP_OPERATION_SET           = 6, /* Set something             */
	EP_OPERATION_UNSET         = 7, /* Unset something           */
	EP_OPERATION_START         = 8, /* Start something           */
	EP_OPERATION_STOP          = 9, /* Stop something            */
} ep_op_type;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PROTOCOLS_OPERATION_TYPE_H */
