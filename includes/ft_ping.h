#ifndef FT_PING_H
# define FT_PING_H

# define _GNU_SOURCE
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <signal.h>
# include <math.h>
# include <limits.h>
# include <errno.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/ip.h>
# include <linux/icmp.h>
# include <linux/errqueue.h>
# include <netdb.h>

// docs: https://linux.die.net/man/8/ping

# define PACKETSIZE 56

typedef struct s_flags
{
	uint8_t	debug;
	int		ttl;
	size_t	size;
	size_t	interval;
	size_t	count;
	uint8_t	verbose;
}	t_flags;

typedef struct s_ping
{
	t_flags				flags;
	char				*hostname;
	char				canonname[NI_MAXHOST];
	char				ip[INET_ADDRSTRLEN];
	struct sockaddr_in	sockaddr;
	int					sockfd;
	void				*pkt;
	struct icmphdr		*header;
	struct timeval		start;
	uint8_t				is_sending;
	size_t				sent;
	size_t				recived;
	long double			avg_time;
	long double			square_time;
	long double			min_time;
	long double			max_time;
}	t_ping;

// debug.c
void	print(t_ping *p);

// free.c
int		free_ping(t_ping *p, int ret);

// libft.c
size_t	ft_strlen(const char *str);
int		ft_strncmp(const char *s1, const char *s2, unsigned int n);
void	*ft_memset(void *b, int c, size_t len);
void	*ft_memcpy(void *dst, const void *src, size_t n);
int		ft_atoi(const char *str);

// parse.c
int		arg_parse(t_ping *p, size_t argc, char **argv);
int		usage(char *name);

// ping.c
int		ping(t_ping *p);

// resolve.c
int		resolve_hostname(t_ping *p);

#endif
