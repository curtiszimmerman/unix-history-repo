/*
 *
 * ===================================
 * HARP  |  Host ATM Research Platform
 * ===================================
 *
 *
 * This Host ATM Research Platform ("HARP") file (the "Software") is
 * made available by Network Computing Services, Inc. ("NetworkCS")
 * "AS IS".  NetworkCS does not provide maintenance, improvements or
 * support of any kind.
 *
 * NETWORKCS MAKES NO WARRANTIES OR REPRESENTATIONS, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE, AS TO ANY ELEMENT OF THE
 * SOFTWARE OR ANY SUPPORT PROVIDED IN CONNECTION WITH THIS SOFTWARE.
 * In no event shall NetworkCS be responsible for any damages, including
 * but not limited to consequential damages, arising from or relating to
 * any use of the Software or related support.
 *
 * Copyright 1994-1998 Network Computing Services, Inc.
 *
 * Copies of this Software may be made, however, the above copyright
 * notice must be reproduced on all copies.
 *
 *	@(#) $FreeBSD$
 *
 */

/*
 * IP Over ATM Support
 * -------------------
 *
 * Process stack and data input
 *
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <net/if.h>
#include <net/netisr.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netatm/port.h>
#include <netatm/queue.h>
#include <netatm/atm.h>
#include <netatm/atm_sys.h>
#include <netatm/atm_sap.h>
#include <netatm/atm_cm.h>
#include <netatm/atm_if.h>
#include <netatm/atm_stack.h>
#include <netatm/atm_pcb.h>
#include <netatm/atm_var.h>

#include <netatm/ipatm/ipatm_var.h>

#ifndef lint
__RCSID("@(#) $FreeBSD$");
#endif


/*
 * Process VCC Input Data
 * 
 * Arguments:
 *	tok	ipatm connection token (pointer to ipvcc)
 *	m	pointer to input packet buffer chain
 *
 * Returns:
 *	none
 *
 */
void
ipatm_cpcs_data(tok, m)
	void		*tok;
	KBuffer		*m;
{
	struct ipvcc	*ivp = tok;

#ifdef DIAGNOSTIC
	if (ipatm_print) {
		atm_pdu_print(m, "ipatm_input");
	}
#endif

	/*
	 * Handle input packet
	 */
	if (ivp->iv_state != IPVCC_ACTIVE) {
		KB_FREEALL(m);
		ipatm_stat.ias_rcvstate++;
		return;
	}

	/*
	 * IP packet - reset idle timer
	 */
	ivp->iv_idle = 0;

	/*
	 * Pass packet to IP
	 */
	(void) ipatm_ipinput(ivp->iv_ipnif, m);
}


/*
 * IP Input Packet Handler
 * 
 * All IP packets received from various ATM sources will be sent here
 * for final queuing to the IP layer.
 *
 * Arguments:
 *	inp	pointer to packet's receiving IP network interface
 *	m	pointer to packet buffer chain
 *
 * Returns:
 *	0	packet successfully queued to IP layer
 *	else	error queuing packet, buffer chain freed
 *
 */
int
ipatm_ipinput(inp, m)
	struct ip_nif	*inp;
	KBuffer		*m;
{
	int		s;
#if	BSD < 199103
	int		space;
#endif

#ifdef DIAGNOSTIC
	if (ipatm_print) {
		atm_pdu_print(m, "ipatm_ipinput");
	}
#endif

#if defined(BSD)
#if BSD >= 199103

#ifdef DIAGNOSTIC
	if (!KB_ISPKT(m)) {
		panic("ipatm_ipinput: no packet header");
	}
	{
		int	cnt = 0;
		KBuffer	*m0 = m;

		while (m0) {
			cnt += KB_LEN(m0);
			m0 = KB_NEXT(m0);
		}
		if (m->m_pkthdr.len != cnt) {
			panic("ipatm_ipinput: packet length incorrect");
		}
	}
#endif
	/*
	 * Save the input ifnet pointer in the packet header
	 */
	m->m_pkthdr.rcvif = (struct ifnet *)inp->inf_nif;

#else	/* ! BSD >= 199103 */
	/*
	 * Stick ifnet pointer onto front of packet - hopefully 
	 * there'll be room in the first buffer.
	 */
	KB_HEADROOM(m, space);
	if (space < sizeof(struct ifnet *)) {
		KBuffer		*n;

		/*
		 * We have to allocate another buffer and tack it
		 * onto the front of the packet
		 */
		KB_ALLOCPKT(n, sizeof(struct ifnet *),
			KB_F_NOWAIT, KB_T_HEADER);
		if (n == 0) {
			KB_FREEALL(m);
			ipatm_stat.ias_rcvnobuf++;
			return (1);
		}
		KB_LEN(n) = sizeof(struct ifnet *);
		KB_LINKHEAD(n, m);
		m = n;
	} else {
		/*
		 * Header fits, just adjust buffer controls
		 */
		KB_HEADADJ(m, sizeof(struct ifnet *));
	}
	{
		struct ifnet	**p;

		KB_DATASTART(m, p, struct ifnet **);
		*p = (struct ifnet *)inp->inf_nif;
	}
#endif	/* ! BSD >= 199103 */

	/*
	 * Finally, hand packet off to IP.
	 *
	 * NB: Since we're already in the softint kernel state, we
	 * just call IP directly to avoid the extra unnecessary 
	 * kernel scheduling.
	 */
	s = splimp();
	if (IF_QFULL(&ipintrq)) {
		IF_DROP(&ipintrq);
		(void) splx(s);
		KB_FREEALL(m);
		return (1);
	}

	IF_ENQUEUE(&ipintrq, m);
	(void) splx(s);
#if	BSD < 199506
	ipintr();
#else
	schednetisr ( NETISR_IP );
#endif	/* BSD >= 199506 */
#endif	/* defined(BSD) */

	return (0);
}

