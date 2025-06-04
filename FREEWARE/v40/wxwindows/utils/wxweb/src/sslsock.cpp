////////////////////////////////////////////////////////////////////////
// This module assumes the presence of SSLeay for compilation.
////////////////////////////////////////////////////////////////////////

#if USE_SSL

#include "sslsock.h"

static SSL_CTX* ssl_ctx = 0;


////////////////////////////////////////////////////////////////////////

SSL_SockClient::SSL_SockClient(unsigned flags, char eom) :
	SockClient(flags, eom)
{
	if (!ssl_ctx)
	{
		ssl_ctx = (SSL_CTX*)SSL_CTX_new();
	}

	ssl_con = (SSL*)SSL_new(ssl_ctx);
}

////////////////////////////////////////////////////////////////////////

int SSL_SockClient::Close()
{
	SSL_free(ssl_con);

	return SockClient::Close();
}

////////////////////////////////////////////////////////////////////////

int SSL_SockClient::Connect(const char* host, const char* service)
{
	int stat;

	if ((stat = SockClient::Connect(host, service)) < 0)
		return stat;

	SSL_set_fd(ssl_con, GetFD());

	SSL_set_verify(ssl_con, SSL_VERIFY_NONE, 0);

	SSL_set_pref_cipher(ssl_con, "CBC-DES-MD5:RC4-MD5:CBC3-DES-MD5:EXP-RC4-MD5:CFB-DES-M1:CFB-DES-NULL");

	X509_set_default_verify_paths(ssl_ctx->cert);

	if (SSL_connect(ssl_con) <= 0)
		return -1;

	SSL_set_read_ahead(ssl_con, 0);

	return stat;
}

////////////////////////////////////////////////////////////////////////

int SSL_SockClient::Read(char* buffer, int nbytes)
{
	return SSL_read(ssl_con, buffer, nbytes);
}

////////////////////////////////////////////////////////////////////////

int SSL_SockClient::Write(const char* buffer, int nbytes)
{
	return SSL_write(ssl_con, buffer, nbytes);
}

////////////////////////////////////////////////////////////////////////

int SSL_SockClient::ReadMsg(char* buffer, int nbytes)
{
	char* dst = buffer;

	while (nbytes-- > 0)
	{
		int len;
		char ch;

		if ((len = SSL_read(ssl_con, &ch, 1)) < 0)
			return -1;

		if (!len)
			return 0;

		*dst++ = ch;

		if (ch == eom)
			break;
	}

	return (int)(dst - buffer);
}

////////////////////////////////////////////////////////////////////////

int SSL_SockClient::Wait(long seconds, long microseconds)
{
	if (_Wait(/*seconds*/0, /*microseconds*/0, 5) < 0)
		return 0;

	return 1;
}

#endif
