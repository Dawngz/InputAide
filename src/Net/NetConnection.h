#pragma once
#include "NetCommon.h"

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
		void setServer(NetServerPtr server);
		void onDataReceived(const ByteArray& data, int len);
	protected:
		virtual void onConnectionMade();
		virtual void onConnectionError(const boost::system::error_code& err);
		void recvData();
		void handleRecvData(const boost::system::error_code& err, int bytesReceived);
		void handleConnectionError(const boost::system::error_code& err);
	private:
		StrandPtr				m_strand;
		SocketPtr				m_socket;
		string                  m_peerIp;               /* �Զ�ip */
		unsigned short          m_peerPort;				/* �Զ�port */
		bool					m_bConnected;
		int						m_lastSendTime;
		int						m_lastRecvTime;
		ByteArray               m_recvBuf;          /* ���ջ���*/
		int						m_recvBufLen;
		NetServerPtr            m_server;
	};
}
