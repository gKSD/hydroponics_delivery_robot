//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <boost/bind.hpp>
#include <signal.h>

#include <iostream>

namespace http {
namespace server {

server::server(const std::string& address, const std::string& port,
    const std::string& doc_root)
  : io_service_(),
    signals_(io_service_),
    acceptor_(io_service_),
    connection_manager_(),
    new_connection_(),
    request_handler_(doc_root),
    preferences_()
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&server::handle_stop, this));

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  thr_ = new std::thread(&server::msgs_queue_thread, this); // разгребает очереди сообщений

  start_accept();
}

void server::msgs_queue_thread()
{
    //сюда прилетают все сообщения от приложения
    // принимается решение кому эти сообщения предназанчены и отправляются соответствующему устрйоству
    while (1)
    {
        if (!Singleton::Instance()->messages_for_base.empty())
        {
            std::string msg = Singleton::Instance()->messages_for_base.front();
            Singleton::Instance()->messages_for_base.pop();
            connection_manager_.get_base_connection()->write(msg);
        }
        else if (!Singleton::Instance()->messages_for_django.empty())
        {
            std::string msg = Singleton::Instance()->messages_for_django.front();
            Singleton::Instance()->messages_for_django.pop();
            connection_manager_.get_base_connection()->write(msg);
        }
    }
}

void server::run()
{
  // The io_service::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  io_service_.run();
}

void server::start_accept()
{
  new_connection_.reset(new connection(io_service_,
        connection_manager_, request_handler_));
  acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&server::handle_accept, this,
        boost::asio::placeholders::error));
}

void server::handle_accept(const boost::system::error_code& e)
{
  // Check whether the server was stopped by a signal before this completion
  // handler had a chance to run.
  if (!acceptor_.is_open())
  {
    return;
  }

  if (!e)
  {
    connection_manager_.start(new_connection_);
  }

  start_accept();
}

void server::handle_stop()
{
  // The server is stopped by cancelling all outstanding asynchronous
  // operations. Once all operations have finished the io_service::run() call
  // will exit.
  std::cout << "server::handle_stop" << std::endl;
  acceptor_.close();
  connection_manager_.stop_all();
}

} // namespace server
} // namespace http