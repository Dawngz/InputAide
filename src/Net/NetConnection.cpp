#include "NetConnection.h"
#include "NetServer.h"
namespace net
{
	NetConnection::NetConnection(StrandPtr strand, SocketPtr socket)
		:m_strand(strand),m_socket(socket)
	{

	}

	boost::asio::ip::tcp::socket & NetConnection::getSocket()
	{
		return *m_socket;
	}

	void NetConnection::setPeerIp(string ip)
	{
		m_peerIp = ip;
	}

	void NetConnection::setPeerPort(unsigned short port)
	{
		m_peerPort = port;
	}

	void NetConnection::close()
	{

	}

	int NetConnection::start()
	{
		m_bConnected = true;
		m_lastSendTime = (int)time(NULL);
		m_lastRecvTime = (int)time(NULL);

		onConnectionMade();

		/* ¶ÁÈ¡°ü */
		m_strand->post(boost::bind(&NetConnection::recvData, shared_from_this()));

		return 0;
	}

	void NetConnection::onConnectionMade()
	{

	}

	void NetConnection::onConnectionError(const boost::system::error_code& err)
	{

	}

	void NetConnection::recvData()
	{
		if (!m_bConnected)
		{
			return;
		}

		try
		{
			m_socket->async_read_some(boost::asio::buffer(&m_recvBuf[0], m_recvBuf.size()), m_strand->wrap(boost::bind(&NetConnection::handleRecvData, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
		}
		catch (...)
		{
			boost::system::error_code err(boost::asio::error::network_down);
			m_strand->post(boost::bind(&NetConnection::handleConnectionError, shared_from_this(), err));
			//handleConnectionError(err);
		}
	}

	void NetConnection::handleRecvData(const boost::system::error_code& err, int bytesReceived)
	{
		if (!err)
		{
			m_lastRecvTime = (int)time(NULL);

			m_recvBufLen = bytesReceived;
			if (m_recvBufLen > 0)
			{
				try
				{
					onDataReceived(m_recvBuf, m_recvBufLen);
				}
				catch (const std::exception& e)
				{
					//XT_OBJLOG(LLV_ERROR, BF("%1%") % e.what());
				}
				catch (...)
				{
					//XT_OBJLOG(LLV_ERROR, BF("unknown error"));
				}
			}
			m_recvBufLen = 0;
			recvData();
		}
		else
		{
			handleConnectionError(err);
		}
	}

	void NetConnection::handleConnectionError(const boost::system::error_code& err)
	{
		if (m_bConnected)
		{
			close();
			onConnectionError(err);
			m_server->removeConnection(shared_from_this());
		}
	}

	void NetConnection::setServer(NetServerPtr server)
	{
		m_server = server;
	}

	void NetConnection::onDataReceived(const ByteArray& data, int len)
	{

	}
}