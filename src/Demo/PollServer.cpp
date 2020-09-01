#include "PollServer.h"

using namespace net;

boost::shared_ptr<NetConnection> PollServer::createConnection()
{

}
void PollServer::onPackageReceived(const unsigned char* content, const size_t& contentLen, boost::shared_ptr<net::NetConnection> connection)
{
	unsigned char buf[64] = { 0 };
	memcpy(buf, content, contentLen);
	std::cout << buf << std::endl;
	delete[] content;
}