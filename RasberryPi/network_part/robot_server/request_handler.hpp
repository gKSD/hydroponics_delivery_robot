//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>
//#include <thread>
#include <boost/noncopyable.hpp>

#include "Singleton.hpp"

namespace http {
namespace server {

struct reply;
struct request;
#define REQ_HANDLER_INVALID_BASE_ID -4
#define REQ_HANDLER_INVALID_CLIENT_GIVEN -3
#define REQ_HANDLER_NO_CLIENT_GIVEN -2
#define REQ_HANDLER_ERROR_OCCURED -1
#define REQ_HANDLER_NO_REPLY 0
#define REQ_HANDLER_NEED_REPLY 1
#define REQ_HANDLER_IS_BASE 2
#define REQ_HANDLER_IS_DJANGO 3

/// The common handler for all incoming requests.
class request_handler
  : private boost::noncopyable
{
public:
  /// Construct with a directory containing files to be served.
  explicit request_handler(const std::string& doc_root);

  /// Handle a request and produce a reply.
  int handle_request(const request& req, reply& rep);
  int get_type(const request& req);
  // 
  //void thread_function();
private:
  /// The directory containing the files to be served.
  std::string doc_root_;
  //std::thread *thr;

  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP