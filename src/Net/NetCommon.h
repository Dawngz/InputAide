#pragma once

#ifdef DLL_EXPORT_NET //�������������꣬�Ͳ�ʹ������Ϊ�����ؼ��ֵĺ궨��
#else
#define DLL_EXPORT_NET _declspec(dllimport)
#endif

#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <set>
#include <boost/thread/recursive_mutex.hpp>
using namespace boost::asio;
using namespace boost::asio::ip;
using std::string;
using std::set;

namespace net
{
#define NET_REACCEPT_INTERVAL             3       /* ���½������ӵ�ʱ���� */

	class NetServer;
	typedef boost::shared_ptr<NetServer> NetServerPtr;

	typedef boost::shared_ptr<boost::asio::strand> StrandPtr;

	class NetConnection;
	typedef boost::shared_ptr<NetConnection> NetConnectionPtr;

	typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

	typedef std::vector<BYTE> ByteArray;
}