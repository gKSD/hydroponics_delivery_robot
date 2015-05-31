#include "request_parser.hpp"
#include "request.hpp"

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <map>

namespace http {
namespace server {

request_parser::request_parser()
  : state_(method_start)
{
}

void request_parser::reset()
{
  state_ = method_start;
}

int request_parser::parse_next_param (char ** str, char *end, char *param, char * param_value)
{
    char *cursor = (char *)*str;
    std::cout << "Parse next param => " << cursor << std::endl;
    
    while (cursor != end &&  isspace (*cursor)) cursor ++;
    if (cursor == end)
    {
        std::cout << "Parse: empty json" << std::endl;
        return 0;
    }

    char quote = '\0';
    if (*cursor == '\'' || *cursor == '"')
      quote = *cursor;
    else
    {
        std::cout << "Parse error: quote parse error" << std::endl;
        return -1;
    }

    char *param_pos = ++cursor;
    std::cout << "Debug param_pos: " << param_pos << std::endl;
    while (cursor != end && *cursor != quote) cursor++;
    if (cursor == end)
    {
        std::cout << "Parse error: key parse error" << std::endl;
        return -1;
    }
    int param_len = cursor - param_pos + 1;
    snprintf(param, param_len, "%s", param_pos);
    std::cout << "Debug param: " << param << std::endl;

    while (cursor != end && *cursor != ':') cursor++;
    if (cursor == end)
    {
        std::cout << "Parse error: no ':' found " << std::endl;
        return -1;
    }
    std::cout << "Debug colon found: " << cursor << std::endl;
    cursor++;

    while (cursor != end &&  isspace (*cursor)) cursor ++;
    if (cursor == end)
    {
        std::cout << "Parse: no value found" << std::endl;
        return -1;
    }

    if (*cursor == '\'' || *cursor == '"')
      quote = *cursor;
    else
    {
        std::cout << "Parse error: quote parse error" << std::endl;
        return -1;
    }
    char *param_value_pos = ++cursor;
    std::cout << "Debug param_value_pos: " << param_value_pos << std::endl;
    while (cursor != end && *cursor != quote) cursor++;

    if (cursor == end)
    {
        std::cout << "Parse error: value parse error" << std::endl;
        return -1;
    }
    int param_value_len = cursor - param_value_pos + 1;
    snprintf(param_value, param_value_len, "%s", param_value_pos);
    std::cout << "Debug param_value: " << param_value << std::endl;

    while (cursor != end && *cursor != ',' && *cursor != '}') cursor++;
    if (cursor == end)
    {
        std::cout << "Parse error: end value error" << std::endl;
        return -1;
    }
    else cursor++;
    *str = cursor;

    return 1;
}

bool request_parser::parse_(request& req, char *begin, char *end)
{
    //Entered string => {"client": "django", "command": "turn_left"}
    std::cout << "Entered string => " << begin << std::endl;
    bool open_bracket = false;
    while (begin != end)
    {
        std::cout << *begin << std::endl;
        if (*begin  == '{')
        {
            open_bracket = true;
            begin++;
        }
        else
        {
            if (!open_bracket)
                begin++;
            else
            {
                char key[1024];
                char value [1024];
                memset (key, 0, sizeof (key));
                memset (value, 0, sizeof(value));
                int state = parse_next_param(&begin, end, key, value);
                if (state == 1)
                {
                    std::cout << "Key: "<< key << "  value: "<< value << std::endl;
                    std::string key_str (key);
                    std::string value_str (value);
                    req.request_json[key_str] = value_str;
                }
                else if (state == 0)
                    break;
                else
                {
                    std::cout << "Parse error: error parsing key-value!" << std::endl;
                    return false;
                }
            }
        }
    }
    if (begin == end && !open_bracket)
    {
        std::cout << "Parse error: invalid json construction!" << std::endl;
        return false;
    }
    return true;
}

boost::tribool request_parser::consume(request& req, char input)
{
  switch (state_)
  {
  case method_start:
    if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      state_ = method;
      req.method.push_back(input);
      return boost::indeterminate;
    }
  case method:
    if (input == ' ')
    {
      state_ = uri;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      req.method.push_back(input);
      return boost::indeterminate;
    }
  case uri_start:
    if (is_ctl(input))
    {
      return false;
    }
    else
    {
      state_ = uri;
      req.uri.push_back(input);
      return boost::indeterminate;
    }
  case uri:
    if (input == ' ')
    {
      state_ = http_version_h;
      return boost::indeterminate;
    }
    else if (is_ctl(input))
    {
      return false;
    }
    else
    {
      req.uri.push_back(input);
      return boost::indeterminate;
    }
  case http_version_h:
    if (input == 'H')
    {
      state_ = http_version_t_1;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_t_1:
    if (input == 'T')
    {
      state_ = http_version_t_2;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_t_2:
    if (input == 'T')
    {
      state_ = http_version_p;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_p:
    if (input == 'P')
    {
      state_ = http_version_slash;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_slash:
    if (input == '/')
    {
      req.http_version_major = 0;
      req.http_version_minor = 0;
      state_ = http_version_major_start;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_major_start:
    if (is_digit(input))
    {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      state_ = http_version_major;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_major:
    if (input == '.')
    {
      state_ = http_version_minor_start;
      return boost::indeterminate;
    }
    else if (is_digit(input))
    {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_minor_start:
    if (is_digit(input))
    {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      state_ = http_version_minor;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_minor:
    if (input == '\r')
    {
      state_ = expecting_newline_1;
      return boost::indeterminate;
    }
    else if (is_digit(input))
    {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case expecting_newline_1:
    if (input == '\n')
    {
      state_ = header_line_start;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case header_line_start:
    if (input == '\r')
    {
      state_ = expecting_newline_3;
      return boost::indeterminate;
    }
    else if (!req.headers.empty() && (input == ' ' || input == '\t'))
    {
      state_ = header_lws;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      req.headers.push_back(header());
      req.headers.back().name.push_back(input);
      state_ = header_name;
      return boost::indeterminate;
    }
  case header_lws:
    if (input == '\r')
    {
      state_ = expecting_newline_2;
      return boost::indeterminate;
    }
    else if (input == ' ' || input == '\t')
    {
      return boost::indeterminate;
    }
    else if (is_ctl(input))
    {
      return false;
    }
    else
    {
      state_ = header_value;
      req.headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case header_name:
    if (input == ':')
    {
      state_ = space_before_header_value;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      req.headers.back().name.push_back(input);
      return boost::indeterminate;
    }
  case space_before_header_value:
    if (input == ' ')
    {
      state_ = header_value;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case header_value:
    if (input == '\r')
    {
      state_ = expecting_newline_2;
      return boost::indeterminate;
    }
    else if (is_ctl(input))
    {
      return false;
    }
    else
    {
      req.headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case expecting_newline_2:
    if (input == '\n')
    {
      state_ = header_line_start;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case expecting_newline_3:
    return (input == '\n');
  default:
    return false;
  }
}

bool request_parser::is_char(int c)
{
  return c >= 0 && c <= 127;
}

bool request_parser::is_ctl(int c)
{
  return (c >= 0 && c <= 31) || (c == 127);
}

bool request_parser::is_tspecial(int c)
{
  switch (c)
  {
  case '(': case ')': case '<': case '>': case '@':
  case ',': case ';': case ':': case '\\': case '"':
  case '/': case '[': case ']': case '?': case '=':
  case '{': case '}': case ' ': case '\t':
    return true;
  default:
    return false;
  }
}

bool request_parser::is_digit(int c)
{
  return c >= '0' && c <= '9';
}

} // namespace server
} // namespace http