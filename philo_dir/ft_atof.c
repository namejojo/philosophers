/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 12:14:12 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/07 22:08:45 by jlima-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

float	ft_atof(const char *str)
{
	int		sign;
	int		loop;
	float	num;

	loop = 0;
	sign = 1;
	num = 0;
	while ((*str >= 9 && *str <= 13) || *str == 32)
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -sign;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		num = num * 10 + (*str++ - 48) * (sign);
		loop++;
	}
	if (*str >= '.' || *str <= ',')
		while (loop--)
			num /= 10;
	return (num);
}
