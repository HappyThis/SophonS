#include "ThreadOfNetWork.h"



ThreadOfNetWork::ThreadOfNetWork()
{
	InterlockedExchange(&isActive, NOTRUNNING);

	memset(Protocol, 0, sizeof(Protocol));
}


ThreadOfNetWork::~ThreadOfNetWork()
{
}

void ThreadOfNetWork::Lock(){

	Mutex.lock();
}

void ThreadOfNetWork::UnLock(){
	Mutex.unlock();
}

void ThreadOfNetWork::Stop() {

	InterlockedExchange(&isActive, NOTRUNNING);
}

void ThreadOfNetWork::CmdGetIp(){

	Lock();

	QByteArray ipHashData;

	QHash<QString, IpContainerUnit*>::iterator it = ipContainer.begin();

	for (; it != ipContainer.end(); it++) {

		ipHashData.append((const char*)it.value(), sizeof(IpContainerUnit));

		qDebug() << it.value()->ip;
	}

	qDebug() << ipHashData.size();

	emit Send(ipHashData, CommandGetIp);

	UnLock();
}

void ThreadOfNetWork::CmdGetIo(){

	Lock();

	QByteArray IoListData;

	for (QPair<unsigned int, unsigned int> i : ioInfoContainer) {

		IoListData.append((const char*)&(i.first), sizeof(unsigned int));
		IoListData.append((const char*)&(i.second), sizeof(unsigned int));
	}
	emit Send(IoListData, CommandGetIo);

	UnLock();
}

void ThreadOfNetWork::CmdGetProtocol(){

	Lock();

	emit Send(QByteArray().append((const char*)Protocol,sizeof(unsigned int)*MaxProtocol),CommandGetProtocol);

	UnLock();
}

void  ThreadOfNetWork::ip4tos(u_long in, char * addressV4)
{
	u_char *p;

	p = (u_char*)&in;

	sprintf(addressV4, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);

	return;
}

void ThreadOfNetWork::ip6tos(sockaddr * sockaddr, char * addressV6, int addrlen) {

	socklen_t sockaddrlen;

#ifdef WIN32
	sockaddrlen = sizeof(struct sockaddr_in6);
#else
	sockaddrlen = sizeof(struct sockaddr_storage);
#endif


	if (getnameinfo(sockaddr,
		sockaddrlen,
		addressV6,
		addrlen,
		NULL,
		0,
		NI_NUMERICHOST) != 0) addressV6 = NULL;

	return;
}

void ThreadOfNetWork::AnalyzeEth(const u_char * pkt) {

	struct ethhdr *ethh = (struct ethhdr *)pkt;

	switch (ntohs(ethh->type)) {

	case 0x0806: {
		/*
		* 分析ARP
		*/

		strcpy(ICU->ip, "NULL");

		AnalyzeArp(pkt + sizeof(ethhdr));

		break;
	}
	case 0x0800: {
		/*
		* 分析IPV4
		*/
		AnalyzeIpV4(pkt + sizeof(ethhdr));

		break;
	}
	case 0x86dd: {
		/*
		* 分析IPV6
		*/
		AnalyzeIpV6(pkt + sizeof(ethhdr));

		break;
	}
	default: {

		/*其他*/

		strcpy(ICU->ip, "NULL");

		TryToInsert(OTHER);

		Protocol[OTHER]++;

	}
	}
}
void ThreadOfNetWork::AnalyzeArp(const u_char * pkt) {

	/*struct arphdr* arph = (struct arphdr *)pkt;

	QString destip, srcip;

	for (int i = 0; i < 4; i++) {
		if (i == 3) {
			destip.append(QString::number(arph->ar_destip[i]));
			srcip.append(QString::number(arph->ar_srcip[i]));
		}
		else {
			destip.append(QString::number(arph->ar_destip[i]) + ".");
			srcip.append(QString::number(arph->ar_srcip[i]) + ".");
		}
	}

	if (strcmp(destip.toStdString().data(), ipV4) == 0) {

		strcpy(packetInfo->ip, srcip.toStdString().data());
	}
	else {
		strcpy(packetInfo->ip, destip.toStdString().data());
	}*/

	TryToInsert(ARP);

	Protocol[ARP]++;

}
void ThreadOfNetWork::AnalyzeIpV6(const u_char * pkt) {

	/*确定IPV6地址*/

	struct iphdr6 *ip6h = (iphdr6*)pkt;

	inet_ntop(AF_INET6, ip6h->daddr, ICU->ip, sizeof(ICU->ip));

	if (strcmp(ICU->ip, ipV6) == 0) {

		inet_ntop(AF_INET6, ip6h->saddr, ICU->ip, sizeof(ICU->ip));
	}

	switch (ip6h->nh) {
		/*ICMPV6*/
	case 0x003a: {


		TryToInsert(ICMP);

		Protocol[ICMP]++;

		break;
	}
				 /*IGMP*/
	case 0x0002: {

		TryToInsert(IGMP);

		Protocol[IGMP]++;

		break;
	}
				 /*TCP*/
	case 0x0006: {

		AnalyzeTcp(pkt + 40);

		break;
	}
				 /*UDP*/
	case 0x0011: {

		AnalyzeUdp(pkt + 40);
		break;
	}
				 /*OSPF*/
	case 0x0059: {

		TryToInsert(OSPF);

		Protocol[OSPF]++;

		break;
	}
				 /*EIGRP*/
	case 0x0058: {

		TryToInsert(EIGRP);

		Protocol[EIGRP]++;

		break;
	}
	default: {
		TryToInsert(IPV6);
		Protocol[IPV6]++;
	}
	}
}

void ThreadOfNetWork::AnalyzeIpV4(const u_char * pkt) {


	/*确定IPv4地址*/
	struct iphdr *iph = (iphdr*)pkt;

	inet_ntop(AF_INET, (const void*)&iph->daddr, ICU->ip, sizeof(ICU->ip));

	if (strcmp(ICU->ip, ipV4) == 0) {

		inet_ntop(AF_INET, (const void*)&iph->saddr, ICU->ip, sizeof(ICU->ip));
	}

	int iplen = iph->ihl * 4;

	switch (iph->proto)
	{
		/*IGMP*/
	case 0x0002: {

		TryToInsert(IGMP);

		Protocol[IGMP]++;

		break;
	}
				 /*ICMP*/
	case 0x0001: {

		TryToInsert(ICMP);

		Protocol[ICMP]++;

		break;
	}
				 /*TCP*/
	case 0x0006: {

		AnalyzeTcp(pkt + iplen);

		break;
	}
				 /*UDP*/
	case 0x0011: {

		AnalyzeUdp(pkt + iplen);

		break;
	}
				 /*OSPF*/
	case 0x0059: {

		TryToInsert(OSPF);

		Protocol[OSPF]++;

		break;
	}
				 /*EIGRP*/
	case 0x0058: {

		TryToInsert(EIGRP);

		Protocol[EIGRP]++;

		break;
	}

	default: {
		TryToInsert(IPV4);

		Protocol[IPV4]++;

		/*统计结束*/

	}
	}

}

void ThreadOfNetWork::AnalyzeTcp(const u_char * pkt) {

	struct tcphdr *tcph = (struct tcphdr*)pkt;

	tcph->dport = ntohs(tcph->dport);


	tcph->sport = ntohs(tcph->sport);

	if (tcph->dport == 20 || tcph->sport == 20 || tcph->dport == 21 || tcph->sport == 21) {


		TryToInsert(FTP);

		Protocol[FTP]++;
	}
	else if (tcph->dport == 22 || tcph->sport == 22) {

		TryToInsert(SSH);

		Protocol[SSH]++;
	}
	else if (tcph->dport == 23 || tcph->sport == 23) {

		TryToInsert(TELNET);

		Protocol[TELNET]++;

	}
	else if (tcph->dport == 25 || tcph->sport == 25) {

		TryToInsert(SMTP);

		Protocol[SMTP]++;

	}
	else if (tcph->dport == 53 || tcph->sport == 53) {

		TryToInsert(DNS);

		Protocol[DNS]++;

	}
	else if (tcph->dport == 68 || tcph->sport == 68) {

		TryToInsert(DHCP);

		Protocol[DHCP]++;

	}
	else if (tcph->dport == 80 || tcph->sport == 80) {
		TryToInsert(HTTP);

		Protocol[HTTP]++;

	}
	else if (tcph->dport == 110 || tcph->sport == 110) {
		TryToInsert(POP3);
		Protocol[POP3]++;

	}
	else if (tcph->dport == 179 || tcph->sport == 179) {
		TryToInsert(BGP);

		Protocol[BGP]++;

	}
	else if (tcph->dport == 443 || tcph->sport == 443) {
		TryToInsert(HTTPS);

		Protocol[HTTPS]++;

	}
	else if (tcph->dport == 464 || tcph->sport == 464) {
		TryToInsert(KerberosV5);

		Protocol[KerberosV5]++;

	}
	else if (tcph->dport == 514 || tcph->sport == 514) {

		TryToInsert(SYSLOG);

		Protocol[SYSLOG]++;
	}
	else if (tcph->dport == 3389 || tcph->sport == 3389) {

		TryToInsert(RDP);

		Protocol[RDP]++;
	}
	else {
		TryToInsert(TCP);
	}
}

void ThreadOfNetWork::AnalyzeUdp(const u_char * pkt) {

	struct udphdr* udph = (struct udphdr*)pkt;

	udph->dport = ntohs(udph->dport);

	udph->sport = ntohs(udph->sport);


	if (udph->dport == 53 || udph->sport == 53) {

		TryToInsert(DNS);

		Protocol[DNS]++;
	}
	else if (udph->dport == 67 || udph->sport == 67) {

		TryToInsert(DHCP);

		Protocol[DHCP]++;
	}
	else if (udph->dport == 69 || udph->sport == 69) {
		TryToInsert(TFTP);
		Protocol[TFTP]++;
	}
	else if (udph->dport == 123 || udph->sport == 123) {
		TryToInsert(NTP);
		Protocol[NTP]++;
	}
	else if (udph->dport == 161 || udph->sport == 161) {
		TryToInsert(SNMP);
		Protocol[SNMP]++;
	}
	else if (udph->dport == 162 || udph->sport == 162) {
		TryToInsert(SNMPTRAP);
		Protocol[SNMPTRAP]++;
	}
	else if (udph->dport == 389 || udph->sport == 389) {
		TryToInsert(LDAP);
		Protocol[LDAP]++;
	}
	else if (udph->dport == 500 || udph->sport == 500) {
		TryToInsert(ISAKMP);
		Protocol[ISAKMP]++;
	}
	else if (udph->dport == 520 || udph->sport == 520) {
		TryToInsert(RIP);
		Protocol[RIP]++;
	}
	else if (udph->dport == 1812 || udph->sport == 1812 || udph->dport == 1813 || udph->sport == 1813) {
		TryToInsert(RADIUS);
		Protocol[RADIUS]++;
	}
	else if (udph->dport == 1701 || udph->sport == 1701) {
		TryToInsert(L2TP);
		Protocol[L2TP]++;
	}
	else if (udph->dport == 4500 || udph->sport == 4500) {
		TryToInsert(NAT_T);
		Protocol[NAT_T]++;
	}
	else if (udph->dport == 8000 || udph->sport == 8000) {
		TryToInsert(OICQ);
		Protocol[OICQ]++;
	}
	else if (udph->dport == 137 || udph->sport == 137) {
		TryToInsert(NBNS);
		Protocol[NBNS]++;
	}
	else {
		TryToInsert(UDP);
		Protocol[UDP]++;
	}

}

void ThreadOfNetWork::TryToInsert(int type)
{

	ICU->conter = 1;

	ICU->protocol[type]++;

	QHash<QString, IpContainerUnit*>::iterator it;

	if ((it =ipContainer.find(QString(ICU->ip))) == ipContainer.end()) {

		ipContainer.insert(QString(ICU->ip), ICU);

	}
	else {

		(*it)->conter++;

		strcpy((*it)->endTime, ICU->startTime);

		(*it)->protocol[type]++;

		(*it)->totLength += ICU->totLength;

		delete ICU;


	}
}

void ThreadOfNetWork::StartCapture(char *ip) {


	bool suc = false;


	u_int netmask;


	struct bpf_program fcode;


	pcap_if_t *alldevs;

	char errbuf[PCAP_ERRBUF_SIZE];


	pcap_t *adhandle;

	struct pcap_pkthdr *header;

	const u_char *pkt_data;

	time_t local_tv_sec;

	struct tm *ltime;

	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1) {
		fprintf(stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf);
		return;
	}


	for (pcap_if_t *d = alldevs; d != NULL && !suc; d = d->next)
	{
		for (pcap_addr* t = d->addresses; t && !suc; t = t->next)
		{

			if (t->addr->sa_family == AF_INET6) {


				ip6tos(t->addr, ipV6, sizeof(ipV6));

				continue;
			}

			if (t->addr->sa_family != AF_INET) {

				continue;

			}

			ip4tos(((struct sockaddr_in *)t->addr)->sin_addr.s_addr, ipV4);


			if (strcmp(ipV4, ip) == 0)
			{

				qDebug() << "Adapter match succeeded";

				if ((adhandle = pcap_open(d->name,
					65536,           
					PCAP_OPENFLAG_PROMISCUOUS,   
					1000,           
					NULL,            
					errbuf            
				)) == NULL)
				{
					fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);

					pcap_freealldevs(d);
					return;
				}


				if (d->addresses != NULL)

					netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
				else

					netmask = 0xffffff;


				if (pcap_compile(adhandle, &fcode, "not port 1997", 1, netmask) < 0)
				{
					fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");

					pcap_freealldevs(alldevs);
					return;
				}


				if (pcap_setfilter(adhandle, &fcode) < 0)
				{
					fprintf(stderr, "\nError setting the filter.\n");

					pcap_freealldevs(alldevs);
					return;
				}
				suc = true;

			}

		}
	}

	if (suc == false) {

		qDebug() << "No Mached!";

		return;
	}

	int len = strlen(ipV6);

	for (int i = 0; i < len; i++) {
		if (ipV6[i] == '%') {
			ipV6[i] = '\0';
			break;
		}
	}

	qDebug() << "Local Ipv4" << ipV4;

	qDebug() << "Local Ipv6" << ipV6;

	InterlockedExchange(&isActive, RUNNING);

	int res;

	unsigned int Lenth=0;


	DWORD TIME = GetTickCount();

	while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0 && isActive==RUNNING) {

		if (res == 0) {
			continue;
		}

		Lock();

		ICU = new IpContainerUnit();

		local_tv_sec = header->ts.tv_sec;

		ltime = localtime(&local_tv_sec);

		strftime(ICU->startTime, sizeof(ICU->startTime), "%H:%M:%S:", ltime);

		strcat(ICU->startTime, QString("%1").arg(header->ts.tv_usec).toStdString().data());

		strcpy(ICU->endTime, ICU->startTime);

		ICU->totLength = header->len;

		Lenth += header->len;

		AnalyzeEth(pkt_data);

		/*Interval of 1s*/

		if (GetTickCount() - TIME >= 1500) {

			ioInfoContainer.append(qMakePair((unsigned int)TIME,Lenth));

			Lenth = 0;

			TIME = GetTickCount();

		}
		UnLock();
	}

	qDebug() << "Capture Stop!";

}
