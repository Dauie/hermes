# include "../../incl/job.h"
# include "../../incl/parser.h"

void			h_append_output(t_mgr *mgr)
{
	mgr->job.opts.bitops.apnd_file = TRUE;
}

void			h_norm_output(t_mgr *mgr)
{
	mgr->job.opts.bitops.output_norm = TRUE;
}

void			h_show_only_open_ports(t_mgr *mgr)
{
	mgr->job.opts.bitops.show_only_open_ports = TRUE;
}
