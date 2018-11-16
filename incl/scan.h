#ifndef FT_NMAP_SCAN_H
# define FT_NMAP_SCAN_H

# include "hermes.h"

typedef struct	s_tcp_head {
	uint32_t	srcprt:16;
	uint32_t	dstprt:16;

	uint32_t	seqnum;
	uint32_t	acknum;

	uint32_t	offset:4;
	uint32_t	rsrved:6;
	uint32_t	urg:1;
	uint32_t	ack:1;
	uint32_t	psh:1;
	uint32_t	rst:1;
	uint32_t	syn:1;
	uint32_t	fin:1;
	uint32_t	window:16;

	uint32_t	chksum:16;
	uint32_t	urgptr:16;
	uint32_t	optins:24;
	uint32_t	pdding:8;
}				t_tcp;

#endif //FT_NMAP_SCAN_H
