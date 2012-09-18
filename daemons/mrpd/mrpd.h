/******************************************************************************

  Copyright (c) 2012, Intel Corporation 
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without 
  modification, are permitted provided that the following conditions are met:
  
   1. Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
  
   2. Redistributions in binary form must reproduce the above copyright 
      notice, this list of conditions and the following disclaimer in the 
      documentation and/or other materials provided with the distribution.
  
   3. Neither the name of the Intel Corporation nor the names of its 
      contributors may be used to endorse or promote products derived from 
      this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/
/*
 * an endpoint implementation of 802.1Q-2011 MRP (MMRP, MVRP, MSRP)
 */

/* Operating specific defines */
#if defined WIN32
#include <winsock2.h>
struct win_timer {
	void *timer_struct;
};
typedef struct win_timer HTIMER;
#define snprintf _snprintf
#elif defined __linux__
#include <netinet/in.h>
typedef int HTIMER;
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#endif

#include <stdint.h> // for uint8_t etc

typedef struct eth_hdr {
	uint8_t	destaddr[6];
	uint8_t	srcaddr[6];
	uint16_t	typelen;
} eth_hdr_t;

/*
 * NumberOfValues is the number of EVENTS encoded in the various 
 * 3/4 PACK Bytes.  Non-multiples of (3) [ for 3-packed ] or (4) 
 * [ for 4-packed ] indicates the trailing events are transmitted 
 * as -0- and should be ignored (and NOT interpreted as an ENDMARK)
 * A value of -0- indicates there are no events to report (a NULL).
 *
 * furthermore, the events are 'vector' operations - the first encoded event
 * operates on the (FirstEvent) attribute, the second encoded event
 * operates on the (FirstEvent+1) attribute, and so forth.
 */
typedef struct mrpdu_message {
	uint8_t	AttributeType;
	uint8_t	AttributeLength;	/* length of FirstValue */
	/* Microsoft does not support 0 length arrays
	 * uint8_t	Data[];
	 * parsing of the data field is application specific - either
	 * a ushort with an attribute list length followed by vector
	 * attributes, or just a list of vector attributes ...
	 */

	/* table should have a trailing NULL (0x0000) indicating the ENDMARK */
} mrpdu_message_t;
#define MRPD_GET_MRPDU_MESSAGE_VECTOR(a,n) (mrpdu_vectorattrib_t *) \
	((unsigned char *)&((a)->AttributeType) + 2 + n)

#define MRPD_SET_MRPDU_MESSAGE_DATA(a, n, d) \
	*(&a->AttributeLength + 1 + n) = d

typedef struct mrpdu {
	uint8_t	ProtocolVersion;
	/* Microsoft does not support 0 length arrays
	 * mrpdu_message_t	MessageList[];
	 * mrpdu should have trailing NULL (0x0000) indicating the ENDMARK */
} mrpdu_t;

#define MRPD_GET_MRPDU_MESSAGE_LIST(a) ((unsigned char *)&((a)->ProtocolVersion) + 1)
#define MRPD_OFFSETOF_MRPD_GET_MRPDU_MESSAGE_LIST (1)

#define MAX_FRAME_SIZE		2000
#define MRPD_PORT_DEFAULT	7500
#define MAX_MRPD_CMDSZ		(1500)

int mrpd_timer_start(HTIMER timerfd, unsigned long value_ms);
int mrpd_timer_stop(HTIMER timerfd);
int mrpd_send_ctl_msg(struct sockaddr_in *client_addr, char *notify_data,
		int notify_len);
int mrpd_init_protocol_socket(uint16_t etype, int *sock,
		unsigned char *multicast_addr);

int mrpd_recvmsgbuf(SOCKET sock, char **buf);


