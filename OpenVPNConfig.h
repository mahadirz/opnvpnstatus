#ifndef _OPENVPNCONFIG_H_
#define _OPENVPNCONFIG_H_

typedef struct OpenVPNCOnf
{
	int port;
	char proto[4];
	char status[256];

} MiniOpenVPNConf;

int ReadOpenVPNConf ( char * ,MiniOpenVPNConf *);
#endif