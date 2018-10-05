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
#include "../incl/job.h"
#include "../incl/parser.h"

int			 main(int ac, char **av)
{
	t_mgr	*mgr;

	if (ac < 2)
		return (FAILURE); //h_help();
	if (!(mgr = (t_mgr *)memalloc(sizeof(t_mgr))))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	parse_opts(mgr, ac, av);
	sanity_check(mgr);
	do_exclusions(mgr);
	manager(mgr);
	return (SUCCESS);
}
