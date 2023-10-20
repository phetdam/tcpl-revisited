/**
 * @file cdcl_type_spec.hh
 * @author Derek Huang
 * @brief C++ header for the C declaration type specification
 * @copyright MIT License
 */

#ifndef PDCPL_CDCL_TYPE_SPEC_HH_
#define PDCPL_CDCL_TYPE_SPEC_HH_

#include <ostream>
#include <stdexcept>
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
 * Return a string representation for the specified `cdcl_type` value.
 *
 * @param type Type value to get string representation for
 */
inline std::string cdcl_type_printer(cdcl_type type)
{
  switch (type) {
    case cdcl_type::invalid:
      return "[invalid type]";
    case cdcl_type::gvoid:
      return "void";
    case cdcl_type::gchar:
      return "char";
    case cdcl_type::schar:
      return "signed char";
    case cdcl_type::uchar:
      return "unsigned char";
    case cdcl_type::sint:
      return "signed int";
    case cdcl_type::uint:
      return "unsigned int";
    case cdcl_type::sshort:
      return "signed short";
    case cdcl_type::ushort:
      return "unsigned short";
    case cdcl_type::slong:
      return "signed long";
    case cdcl_type::ulong:
      return "unsigned long";
    case cdcl_type::gfloat:
      return "float";
    case cdcl_type::gdouble:
      return "double";
    case cdcl_type::gldouble:
      return "long double";
    case cdcl_type::genum:
      return "enum";
    case cdcl_type::gstruct:
      return "struct";
    case cdcl_type::gtype:
      return "typedef";
    default:
      throw std::runtime_error{"cannot print unknown cdcl_type value"};
  }
}

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
 * Return a string representation for the specified `cdcl_qual` value.
 *
 * @param qual Qualifier value to get string representation for
 */
inline std::string cdcl_qual_printer(cdcl_qual qual)
{
  switch (qual) {
    case cdcl_qual::invalid:
      return "[invalid cv-qualifier]";
    case cdcl_qual::qnone:
      return "";
    case cdcl_qual::qconst:
      return "const";
    case cdcl_qual::qvolatile:
      return "volatile";
    case cdcl_qual::qconst_volatile:
      return "const volatile";
    default:
      throw std::runtime_error{"cannot print unknown cdcl_qual value"};
  }
}

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
 * Return a string representation for the specified `cdcl_storage` value.
 *
 * @param storage Storage specifier value to get string representation for
 */
inline std::string cdcl_storage_printer(cdcl_storage storage)
{
  switch (storage) {
    case cdcl_storage::invalid:
      return "[invalid storage qualifier]";
    case cdcl_storage::st_auto:
      return "";
    case cdcl_storage::st_extern:
      return "extern";
    case cdcl_storage::st_register:
      return "register";
    case cdcl_storage::st_static:
      return "static";
    default:
      throw std::runtime_error{"cannot print unknown cdcl_qual value"};
  }
}

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

  auto& write(std::ostream& out) const
  {
    out << cdcl_type_printer(type_);
    if (iden_.size())
      out << " " << iden_;
    return out;
  }

private:
  cdcl_type type_;
  std::string iden_;
};

inline auto& operator<<(std::ostream& out, const cdcl_type_spec& spec)
{
  return spec.write(out);
}

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

  auto& write(std::ostream& out) const
  {
    auto qual = cdcl_qual_printer(qual_);
    // only print a separating space if qualified
    if (qual.size())
      out << qual << " ";
    // write type specifier
    out << spec_;
    return out;
  }

private:
  cdcl_qual qual_;
  cdcl_type_spec spec_;
};

inline auto& operator<<(std::ostream& out, const cdcl_qtype_spec& spec)
{
  return spec.write(out);
}

}  // namespace pdcpl

#endif  // PDCPL_CDCL_TYPE_SPEC_HH_