#pragma once
#ifndef IL2CPP_LOG
#define IL2CPP_LOG(x, ...) std::printf(x "\n", __VA_ARGS__)
#endif // !IL2CPP_LOG

#ifndef IL2CPP_ASSERT
#include <cassert>
#define IL2CPP_ASSERT(x) assert(x)
#endif // !IL2CPP_LOG

#define IL2CPP_NAMEOF(x) #x