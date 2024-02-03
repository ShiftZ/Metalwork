#pragma once

#include "B2Rig/B2Vec.h"

inline Float Unwind(Float angle) { return remainder(angle, 2*pi); }
