#pragma once

#include <string>
#include <cassert>


namespace vox {

using i32 = int32_t;

std::string formatBytes( size_t );
i32	positiveModulo(i32 value, i32 modulus);

}	// namespace vox