#include <quartz/types.hpp>

static_assert(sizeof(qz::s8) == 1, "qz::s8 is expected to be 1-bytes large.");
static_assert(sizeof(qz::s16) == 2, "qz::s16 is expected to be 2-bytes large.");
static_assert(sizeof(qz::s32) == 4, "qz::s32 is expected to be 4-bytes large.");
static_assert(sizeof(qz::s64) == 8, "qz::s64 is expected to be 8-bytes large.");

static_assert(sizeof(qz::u8) == 1, "qz::u8 is expected to be 1-bytes large.");
static_assert(sizeof(qz::u16) == 2, "qz::u16 is expected to be 2-bytes large.");
static_assert(sizeof(qz::u32) == 4, "qz::u32 is expected to be 4-bytes large.");
static_assert(sizeof(qz::u64) == 8, "qz::u64 is expected to be 8-bytes large.");

static_assert(sizeof(qz::f32) == 4, "qz::f32 is expected to be 4-bytes large.");
static_assert(sizeof(qz::f64) == 8, "qz::f64 is expected to be 8-bytes large.");
