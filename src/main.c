/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/01 14:26:49 by rlutt             #+#    #+#             */
/*   Updated: 2018/09/01 14:27:24 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/hermes.h"
#include "../incl/parser.h"

void		convert_assets(t_mgr *mgr)
{
	if (mgr->targets.ips)
		mgr->targets.ips = bst_to_clist(&mgr->targets.ips);
	if (mgr->targets.iprngs)
		mgr->targets.iprngs = bst_to_clist(&mgr->targets.iprngs);
	if (mgr->job.ports.ports)
		mgr->job.ports.ports = bst_to_clist(&mgr->job.ports.ports);
	if (mgr->job.ports.prtrngs)
		mgr->job.ports.prtrngs = bst_to_clist(&mgr->job.ports.prtrngs);
}

int			 main(int ac, char **av)
{
	t_mgr	*mgr;

	if (ac < 2)
		return (FAILURE); // usage();
	if (!(mgr = new_mgr()))
		return (FAILURE);
	if (parse_opts(mgr, ac, av) == FAILURE)
		return (EXIT_FAILURE);
	if (sanity_check(mgr) == FAILURE)
		return (EXIT_FAILURE);
	do_exclusions(mgr);
	convert_assets(mgr);
	manager_loop(mgr);
	return (SUCCESS);
}
