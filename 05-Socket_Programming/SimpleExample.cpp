/*
 * SimpleExample.cpp
 *
 *  Created on: Jul 9, 2025
 *      Author: Osama
 */

#include <iostream>
#include <thread>
#include <chrono>

#ifndef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

std::vector<char> vBuffer(1*1024);

void GrabSomeData(asio::ip::tcp::socket& socket)
{
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
			[&](std::error_code ec, std::size_t length)
			{
					if (!ec)
					{
						std::cout << "\n\rRead "<< length << " bytes\n\r";

						for(int i=0; i<length; i++)
							std::cout << vBuffer[i];

						GrabSomeData(socket);
					}
			}
	);
}
int main()
{
	asio::error_code ec;

	asio::io_context context;

	//Give some fake tasks to asio so the context doesnt finish
	asio::io_context::work idleWork(context);

	//start the context
	std::thread thrContext = std::thread([&]() { context.run(); });

	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("23.192.228.84",ec), 80);

	asio::ip::tcp::socket socket(context);

	socket.connect(endpoint, ec);

	if(!ec)
	{
		std::cout << "Connected!"<<std::endl;
	}
	else
	{
		std::cout << "Failed to connect to address\n" << ec.message() << std::endl;
	}

	if(socket.is_open())
	{
		GrabSomeData(socket);

		std::string sRequest =
				"GET /index.html HTTP/1.1\r\n"
				"HOST: example.com\r\n"
				"Connection: close\r\n\r\n";
		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);


		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);

		context.stop();
		if (thrContext.joinable()) thrContext.join();
	}

	system("pause");

	return 0;
}
