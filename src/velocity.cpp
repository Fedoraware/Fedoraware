/*
 * velocity.cpp
 *
 *  Created on: May 27, 2017
 *      Author: nullifiedcat
 */

#include "velocity.hpp"
#include "common.hpp"
#include "copypasted/CSignature.h"

namespace velocity
{

EstimateAbsVelocity_t EstimateAbsVelocity{};

void Init()
{
    EstimateAbsVelocity = (void (*)(IClientEntity *, Vector &)) gSignatures.GetClientSignature("55 89 E5 56 53 83 EC 20 8B 5D ? 8B 75 ? E8 ? ? ? ? 39 D8");
}
} // namespace velocity
