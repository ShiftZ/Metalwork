#pragma once

#include "B2Rig/B2Vec.h"

inline Float Unwind(Float Angle) { return remainder(Angle, 2*pi); }