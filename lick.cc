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

#include "lick.h"

#include <iostream>

#include <unistd.h>

namespace lick {

const char
    *pass = "pass", *fail = "fail",
    *separator = "; ",
    *red = "\033[1;31m", *green = "\033[1;32m",
    *bold = "\033[1m", *plain = "\033[0m";

static void write_ex(
    std::ostream &strm, const std::exception &ex, bool is_nested = false) {
  if (is_nested) {
    strm << separator;
  }
  strm << ex.what();
  try {
    std::rethrow_if_nested(ex);
  } catch(const std::exception &nested_ex) {
    write_ex(strm, nested_ex, true);
  } catch (...) {
    strm << separator << get_ex_msg();
  }
}

std::string get_ex_msg(const std::exception &ex) {
  std::ostringstream strm;
  write_ex(strm, ex);
  return strm.str();
}

const std::string &get_ex_msg() {
  static const std::string ex_msg = "non-standard exception";
  return ex_msg;
}

cfg_t::cfg_t()
    : strm(&std::cout), regex(".*"), verbosity(1), strict(false) {}

bool cfg_t::parse(cfg_t &cfg, int argc, char *argv[]) {
  bool ok = true;
  do {
    int opt = getopt(argc, argv, "n:sv:");
    if (opt < 0) {
      break;
    }
    switch (opt) {
      case 'n': {
        cfg.regex = std::regex { optarg };
        break;
      }
      case 's': {
        cfg.strict = true;
        break;
      }
      case 'v': {
        cfg.set_verbosity(atoi(optarg));
        break;
      }
      default: {
        ok = false;
      }
    }
  } while (ok);
  return ok;
}

ctxt_t::ctxt_t(const fixture_t *fixture_, const cfg_t &cfg_)
    : fixture(fixture_), cfg(cfg_), showing(false), ok(true) {
  singleton = this;
  if (cfg.get_verbosity() >= 2) {
    on_begin_show();
  }
}

ctxt_t::~ctxt_t() {
  on_end_show();
  singleton = nullptr;
}

void ctxt_t::on_begin_show() const {
  if (showing) {
    return;
  }
  showing = true;
  cfg.get_strm()
      << fixture->get_loc() << separator
      << "begin " << bold << fixture->get_name() << plain
      << std::endl;
}

void ctxt_t::on_end_show() const {
  if (!showing) {
    return;
  }
  cfg.get_strm()
      << "end " << bold << fixture->get_name() << plain << separator
      << pf_t { ok } << std::endl;
}

thread_local ctxt_t *ctxt_t::singleton = nullptr;

fixture_t::fixture_t(const loc_t &loc_, const char *name_, fn_t fn_)
    : loc(loc_), name(name_), fn(fn_), next(nullptr) {
  (last ? last->next : first) = this;
  last = this;
}

bool fixture_t::operator()(const cfg_t &cfg) const {
  ctxt_t ctxt { this, cfg };
  auto stalled = stall(fn);
  if (!stalled) {
    cfg.get_strm()
        << indent_t { 1 }
        << red << "exception" << plain << separator
        << stalled.msg << std::endl;
    return false;
  }
  return ctxt;
}

bool fixture_t::for_each(const cb_t &cb) {
  for (auto *fixture = first; fixture; fixture = fixture->next) {
    if (!cb(*fixture)) {
      return false;
    }
  }  // for
  return true;
}

fixture_t
    *fixture_t::first = nullptr,
    *fixture_t::last = nullptr;

any_operand_t::~any_operand_t() = default;

predicate_t::~predicate_t() = default;

void predicate_t::write_src(std::ostream &strm) const {
  const char *name = get_name();
  strm << "EXPECT";
  if (*name) {
    strm << '_' << name;
  }
  strm << '(';
  bool needs_comma = false;
  for_each_operand(
    [&](const any_operand_t &operand) {
      if (needs_comma) {
        strm << ", ";
      } else {
        needs_comma = true;
      }
      strm << src_of(operand);
      return true;
    }
  );
  strm << ')';
}

bool unary_t::for_each_operand(const cb_t &cb) const {
  return cb(operand);
}

bool binary_t::for_each_operand(const cb_t &cb) const {
  return cb(lhs) && cb(rhs);
}

bool ternary_t::for_each_operand(const cb_t &cb) const {
  return cb(lhs) && cb(rhs) && cb(coef);
}

namespace predicate {

const char *true_t::get_name() const {
  return "";
}

const char *false_t::get_name() const {
  return "NOT";
}

const char *eq_t::get_name() const {
  return "EQ";
}

const char *ne_t::get_name() const {
  return "NE";
}

const char *lt_t::get_name() const {
  return "LT";
}

const char *le_t::get_name() const {
  return "LE";
}

const char *gt_t::get_name() const {
  return "GT";
}

const char *ge_t::get_name() const {
  return "GE";
}

const char *almost_eq_t::get_name() const {
  return "ALMOST_EQ";
}

const char *not_almost_eq_t::get_name() const {
  return "NOT_ALMOST_EQ";
}

}  // predicate

expectation_t::expectation_t(const loc_t &loc_, const predicate_t &predicate_)
    : loc(loc_), predicate(predicate_), ok(predicate) {}

expectation_t::~expectation_t() {
  auto *ctxt = ctxt_t::get_singleton();
  if (!ok) {
    ctxt->fail();
  }
  auto &cfg = ctxt->get_cfg();
  if (!ok || cfg.get_verbosity() >= 2) {
    auto &strm = ctxt->get_strm();
    strm
        << indent_t { 1 }
        << loc << separator
        << pf_t { ok } << separator
        << src_of(predicate);
    predicate.for_each_operand(
      [&](const any_operand_t &operand) {
        const char *src = operand.get_src();
        if (!isdigit(*src) && *src != '\'' && *src != '"') {
          strm << separator << src << '=' << val_of(operand);
        }
        return true;
      }
    );
    auto extra = string_builder.str();
    if (!extra.empty()) {
      strm << separator << extra;
    }
    strm << std::endl;
  }
}

bool run_fixtures(const cfg_t &cfg) {
  auto &strm = cfg.get_strm();
  int pass_cnt = 0, fail_cnt = 0, skip_cnt = 0;
  fixture_t::for_each(
    [&](const fixture_t &fixture) {
      if (std::regex_match(fixture.get_name(), cfg.get_regex())) {
        ++(fixture(cfg) ? pass_cnt : fail_cnt);
      } else {
        ++skip_cnt;
      }
      return true;
    }
  );
  bool ok = cfg.is_strict()
      ? (pass_cnt != 0 && fail_cnt == 0)
      : (fail_cnt == 0);
  if (!ok || cfg.get_verbosity() >= 1) {
    strm
        << "passed " << pass_cnt << separator
        << "failed " << fail_cnt << separator
        << "skipped " << skip_cnt << separator
        << pf_t { ok } << std::endl;
  }
  return ok;
}

int main(int argc, char *argv[]) {
  auto stalled = stall(
    [&] {
      cfg_t cfg;
      return cfg_t::parse(cfg, argc, argv) ? run_fixtures(cfg) : false;
    }
  );
  if (!stalled) {
    std::cerr << stalled.msg << std::endl;
    return EXIT_FAILURE;
  }
  return stalled.ret ? EXIT_SUCCESS : EXIT_FAILURE;
}

}  // lick

int main(int argc, char *argv[]) {
  return lick::main(argc, argv);
}
