#pragma once

#define AERO_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

// For the ref and scope for shared_ptr and unique_ptr
namespace aero
{
}
