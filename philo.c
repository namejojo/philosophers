/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 14:24:31 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/06 21:16:09 by jlima-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>

// int	get_order(t_list *philo, int to_find, int loop)
// {
// 	int	count;
// 	int	last_count;
// 	int	group_lcount;
// 	int	loop_count;
	
// 	count = -1;
// 	loop_count = loop /* % philo->info->nbr_of_philosophers */;
// 	while (++count || 1)
// 	{
// 		group_lcount = philo->info->nbr_of_philosophers / 2;
// 		while (group_lcount-- > 0)
// 		{
// 			if (philo->p_nbr == to_find && loop == 1)
// 				return (count);
// 			else if (philo->p_nbr == to_find)
// 				loop_count--;
// 			if (loop_count == 1 && philo->p_nbr == to_find )
// 				last_count = count;
// 			if (loop_count <= 0)
// 				return (count - last_count);
// 			// printf("%d000 %d\n", count, philo->p_nbr);
// 			// fflush(stdout);
// 			philo = philo->right->right;
// 		}
// 		// sleep(1);
// 		if (philo->info->nbr_of_philosophers % 2 - 1)
// 		{
// 			if (2 == philo->p_nbr)
// 				philo = philo->left;
// 			else if (1 == philo->p_nbr)
// 				philo = philo->right;
// 		}
// 		// printf("\n");
// 		// sleep(1);
// 	}
// }
void	gulag(t_list *philo, long total_time)
{
	struct timeval time;

	gettimeofday(&time, NULL);
	if (MICRO * (time.tv_sec - philo->lta.tv_sec) + time.tv_usec - philo->lta.tv_usec >= philo->info->time_to_die)
	{
		total_time = (MICRO * (time.tv_sec - philo->lta.tv_sec) + time.tv_usec - philo->lta.tv_usec) / MILI;
		philo->info->all_alive--;
		while (philo->info->talky_talk == 0);
		pthread_mutex_lock(&philo->info->talky_talk_prot);
		philo->info->talky_talk = 0;
		pthread_mutex_unlock(&philo->info->talky_talk_prot);
		if (philo->info->all_alive == 0)
			printf("%lu %d is dead\n", total_time, philo->p_nbr);
		pthread_exit(NULL);
	}
}

void	get_ready_to_talk(t_list *philo, long total_time)
{
	while (philo->info->talky_talk == 0 && philo->info->all_alive)
		gulag(philo, total_time);
	if (philo->info->all_alive < 1)
		pthread_exit(NULL);

	pthread_mutex_lock(&philo->info->talky_talk_prot);
	philo->info->talky_talk = 0;
	pthread_mutex_unlock(&philo->info->talky_talk_prot);
}

int	ft_thinking(t_list *philo, unsigned long total_time, struct timeval last_time)
{
	struct timeval	time;

	get_ready_to_talk(philo, total_time);

	gettimeofday(&time, NULL);
	total_time += (MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec) / MILI;
	if (philo->info->all_alive)
		printf("%lu %d is thinking\n", total_time, philo->p_nbr);
	else
		pthread_exit(NULL);
	philo->info->talky_talk = 1;
	
	// if (philo->info->time_to_sleep - philo->info->time_to_eat >= 0)
		// usleep(philo->info->time_to_sleep - philo->info->time_to_eat - 1);
	// else
		// usleep(philo->info->time_to_eat - philo->info->time_to_sleep - 1);
	return (0);
}

int	ft_sleeping(t_list *philo, unsigned long total_time, struct timeval last_time)
{
	struct timeval	time;

	get_ready_to_talk(philo, total_time);

	gettimeofday(&time, NULL);
	total_time += (MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec) / MILI;
	if (philo->info->all_alive)
		printf("%lu %d is sleeping\n", total_time, philo->p_nbr);
	else
		pthread_exit(NULL);

	philo->info->talky_talk = 1;

	usleep(philo->info->time_to_sleep);
	return (0);
}

int	ft_eating(t_list *philo, unsigned long total_time, struct timeval last_time)
{
	struct timeval	time;

	while (philo->fork == 0 || philo->left->fork == 0)
		gulag(philo, total_time);


	pthread_mutex_lock(&philo->left->fork_prot);
	philo->left->fork = 0;
	pthread_mutex_lock(&philo->fork_prot);
	philo->fork = 0;
	pthread_mutex_unlock(&philo->left->fork_prot);
	pthread_mutex_unlock(&philo->fork_prot);

	get_ready_to_talk(philo, total_time);
	gettimeofday(&time, NULL);

	total_time += (MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec) / MILI;
	last_time = time;
	if (philo->info->all_alive)
	{
		printf("%lu %d has taken a fork\n", total_time, philo->p_nbr);
		printf("%lu %d has taken a fork\n", total_time, philo->p_nbr);
	}
	else
		pthread_exit(NULL);

	philo->info->talky_talk = 1;

	get_ready_to_talk(philo, total_time);
	gettimeofday(&time, NULL);
	philo->lta = time;
	total_time += (MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec) / MILI;
	if (philo->info->all_alive)
		printf("%lu %d is eating\n", total_time, philo->p_nbr);
	else
		pthread_exit(NULL);
	philo->info->talky_talk = 1;
	usleep(philo->info->time_to_eat);
	gettimeofday(&time, NULL);
	philo->lta = time;
	philo->left->fork = 1;
	philo->fork = 1;
	return (0);
}

void	*run_code(void *arg)
{
	int				ind;
	t_list			*philo;
	struct timeval	time;
	struct timeval	last_time;
	long int		total_time;

	philo = (t_list *)arg;
	total_time = 0;
	ind = -1;
	gettimeofday(&last_time, NULL);
	philo->lta = last_time;
	while (++ind || 1)
	{
		gettimeofday(&time, NULL);
		total_time += (MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec) / MILI;
		last_time = time;
		ft_eating(philo, total_time, last_time);
		// philo->info->all_alive = 0;
		// pthread_exit(NULL);
		gettimeofday(&time, NULL);
		total_time += (MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec) / MILI;
		last_time = time;
		ft_sleeping(philo, total_time, last_time);
		gettimeofday(&time, NULL);
		total_time += (MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec) / MILI;
		last_time = time;
		ft_thinking(philo, total_time, last_time);
	}
	return (NULL);
}

t_list	*init_fork_prot(int nbr, t_info *info)
{
	t_list			*philo;
	t_list			*head;
	int				ind;

	philo = ft_lstnew(NULL, NULL, 1, info);
	if (philo == NULL)
		return (NULL);
	head = philo;
	ind = 1;
	while (++ind < nbr)
	{
		philo->right = ft_lstnew(philo, NULL, ind, info);
		if (philo->right == NULL)
			return (ft_lstclear(&head, ind - 1), NULL);
		philo = philo->right;
	}
	philo->right = ft_lstnew(philo, head, nbr, info);
	if (philo->right == NULL)
		return (ft_lstclear(&head, ind - 1), NULL);
	head->left = philo->right;
	return (head);
}

int	init_infosophers(t_info *info)
{
	int				ind;
	t_list			*philo;
	pthread_t		*nof;

	nof = malloc(info->nbr_of_philosophers * sizeof(pthread_t));
	if (nof == NULL)
		return (1);
	philo = init_fork_prot(info->nbr_of_philosophers, info);
	if (philo == NULL)
		return (free(nof), 1);
	ind = -1;
	while (++ind < info->nbr_of_philosophers)
	{
		printf("starting thread %d\n", ind);
		if (pthread_create(nof + ind, NULL, run_code, philo))
			return (free(nof), ft_lstclear(&philo, ind), 1);
		pthread_detach(nof[ind]);
		philo = philo->right;
	}
	while (philo->info->all_alive == 1);
	usleep(1);
	while (ind--)
	{
		printf("closing thread %d\n", ind);
		pthread_join(nof[ind], NULL);
	}
	pthread_mutex_destroy(&philo->info->talky_talk_prot);
	return (free(nof), ft_lstclear(&philo, info->nbr_of_philosophers), exit(0), 0);
}

int	init_info(int ac, char **av, t_info *info)
{
	info->nbr_of_philosophers = ft_atoi(av[1]);
	info->time_to_die = ft_atoi(av[2]) * 1000;
	info->time_to_eat = ft_atoi(av[3]) * 1000;
	info->time_to_sleep = ft_atoi(av[4]) * 1000;
	info->notepme = -1;
	if (ac > 5)
		info->notepme = ft_atoi(av[5]);
	info->talky_talk = 1;
	info->all_alive = 1;
	if (pthread_mutex_init(&info->start_banquet, NULL))
		return (1);
	if (pthread_mutex_init(&info->talky_talk_prot, NULL))
		return (1);
	return (0);
}

int	exit_message(t_info info)
{
	write(2, "invalid number of philosophers\n", \
		(info.nbr_of_philosophers <= 0) * 32);
	write(2, "invalid time to die\n", (info.time_to_die <= 0) * 21);
	write(2, "invalid time to eat\n", (info.time_to_eat <= 0) * 21);
	write(2, "invalid time to sleep\n", (info.time_to_sleep <= 0) * 23);
	write(2, "invalid number of times each philosopher must eat \n", \
		(info.notepme <= 0) * 52);
	return ((info.nbr_of_philosophers <= 0) + (info.time_to_die <= 0) + \
		(info.time_to_eat <= 0) + (info.time_to_sleep <= 0) + \
		(info.notepme <= 0));
}

int	main(int ac, char **av)
{
	t_info	info;

	if (ac < 5 || ac > 6)
		return (write(2, "invalid number of arguments\n", 28));
	if (init_info(ac, av, &info))
		return (1);
	if (exit_message(info))
		return (1);
	if (pthread_mutex_init(&(info.talky_talk_prot), NULL))
		return (1);
	// if (pthread_mutex_init(&(info.start_banquet), NULL))
		// return (1);
	printf("nbr_of_philosophers:%d\ntime_to_die:%d\ntime_to_eat:%d\ntime_to_sleep:%d\nnotepme:%d\n\nstarting now\n", info.nbr_of_philosophers, info.time_to_die / 1000, info.time_to_eat / 1000, info.time_to_sleep / 1000, info.notepme);
	init_infosophers(&info);
}