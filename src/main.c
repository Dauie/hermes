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

void		free_syn_ports(t_mgr **mgr)
{
	if ((*mgr)->env.syn_ports->ports)
		del_list(&(*mgr)->env.syn_ports->ports, true);
	if ((*mgr)->env.syn_ports->prtrngs)
		del_list(&(*mgr)->env.syn_ports->prtrngs, true);
	free((*mgr)->env.syn_ports);
}

void		destroy_manager(t_mgr **mgr)
{
	if ((*mgr)->results.results)
		del_list(&(*mgr)->results.results, true);
	if ((*mgr)->targets.ips)
		del_list(&(*mgr)->targets.ips, true);
	if ((*mgr)->targets.iprngs)
		del_list(&(*mgr)->targets.iprngs, true);
	if ((*mgr)->env.ports.ports)
		del_list(&(*mgr)->env.ports.ports, true);
	if ((*mgr)->env.ports.prtrngs)
		del_list(&(*mgr)->env.ports.prtrngs, true);
	if ((*mgr)->env.ports.flat)
		free((*mgr)->env.ports.flat);

	if ((*mgr)->env.ack_ports)
	{
		if ((*mgr)->env.ack_ports->ports)
			del_list(&(*mgr)->env.ack_ports->ports, true);
		if ((*mgr)->env.ack_ports->prtrngs)
			del_list(&(*mgr)->env.ack_ports->prtrngs, true);
		if ((*mgr)->env.ack_ports->flat)
			free((*mgr)->env.ack_ports);
		free((*mgr)->env.ack_ports);
	}
	if ((*mgr)->env.syn_ports)
	{
		if ((*mgr)->env.syn_ports->ports)
			del_list(&(*mgr)->env.syn_ports->ports, true);
		if ((*mgr)->env.syn_ports->prtrngs)
			del_list(&(*mgr)->env.syn_ports->prtrngs, true);
		if ((*mgr)->env.syn_ports->flat)
			free((*mgr)->env.syn_ports);
		free((*mgr)->env.syn_ports);
	}

	if ((*mgr)->env.udp_ports)
	{
		if ((*mgr)->env.udp_ports->ports)
			del_list(&(*mgr)->env.udp_ports->ports, true);
		if ((*mgr)->env.udp_ports->prtrngs)
			del_list(&(*mgr)->env.udp_ports->prtrngs, true);
		if ((*mgr)->env.udp_ports->flat)
			free((*mgr)->env.udp_ports);
		free((*mgr)->env.udp_ports);
	}
	if ((*mgr)->workers.wrkrs)
	{
		if ((*mgr)->workers.wrkrs->data)
			free((*mgr)->workers.wrkrs->data);
		free((*mgr)->workers.wrkrs);
	}
	free(*mgr);
}

void		convert_assets(t_mgr *mgr)
{
	if (mgr->targets.ips)
		mgr->targets.ips = bst_to_clist(&mgr->targets.ips);
	if (mgr->targets.iprngs)
		mgr->targets.iprngs = bst_to_clist(&mgr->targets.iprngs);
	if (mgr->env.ports.ports)
		mgr->env.ports.ports = bst_to_clist(&mgr->env.ports.ports);
	if (mgr->env.ports.prtrngs)
		mgr->env.ports.prtrngs = bst_to_clist(&mgr->env.ports.prtrngs);
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
	destroy_manager(&mgr);
	return (SUCCESS);
}
