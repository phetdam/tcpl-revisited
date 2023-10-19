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
  invalid,         // for use in default ctors
  qnone,           // no qualifier
  qconst,          // const
  qvolatile,       // volatile
  qconst_volatile  // const volatile
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
 * C declaration type specifier.
 */
class cdcl_type_spec {
public:
  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_type_spec() : cdcl_type_spec{cdcl_type::invalid} {}

  /**
   * Ctor for builtin tyoes with no identifier.
   *
   * @param type Type
   */
  cdcl_type_spec(cdcl_type type) : cdcl_type_spec{type, ""} {}

  /**
   * Ctor.
   *
   * @param type Type
   * @param iden Type identifier for struct, enum, or typedef
   */
  cdcl_type_spec(cdcl_type type, const std::string& iden)
    : type_{type}, iden_{iden}
  {}

  /**
   * Return the declaration's type.
   */
  auto type() const noexcept { return type_; }

  /**
   * Return the type's identifier.
   *
   * If empty, e.g. for builtin types, the type has no identifier.
   */
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
  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_qtype_spec() : qual_{cdcl_qual::invalid}, spec_{} {}

  /**
   * Ctor.
   *
   * Constructs a none-qualified type specifier from a type specifier.
   *
   * @param spec Type specifier
   */
  cdcl_qtype_spec(const cdcl_type_spec& spec)
    : cdcl_qtype_spec{cdcl_qual::qnone, spec}
  {}

  /**
   * Ctor.
   *
   * Constructs a qualified type specifier from a type specifier.
   *
   * @param qual Type qualifier
   * @param spec Type specifier
   */
  cdcl_qtype_spec(cdcl_qual qual, const cdcl_type_spec& spec)
    : qual_{qual}, spec_{spec}
  {}

  /**
   * Ctor.
   *
   * Constructs a qualified type specifier by moving from a type specifier.
   *
   * @param qual Type qualifier
   * @param spec Type specifier
   */
  cdcl_qtype_spec(cdcl_qual qual, cdcl_type_spec&& spec)
    : qual_{qual}, spec_{std::move(spec)}
  {}

  /**
   * Return type qualifier.
   */
  auto qual() const noexcept { return qual_; }

  /**
   * Return type specifier.
   */
  const auto& spec() const noexcept { return spec_; }

private:
  cdcl_qual qual_;
  cdcl_type_spec spec_;
};

}  // namespace pdcpl

#endif  // PDCPL_CDCL_TYPE_SPEC_HH_