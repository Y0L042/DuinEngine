#pragma once

#include <doctest.h>

// Remove extra parentheses around macro parameters
#define MSG_CHECK(var, expr) CHECK_MESSAGE((expr), (#var " := ", var))

#define MSG_CHECK_FALSE(var, expr) CHECK_FALSE_MESSAGE((expr), (#var " := ", var))
