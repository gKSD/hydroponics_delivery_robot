//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_manager.hpp"
#include <algorithm>
#include <boost/bind.hpp>

namespace http {
namespace server {

void connection_manager::start(connection_ptr c)
{
  connections_.insert(c);
  c->start();
}

void connection_manager::stop(connection_ptr c)
{
  connections_.erase(c);
  c->stop();
}

void connection_manager::stop_all()
{
  std::for_each(connections_.begin(), connections_.end(),
      boost::bind(&connection::stop, _1));
  connections_.clear();
}

const connection_ptr& connection_manager::get_base_connection()
{
	std::set<connection_ptr>::const_iterator it = connections_.begin();
	for (it=connections_.begin(); it!=connections_.end(); ++it)
	{
    	if ((*it)->is_base())
    	{
    		return (*it);
    	}
	}
}
const connection_ptr& connection_manager::get_django_connection()
{
	std::set<connection_ptr>::const_iterator it = connections_.begin();
	for (it=connections_.begin(); it!=connections_.end(); ++it)
	{
    	if (!(*it)->is_base())
    	{
    		return (*it);
    	}
	}
}

} // namespace server
} // namespace http
