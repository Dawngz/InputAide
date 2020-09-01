#pragma once
#include "NetCommon.h"
#include "NetManager.h"
namespace net
{
	class DLL_EXPORT_NET NetServer :public NetManager , public boost::enable_shared_from_this<NetServer>
	{
	public:
		NetServer(io_service& ioservice, string listenIp, unsigned short listenPort, bool bReuseAddr = true, bool bAutoBind = false);
		virtual ~NetServer();
	public:
		virtual int start(bool async = true)override;
		virtual boost::shared_ptr<NetConnection> createConnection();
		virtual void onAccept(boost::shared_ptr<NetConnection> connection);
		virtual void onPackageReceived(const unsigned char* content, const size_t& contentLen, boost::shared_ptr<net::NetConnection> connection)override
		{}
	protected:
		void startAccept();
		void onListenTimer(const boost::system::error_code& error);
		void onAcceptTimer(const boost::system::error_code& error);
		void handleAccept(const boost::system::error_code& err, boost::shared_ptr<NetConnection> connection);
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
	};
}//end of namespace net
