#include "reply.hpp"
#include <string>
#include <boost/lexical_cast.hpp>

namespace http {
namespace server {

namespace misc_strings {

const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

} // namespace misc_strings

std::vector<boost::asio::const_buffer> reply::to_buffers()
{
  std::vector<boost::asio::const_buffer> buffers;
  //buffers.push_back(status_strings::to_buffer(status));
  /*for (std::size_t i = 0; i < headers.size(); ++i)
  {
    header& h = headers[i];
    buffers.push_back(boost::asio::buffer(h.name));
    buffers.push_back(boost::asio::buffer(misc_strings::name_value_separator));
    buffers.push_back(boost::asio::buffer(h.value));
    buffers.push_back(boost::asio::buffer(misc_strings::crlf));
  }
  buffers.push_back(boost::asio::buffer(misc_strings::crlf));*/
  buffers.push_back(boost::asio::buffer(content));
  buffers.push_back(boost::asio::buffer(misc_strings::crlf));
  return buffers;
}


void reply::reset()
{
  content.clear();
  headers.clear();
}
} // namespace server
} // namespace http
