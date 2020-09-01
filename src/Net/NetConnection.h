#pragma once
#include "NetCommon.h"
#include "NetManager.h"
namespace net
{
	class DLL_EXPORT_NET NetConnection : public boost::enable_shared_from_this<NetConnection>
	{
	public:
		NetConnection(StrandPtr strand, SocketPtr socket);
		boost::asio::ip::tcp::socket & getSocket();
		void setPeerIp(string ip);
		void setPeerPort(unsigned short port);
		void close();
		int start();
		void setNetManager(boost::shared_ptr<NetManager> pNetManager);
		void onDataReceived(const ByteArray& data, int len);
	protected:
		virtual void onConnectionMade();
		virtual void onConnectionError(const boost::system::error_code& err);
		void recvData();
		void handleRecvData(const boost::system::error_code& err, int bytesReceived);
		void handleConnectionError(const boost::system::error_code& err);
	private:
		void init();
		StrandPtr				m_strand;
		SocketPtr				m_socket;
		string                  m_peerIp;               /* 对端ip */
		unsigned short          m_peerPort;				/* 对端port */
		bool					m_bConnected;
		int						m_lastSendTime;
		int						m_lastRecvTime;
		ByteArray               m_recvBuf;          /* 接收缓冲*/
		int						m_recvBufLen;
		boost::weak_ptr<NetManager>     m_pNetManager;          /* 连接管理器 */
	};
}
