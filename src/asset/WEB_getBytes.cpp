#include "WEB_getBytes.hpp"

#include <emscripten/fetch.h>

#include <cstring>
#include <future>
#include <stdexcept>
#include <string>
#include <vector>

struct FetchContext
{
    std::promise<std::vector<std::byte>> promise;
};

void onFetchSuccess(emscripten_fetch_t *fetch)
{
    auto *ctx = static_cast<FetchContext *>(fetch->userData);

    try
    {
        std::vector<std::byte> bytes(static_cast<size_t>(fetch->numBytes));
        if (fetch->numBytes > 0 && fetch->data != nullptr)
        {
            std::memcpy(bytes.data(), fetch->data, static_cast<size_t>(fetch->numBytes));
        }
        ctx->promise.set_value(std::move(bytes));
    }
    catch (...)
    {
        ctx->promise.set_exception(std::current_exception());
    }

    delete ctx;
    emscripten_fetch_close(fetch);
}

void onFetchError(emscripten_fetch_t *fetch)
{
    auto *ctx = static_cast<FetchContext *>(fetch->userData);

    ctx->promise.set_exception(std::make_exception_ptr(
        std::runtime_error(std::string("Failed to fetch URL: ") + (fetch->url ? fetch->url : ""))));

    delete ctx;
    emscripten_fetch_close(fetch);
}

std::future<std::vector<std::byte>> getBytes(const std::string &url)
{
    auto *ctx = new FetchContext();
    std::future<std::vector<std::byte>> future = ctx->promise.get_future();

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    std::strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = onFetchSuccess;
    attr.onerror = onFetchError;
    attr.userData = ctx;

    emscripten_fetch(&attr, url.c_str());
    return future;
}