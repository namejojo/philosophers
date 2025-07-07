/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 17:30:25 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/07 17:36:30 by jlima-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*run_code(void *arg)
{
	printf("%d\n", *((int*)arg));
	return (NULL);
}

int	main(void)
{
	pthread_t	nof;
	int			*arg;
	
	arg = malloc (sizeof(int));
	*arg = 0;
	pthread_create(&nof, NULL, run_code, arg);
	pthread_join(nof, NULL);
	free(arg);
}