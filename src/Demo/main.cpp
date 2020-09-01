#include <iostream>
#include <Net/NetServer.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
int main()
{
	boost::asio::io_service ios;
	boost::asio::io_service::work work(ios);
	auto th = boost::thread(boost::bind(&io_service::run, &ios));
	net::NetServerPtr ptr(new net::NetServer(ios,"192.168.1.110",2000));
	ptr->start();
	th.join();
	system("pause");
}