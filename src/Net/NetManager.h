#pragma once
#include "NetCommon.h"
#include "NetConnection.h"
namespace net
{
	class DLL_EXPORT_NET NetManager
	{
	public:
		NetManager() {};
		virtual ~NetManager() {};
	public:
		void addConnection(boost::shared_ptr<NetConnection> connection);
		void removeConnection(boost::shared_ptr<NetConnection> connection);
		virtual void onPackageReceived(const unsigned char* content, const size_t& contentLen, boost::shared_ptr<net::NetConnection> connection) = 0;
		virtual int start(bool async = true) = 0;
	private:
		boost::recursive_mutex m_mutex;                             /* 全局锁 */
		set<boost::shared_ptr<NetConnection> > m_connectionPool;    /* 连接池 */
	};

}
