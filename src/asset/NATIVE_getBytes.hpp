#pragma once
#include <vector>
#include <cstddef>
#include <string>
#include <future>

std::future<std::vector<std::byte>> getBytes(const std::string &path);