#include "ft_ping.h"

uint8_t	*g_is_sending;

void	init_ping(t_ping *p)
{
	p->flags.debug = 0;
	p->flags.ttl = 0;
	p->flags.size = PACKETSIZE;
	p->flags.interval = 0;
	p->flags.count = -1;
	p->flags.verbose = 0;
	p->hostname = NULL;
	ft_memset(p->canonname, '\0', NI_MAXHOST);
	ft_memset(p->ip, '\0', INET_ADDRSTRLEN);
	p->sockfd = 0;
	p->pkt = NULL;
	p->is_sending = 1;
	p->sent = 0;
	p->recived = 0;
	p->avg_time = 0;
	p->square_time = 0;
	p->min_time = 0;
	p->max_time = 0;
}

void	signal_handler(int sig)
{
	if (sig == SIGINT)
	{
		*g_is_sending = 0;
	}
	else if (sig == SIGALRM)
	{
		*g_is_sending = 1;
	}
}

int	main(int argc, char **argv)
{
	t_ping	p;

	if (argc < 2 || !ft_strncmp("-h", argv[1], 3))
		return (usage(argv[0]));
	if (getuid() != 0)
	{
		printf("Permission required\n");
		return (1);
	}
	init_ping(&p);
	if (arg_parse(&p, (size_t)(argc - 1), &argv[1]))
		return (1);
	g_is_sending = &p.is_sending;
	signal(SIGINT, signal_handler);
	signal(SIGALRM, signal_handler);
	if (resolve_hostname(&p))
		return (1);
	if (ping(&p))
		return (1);
	return (0);
}
