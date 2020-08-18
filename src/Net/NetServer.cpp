#define DLL_EXPORT_NET _declspec(dllexport)

#include "NetServer.h"
#include "NetConnection.h"
using namespace net;

NetServer::NetServer(io_service& ioservice, string listenIp, unsigned short listenPort, bool bReuseAddr, bool autoBind)
	:m_ioservice(ioservice),
m_listenIp(listenIp),
m_listenPort(listenPort),
m_acceptorStrand(new boost::asio::strand(ioservice)),
m_acceptor(ioservice), 
m_acceptTimer(ioservice),
m_linstenTimer(ioservice),
m_bStopped(false),
m_bAutoBind(autoBind),
m_bReuseAddress(bReuseAddr)
{
	if (m_bAutoBind)
	{
		bool bNeedBind = true;
		try
		{
			do
			{
				/* 初始化连接接收器 */
				boost::asio::socket_base::reuse_address reuseAddrOption(bReuseAddr);
				boost::asio::socket_base::keep_alive keepaliveOption(true);
				tcp::endpoint listenEndpoint(ip::address::from_string(m_listenIp.c_str()), m_listenPort);
				m_acceptor.open(listenEndpoint.protocol());
				boost::system::error_code error;
				if (bReuseAddr)
				{
					m_acceptor.set_option(reuseAddrOption, error);
				}
				m_acceptor.set_option(keepaliveOption, error);
				m_acceptor.bind(listenEndpoint, error);
				if (!error)
				{
					bNeedBind = false;
					m_acceptor.listen();
				}
				else
				{
					m_acceptor.close();
					if (m_listenPort < 10000)
					{
						m_listenPort = 10000;
					}
					if (m_listenPort < 65534)
					{
						++m_listenPort;
					}
					else
					{
						m_listenPort = 10000;
					}

				}
			} while (bNeedBind);
		}
		catch (const boost::system::error_code& error)
		{
			boost::system::error_code _error;
			m_acceptor.cancel(_error);
		}
		catch (...)
		{
			boost::system::error_code error;
			m_acceptor.cancel(error);
		}
	}
}

int NetServer::start(bool async)
{
	if (!m_bAutoBind)
	{
		/* 初始化连接接收器 */
		boost::asio::socket_base::reuse_address reuseAddrOption(m_bReuseAddress);
		boost::asio::socket_base::keep_alive keepaliveOption(true);
		tcp::endpoint listenEndpoint(ip::address::from_string(m_listenIp.c_str()), m_listenPort);
		m_acceptor.open(listenEndpoint.protocol());
		boost::system::error_code error;
		if (m_bReuseAddress)
		{
			m_acceptor.set_option(reuseAddrOption, error);
		}
		m_acceptor.set_option(keepaliveOption, error);
		m_acceptor.bind(listenEndpoint, error);
		if (error != boost::system::error_code())
		{
			m_acceptor.close();
			//TT_STDLOG(LLV_ERROR, "%s listen port %d error %s", __FUNCTION__, m_listenPort, error.message().c_str());
		}
		else
		{
			m_acceptor.listen();
		}
	}

	if (m_acceptor.is_open())
	{
		m_acceptorStrand->post(boost::bind(&NetServer::startAccept, shared_from_this()));
	}
	else {

		//接口单同步启动一个服务，如果监听失败，需要抛出一个异常
		if (!async)
		{
			throw "端口监听失败";
		}

		m_linstenTimer.expires_from_now(boost::posix_time::seconds(NET_REACCEPT_INTERVAL));
		m_linstenTimer.async_wait(m_acceptorStrand->wrap(boost::bind(&NetServer::onListenTimer, shared_from_this(), _1)));
	}
	return 0;
}

NetServer::~NetServer()
{}

void NetServer::startAccept()
{
	boost::shared_ptr<NetConnection> connection = createConnection();
	connection->setServer(shared_from_this());
	m_acceptor.async_accept(connection->getSocket(), m_acceptorStrand->wrap(boost::bind(&NetServer::handleAccept, shared_from_this(), boost::asio::placeholders::error, connection)));
}

void NetServer::onListenTimer(const boost::system::error_code& error)
{
	if (!error && !m_bStopped)
	{
		if (!m_acceptor.is_open())
		{
			tcp::endpoint listenEndpoint(ip::address::from_string(m_listenIp.c_str()), m_listenPort);
			m_acceptor.open(listenEndpoint.protocol());
			boost::system::error_code error;
			m_acceptor.bind(listenEndpoint, error);
			if (!error)
			{
				m_acceptor.listen(boost::asio::socket_base::max_connections, error);
			}
			if (!error)
			{
				startAccept();
			}
			else {
				m_acceptor.close(error);
				m_linstenTimer.expires_from_now(boost::posix_time::seconds(NET_REACCEPT_INTERVAL));
				m_linstenTimer.async_wait(m_acceptorStrand->wrap(boost::bind(&NetServer::onListenTimer, shared_from_this(), _1)));
			}
		}
	}
}

boost::shared_ptr<NetConnection> NetServer::createConnection()
{
	net::StrandPtr strand(new boost::asio::strand(m_ioservice));
	SocketPtr socket(new boost::asio::ip::tcp::socket(m_ioservice));
	return boost::shared_ptr<NetConnection>(new NetConnection(strand, socket));
}

void NetServer::handleAccept(const boost::system::error_code& err, boost::shared_ptr<NetConnection> connection)
{
	if (!err)
	{
		try
		{
			tcp::endpoint endpoint = connection->getSocket().remote_endpoint();
			connection->setPeerIp(endpoint.address().to_string());
			connection->setPeerPort(endpoint.port());
		}
		catch (...)
		{
		}

		onAccept(connection);


		/* 监听新连接 */
		startAccept();
	}
	else
	{
		if (!m_bStopped && m_acceptor.is_open())
		{
			m_acceptTimer.expires_from_now(boost::posix_time::seconds(NET_REACCEPT_INTERVAL));
			m_acceptTimer.async_wait(m_acceptorStrand->wrap(boost::bind(&NetServer::onAcceptTimer, shared_from_this(), _1)));
		}
	}
}

void NetServer::addConnection(boost::shared_ptr<NetConnection> connection)
{
	boost::recursive_mutex::scoped_lock rlock(m_mutex);
	m_connectionPool.insert(connection);
}

void NetServer::removeConnection(boost::shared_ptr<NetConnection> connection)
{
	boost::recursive_mutex::scoped_lock rlock(m_mutex);
	connection->close();
	if (m_connectionPool.find(connection) != m_connectionPool.end())
		m_connectionPool.erase(connection);
}

void NetServer::onAccept(boost::shared_ptr<NetConnection> connection)
{
	addConnection(connection);
	connection->start();
}

void NetServer::onAcceptTimer(const boost::system::error_code& error)
{
	if (!error)
	{
		startAccept();
	}
}
