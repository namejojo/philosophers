/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verify.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 11:05:26 by jlima-so          #+#    #+#             */
/*   Updated: 2025/07/15 11:08:45 by jlima-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int	fd = open("teste.c", O_RDWR, 0644);
	char c;

	printf("starting\n");
	while (read(fd, &c, 1))
		write(1, &c, 1);
	
}