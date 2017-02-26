# Lick

Lick is a small, lightweight unit testing framework for modern C++.

## Developing with Lick

Lick is meant to encourage you to write in small, testable modules and to
exercise those modules with unit tests.

Here are the general steps to follow:

1. Create a useful module.  This usually consists of a pair of files like
   `foo.h` and `foo.cc`.
1. Create a a file in which to place your test fixtures.  For example,
   `foo-test.cc`.  Have `foo-test.cc` include `foo.h` and `lick.h`.
1. Compile your test and link it with `lick.o`. This will produce an executable
   file, in this case called `foo-test`.
1. Run the executable. It will execute each of your test fixtures and report
   on anything that didn't go as expected.
1. Iterate on your code until the module is doing everything you want it to do
   and all the tests are passing.

# Writing Fixtures

A test module is made up of fixtures. Each fixture is a function body which
will be executed during test.

Declare a fixture using the `FIXTURE` macro, like this:

```
FIXTURE(typical_use) {
  auto a = f();
  auto b = g();
  EXPECT_EQ(a, b);
}
```

By default, lick executes all the fixtures in the test module, in the order
in which they are defined.

## Expecting Things

The various `EXPECT` macros define the conditions which will be tested for
within each fixture.  See the next section for more information about
expectations.

## Streaming within Fixtures

You can write to the lick console from within a fixture:

```
FIXTURE(say_something) {
  lick::strm() << "starting the thing" << std::endl;
  auto ok = thing.start();
  EXPECT(ok);
  lick::strm() << "thing started" << std::endl;
}
```

Don't write to `cout` or `cerr` directly if you intend your output to be part
of the written record of the test.

# Expectations

An expectation is a testable condition within a fixture.  Each expectations
makes a statement about a condition which is expected to be true in properly
running code. If any expectation within a fixture fails, the fixture as a
whole fails.

There are several kinds of expectations, each with its own macro:


```
EXPECT(operand)
EXPECT_NOT(operand)
EXPECT_EQ(lhs, rhs)
EXPECT_NE(lhs, rhs)
EXPECT_LT(lhs, rhs)
EXPECT_LE(lhs, rhs)
EXPECT_GT(lhs, rhs)
EXPECT_GE(lhs, rhs)
EXPECT_ALMOST_EQ(lhs, rhs, coef)
EXPECT_NOT_ALMOST_EQ(lhs, rhs, coef)
```

You may only use expectations with a fixture.  Don't put them elsewhere in
your code.

## Expectations as Boolean Expressions

The result of an expectation helps to determine the success or failure of its
fixture, but it's also a Boolean value in its own right. You can use an
expectation as a predicate:

```
if (EXPECT_EQ(a, b)) {
  // do something
}
```

## Expectations as Streaming Targets

If an expectation requires some additional documentation, consider adding it
as a stream:

```
EXPECT_EQ(a, b) << "required by algorithm foo";
```

The extra message will be included in the report at the point where the
expectation's result is displayed.

# Running a Lick Test Program

Following this method, each of your code modules will have associated with it
a executable test program. All of these test programs work similarly at the
command line level.

If all the fixtures in a test program pass, the test run as a whole passes.
The program returns the standard status codes to the shell (`EXIT_SUCCESS`
or `EXIT_FAILURE`) to indicate its overall success or failure.

## Command Line Options

All lick test programs take the same command line options.

### Name

> -n _regex_

A fixture only runs if its name matches the regex. Use this flag to run only a
subset of your fixtures, or only a single fixture. Useful during development
when you're focusing on one area of code at a time.

### Strict Mode
> -s

Requires at least one fixture to pass and no fixtures to fail for a run to pass
overall. Without this flag, all that is required for an overall pass is that no
fixtures fail. This means a run that runs no fixtures would pass.

### Verbosity

> -v _0, 1, or 2_

If an expectation fails, lick always shows it. If a fixture contains one more
more failing expectations, lick always shows the fixture. If a test run
contains one or more failing fixtures, lick always shows a summary line
indicating the overall failure.

However, if things pass, lick may or may not show them, depending on the
verbosity level:

| Level | Effect |
| ----- | ------ |
| 0 | Show only failures. Do not show the summary line unless something fails. |
| 1 | Show only failures, but show the summary line even if everything's ok. |
| 2 | Show all work, including passes and fails, and show the summary line. |

The default is level 1.

# Building Lick

I've so far build lick only with clang 3.8. It builds with no warnings with the following options:

```
--std=c++14 -Weverything
-Wno-c++98-compat -Wno-c++98-compat-bind-to-temporary-copy
-Wno-global-constructors -Wno-exit-time-destructors -Wno-padded
```
