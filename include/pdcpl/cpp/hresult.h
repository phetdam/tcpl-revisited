/**
 * @file cpp/hresult.h
 * @author Derek Huang
 * @brief C++ header for HRESULT helpers
 */

#ifndef PDCPL_CPP_HRESULT_H_
#define PDCPL_CPP_HRESULT_H_

#ifndef _WIN32
#error "cpp/hresult.h is only available on Windows"
#endif  // _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <sstream>
#include <string>

namespace pdcpl {

/**
 * Return string message with `HRESULT` on current thread's last error code.
 *
 * @param prefix Message prefix
 */
inline auto hresult_message(const char* prefix = "Win32 error")
{
  std::stringstream ss;
  ss << prefix << ": HRESULT " << std::hex << HRESULT_FROM_WIN32(GetLastError());
  return ss.str();
}

/**
 * Return string message with `HRESULT` on current thread's last error code.
 *
 * @param prefix Message prefix
 */
inline auto hresult_message(const std::string& prefix)
{
  return hresult_message(prefix.c_str());
}

}  // namespace pdcpl

// don't pollute translation units
#undef WIN32_LEAN_AND_MEAN

#endif  // PDCPL_CPP_HRESULT_H_
