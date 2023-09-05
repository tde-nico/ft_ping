#include "ft_ping.h"

void	print(t_ping *p)
{
	printf("\tPing\n");
	printf("hostname:\t%s\n", p->hostname);
	printf("ip-addr:\t%s\n", p->ip);
	printf("canonname:\t%s\n", p->canonname);
	printf("socket-fd:\t%d\n", p->sockfd);
	printf("is_sending:\t%d\n", p->is_sending);
	printf("sent:\t\t%ld\n", p->sent);
	printf("recived:\t%ld\n", p->recived);

	printf("\n\tFlags\n");
	printf("debug:\t\t%d\n", p->flags.debug);
	printf("ttl:\t\t%d\n", p->flags.ttl);
	printf("size:\t\t%ld\n", p->flags.size);
	printf("interval:\t%ld\n", p->flags.interval);
	printf("count:\t\t%ld\n", p->flags.count);
	printf("verbose:\t%d\n", p->flags.verbose);

	printf("\n");
}
