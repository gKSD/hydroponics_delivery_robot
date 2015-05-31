//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
//#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

#include <map>

#include <iostream>
//#include <thread>

#include "Server_preferences.hpp"

namespace http {
namespace server {

request_handler::request_handler(const std::string& doc_root)
  : doc_root_(doc_root)
{
}

int request_handler::handle_request(const request& req, reply& rep)
{
  /*
  // Decode url to path.
  std::string request_path;
  if (!url_decode(req.uri, request_path))
  {
    //rep = reply::stock_reply(reply::bad_request);
    return;
  }

  // Request path must be absolute and not contain "..".
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos)
  {
    //rep = reply::stock_reply(reply::bad_request);
    return;
  }

  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_path[request_path.size() - 1] == '/')
  {
    request_path += "index.html";
  }

  // Determine the file extension.
  std::size_t last_slash_pos = request_path.find_last_of("/");
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
    extension = request_path.substr(last_dot_pos + 1);
  }

  // Open the file to send back.
  std::string full_path = doc_root_ + request_path;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
    //rep = reply::stock_reply(reply::not_found);
    return;
  }

  // Fill out the reply to be sent to the client.
  //rep.status = reply::ok;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0)
    rep.content.append(buf, is.gcount());
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
  rep.headers[1].name = "Content-Type";
  //rep.headers[1].value = mime_types::extension_to_type(extension);
  */

  std::map<std::string, std::string>::const_iterator it = req.request_json.find("client");
  if (it == req.request_json.end())
  {
      std::cout << "Request handler error: no client given" << std::endl;
      return REQ_HANDLER_NO_CLIENT_GIVEN;
  }
  else
  {
      std::string client = it->second;
      if (client == "django")
      {
          std::cout << "Django client send" << std::endl;

          it = req.request_json.find("command");
          if (it != req.request_json.end())
          {
              Singleton::Instance()->messages_for_aplication.push(it->second);
              return REQ_HANDLER_NO_REPLY;
          }

          it = req.request_json.find("gardenbed_posx_max");
          if (it != req.request_json.end())
          {
              Server_preferences::field_width = atoi(it->second.c_str());
              std::string res = "gardenbed_posx_max : " + it->second;
              Singleton::Instance()->messages_for_aplication.push(res);
              return REQ_HANDLER_NO_REPLY;
          }

          it = req.request_json.find("gardenbed_posy_max");
          if (it != req.request_json.end())
          {
              Server_preferences::field_height = atoi(it->second.c_str());
              std::string res = "gardenbed_posy_max : " + it->second;
              Singleton::Instance()->messages_for_aplication.push(res);
              return REQ_HANDLER_NO_REPLY;
          }

          it = req.request_json.find("base_pos");
          if (it != req.request_json.end())
          {
              const char *p = it->second.c_str();
              *(Server_preferences::base_pos) = *p;
              *(Server_preferences::base_pos + 1) = *(p + 1);
              *(Server_preferences::base_pos + 2) = '\0';
              std::string tmp = Server_preferences::base_pos;
              std::string res = "base_pos : " + tmp;
              Singleton::Instance()->messages_for_aplication.push(res);
              return REQ_HANDLER_NO_REPLY;
          }
          it = req.request_json.find("robot_tank_volume");
          if (it != req.request_json.end())
          {
              Server_preferences::max_robot_volume = atoi(it->second.c_str());
              std::string res = "robot_tank_volume : " + it->second;
              Singleton::Instance()->messages_for_aplication.push(res);
              return REQ_HANDLER_NO_REPLY;
          }
          it = req.request_json.find("base_ip");
          if (it != req.request_json.end())
          {
              Server_preferences::base_ip = it->second;
              std::string res = "base_ip : " + it->second;
              Singleton::Instance()->messages_for_aplication.push(res);
              return REQ_HANDLER_NO_REPLY;
          }
      }
      else if (client == "base")
      {
          it = req.request_json.find("command_type");
          if (it != req.request_json.end())
          {
              if (it->second == "return")
              // команда запроса возвращения на базу
              {
                  //thr = new std::thread(&request_handler::thread_function, this);
                  Singleton::Instance()->messages_for_aplication.push(it->second);
              }
              else if (it->second == "start_task")
              {
                  Singleton::Instance()->messages_for_aplication.push(it->second);
              }
          }
      }
      else
      {
          std::cout << "Request handler error: invalid client given" << std::endl;
          return REQ_HANDLER_INVALID_CLIENT_GIVEN;
      }
  }

  std::cout << "Request handler call" << std::endl;
  char buf[] = "Hello, it is robot";
  //rep.content.clear();
  rep.content.append(buf, sizeof (buf));
}

int request_handler::get_type(const request& req)
{
    std::map<std::string, std::string>::const_iterator it = req.request_json.find("client");
    if (it == req.request_json.end())
    {
        std::cout << "Request handler error: no client given" << std::endl;
        return REQ_HANDLER_NO_CLIENT_GIVEN;
    }
    if (it->second == "base")
    {
          it = req.request_json.find("base_id");
          if (it != req.request_json.end())
          {
              if (it->second == "1") // TODO: make a constant
                  return REQ_HANDLER_IS_BASE;
              std::cout << "Request handler error: wrong base id given" << std::endl;
              return REQ_HANDLER_INVALID_BASE_ID;    
          }
          std::cout << "Request handler error: no base id given" << std::endl;
          return REQ_HANDLER_INVALID_BASE_ID;
    }
    else if (it->second == "django")
    {
        return REQ_HANDLER_IS_DJANGO;
    }
    else
    {
        std::cout << "Request handler error: invalid client given" << std::endl;
        return REQ_HANDLER_INVALID_CLIENT_GIVEN;
    }
}

/*void request_handler::thread_function()
{
    while (1) 
    {
        std::cout << "Thread" << std::endl;
    }
}*/

bool request_handler::url_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return true;
}

} // namespace server
} // namespace http
