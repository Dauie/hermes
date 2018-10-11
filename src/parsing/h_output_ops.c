# include "../../incl/job.h"
# include "../../incl/parser.h"

int			h_append_output(t_mgr *mgr)
{
	mgr->job.opts.bitops.apnd_file = true;
	return (SUCCESS);
}

int			h_norm_output(t_mgr *mgr)
{
	mgr->job.opts.bitops.output_norm = true;
	return (SUCCESS);
}

int			h_show_only_open_ports(t_mgr *mgr)
{
	mgr->job.opts.bitops.show_only_open_ports = true;
	return (SUCCESS);
}
