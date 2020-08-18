#pragma once
#include "NetCommon.h"

namespace net
{
	class DLL_EXPORT_NET NetServer : public boost::enable_shared_from_this<NetServer>
	{
	public:
		
		NetServer(io_service& ioservice, string listenIp, unsigned short listenPort, bool bReuseAddr = true, bool bAutoBind = false);
		virtual ~NetServer();
		int start(bool async = true);
		virtual boost::shared_ptr<NetConnection> createConnection();
		virtual void onAccept(boost::shared_ptr<NetConnection> connection);
		void removeConnection(boost::shared_ptr<NetConnection> connection);
	protected:
		void startAccept();
		void onListenTimer(const boost::system::error_code& error);
		void onAcceptTimer(const boost::system::error_code& error);
		void handleAccept(const boost::system::error_code& err, boost::shared_ptr<NetConnection> connection);
		void addConnection(boost::shared_ptr<NetConnection> connection);
	private:
		io_service &    m_ioservice;
		string			m_listenIp;
		unsigned short  m_listenPort;
		StrandPtr       m_acceptorStrand;//保证任务能够顺序执行
		tcp::acceptor   m_acceptor;//连接接收器
		deadline_timer  m_acceptTimer; //接受连接的定时器
		deadline_timer  m_linstenTimer; //接受连接的定时器
		bool            m_bStopped;
		bool            m_bAutoBind; //监听正常
		bool            m_bReuseAddress;
		boost::recursive_mutex m_mutex;                             /* 全局锁 */
		set<boost::shared_ptr<NetConnection> > m_connectionPool;    /* 连接池 */
	};
}//end of namespace net
