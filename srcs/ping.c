#include "ft_ping.h"

int	create_packet(t_ping *p)
{
	void	*payload;

	p->pkt = malloc(sizeof(struct icmphdr) + (p->flags.size));
	if (!p->pkt)
		return (printf("Error: malloc error\n"));
	payload = p->pkt + sizeof(struct icmphdr);
	ft_memset(payload, '\0', p->flags.size);
	return (0);
}

uint16_t	checksum(t_ping *p)
{
	uint32_t	len;
	uint32_t	sum;
	uint16_t	*tmp;

	len = p->flags.size + sizeof(struct icmphdr);
	sum = 0;
	tmp = p->pkt;
	while (len > 1)
	{
		sum += *tmp++;
		len -= 2;
	}
	if (len == 1)
		sum += *(uint8_t *)tmp;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	sum = ~sum;
	return (sum);
}

int	recv_pkt(t_ping *p, size_t seq)
{
	struct timeval		s_recv;
	struct timeval		e_recv;
	ssize_t				bytes;
	char				name[NI_MAXHOST];
	char				ip_str[INET_ADDRSTRLEN];
	struct msghdr		msg;
	struct iovec		iov;
	struct sockaddr_in	buf_in;
	struct icmphdr		*icmp;
	struct iphdr		*ip;
	int					hlen;
	long double			time;

	memset(p->pkt, 0, p->flags.size + sizeof(struct icmphdr));
	iov.iov_base = p->pkt;
	iov.iov_len = p->flags.size + sizeof(struct icmphdr);
	msg.msg_name = &buf_in;
	msg.msg_namelen = sizeof(buf_in);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	gettimeofday(&s_recv, 0);
	bytes = recvmsg(p->sockfd, &msg, MSG_WAITALL);
	if (bytes < 0)
		return (-1);
	gettimeofday(&e_recv, 0);
	ip = (struct iphdr *)msg.msg_iov->iov_base;
	hlen = ip->ihl * 4;
	icmp = (struct icmphdr *)(msg.msg_iov->iov_base + hlen);
	if (icmp->un.echo.id != getpid())
		return (-2);
	inet_ntop(buf_in.sin_family, &buf_in.sin_addr, ip_str, INET_ADDRSTRLEN);
	getnameinfo((struct sockaddr *)&buf_in, sizeof(struct sockaddr), name, sizeof(name), 0, 0, NI_IDN);
	timersub(&e_recv, &s_recv, &s_recv);
	time = (s_recv.tv_sec * (uint64_t)1000) + (s_recv.tv_usec / 1000.0);
	printf("%ld bytes from %s (%s): icmp_seq=%ld ttl=%d time=%.2Lf ms\n",
		bytes, name, ip_str, seq, ip->ttl, time);
	p->avg_time += time;
	p->square_time += time * time;
	if (time < p->min_time || !p->min_time)
		p->min_time = time;
	if (time > p->max_time || !p->max_time)
		p->max_time = time;
	return (0);
}

void	recv_error(t_ping *p, size_t seq)
{
	char						cbuf[4096];
	struct iovec				iov;
	struct msghdr				msg;
	struct sockaddr_in			target;
	struct cmsghdr				*cmsgh;
	struct sock_extended_err	*e;
	struct icmphdr				icmph;
	char						ip_str[INET_ADDRSTRLEN];
	char						name[NI_MAXHOST];

	e = NULL;

	iov.iov_base = &icmph;
	iov.iov_len = sizeof(icmph);

	msg.msg_name = &target;
	msg.msg_namelen = sizeof(target);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;
	msg.msg_control = cbuf;
	msg.msg_controllen = sizeof(cbuf);

	recvmsg(p->sockfd, &msg, MSG_ERRQUEUE);

	cmsgh = CMSG_FIRSTHDR(&msg);
	while (cmsgh)
	{
		if (cmsgh->cmsg_level == SOL_IP)
		{
			if (cmsgh->cmsg_type == IP_RECVERR)
				e = (struct sock_extended_err *)CMSG_DATA(cmsgh);
		}
		cmsgh = CMSG_NXTHDR(&msg, cmsgh);
	}
	if (e)
	{
		struct sockaddr_in	*sin;

		sin = (struct sockaddr_in *)(e + 1);
		inet_ntop(AF_INET, &sin->sin_addr, ip_str, INET_ADDRSTRLEN);
		getnameinfo((struct sockaddr *)sin, sizeof(struct sockaddr), name, sizeof(name), 0, 0, NI_IDN);
		if (e->ee_type == ICMP_TIME_EXCEEDED)
			printf("From %s (%s) icmp_seq=%ld Time to live exceeded\n", name, ip_str, seq);
		else if (e->ee_type == ICMP_DEST_UNREACH)
			printf("From %s (%s) icmp_seq=%ld Destination Host Unreachable\n", name, ip_str, seq);
		else if (p->flags.verbose)
			printf("From %s: icmp_seq=%ld type=%d code=%d info=%d\n", p->hostname, seq, e->ee_type, e->ee_code, e->ee_info);
	}
	else if (p->flags.verbose)
		printf("From %s: icmp_seq=%ld type=%d code=%d\n", p->hostname, seq, icmph.type, icmph.code);
}

void	ping_stats(t_ping *p)
{
	struct timeval	end;
	long double		mdev;

	gettimeofday(&end, 0);
	timersub(&end, &p->start, &end);
	p->avg_time /= p->sent;
	mdev = (p->square_time / p->sent) - p->avg_time * p->avg_time;
	mdev = sqrt(mdev);
	printf("\n--- %s ping statistics ---\n", p->hostname);
	printf("%ld packets transmitted, %ld recived, %.0f%% packet loss, time %.0fms\n",
		p->sent, p->recived, (p->sent - p->recived) / p->sent * 100.0, (end.tv_sec * (uint64_t)1000) + (end.tv_usec / 1000.0));
	printf("rtt min/avg/max/mdev = %.3Lf/%.3Lf/%.3Lf/%.3Lf ms\n", p->min_time, p->avg_time, p->max_time, mdev);
}

int	ping(t_ping *p)
{
	struct icmphdr	*header;
	size_t			seq;
	int				ret;

	seq = 1;
	if (create_packet(p))
		return (free_ping(p, 1));
	header = (struct icmphdr *)p->pkt;
	gettimeofday(&p->start, NULL);
	while (p->is_sending)
	{
		if (p->is_sending == 1)
		{
			header->type = ICMP_ECHO;
			header->code = 0;
			header->un.echo.id = getpid();
			header->un.echo.sequence = htons(seq);
			header->checksum = 0;
			header->checksum = checksum(p);

			if (sendto(p->sockfd, p->pkt, p->flags.size + sizeof(struct icmphdr),
					0, (struct sockaddr *)&p->sockaddr, sizeof(struct sockaddr_in)) <= 0)
				printf("Error: coud not send the packet");
			else
				p->sent++;

			ret = recv_pkt(p, seq);
			if (ret == -1)
				recv_error(p, seq);
			else if (ret >= 0)
				p->recived++;

			if (p->flags.count > 0 && seq == p->flags.count)
				break ;
			if (ret != -2)
				++seq;

			p->is_sending = 2;
			alarm(p->flags.interval);
		}
	}
	ping_stats(p);
	return (free_ping(p, 0));
}
