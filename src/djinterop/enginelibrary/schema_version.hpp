/*
    This file is part of libdjinterop.

    libdjinterop is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdjinterop is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libdjinterop.  If not, see <http://www.gnu.org/licenses/>.
 */

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_SCHEMA_VERSION_HPP
#define DJINTEROP_ENGINELIBRARY_SCHEMA_VERSION_HPP

#include <ostream>

namespace djinterop {
namespace enginelibrary {

struct schema_version
{
    int maj;
    int min;
    int pat;
};

static constexpr schema_version version_firmware_1_0_0 { 1, 6, 0 };
static constexpr schema_version version_firmware_1_0_3 { 1, 7, 1 };
static constexpr schema_version version_latest = version_firmware_1_0_3;

class database_inconsistency : public std::logic_error
{
public:
    explicit database_inconsistency(const std::string &what_arg) noexcept :
        logic_error{what_arg}
    {}
    virtual ~database_inconsistency() = default;
};

class unsupported_database_version : public std::runtime_error
{
public:
    explicit unsupported_database_version(
            const std::string &what_arg,
            const schema_version version) noexcept :
        runtime_error{what_arg},
        version_{version}
    {}
    virtual ~unsupported_database_version() = default;
    const schema_version version() const { return version_; }
private:
    schema_version version_;
};

inline std::ostream &operator <<(std::ostream &os, const schema_version &version)
{
    return os << version.maj << "." << version.min << "." << version.pat;
}

inline bool operator ==(const schema_version &a, const schema_version &b)
{
    return a.maj == b.maj && a.min == b.min && a.pat == b.pat;
}

inline bool operator !=(const schema_version &a, const schema_version &b)
{
    return !(a.maj == b.maj && a.min == b.min && a.pat == b.pat);
}

inline bool operator >=(const schema_version &a, const schema_version &b)
{
    if (a.maj != b.maj)
        return a.maj > b.maj;
    if (a.min != b.min)
        return a.min > b.min;
    return a.pat >= b.pat;
}

inline bool operator >(const schema_version &a, const schema_version &b)
{
    if (a.maj != b.maj)
        return a.maj > b.maj;
    if (a.min != b.min)
        return a.min > b.min;
    return a.pat > b.pat;
}

inline bool operator <=(const schema_version &a, const schema_version &b)
{
    if (a.maj != b.maj)
        return a.maj < b.maj;
    if (a.min != b.min)
        return a.min < b.min;
    return a.pat <= b.pat;
}

inline bool operator <(const schema_version &a, const schema_version &b)
{
    if (a.maj != b.maj)
        return a.maj < b.maj;
    if (a.min != b.min)
        return a.min < b.min;
    return a.pat < b.pat;
}

} // enginelibrary
} // djinterop

#endif // DJINTEROP_ENGINELIBRARY_DATABASE_HPP