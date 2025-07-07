/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 14:24:31 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/07 21:09:49 by jlima-so         ###   ########.fr       */
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
		// printf("time waited %ld--", time_left/1000);
		// fflush(stdout);
		usleep(10 * 1000);
		gettimeofday(&time, NULL);
		time_left += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
		last_time = time;
	}
	// printf("\n");
	if (philo->info->all_alive == 0)
		pthread_exit(NULL);
	else if (time_left < time_to_sleep)
		usleep(time_to_sleep - time_left);
}
void	time_left_alive(t_list *philo, long int time_to_sleep, long int	total_time, struct timeval last_time)
{
	struct timeval	time;
	long int		count_down;

	gettimeofday(&time, NULL);
	count_down = philo->info->time_to_die -
		(MICRO * (time.tv_sec - philo->lta.tv_sec) + time.tv_usec - philo->lta.tv_usec);
	if (count_down - time_to_sleep <= 0 && philo->info->all_alive == 1)
	{
		// fprintf(stderr, "erro\n");
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

		while (philo->info->talky_talk == 0);
		pthread_mutex_lock(&philo->info->talky_talk_prot);
		philo->info->talky_talk = 0;
		pthread_mutex_unlock(&philo->info->talky_talk_prot);
		
		
		gettimeofday(&time, NULL);
		total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
		last_time = time;

		if (philo->info->all_alive == 0)
			printf("%ld %d is dead\n", total_time / 1000, philo->p_nbr);
		philo->info->talky_talk = 1;
		// free(philo);
		// printf("%d out\n", philo->p_nbr);
		pthread_exit(NULL);
	}
	else if (philo->info->all_alive == 0)
	{
		// free(philo);
		// printf("%d out\n", philo->p_nbr);
		pthread_exit(NULL);
	}
	// return (count_down - time_to_sleep);
}

void	wait_to_talk(t_list *philo, long int time_to_sleep, long int total_time, struct timeval last_time)
{
	struct timeval	time;

	while (philo->info->talky_talk == 0 && philo->info->all_alive)
	{
		gettimeofday(&time, NULL);
		total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
		last_time = time;
		time_left_alive(philo, time_to_sleep, total_time, last_time);
	}
	if (philo->info->all_alive == 0)
	{
		// printf("%d out", philo->p_nbr);
		pthread_exit(NULL);
	}
	pthread_mutex_lock(&philo->info->talky_talk_prot);
	philo->info->talky_talk = 0;
	pthread_mutex_unlock(&philo->info->talky_talk_prot);
}

void	ft_sleeping(t_list *philo, long int total_time, struct timeval last_time)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
	last_time = time;

	wait_to_talk(philo, 0, total_time, last_time);
	printf("%ld %d is sleeping\n", total_time / 1000, philo->p_nbr);
	philo->info->talky_talk = 1;
	time_left_alive(philo, philo->info->time_to_sleep, total_time, last_time);
	usleep_func(philo, philo->info->time_to_sleep);
}

void	ft_thinking(t_list *philo, long int total_time, struct timeval last_time)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
	last_time = time;

	wait_to_talk(philo, 0, total_time, last_time);
	printf("%ld %d is thinking\n", total_time / 1000, philo->p_nbr);
	philo->info->talky_talk = 1;
	time_left_alive(philo, philo->info->time_to_sleep, total_time, last_time);
	usleep_func(philo, philo->info->time_to_sleep);
}

void	ft_eating(t_list *philo, long int total_time, struct timeval last_time)
{
	struct timeval	time;

	// fflush(stdout);
	if (philo->fork == 0 || philo->left->fork == 0 && philo->info->all_alive)
	{
		// ft_thinking(philo, total_time, last_time);
		while (philo->fork == 0 || philo->left->fork == 0 && philo->info->all_alive)
		{
			// printf("%ld %d forks %d %d\n", total_time / 1000, philo->p_nbr, philo->fork, philo->left->fork);
			// usleep(1000);
			gettimeofday(&time, NULL);
			total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
			last_time = time;
			time_left_alive(philo, 0, total_time, last_time);
		}
	}
	pthread_mutex_lock(&philo->left->fork_prot);
	pthread_mutex_lock(&philo->fork_prot);
	wait_to_talk(philo, 0, total_time, last_time);
	gettimeofday(&time, NULL);
	total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
	last_time = time;
	printf("%ld %d picked up a fork\n", total_time / 1000, philo->p_nbr);
	printf("%ld %d picked up a fork\n", total_time / 1000, philo->p_nbr);
	philo->info->talky_talk = 1;
	philo->left->fork = 0;
	philo->fork = 0;
	pthread_mutex_unlock(&philo->left->fork_prot);
	pthread_mutex_unlock(&philo->fork_prot);
	if (philo->info->all_alive == 0)
	{
		// printf("%d out", philo->p_nbr);
		pthread_exit(NULL);
	}

	gettimeofday(&time, NULL);
	total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
	last_time = time;

	wait_to_talk(philo, 0, total_time, last_time);
	time_left_alive(philo, 0, total_time, last_time);
	// gettimeofday(&philo->lta, NULL);
	printf("%ld %d is eating for %lu\n", total_time / 1000, philo->p_nbr, philo->info->time_to_eat / 1000);
	philo->info->talky_talk = 1;
	usleep_func(philo, philo->info->time_to_eat);
	// gettimeofday(&time, NULL);
	// total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
	// last_time = time;
	// printf("%ld %d dropped the forks\n", total_time / 1000, philo->p_nbr);
	philo->left->fork = 1;
	philo->fork = 1;
	gettimeofday(&philo->lta, NULL);
}

void	*run_code(void *arg/* or t_list *philo ????*/)
{
	int				ind;
	t_list			*philo;
	struct timeval	time;
	struct timeval	last_time;
	long int		total_time;

	philo = (t_list *)arg;
	ind = -1;
	while (philo->info->talky_talk == 0);
	gettimeofday(&last_time, NULL);
	philo->lta = last_time;
	total_time = 0;
	if (philo->info->nbr_of_philosophers == 1)
	{
		printf("%lu %d is thiking\n", total_time, philo->p_nbr);
		usleep(philo->info->time_to_die);
		printf("%lu %d is dead\n", philo->info->time_to_die / 1000, philo->p_nbr);
		philo->info->all_alive = 0;
		free(philo);
		// printf("%d out", philo->p_nbr);
		pthread_exit(NULL);
	}
	while (++ind || 1 && philo->info->all_alive)
	{
		// write(2, "erro aqui\n", 11);
		gettimeofday(&time, NULL);
		total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
		last_time = time;
		// time_left_alive(philo, 0, total_time, last_time);
		ft_eating(philo, total_time, last_time);

		gettimeofday(&time, NULL);
		total_time += MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
		last_time = time;
		// time_left_alive(philo, philo->info->time_to_sleep, total_time, last_time);
		ft_sleeping(philo, total_time, last_time);

		// pthread_exit(NULL);
		// gettimeofday(&time, NULL);
		// total_time = MICRO * (time.tv_sec - last_time.tv_sec) + time.tv_usec - last_time.tv_usec;
		// last_time = time;
		// ft_eating(philo, total_time, last_time);
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
	// info->talky_talk = 0;
	while (++ind < info->nbr_of_philosophers)
	{
		if (pthread_create(nof + ind, NULL, run_code, philo))
			return (free(nof), ft_lstclear(&philo, ind + 1), 1);
		// pthread_detach(nof[ind]);
		philo = philo->right;
	}
	// info->talky_talk = 1;
	// while (info->all_alive);
	// usleep_func(philo, 1000);
	while (ind--)
		pthread_join(nof[ind], NULL);
	pthread_mutex_destroy(&info->talky_talk_prot);
	return (free(nof), ft_lstclear(&philo, info->nbr_of_philosophers), 0);
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
	// if (pthread_mutex_init(&(info.start_banquet), NULL))
		// return (1);
	printf("\nnbr_of_philosophers:%d\ntime_to_die:%lu\ntime_to_eat:%lu\ntime_to_sleep:%lu\nnotepme:%d\n\nstarting now\n", info.nbr_of_philosophers, info.time_to_die / 1000, info.time_to_eat / 1000, info.time_to_sleep / 1000, info.notepme);
	init_infosophers(&info);
}