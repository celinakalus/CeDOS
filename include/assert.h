#ifndef ASSERT_H
#define ASSERT_H

#include "cedos/core.h"

#include <stdint.h>

#define assert(cond) if (!(cond)) { kpanic("Assertion failed: " #cond); }

#endif