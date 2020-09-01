#define DLL_EXPORT_NET _declspec(dllexport)
#include "NetConnection.h"
#include "NetServer.h"
namespace net
{
	NetConnection::NetConnection(StrandPtr strand, SocketPtr socket)
		:m_strand(strand),m_socket(socket)
	{
		init();
	}

	void NetConnection::init()
	{
		m_bConnected = false;
		m_recvBuf.resize(100);
		m_recvBufLen = 0;
		m_lastSendTime = INT32_MAX;
		m_lastRecvTime = INT32_MAX;
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
		std::cout << "connect success!" << std::endl;
	}

	void NetConnection::onConnectionError(const boost::system::error_code& err)
	{
		std::cout << "disconnect!" << std::endl;
	}

	void NetConnection::recvData()
	{
		if (!m_bConnected)
		{
			return;
		}

		try
		{
			m_socket->async_read_some(boost::asio::buffer((void*)&m_recvBuf[0], m_recvBuf.size()), m_strand->wrap(boost::bind(&NetConnection::handleRecvData, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
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
			boost::shared_ptr<NetManager> manager = m_pNetManager.lock();
			if (NULL != manager)
			{
				manager->removeConnection(shared_from_this());
			}
		}
	}

	void NetConnection::setNetManager(boost::shared_ptr<NetManager> pNetManager)
	{
		m_pNetManager = pNetManager;
	}

	void NetConnection::onDataReceived(const ByteArray& data, int len)
	{
		unsigned char* content = new unsigned char[64];
		memset(content, 0, sizeof(content));
		for (int i = 0; i < len; ++i)
		{
			content[i] = data[i];
		}
		boost::shared_ptr<NetManager> manager = m_pNetManager.lock();
		if (NULL != manager)
		{
			manager->onPackageReceived(content, len, shared_from_this());
		}
	}
}