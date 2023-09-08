#include "ft_ping.h"

struct addrinfo	*get_info(char *hostname)
{
	int				res;
	struct addrinfo	hints;
	struct addrinfo	*info;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	hints.ai_flags = AI_CANONNAME;
	res = getaddrinfo(hostname, NULL, &hints, &info);
	if (res)
	{
		printf("Error: %s -> %s\n", hostname, gai_strerror(res));
		return (NULL);
	}
	return (info);
}

struct addrinfo	*get_socket(t_ping *p, struct addrinfo *info)
{
	struct addrinfo	*tmp;

	tmp = info;
	while (tmp)
	{
		p->sockfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (p->sockfd)
			break ;
		tmp = tmp->ai_next;
	}
	if (p->sockfd < 0 || !tmp)
	{
		printf("Error: cannot open socket: %s\n", strerror(errno));
		freeaddrinfo(info);
		return (NULL);
	}
	return (tmp);
}

int	config_socket(t_ping *p)
{
	struct timeval	timeout;
	int				opt;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	opt = 1;
	if (setsockopt(p->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
		return (printf("Error: setsockopt failure\n"));
	if (setsockopt(p->sockfd, SOL_IP, IP_RECVERR, &opt, sizeof(opt)))
		return (printf("Error: setsockopt failure\n"));
	if (p->flags.debug)
	{
		if (setsockopt(p->sockfd, SOL_SOCKET, SO_DEBUG, &opt, sizeof(opt)))
			return (printf("Error: setsockopt failure\n"));
	}
	if (p->flags.ttl)
	{
		if (setsockopt(p->sockfd, IPPROTO_IP, IP_TTL, &p->flags.ttl, sizeof(p->flags.ttl)))
			return (printf("Error: setsockopt failure\n"));
	}
	return (0);
}

void	resolve_address(t_ping *p, struct addrinfo *tmp)
{
	struct sockaddr_storage	addr;

	ft_memcpy(&addr, tmp->ai_addr, tmp->ai_addrlen);
	inet_ntop(addr.ss_family, (void *)&((struct sockaddr_in *)&addr)->sin_addr, p->ip, INET_ADDRSTRLEN);
	ft_memcpy(&(p->sockaddr.sin_addr), &((struct sockaddr_in *)&addr)->sin_addr, sizeof(struct in_addr));
	p->sockaddr.sin_port = htons(0);
	p->sockaddr.sin_family = AF_INET;
	ft_memcpy(p->canonname, tmp->ai_canonname, ft_strlen(tmp->ai_canonname));
}

int	resolve_hostname(t_ping *p)
{
	struct addrinfo	*info;
	struct addrinfo	*tmp;

	info = get_info(p->hostname);
	if (!info)
		return (1);
	tmp = get_socket(p, info);
	if (!tmp)
		return (1);
	if (config_socket(p))
	{
		freeaddrinfo(info);
		return (free_ping(p, 1));
	}
	resolve_address(p, tmp);
	freeaddrinfo(info);
	printf("PING %s (%s) %ld(%ld) bytes of data.\n", p->hostname, p->ip, p->flags.size, p->flags.size + sizeof(struct icmphdr));
	return (0);
}
