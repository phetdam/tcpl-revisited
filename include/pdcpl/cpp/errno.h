/**
 * @file cpp/errno.h
 * @author Derek Huang
 * @brief C++ header for errno helpers
 * @copyright MIT License
 */

#ifndef PDCPL_CPP_ERRNO_H_
#define PDCPL_CPP_ERRNO_H_

#include <cerrno>
#include <sstream>
#include <string>

namespace pdcpl {

inline auto errno_message(const char* prefix = "errno")
{
  std::stringstream ss;
  ss << prefix << ": " << std::strerror(errno) <<
    " (" << std::to_string(errno) << ")";
  return ss.str();
}

inline auto errno_message(const std::string& prefix)
{
  return errno_message(prefix.c_str());
}

}  // namespace pdcpl

#endif  // PDCPL_CPP_ERRNO_H_
