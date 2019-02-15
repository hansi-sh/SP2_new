#ifndef UTILITY_H
#define UTILITY_H

#include "Light.h"
#include "MatrixStack.h"

Position operator*(const Mtx44& lhs, const Position& rhs);

#endif // !UTILITY_H