/**
 * @file cdcl_type_spec.hh
 * @author Derek Huang
 * @brief C++ header for the C declaration type specification
 * @copyright MIT License
 */

#ifndef PDCPL_CDCL_TYPE_SPEC_HH_
#define PDCPL_CDCL_TYPE_SPEC_HH_

#include <string>
#include <utility>

namespace pdcpl {

/**
 * Enum indicating the unqualified type of a declaration.
 *
 * The "g" prefix indicates the type has no relevant sign. "s" indicates the
 * type is signed, while "u" indicate the type is unsigned.
 */
enum class cdcl_type {
  invalid,   // for use in default ctors
  gvoid,     // void
  gchar,     // char, sign is platform-dependent
  schar,     // signed char
  uchar,     // unsigned char
  sint,      // signed int
  uint,      // unsigned int
  sshort,    // signed short
  ushort,    // unsigned short
  slong,     // signed long
  ulong,     // unsigned long
  gfloat,    // float
  gdouble,   // double
  gldouble,  // long double
  gstruct,   // struct
  genum,     // enum
  gtype      // typedef
};

/**
 * Enum indicating a type cv-qualifier.
 */
enum class cdcl_qual {
  invalid,   // for use in default ctors
  qnone,     // no qualifier
  qconst,    // const
  qvolatile  // volatile
};

/**
 * Enum indicating a storage specifier.
 */
enum class cdcl_storage {
  invalid,      // for use in default ctors
  st_auto,      // auto
  st_extern,    // extern
  st_register,  // register
  st_static     // static
};

/**
 * C declaraton type specifier.
 */
class cdcl_type_spec {
public:
  cdcl_type_spec() : cdcl_type_spec{cdcl_type::invalid} {}

  cdcl_type_spec(cdcl_type type) : cdcl_type_spec{type, ""} {}

  cdcl_type_spec(cdcl_type type, const std::string& iden)
    : type_{type}, iden_{iden}
  {}

  auto type() const noexcept { return type_; }
  const auto& iden() const noexcept { return iden_; }

private:
  cdcl_type type_;
  std::string iden_;
};

/**
 * C declaration qualified type specifier.
 */
class cdcl_qtype_spec {
public:
  cdcl_qtype_spec() : qual_{cdcl_qual::invalid}, spec_{} {}

  cdcl_qtype_spec(const cdcl_type_spec& spec)
    : cdcl_qtype_spec{cdcl_qual::qnone, spec}
  {}

  cdcl_qtype_spec(cdcl_qual qual, const cdcl_type_spec& spec)
    : qual_{qual}, spec_{spec}
  {}

  cdcl_qtype_spec(cdcl_qual qual, cdcl_type_spec&& spec)
    : qual_{qual}, spec_{std::move(spec)}
  {}

  const auto& spec() const noexcept { return spec_; }

private:
  cdcl_qual qual_;
  cdcl_type_spec spec_;
};

}  // namespace pdcpl

#endif  // PDCPL_CDCL_TYPE_SPEC_HH_