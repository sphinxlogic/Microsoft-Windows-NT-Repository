#include "sysincludes.h"
#include "mtools.h"

int got_signal = 0;

void signal_handler(int dummy)
{
	got_signal = 1;

	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void setup_signal(void)
{
	/* catch signals */
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);
}
