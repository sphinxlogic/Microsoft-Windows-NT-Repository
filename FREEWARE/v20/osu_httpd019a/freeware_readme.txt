OSU HTTP server 1.9a, SOFTWARE,  DECthreads-based HTTP server.

The OSU HTTP server is a network daemon for serving documents to the
World Wide Web using the HyperText Transport Protocol.  It uses DECthreads
to allow a single process to concurrently service multple requests.

Requirements:

  - OpenVMS version 5.5 (VAX or Alpha) or higher (DECthreads is bundled with
    the OS).

  - TCP/IP support, the following vendors are supported:
	DEC TCP/IP services for OpenVMS (UCX), TGV Multinet,
	Wollongong Pathway, Process Software TCPWare, CMU TCP/IP

  - DECnet. Used for script processing.

Installation:

  The aaareadme.txt file in the [.src] directory for information on
  building and installing the server.

The executeables in the demo were compiled and linked under OpenVMS 6.1
