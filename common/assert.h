#ifndef ASSERT_H
#define ASSERT_H

#include <stdint.h>

extern void assert_failed(const char * message);

#define assert(cond) if (!(cond)) { assert_failed("Assertion failed: " #cond); }

#endif