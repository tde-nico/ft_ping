#include "ft_ping.h"

int	set_debug(t_ping *p, char **args, size_t *i)
{
	p->flags.debug = 1;
	(void)args;
	(void)i;
	return (0);
}

int	set_ttl(t_ping *p, char **args, size_t *i)
{
	int	ttl;

	if (!args[1])
		return (printf("Please specify a TTL\n"));
	ttl = ft_atoi(args[1]);
	if (!(ttl > 0))
		return (printf("Please specify a valid TTL > 0\n"));
	p->flags.ttl = ttl;
	++(*i);
	return (0);
}

int	set_packetsize(t_ping *p, char **args, size_t *i)
{
	size_t	pktsize;

	if (!args[1])
		return (printf("Please specify a packet size\n"));
	pktsize = ft_atoi(args[1]);
	if (!(pktsize > 0))
		return (printf("Please specify a valid packetsize > 0\n"));
	p->flags.size = pktsize;
	++(*i);
	return (0);
}

int	set_interval(t_ping *p, char **args, size_t *i)
{
	size_t	interval;

	if (!args[1])
		return (printf("Please specify an interval\n"));
	interval = ft_atoi(args[1]);
	if (!(interval > 0))
		return (printf("Please specify a valid interval > 0\n"));
	p->flags.interval = interval;
	++(*i);
	return (0);
}

int	set_count(t_ping *p, char **args, size_t *i)
{
	size_t	count;

	if (!args[1])
		return (printf("Please specify a number of packets\n"));
	count = ft_atoi(args[1]);
	if (!(count > 0))
		return (printf("Please specify a valid number > 0\n"));
	p->flags.count = count;
	++(*i);
	return (0);
}

int	set_verbose(t_ping *p, char **args, size_t *i)
{
	p->flags.verbose = 1;
	(void)args;
	(void)i;
	return (0);
}

char	*flags_usage[] = {
	"-h", "prints the usage",
	"-d", "prints debug informations",
	"-t", "sets the time to live",
	"-s", "specifies packetsize",
	"-i", "specifies interval between packets",
	"-c", "specifies the number of packets sent",
	"-v", "enable verbose",
	NULL, NULL
};

int	(*flags[])(t_ping *p, char **args, size_t *i) = {
	&set_debug,
	&set_ttl,
	&set_packetsize,
	&set_interval,
	&set_count,
	&set_verbose,
	NULL
};

int	arg_parse(t_ping *p, size_t argc, char **argv)
{
	size_t	i;
	size_t	j;
	int		flag;

	i = 0;
	while (i < argc)
	{
		j = 0;
		flag = 0;
		while (flags_usage[(j + 1) * 2] != NULL)
		{
			if (!ft_strncmp(flags_usage[(j + 1) * 2], argv[i], ft_strlen(flags_usage[(j + 1) * 2]) + 1))
			{
				if (flags[j](p, &argv[i], &i))
					return (1);
				flag = 1;
				break ;
			}
			++j;
		}
		if (!flag)
		{
			if (p->hostname != NULL)
				return (printf("Invalid argument: %s\n", argv[i]));
			p->hostname = argv[i];
		}
		++i;
	}
	if (p->flags.interval == 0)
		p->flags.interval = 1;
	return (0);
}

int	usage(char *name)
{
	size_t	i;

	printf("Usage: %s [flags]\n", name);
	i = 0;
	while (flags_usage[i])
	{
		printf("\t%s\t%s\n", flags_usage[i], flags_usage[i + 1]);
		i += 2;
	}
	return (1);
}
