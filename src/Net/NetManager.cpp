#define DLL_EXPORT_NET _declspec(dllexport)
#include "NetManager.h"

using namespace net;

void NetManager::addConnection(boost::shared_ptr<NetConnection> connection)
{
	boost::recursive_mutex::scoped_lock rlock(m_mutex);
	m_connectionPool.insert(connection);
}

void NetManager::removeConnection(boost::shared_ptr<NetConnection> connection)
{
	boost::recursive_mutex::scoped_lock rlock(m_mutex);
	connection->close();
	if (m_connectionPool.find(connection) != m_connectionPool.end())
		m_connectionPool.erase(connection);
}