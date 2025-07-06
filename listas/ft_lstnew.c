/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 16:57:41 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/06 16:51:07 by jlima-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

t_list	*ft_lstnew(t_list *left, t_list *right, int nbr, t_info *info)
{
	t_list			*new;

	new = malloc(sizeof(t_list));
	if (new == NULL)
		return (NULL);
	if (pthread_mutex_init(&new->fork_prot, NULL))
		return (free(new), NULL);
	new->right = right;
	new->left = left;
	new->p_nbr = nbr;
	new->info = info;
	new->fork = 1;
	new->ate = 0;
	return (new);
}
