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
	if (!(mgr = new_mgr()))
		return (FAILURE);
	if (parse_opts(mgr, ac, av) == FAILURE)
		exit(EXIT_FAILURE);
	sanity_check(mgr);

	printf("pre exclusions\n");

	print_tree(mgr->job.targets->iprngs, print_iprng_struct);
	print_tree(mgr->job.targets->ips, print_ip_struct);

	do_exclusions(mgr);

	printf("\npre heapify\n");

	print_tree(mgr->job.targets->iprngs, print_iprng_struct);
	print_tree(mgr->job.targets->ips, print_ip_struct);

	heapify(&mgr->job.targets->iprngs, max_heapify);
	heapify(&mgr->job.targets->ips, max_heapify);

	printf("\npost heapify\n");

	print_tree(mgr->job.targets->iprngs, print_iprng_struct);
	print_tree(mgr->job.targets->ips, print_ip_struct);


//	manager_loop(mgr);
	return (SUCCESS);
}
