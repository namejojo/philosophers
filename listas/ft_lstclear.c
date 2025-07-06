/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 17:21:27 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/06 21:01:18 by jlima-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

void	ft_lstclear(t_list **lst, int nbr)
{
	t_list	*right_node;

	if (lst == NULL || *lst == NULL)
		return ;
	while ((*lst)->right != NULL && --nbr > 0)
	{
		pthread_mutex_destroy(&((*lst)->fork_prot));
		right_node = (*lst)->right;
		free (*lst);
		*lst = right_node;
	}
	pthread_mutex_destroy(&(*lst)->fork_prot);
	free (*lst);
	*lst = NULL;
}
