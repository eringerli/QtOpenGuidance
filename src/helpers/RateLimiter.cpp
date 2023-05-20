// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RateLimiter.h"

double RateLimiter::intervals[( int )RateLimiter::Type::End] = { 0 };
