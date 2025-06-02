21-Dec-85 20:10:35-MST,1374;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 21 Dec 85 20:10:31-MST
Received: from usenet by TGR.BRL.ARPA id a025820; 21 Dec 85 21:41 EST
From: Ken Lalonde <kwlalonde@watmath.uucp>
Newsgroups: net.sources.bugs
Subject: Serious bug in "zonk"
Message-ID: <588@watmath.UUCP>
Date: 20 Dec 85 11:32:47 GMT
To:       unix-sources-bugs@BRL-TGR.ARPA

Last September I posted the code and support programs for "zonk", a
kill-by-uid system call.  There is a bug in the system call code that
can cause a signal to be sent to as-yet unborn processes. 
The corrected code follows.

--

/*
 *  Zonk system call - apply a signal to every process owned by a user.
 *  A count of the affected processes is returned.
 *  If passed signal zero, no signal is sent; only the count is returned.
 */
zonk()
{
	struct a {
		int	uid;
		int	sig;
	} *uap = (struct a *)u.u_ap;
	register int sig = uap->sig;
	register int count, uid;
	register struct proc *p;

	if ((uid = uap->uid) != u.u_uid && !suser())
		return;
	if (uid == 0 && sig || (unsigned)sig >= NSIG) {
		u.u_error = EINVAL;
		return;
	}
	for (count = 0, p = proc; p < procNPROC; p++) {
		if (p->p_stat == NULL)
			continue;
		if (p->p_uid != uid)
			continue;
		if (sig)
			psignal(p, sig);
		count++;
	}
	u.u_r.r_val1 = count;
}
