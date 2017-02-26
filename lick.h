/* ----------------------------------------------------------------------------
lick.h

Copyright 2017 Jason Lucas (JasonL9000@gmail.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
  HTTP://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
---------------------------------------------------------------------------- */

#pragma once

#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <ostream>
#include <regex>
#include <stdexcept>
#include <sstream>
#include <string>
#include <type_traits>

// Marks the current file:line position within source code.
#define HERE ::lick::loc_t { __FILE__, __LINE__ }

// Define a test fixture.
#define FIXTURE(name)                               \
  static void name();                               \
  static const ::lick::fixture_t                    \
      lick_fixture__##name { HERE, #name, name };   \
  static void name()

// Defines an expectation that the operand is true.
#define EXPECT(operand) (                           \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::true_t {                 \
          ::lick::as_operand(#operand, operand)     \
        }                                           \
      }                                             \
    )

// Defines an expectation that the operand is false.
#define EXPECT_NOT(operand) (                       \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::false_t {                \
          ::lick::as_operand(#operand, operand)     \
        }                                           \
      }                                             \
    )

// Defines an expectation that the operands are equal.
#define EXPECT_EQ(lhs, rhs) (                       \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::eq_t {                   \
          ::lick::as_operand(#lhs, lhs),            \
          ::lick::as_operand(#rhs, rhs)             \
        }                                           \
      }                                             \
    )

// Defines an expectation that the operands are unequal.
#define EXPECT_NE(lhs, rhs) (                       \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::ne_t {                   \
          ::lick::as_operand(#lhs, lhs),            \
          ::lick::as_operand(#rhs, rhs)             \
        }                                           \
      }                                             \
    )

// Defines an expectation that lhs < rhs.
#define EXPECT_LT(lhs, rhs) (                       \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::lt_t {                   \
          ::lick::as_operand(#lhs, lhs),            \
          ::lick::as_operand(#rhs, rhs)             \
        }                                           \
      }                                             \
    )

// Defines an expectation that lhs <= rhs.
#define EXPECT_LE(lhs, rhs) (                       \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::le_t {                   \
          ::lick::as_operand(#lhs, lhs),            \
          ::lick::as_operand(#rhs, rhs)             \
        }                                           \
      }                                             \
    )

// Defines an expectation that lhs > rhs.
#define EXPECT_GT(lhs, rhs) (                       \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::gt_t {                   \
          ::lick::as_operand(#lhs, lhs),            \
          ::lick::as_operand(#rhs, rhs)             \
        }                                           \
      }                                             \
    )

// Defines an expectation that lhs >= rhs.
#define EXPECT_GE(lhs, rhs) (                       \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::ge_t {                   \
          ::lick::as_operand(#lhs, lhs),            \
          ::lick::as_operand(#rhs, rhs)             \
        }                                           \
      }                                             \
    )

#define EXPECT_ALMOST_EQ(lhs, rhs, coef) (          \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::almost_eq_t {            \
          ::lick::as_operand(#lhs, lhs),            \
          ::lick::as_operand(#rhs, rhs),            \
          ::lick::as_operand(#coef, coef)           \
        }                                           \
      }                                             \
    )

#define EXPECT_NOT_ALMOST_EQ(lhs, rhs, coef) (      \
      ::lick::expectation_t {                       \
        HERE,                                       \
        ::lick::predicate::not_almost_eq_t {        \
          ::lick::as_operand(#lhs, lhs),            \
          ::lick::as_operand(#rhs, rhs),            \
          ::lick::as_operand(#coef, coef)           \
        }                                           \
      }                                             \
    )

// These macros exist for backward compatibility.
#define EXPECT_TRUE(operand) EXPECT(operand)
#define EXPECT_FALSE(operand) EXPECT_NOT(operand)

namespace lick {

std::ostream &strm() noexcept;

template <typename val_t>
void write(std::ostream &strm, const val_t &val) {
  strm << val;
}

template <typename operand_t>
bool as_bool(const operand_t &operand) {
  return static_cast<bool>(operand);
}

template <typename lhs_t, typename rhs_t>
bool eq(const lhs_t &lhs, const rhs_t &rhs) {
  return lhs == rhs;
}

template <typename lhs_t, typename rhs_t>
bool ne(const lhs_t &lhs, const rhs_t &rhs) {
  return lhs != rhs;
}

template <typename lhs_t, typename rhs_t>
bool lt(const lhs_t &lhs, const rhs_t &rhs) {
  return lhs < rhs;
}

template <typename lhs_t, typename rhs_t>
bool le(const lhs_t &lhs, const rhs_t &rhs) {
  return lhs <= rhs;
}

template <typename lhs_t, typename rhs_t>
bool gt(const lhs_t &lhs, const rhs_t &rhs) {
  return lhs > rhs;
}

template <typename lhs_t, typename rhs_t>
bool ge(const lhs_t &lhs, const rhs_t &rhs) {
  return lhs >= rhs;
}

template <typename lhs_t, typename rhs_t, typename coef_t>
bool almost_eq(lhs_t lhs, rhs_t rhs, coef_t coef) {
  static constexpr auto epsilon = std::numeric_limits<lhs_t>::epsilon();
  return std::fabs(rhs - lhs) < (epsilon * static_cast<lhs_t>(coef));
}

// NB: The following four overloads are left intentionally left undefined.
// Don't compare floating-point numbers for equality.
// Use ALMOST_EQUALS instead.
template <typename rhs_t>
bool eq(float, const rhs_t &);

template <typename rhs_t>
bool eq(double, const rhs_t &);

template <typename rhs_t>
bool ne(float, const rhs_t &);

template <typename rhs_t>
bool ne(double, const rhs_t &);

extern const char
    *pass, *fail,
    *separator,
    *red, *green,
    *bold, *plain;

class pf_t final {
public:

  pf_t(bool ok_)
      : ok(ok_) {}

  friend std::ostream &operator<<(std::ostream &strm, const pf_t &that) {
    if (that.ok) {
      strm << green << pass;
    } else {
      strm << red << fail;
    }
    return strm << plain;
  }

private:

  bool ok;

};  // pf_t

class indent_t final {
public:

  indent_t(int depth_)
      : depth(depth_) {}

  friend std::ostream &operator<<(std::ostream &strm, const indent_t &that) {
    for (int i = 0; i < that.depth; ++i) {
      strm << "  ";
    }
    return strm;
  }

private:

  int depth;

};  // indent_t

std::string get_ex_msg(const std::exception &ex);

const std::string &get_ex_msg();

template <typename obj_t>
class writer_t final {
public:

  using p2m_t = void (obj_t::*)(std::ostream &) const;

  writer_t(const obj_t &obj_, p2m_t p2m_)
      : obj(obj_), p2m(p2m_) {}

  writer_t(const writer_t &) = default;

  writer_t &operator=(const writer_t &) = default;

  friend std::ostream &operator<<(std::ostream &strm, const writer_t &that) {
    (that.obj.*(that.p2m))(strm);
    return strm;
  }

private:

  const obj_t &obj;

  p2m_t p2m;

};  // writer_t<obj_t>

class loc_t final {
public:

  loc_t(const char *file_, int line_)
      : file(file_), line(line_) {}

  loc_t(const loc_t &) = default;

  loc_t &operator=(const loc_t &) = default;

  friend std::ostream &operator<<(std::ostream &strm, const loc_t &that) {
    return strm << that.file << ':' << that.line;
  }

private:

  const char *file;

  int line;

};  // loc_t

template <typename val_t>
class opt_t final {
public:

  opt_t() noexcept
      : constructed(false) {}

  opt_t(opt_t &&that) noexcept
      : constructed(that.constructed) {
    if (constructed) {
      new (&storage) val_t(std::forward<val_t>(*that));
    }
  }

  opt_t(const opt_t &that)
      : constructed(that.constructed) {
    if (constructed) {
      new (&storage) val_t(*that);
    }
  }

  opt_t(val_t that)
      : constructed(true) {
    new (&storage) val_t(std::forward<val_t>(that));
  }

  ~opt_t() {
    if (constructed) {
      get()->~val_t();
    }
  }

  opt_t &operator=(opt_t &&that) noexcept {
    if (this != &that) {
      this->~opt_t();
      new (this) opt_t(std::move(that));
    }
    return *this;
  }

  opt_t &operator=(const opt_t &that) {
    return *this = opt_t { that };
  }

  operator bool() const noexcept {
    return constructed;
  }

  const val_t &operator*() const noexcept {
    return *get();
  }

  val_t &operator*() noexcept {
    return *get();
  }

  const val_t *operator->() const noexcept {
    return get();
  }

  val_t *operator->() noexcept {
    return get();
  }

  const val_t *get() const noexcept {
    return reinterpret_cast<const val_t *>(&storage);
  }

  val_t *get() noexcept {
    return reinterpret_cast<val_t *>(&storage);
  }

  opt_t &reset() {
    return *this = opt_t {};
  }

private:

  std::aligned_union<sizeof(val_t), val_t> storage;

  bool constructed;

};  // opt_t<val_t>

template <typename ret_t>
class stalled_t final {
public:

  template <typename fn_t, typename... args_t>
  stalled_t(const fn_t &fn, args_t &&... args) noexcept {
    try {
      ret = fn(std::forward<args_t>(args)...);
      ok = true;
    } catch (const std::exception &ex) {
      ok = false;
      msg = get_ex_msg(ex);
    } catch (...) {
      ok = false;
      msg = get_ex_msg();
    }
  }

  stalled_t(const stalled_t &) = default;

  stalled_t &operator=(const stalled_t &) = default;

  operator bool() const noexcept {
    return ok;
  }

  opt_t<ret_t> ret;

  std::string msg;

private:

  bool ok;

};  // stalled_t<ret_t>

template <>
class stalled_t<void> final {
public:

  template <typename fn_t, typename... args_t>
  stalled_t(const fn_t &fn, args_t &&... args) noexcept {
    try {
      fn(std::forward<args_t>(args)...);
      ok = true;
    } catch (const std::exception &ex) {
      ok = false;
      msg = get_ex_msg(ex);
    } catch (...) {
      ok = false;
      msg = get_ex_msg();
    }
  }

  stalled_t(const stalled_t &) = default;

  stalled_t &operator=(const stalled_t &) = default;

  operator bool() const noexcept {
    return ok;
  }

  std::string msg;

private:

  bool ok;

};  // stalled_t<ret_t>

template <typename fn_t, typename... args_t>
auto stall(const fn_t &fn, args_t &&... args)
    -> stalled_t<decltype(fn(std::forward<args_t>(args)...))>  {
  return { fn, std::forward<args_t>(args)... };
}

class cfg_t final {
public:

  cfg_t();

  cfg_t(const cfg_t &) = default;

  cfg_t &operator=(const cfg_t &) = default;

  const std::regex &get_regex() const noexcept {
    return regex;
  }

  std::ostream &get_strm() const noexcept {
    return *strm;
  }

  bool is_strict() const noexcept {
    return strict;
  }

  int get_verbosity() const noexcept {
    return verbosity;
  }

  void set_regex(std::regex regex_) {
    regex = std::move(regex_);
  }

  void set_strm(std::ostream &strm_) {
    strm = &strm_;
  }

  void set_verbosity(int verbosity_) {
    verbosity = (verbosity_ < 0) ? 0 : (verbosity_ > 2) ? 2 : verbosity_;
  }

  static bool parse(cfg_t &cfg, int argc, char *argv[]);

private:

  std::ostream *strm;

  std::regex regex;

  int verbosity;

  bool strict;

};  // cfg_t

class fixture_t;

class ctxt_t final {
public:

  ctxt_t(const fixture_t *fixture, const cfg_t &cfg);

  ~ctxt_t();

  ctxt_t(const ctxt_t &) = delete;

  ctxt_t &operator=(const ctxt_t &) = delete;

  operator bool() const noexcept {
    return ok;
  }

  void fail() {
    ok = false;
  }

  const cfg_t &get_cfg() const noexcept {
    return cfg;
  }

  const fixture_t *get_fixture() const noexcept {
    return fixture;
  }

  std::ostream &get_strm() const {
    on_begin_show();
    return cfg.get_strm();
  }

  static ctxt_t *get_singleton() {
    return singleton;
  }

private:

  void on_begin_show() const;

  void on_end_show() const;

  const fixture_t *fixture;

  const cfg_t &cfg;

  mutable bool showing;

  bool ok;

  static thread_local ctxt_t *singleton;

};  // ctxt_t

inline std::ostream &strm() noexcept {
  return ctxt_t::get_singleton()->get_strm();
}

class fixture_t final {
public:

  using cb_t = std::function<bool (const fixture_t &)>;
  using fn_t = void (*)();

  fixture_t(const loc_t &loc, const char *name, fn_t fn);

  fixture_t(const fixture_t &) = delete;

  fixture_t &operator=(const fixture_t &) = delete;

  bool operator()(const cfg_t &cfg) const;

  const loc_t &get_loc() const noexcept {
    return loc;
  }

  const char *get_name() const noexcept {
    return name;
  }

  static bool for_each(const cb_t &cb);

private:

  loc_t loc;

  const char *name;

  fn_t fn;

  fixture_t *next;

  static fixture_t *first, *last;

};  // fixture_t

class any_operand_t {
public:

  virtual ~any_operand_t();

  any_operand_t(const any_operand_t &) = delete;

  any_operand_t &operator=(const any_operand_t &) = delete;

  const char *get_src() const noexcept {
    return src;
  }

  void write_src(std::ostream &strm) const {
    strm << src;
  }

  virtual void write_val(std::ostream &strm) const = 0;

protected:

  any_operand_t(const char *src_)
      : src(src_) {}

private:

  const char *src;

};  // any_operand_t

writer_t<any_operand_t> src_of(const any_operand_t &operand);

inline writer_t<any_operand_t> src_of(const any_operand_t &operand) {
  return { operand, &any_operand_t::write_src };
}

writer_t<any_operand_t> val_of(const any_operand_t &operand);

inline writer_t<any_operand_t> val_of(const any_operand_t &operand) {
  return { operand, &any_operand_t::write_val };
}

template <typename val_t>
class operand_t final
    : public any_operand_t {
public:

  operand_t(const char *src, const val_t &val_)
      : any_operand_t(src), val(val_) {}

  virtual void write_val(std::ostream &strm) const override {
    write(strm, val);
  }

  const val_t &val;

};  // operand_t<val_t>

template <typename val_t>
operand_t<val_t> as_operand(const char *src, const val_t &val) {
  return { src, val };
}

class predicate_t {
public:

  using cb_t = std::function<bool (const any_operand_t &)>;

  predicate_t(const predicate_t &) = delete;

  virtual ~predicate_t();

  predicate_t &operator=(const predicate_t &) = delete;

  operator bool() const noexcept {
    return ok;
  }

  virtual const char *get_name() const = 0;

  virtual bool for_each_operand(const cb_t &cb) const = 0;

  void write_src(std::ostream &strm) const;

protected:

  explicit predicate_t(bool ok_)
      : ok(ok_) {}

private:

  bool ok;

};  // predicate_t

writer_t<predicate_t> src_of(const predicate_t &predicate);

inline writer_t<predicate_t> src_of(const predicate_t &predicate) {
  return { predicate, &predicate_t::write_src };
}

class unary_t
    : public predicate_t {
public:

  virtual bool for_each_operand(const cb_t &cb) const override final;

protected:

  unary_t(bool ok, const any_operand_t &operand_)
      : predicate_t(ok), operand(operand_) {}

private:

  const any_operand_t &operand;

};  // unary_t

class binary_t
    : public predicate_t {
public:

  virtual bool for_each_operand(const cb_t &cb) const override final;

protected:

  binary_t(bool ok, const any_operand_t &lhs_, const any_operand_t &rhs_)
      : predicate_t(ok), lhs(lhs_), rhs(rhs_) {}

private:

  const any_operand_t &lhs, &rhs;

};  // binary_t

class ternary_t
    : public predicate_t {
public:

  virtual bool for_each_operand(const cb_t &cb) const override final;

protected:

  ternary_t(
      bool ok, const any_operand_t &lhs_, const any_operand_t &rhs_,
      const any_operand_t &coef_)
      : predicate_t(ok), lhs(lhs_), rhs(rhs_), coef(coef_) {}

private:

  const any_operand_t &lhs, &rhs, &coef;

};  // ternary_t

namespace predicate {

class true_t final
    : public unary_t {
public:

  template <typename val_t>
  explicit true_t(const operand_t<val_t> &operand)
      : unary_t(as_bool(operand.val), operand) {}

  virtual const char *get_name() const override;

};  // true_t

class false_t final
    : public unary_t {
public:

  template <typename val_t>
  explicit false_t(const operand_t<val_t> &operand)
      : unary_t(!as_bool(operand.val), operand) {}

  virtual const char *get_name() const override;

};  // false_t

class eq_t final
    : public binary_t {
public:

  template <typename lhs_t, typename rhs_t>
  eq_t(const operand_t<lhs_t> &lhs, const operand_t<rhs_t> &rhs)
      : binary_t(eq(lhs.val, rhs.val), lhs, rhs) {}

  virtual const char *get_name() const override;

};  // eq_t

class ne_t final
    : public binary_t {
public:

  template <typename lhs_t, typename rhs_t>
  ne_t(const operand_t<lhs_t> &lhs, const operand_t<rhs_t> &rhs)
      : binary_t(ne(lhs.val, rhs.val), lhs, rhs) {}

  virtual const char *get_name() const override;

};  // ne_t

class lt_t final
    : public binary_t {
public:

  template <typename lhs_t, typename rhs_t>
  lt_t(const operand_t<lhs_t> &lhs, const operand_t<rhs_t> &rhs)
      : binary_t(lt(lhs.val, rhs.val), lhs, rhs) {}

  virtual const char *get_name() const override;

};  // lt_t

class le_t final
    : public binary_t {
public:

  template <typename lhs_t, typename rhs_t>
  le_t(const operand_t<lhs_t> &lhs, const operand_t<rhs_t> &rhs)
      : binary_t(le(lhs.val, rhs.val), lhs, rhs) {}

  virtual const char *get_name() const override;

};  // le_t

class gt_t final
    : public binary_t {
public:

  template <typename lhs_t, typename rhs_t>
  gt_t(const operand_t<lhs_t> &lhs, const operand_t<rhs_t> &rhs)
      : binary_t(gt(lhs.val, rhs.val), lhs, rhs) {}

  virtual const char *get_name() const override;

};  // gt_t

class ge_t final
    : public binary_t {
public:

  template <typename lhs_t, typename rhs_t>
  ge_t(const operand_t<lhs_t> &lhs, const operand_t<rhs_t> &rhs)
      : binary_t(ge(lhs.val, rhs.val), lhs, rhs) {}

  virtual const char *get_name() const override;

};  // ge_t

class almost_eq_t final
    : public ternary_t {
public:

  template <typename lhs_t, typename rhs_t, typename coef_t>
  almost_eq_t(
      const operand_t<lhs_t> &lhs, const operand_t<rhs_t> &rhs,
      const operand_t<coef_t> &coef)
      : ternary_t(almost_eq(lhs.val, rhs.val, coef.val), lhs, rhs, coef) {}

  virtual const char *get_name() const override;

};  // almost_eq_t

class not_almost_eq_t final
    : public ternary_t {
public:

  template <typename lhs_t, typename rhs_t, typename coef_t>
  not_almost_eq_t(
      const operand_t<lhs_t> &lhs, const operand_t<rhs_t> &rhs,
      const operand_t<coef_t> &coef)
      : ternary_t(!almost_eq(lhs.val, rhs.val, coef.val), lhs, rhs, coef) {}

  virtual const char *get_name() const override;

};  // not_almost_eq_t

}  // predicate

class expectation_t final {
public:

  expectation_t(const loc_t &loc, const predicate_t &predicate);

  expectation_t(const expectation_t &) = delete;

  ~expectation_t();

  expectation_t &operator=(const expectation_t &) = delete;

  operator bool() const noexcept {
    return ok;
  }

  template <typename val_t>
  expectation_t &operator<<(const val_t &val) {
    write(string_builder, val);
    return *this;
  }

private:

  std::ostringstream string_builder;

  loc_t loc;

  const predicate_t &predicate;

  bool ok;

};  // expectation_t

bool run_fixtures(const cfg_t &cfg);

int main(int argc, char *argv[]);

}  // lick
