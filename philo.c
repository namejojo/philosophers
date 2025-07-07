/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 14:24:31 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/07 23:44:10 by jlima-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>

t_list	*init_fork_prot(int nbr, t_info *info)
{
	t_list			*philo;
	t_list			*head;
	int				ind;

	philo = ft_lstnew(NULL, NULL, 1, info);
	if (philo == NULL)
		return (NULL);
	if (nbr == 1)
		return (philo);
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

void	usleep_func(t_list *philo, long int time_to_sleep)
{
	struct timeval	time;
	struct timeval	last_time;
	long int		time_left;

	gettimeofday(&last_time, NULL);
	time_left = 0;
	while (time_left + 10000 < time_to_sleep && philo->info->all_alive)
	{
		usleep(10 * 1000);
		gettimeofday(&time, NULL);
		time_left += MICRO * (time.tv_sec - last_time.tv_sec) \
			+ time.tv_usec - last_time.tv_usec;
		last_time = time;
	}
	if (philo->info->all_alive == 0)
		pthread_exit(NULL);
	else if (time_left < time_to_sleep)
		usleep(time_to_sleep - time_left);
}

void	time_left_alive_aux(t_list *philo, long int count_down)
{
	pthread_mutex_lock(&philo->info->all_alive_prot);
	if (philo->info->all_alive == 1)
	{
		philo->info->all_alive = 0;
		usleep(count_down);
	}
	else
	{
		pthread_mutex_unlock(&philo->info->all_alive_prot);
		pthread_exit(NULL);
	}
	pthread_mutex_unlock(&philo->info->all_alive_prot);
	while (philo->info->talky_talk == 0)
		usleep(0);
	pthread_mutex_lock(&philo->info->talky_talk_prot);
	philo->info->talky_talk = 0;
	pthread_mutex_unlock(&philo->info->talky_talk_prot);
}

void	time_left_alive(t_list *philo, long int time_to_sleep, \
	long int total_time, struct timeval last_time)
{
	struct timeval	time;
	long int		count_down;

	gettimeofday(&time, NULL);
	count_down = philo->info->time_to_die - (MICRO * (time.tv_sec - \
		philo->lta.tv_sec) + time.tv_usec - philo->lta.tv_usec);
	if (count_down - time_to_sleep <= 1000 && philo->info->all_alive == 1)
	{
		time_left_alive_aux(philo, count_down);
		gettimeofday(&time, NULL);
		total_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
			time.tv_usec - last_time.tv_usec;
		last_time = time;
		if (philo->info->all_alive == 0)
			printf("%ld %d is dead\n", total_time / 1000, philo->p_nbr);
		philo->info->talky_talk = 1;
		pthread_exit(NULL);
	}
	else if (philo->info->all_alive == 0)
		pthread_exit(NULL);
}

void	wait_to_talk(t_list *philo, long int time_to_sleep, \
	long int total_time, struct timeval last_time)
{
	struct timeval	time;

	while (philo->info->talky_talk == 0 && philo->info->all_alive)
	{
		gettimeofday(&time, NULL);
		total_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
		time.tv_usec - last_time.tv_usec;
		last_time = time;
		time_left_alive(philo, time_to_sleep, total_time, last_time);
	}
	if (philo->info->all_alive == 0)
		pthread_exit(NULL);
	pthread_mutex_lock(&philo->info->talky_talk_prot);
	philo->info->talky_talk = 0;
	pthread_mutex_unlock(&philo->info->talky_talk_prot);
}

void	ft_sleeping(t_list *philo, \
	long int total_time, struct timeval last_time)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	total_time += MICRO * (time.tv_sec - last_time.tv_sec) \
	+ time.tv_usec - last_time.tv_usec;
	last_time = time;
	wait_to_talk(philo, 0, total_time, last_time);
	printf("%ld %d is sleeping\n", total_time / 1000, philo->p_nbr);
	philo->info->talky_talk = 1;
	time_left_alive(philo, philo->info->time_to_sleep, total_time, last_time);
	usleep_func(philo, philo->info->time_to_sleep);
}

void	ft_thinking(t_list *philo, \
	long int total_time, struct timeval last_time)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	total_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
		time.tv_usec - last_time.tv_usec;
	last_time = time;
	wait_to_talk(philo, 0, total_time, last_time);
	printf("%ld %d is thinking\n", total_time / 1000, philo->p_nbr);
	philo->info->talky_talk = 1;
}

// void	ft_eating_aux(t_list *philo, long int total_time, \
	// struct timeval last_time, struct timeval time)
// {
	// 
// }

void	pick_up_forks(t_list *philo, \
	long int total_time, struct timeval last_time)
{
	struct timeval	time;

	// ft_thinking(philo, total_time, last_time);
	wait_to_talk(philo, 0, total_time, last_time);
	printf("%ld %d is thinking\n", total_time / 1000, philo->p_nbr);
	philo->info->talky_talk = 1;
	while ((philo->fork == 0 || philo->left->fork == 0) \
		&& philo->info->all_alive)
	{
		gettimeofday(&time, NULL);
		total_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
			time.tv_usec - last_time.tv_usec;
		last_time = time;
		time_left_alive(philo, 0, total_time, last_time);
	}
	pthread_mutex_lock(&philo->left->fork_prot);
	pthread_mutex_lock(&philo->fork_prot);
	philo->left->fork = 0;
	philo->fork = 0;
	pthread_mutex_unlock(&philo->left->fork_prot);
	pthread_mutex_unlock(&philo->fork_prot);
}

void	ft_eating(t_list *philo, long int total_time, struct timeval last_time)
{
	struct timeval	time;

	pthread_mutex_lock(&philo->left->fork_prot);
	pthread_mutex_lock(&philo->fork_prot);
	if (philo->left->fork == 1 && philo->fork == 1)
	{
		philo->left->fork = 0;
		philo->fork = 0;
		pthread_mutex_unlock(&philo->left->fork_prot);
		pthread_mutex_unlock(&philo->fork_prot);
	}
	else
	{
		pthread_mutex_unlock(&philo->left->fork_prot);
		pthread_mutex_unlock(&philo->fork_prot);
		pick_up_forks(philo, total_time, last_time);
	}
	gettimeofday(&time, NULL);
	total_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
		time.tv_usec - last_time.tv_usec;
	last_time = time;

	wait_to_talk(philo, 0, total_time, last_time);
	gettimeofday(&time, NULL);
	total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
	last_time = time;
	printf("%ld %d picked up a fork\n", total_time / 1000, philo->p_nbr);
	printf("%ld %d picked up a fork\n", total_time / 1000, philo->p_nbr);
	philo->info->talky_talk = 1;

	if (philo->info->all_alive == 0)
	{
		philo->left->fork = 1;
		philo->fork = 1;
		pthread_exit(NULL);
	}
	gettimeofday(&time, NULL);
	total_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
		time.tv_usec - last_time.tv_usec;
	last_time = time;
	wait_to_talk(philo, 0, total_time, last_time);
	printf("%ld %d is eating\n", total_time / 1000, philo->p_nbr);
	philo->info->talky_talk = 1;
	usleep_func(philo, philo->info->time_to_eat);
	philo->left->fork = 1;
	philo->fork = 1;
	gettimeofday(&philo->lta, NULL);
}

void	pre_run_code(t_list *philo)
{
	if (philo->info->nbr_of_philosophers == 1)
	{
		printf("%d %d is thiking\n", 0, philo->p_nbr);
		usleep(philo->info->time_to_die);
		printf("%lu %d is dead\n", philo->info->time_to_die / 1000, \
			philo->p_nbr);
		philo->info->all_alive = 0;
		free(philo);
		pthread_exit(NULL);
	}
}

void	*run_code(void *arg)
{
	t_list			*philo;
	struct timeval	time;
	struct timeval	last_time;
	long int		total_time;

	philo = (t_list *)arg;
	pre_run_code(philo);
	gettimeofday(&last_time, NULL);
	philo->lta = last_time;
	total_time = 0;
	while (philo->info->all_alive)
	{
		gettimeofday(&time, NULL);
		total_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
			time.tv_usec - last_time.tv_usec;
		last_time = time;
		time_left_alive(philo, 0, total_time, last_time);
		ft_eating(philo, total_time, last_time);
		gettimeofday(&time, NULL);
		total_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
			time.tv_usec - last_time.tv_usec;
		last_time = time;
		ft_sleeping(philo, total_time, last_time);
	}
	return (NULL);
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
		if (pthread_create(nof + ind, NULL, run_code, philo))
			return (free(nof), ft_lstclear(&philo, ind + 1), 1);
		philo = philo->right;
	}
	while (ind--)
		pthread_join(nof[ind], NULL);
	pthread_mutex_destroy(&info->talky_talk_prot);
	return (free(nof), ft_lstclear(&philo, info->nbr_of_philosophers), 0);
}

int	init_info(int ac, char **av, t_info *info)
{
	info->nbr_of_philosophers = ft_atoi(av[1]);
	info->time_to_die = (ft_atoi(av[2])) * 1000;
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

	if (MICRO != 1000000 || MILI != 1000)
		return (printf("please dont do that thats rude :/"));
	if (ac < 5 || ac > 6)
		return (write(2, "invalid number of arguments\n", 28));
	if (init_info(ac, av, &info))
		return (1);
	if (exit_message(info))
		return (1);
	if (pthread_mutex_init(&(info.talky_talk_prot), NULL))
		return (1);
	if (pthread_mutex_init(&(info.all_alive_prot), NULL))
		return (1);
	init_infosophers(&info);
}
