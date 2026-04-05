#pragma once

#include <memory>

static constexpr size_t SIZE_MAX_BUFFER  = 64;

struct Message{
   char data[SIZE_MAX_BUFFER];
};
