#pragma once
#include <Net/NetServer.h>
#include <Net/NetCommon.h>

namespace net
{
	class PollServer :public net::NetServer
	{
	public:
		PollServer(){}
		virtual ~PollServer(){}
	public:
		virtual boost::shared_ptr<NetConnection> createConnection();
		virtual void onPackageReceived(const unsigned char* content, const size_t& contentLen, boost::shared_ptr<net::NetConnection> connection)override;
	private:

	};
}