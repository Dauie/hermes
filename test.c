#include "incl/hermes.h"


int				main(void)
{
	t_iface		iface;

	iface.name = "wlp60s0";
	get_iface_info(&iface);
}