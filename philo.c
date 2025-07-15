/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 14:24:31 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/15 15:03:41 by jlima-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
	return (head->left);
}

int	usleep_func(t_list *philo, long int time_to_sleep)
{
	struct timeval	time;
	struct timeval	last_time;
	long int		time_done;

	gettimeofday(&last_time, NULL);
	time_done = 0;
	while (time_done + 10 * MILI < time_to_sleep && philo->info->all_alive)
	{
		usleep(10 * MILI);
		gettimeofday(&time, NULL);
		time_done += MICRO * (time.tv_sec - last_time.tv_sec) \
			+ time.tv_usec - last_time.tv_usec;
		last_time = time;
	}
	if (philo->info->all_alive == 0)
		return (1);
	if (time_done < time_to_sleep)
		usleep(time_to_sleep - time_done);
	return (philo->info->all_alive == 0);
}

void	die(t_list *philo, long int tt_time, struct timeval last_time)
{
	struct timeval	time;
	long int		count_down;

	count_down = philo->info->time_to_die - (MICRO * (time.tv_sec - \
		philo->lta.tv_sec) + time.tv_usec - philo->lta.tv_usec);
	pthread_mutex_lock(&philo->info->all_alive_prot);
	if (philo->info->all_alive)
	{
		philo->info->all_alive = 0;
		pthread_mutex_unlock(&philo->info->all_alive_prot);
		// die_aux(philo, count_down);
		usleep(count_down);
		pthread_mutex_lock(&philo->info->talky_talk_prot);
		while (philo->info->talky_talk == 0)
			usleep(0);
		philo->info->talky_talk = 0;
		pthread_mutex_unlock(&philo->info->talky_talk_prot);
		gettimeofday(&time, NULL);
		tt_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
		time.tv_usec - last_time.tv_usec;
		printf("%ld %d is dead\n", tt_time / MILI, philo->p_nbr);
		pthread_exit(NULL);
	}
	pthread_mutex_unlock(&philo->info->all_alive_prot);
	// if (philo->p_nbr == 2)
		// printf("2 is %ld\n", time_left_alive(philo, 0), tt_time);
	// printf("%d died\n", philo->p_nbr);
	pthread_exit(NULL);
}

int	time_left_alive(t_list *philo, long int time_to_sleep)
{
	struct timeval	time;
	long int		count_down;

	if (philo->info->all_alive == 0)
		return (1);
	gettimeofday(&time, NULL);
	count_down = philo->info->time_to_die - (MICRO * (time.tv_sec - \
		philo->lta.tv_sec) + time.tv_usec - philo->lta.tv_usec);
	// if (philo->p_nbr == 2)
		// printf("%ld\n", count_down - time_to_sleep);
	if (count_down - time_to_sleep - 1 * MILI <= 0)
		return (1);
	return (0);
}

int	wait_to_talk(t_list *philo, struct timeval last_time)
{
	struct timeval	time;

	pthread_mutex_lock(&philo->info->talky_talk_prot);
	while (philo->info->talky_talk == 0 && philo->info->all_alive)
	{
		pthread_mutex_unlock(&philo->info->talky_talk_prot);
		if (time_left_alive(philo, 0))
			return (1);
		pthread_mutex_lock(&philo->info->talky_talk_prot);
	}
	philo->info->talky_talk = 0;
	pthread_mutex_unlock(&philo->info->talky_talk_prot);
	return (0 + (philo->info->all_alive == 0));
}

void	ft_sleeping(t_list *philo, long int tt_time, struct timeval last_time)
{
	struct timeval	time;
	
	if (wait_to_talk(philo, last_time))
		die(philo, tt_time, last_time);
	gettimeofday(&time, NULL);
	tt_time += MICRO * (time.tv_sec - last_time.tv_sec) \
		+ time.tv_usec - last_time.tv_usec;
	last_time = time;
	if (time_left_alive(philo, philo->info->time_to_sleep))
		die(philo, tt_time,last_time);
	if (philo->info->talky_talk == 0)
	{
		// printf("%ld %d is sleeping\n", tt_time / MILI, philo->p_nbr);
		philo->info->talky_talk = 1;
	}
	if (time_left_alive(philo, philo->info->time_to_sleep))
		die(philo, tt_time,last_time);
	if (usleep_func(philo, philo->info->time_to_sleep))
		die(philo, tt_time,last_time);
}

void	even_forks(t_list *philo, long int tt_time, struct timeval last_time, struct timeval time)
{	
	if (wait_to_talk(philo, last_time))
		die(philo, tt_time, last_time);
	gettimeofday(&time, NULL);
	tt_time += MICRO * (time.tv_sec - last_time.tv_sec) \
		+ time.tv_usec - last_time.tv_usec;
	last_time = time;
	if (philo->info->talky_talk == 0)
	{
		// printf("%ld %d is thinking\n", tt_time / MILI, philo->p_nbr);
		philo->info->talky_talk = 1;
	}
	pthread_mutex_lock(&philo->fork_prot);
	pthread_mutex_lock(&philo->left->fork_prot);
	while (philo->left->fork == 0 || philo->fork == 0)
	{
		pthread_mutex_unlock(&philo->fork_prot);
		pthread_mutex_unlock(&philo->left->fork_prot);
		gettimeofday(&time, NULL);
		tt_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
			time.tv_usec - last_time.tv_usec;
		last_time = time;
		if (time_left_alive(philo, 0))
			die(philo, tt_time, last_time);
		pthread_mutex_lock(&philo->fork_prot);
		pthread_mutex_lock(&philo->left->fork_prot);
	}
	philo->left->fork = 0;
	philo->fork = 0;
	pthread_mutex_unlock(&philo->fork_prot);
	pthread_mutex_unlock(&philo->left->fork_prot);
	if (wait_to_talk(philo, last_time))
		die(philo, tt_time, last_time);
	gettimeofday(&time, NULL);
	tt_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
		time.tv_usec - last_time.tv_usec;
	// printf("%ld %d picked up a fork\n", tt_time / MILI, philo->p_nbr);
	// printf("%ld %d picked up a fork\n", tt_time / MILI, philo->p_nbr);
	philo->info->talky_talk = 1;
}

void	ft_thinking(t_list *philo, long int tt_time, struct timeval last_time)
{
	struct timeval	time;

	
}

int	 get_fork(int nbr, t_list *philo, long int tt_time, struct timeval last_time)
{
	struct timeval	time;

	pthread_mutex_lock(&philo->fork_prot);
	if (philo->fork == 1)
	{
		philo->fork = 0;
		pthread_mutex_unlock(&philo->fork_prot);
		if (wait_to_talk(philo, last_time))
			die(philo, tt_time, last_time);
		printf("%lu %d picked up a fork\n", tt_time / MILI, nbr);
		philo->info->talky_talk = 1;
		return (1);
	}
	pthread_mutex_unlock(&philo->fork_prot);
	return (0);
}

void	ft_eating(t_list *philo, long int tt_time, struct timeval last_time)
{
	struct timeval	time;
	int				right;
	int				left;
	
	if (philo->left->fork == 0 && philo->fork == 0)
	{
		//ft_thinking(philo, tt_time, last_time); 
		if (time_left_alive(philo, 0))
			die(philo, tt_time, last_time);
		if (wait_to_talk(philo, last_time))
			die(philo, tt_time, last_time);
		printf("%lu %d is thinking\n", tt_time / MILI, philo->p_nbr);
		philo->info->talky_talk = 1;
	}
	left = 1;
	right = 1;
	while (right == 1 || left == 1)
	{
		// printf("%d loop\n", philo->p_nbr);
		if (time_left_alive(philo, 0))
			die(philo, tt_time, last_time);
		gettimeofday(&time, NULL);
		tt_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
			time.tv_usec - last_time.tv_usec;
		last_time = time;
		right -= get_fork(philo->p_nbr, philo, tt_time, last_time);
		left -= get_fork(philo->p_nbr, philo->left, tt_time, last_time);
	}
	if (wait_to_talk(philo, last_time))
		die(philo, tt_time, last_time);
	printf("%lu %d is eating\n", tt_time / MILI, philo->p_nbr);
	philo->info->talky_talk = 1;
	usleep(philo->info->time_to_eat);
	gettimeofday(&philo->lta, NULL);
}

int	pre_run_code(t_list *philo, struct timeval last_time)
{
	if (philo->info->nbr_of_philosophers == 1)
	{
		philo->info->all_alive = 0;
		printf("0 1 is not eating\n");
		usleep(philo->info->time_to_die);
		printf("%lu %d is dead\n", philo->info->time_to_die / MILI, \
			philo->p_nbr);
		free(philo);
		pthread_exit(NULL);
	}
	pthread_mutex_unlock(&philo->info->start_banquet);
	return (0);
}

void	*run_code(void *arg)
{
	t_list			*philo;
	struct timeval	time;
	struct timeval	last_time;
	long int		tt_time;

	philo = (t_list *)arg;
	gettimeofday(&last_time, NULL);
	tt_time = pre_run_code(philo, last_time);
	philo->lta = last_time;
	while (philo->info->all_alive)
	{
		gettimeofday(&time, NULL);
		tt_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
			time.tv_usec - last_time.tv_usec;
		last_time = time;
		ft_eating(philo, tt_time, last_time);
		// if (philo->ate >= philo->info->notepme && philo->info->notepme != -1)
			// pthread_exit(NULL);
		// gettimeofday(&time, NULL);
		// tt_time += MICRO * (time.tv_sec - last_time.tv_sec) + \
			// time.tv_usec - last_time.tv_usec;
		// last_time = time;
		// ft_sleeping(philo, tt_time, last_time);
		usleep(philo->info->time_to_sleep);
	}
	return (NULL);
}

int	init_infosophers(t_info *info)
{
	int				ind;
	t_list			*philo;
	pthread_t		*nof;
	struct timeval	last_time;
	struct timeval	time;
	long int		tt_time;

	nof = malloc(info->nbr_of_philosophers * sizeof(pthread_t));
	if (nof == NULL)
		return (1);
	philo = init_fork_prot(info->nbr_of_philosophers, info);
	if (philo == NULL)
		return (free(nof), 1);
	ind = 0;
	while (ind < info->nbr_of_philosophers && info->all_alive)
	{
		if (pthread_create(nof + ind, NULL, run_code, philo))
			return (free(nof), ft_lstclear(&philo, ind + 1), 1);
		// printf("new philo %d\n", philo->p_nbr);
		if (philo->p_nbr == 2)
		{
			philo = philo->left;
			sleep(1);
			printf("huh\n");
		}
		philo = philo->left->left;
		ind++;
	}
	printf("\ndone\n");
	ind = -1;
	while (++ind < info->nbr_of_philosophers)
	{
		pthread_join(nof[ind], NULL);
		printf("end philo %d\n", ind + 1);
	}
	pthread_mutex_destroy(&info->talky_talk_prot);
	return (free(nof), ft_lstclear(&philo, info->nbr_of_philosophers), 0);
}

int	init_info(int ac, char **av, t_info *info)
{
	info->nbr_of_philosophers = ft_atoi(av[1]);
	info->time_to_die = (ft_atoi(av[2])) * MILI;
	info->time_to_eat = ft_atoi(av[3]) * MILI;
	info->time_to_sleep = ft_atoi(av[4]) * MILI;
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

int	exit_message(t_info info, int ac)
{
	write(2, "invalid number of philosophers\n", \
		(info.nbr_of_philosophers <= 0) * 32);
	write(2, "invalid time to die\n", (info.time_to_die <= 0) * 21);
	write(2, "invalid time to eat\n", (info.time_to_eat <= 0) * 21);
	write(2, "invalid time to sleep\n", (info.time_to_sleep <= 0) * 23);
	if (ac == 6)
		write(2, "invalid number of times each philosopher must eat \n", \
			(info.notepme < 0) * 52);
	return ((info.nbr_of_philosophers <= 0) + (info.time_to_die <= 0) + \
		(info.time_to_eat <= 0) + (info.time_to_sleep <= 0) + \
		((info.notepme < 0) * ac == 6));
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
	if (exit_message(info, ac))
		return (1);
	if (pthread_mutex_init(&(info.talky_talk_prot), NULL))
		return (1);
	if (pthread_mutex_init(&(info.all_alive_prot), NULL))
		return (1);
	if (info.notepme == 0)
		return (0);
	init_infosophers(&info);
}
