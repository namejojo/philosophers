/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 17:21:27 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/06 20:36:45 by jlima-so         ###   ########.fr       */
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
		right_node = (*lst)->right;
		free (*lst);
		*lst = right_node;
	}
	free (*lst);
	*lst = NULL;
}
