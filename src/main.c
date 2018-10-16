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


int			 main(int ac, char **av)
{
	t_mgr	*mgr;

	if (ac < 2)
		return (FAILURE); // usage();
	if (!(mgr = (t_mgr*)memalloc(sizeof(t_mgr))))
		hermes_error(FAILURE, 2, "malloc()", strerror(errno));
	if (parse_opts(mgr, ac, av) == FAILURE)
		exit(EXIT_FAILURE);
	sanity_check(mgr);
	do_exclusions(mgr);
	manager_loop(mgr);
	return (SUCCESS);
}
