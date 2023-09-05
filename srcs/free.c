#include "ft_ping.h"

int	free_ping(t_ping *p, int ret)
{
	if (p->pkt)
	{
		free(p->pkt);
		p->pkt = NULL;
	}
	if (p->sockfd)
	{
		close(p->sockfd);
		p->sockfd = 0;
	}
	return (ret);
}
