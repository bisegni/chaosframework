/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#undef _GLIBCXX_DEBUG

#include "InetUtility.h"

using namespace chaos::common::utility;


#ifndef _WIN32
std::string InetUtility::scanForLocalNetworkAddress(const std::string& _eth_interface_name){
    std::string ip_port;
    void * tmp_addr_ptr=NULL;
    struct ifaddrs * if_addr_struct=NULL;
    struct ifaddrs * ifa=NULL;
    std::string eth_interface_name;
    std::vector<InterfaceInfo> interface_infos;

    //chec if we have a prefered name to use
    if(_eth_interface_name.size()) {
        interface_infos.push_back(InterfaceInfo(_eth_interface_name, ""));
    }

    //default names append to the end of the set
    interface_infos.push_back(InterfaceInfo("en",""));
    interface_infos.push_back(InterfaceInfo("em",""));
    interface_infos.push_back(InterfaceInfo("eth",""));
    interface_infos.push_back(InterfaceInfo("tun",""));
    interface_infos.push_back(InterfaceInfo("utun",""));
    // loopback if no other found
    interface_infos.push_back(InterfaceInfo("lo",""));

    LAPP_ << "Scan for local network interface and ip";
    getifaddrs(&if_addr_struct);

    for (ifa = if_addr_struct; ifa != NULL; ifa = ifa->ifa_next) {
        if(ifa ->ifa_addr){
            if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
                // is a valid IP4 Address
                tmp_addr_ptr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                char address_buffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmp_addr_ptr, address_buffer, INET_ADDRSTRLEN);
                LAPP_ << "Interface " << ifa->ifa_name << " address " << address_buffer << "<- candidate to be chooses";
                checkInterfaceName(interface_infos,
                                   ifa->ifa_name,
                                   address_buffer);
            } else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
                // is a valid IP6 Address
                tmp_addr_ptr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
                char address_buffer[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, tmp_addr_ptr, address_buffer, INET6_ADDRSTRLEN);
                LAPP_ << "Interface " << ifa->ifa_name << " address " << address_buffer;
            }
        }
    }
    if (if_addr_struct!=NULL) freeifaddrs(if_addr_struct);

    //check the first priority address to get
    for(std::vector<InterfaceInfo>::const_iterator it = interface_infos.begin();
        it != interface_infos.end();
        it++){
        if(it->address.size()) {
            ip_port = it->address;
            LAPP_ << "The choosen address is:" << it->address << " his interface is:" << it->interface_name;
            break;
        }
    }
    //in case we don't have found anything fall to de loopback ip
    if(ip_port.size() == 0) {
        // no ip was found go to localhost
        ip_port.assign("127.0.0.1");
    }
    return ip_port;
}
#else
std::string InetUtility::scanForLocalNetworkAddress(const std::string& _eth_interface_name) {
	std::string ip_port;
	
	std::string eth_interface_name;
	std::vector<InterfaceInfo> interface_infos;

	//check if we have a preferred name to use
	if (_eth_interface_name.size()) {
		interface_infos.push_back(InterfaceInfo(_eth_interface_name, ""));
	}

	//default names append to the end of the set
	interface_infos.push_back(InterfaceInfo("en", ""));
	interface_infos.push_back(InterfaceInfo("em", ""));
	interface_infos.push_back(InterfaceInfo("eth", ""));
	interface_infos.push_back(InterfaceInfo("tun", ""));
	interface_infos.push_back(InterfaceInfo("utun", ""));
	// loopback if no other found
	interface_infos.push_back(InterfaceInfo("lo", ""));
	LAPP_ << "Scan for local network interface and ip";

	ULONG outBufLen;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	IP_ADAPTER_INFO *pAdapterInfo;
	DWORD dwRetVal;
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	outBufLen = sizeof(IP_ADAPTER_INFO);
	if (GetAdaptersInfo(pAdapterInfo, &outBufLen) != ERROR_SUCCESS)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)malloc(outBufLen);
	}
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &outBufLen) != ERROR_SUCCESS))
	{
		LERR_ << "GetAdaptersInfo call failed with code " << dwRetVal;
	}
	IP_ADAPTER_INFO * addIter = pAdapterInfo;
	while (addIter)
	{
		checkInterfaceName(interface_infos, addIter->AdapterName, addIter->IpAddressList.IpAddress.String);

		addIter = addIter->Next;
	}
	if (pAdapterInfo)
		free(pAdapterInfo);
	//check the first priority address to get
	for (std::vector<InterfaceInfo>::const_iterator it = interface_infos.begin();
		it != interface_infos.end();
		it++) 
	{
		if (it->address.size())
		{
			ip_port = it->address;
			LAPP_ << "The choosen address is:" << it->address << " his interface is:" << it->interface_name;
			break;
		}
	}
	//in case we don't have found anything fall to de loopback ip
	if (ip_port.size() == 0) 
	{
		// no ip was found go to localhost
		ip_port.assign("127.0.0.1");
	}
	return ip_port;

}
#endif
int InetUtility::scanForLocalFreePort(int basePort) {
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    portno = basePort;
    int err = 0;
    for (; err>=0;) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) return -1;
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_addr.sin_port = htons(portno);
        err = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
#ifdef _WIN32
		closesocket(sockfd);
#else
        close(sockfd);
#endif
        if(err>=0)portno++;
    }
    return portno;
}

void InetUtility::checkInterfaceName(std::vector<InterfaceInfo>& interface_infos,
                                     const std::string& interface_found,
                                     const std::string& interface_address) {
    for(std::vector<InterfaceInfo>::iterator it = interface_infos.begin();
        it != interface_infos.end();
        it++){
        if(boost::starts_with(interface_found, it->interface_name)) {
            it->interface_name = interface_found;
            it->address = interface_address;
            break;
        }
    }
}

bool InetUtility::checkWellFormedHostPort(std::string host_port) {
   boost::system::error_code ec;
   boost::asio::ip::address::from_string( host_port, ec );
   if ( ec )
     return false;
   return true;
}

bool InetUtility::checkWellFormedHostNamePort(std::string host_port) {
    int pos=host_port.find(":");
    if(pos == std::string::npos){
      return false;
    }
    std::string hname=host_port.substr(0,pos);
    std::string port=host_port.substr(pos+1);
    return (gethostbyname(hname.c_str())!=NULL)&&(atoi(port.c_str())>0);
}

bool InetUtility::checkWellFormedHostIpPort(std::string host_port) {
  return InetUtility::checkWellFormedHostNamePort(host_port);
}

void InetUtility::queryDns(std::string hostname, std::vector<std::string>& resolved_endpoints) {
    boost::asio::io_service io;
    boost::system::error_code error;
    boost::asio::ip::tcp::resolver resolver(io);
    boost::asio::ip::tcp::resolver::query q(hostname.c_str(), "");
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(q, error);
    boost::asio::ip::tcp::resolver::iterator end;
    while (iter != end) {
        resolved_endpoints.push_back(iter->endpoint().address().to_string());
        iter++;
    }
}

std::string InetUtility::getHostname() {
    char hostname[1024];
    gethostname(hostname, 1024);
    return std::string(hostname, strlen(hostname));
}
