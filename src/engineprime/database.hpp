/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef ENGINEPRIME_DATABASE_HPP
#define ENGINEPRIME_DATABASE_HPP

#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

#include "schema_version.hpp"

namespace engineprime {

class database
{
public:

    explicit database(const std::string &dir_path);
	database(database &&db) = default;
    ~database();

	bool exists() const;

    const std::string &directory_path() const;
    const std::string &music_db_path() const;
    const std::string &performance_db_path() const;

    const std::string &uuid() const;
	const schema_version &version() const;

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

database create_database(const std::string &dir_path, schema_version version);

} // engineprime

#endif // ENGINEPRIME_DATABASE_HPP
