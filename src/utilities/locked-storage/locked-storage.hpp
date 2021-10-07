#pragma once

#include <fstream>
#include <mutex>
#include <nlohmann/json.hpp>

class LockedStorage {
private:
    static LockedStorage* s_instance;
    static std::mutex s_mutex;

protected:
    LockedStorage() = default;

    ~LockedStorage() = default;

    static nlohmann::json s_storage;

public:
    LockedStorage(LockedStorage& other) = delete;

    void operator=(const LockedStorage&) = delete;

    static auto GetInstance() -> LockedStorage*;

    auto operator[](std::string) -> nlohmann::json;

    template <typename T>
    void set(std::string, T);

    auto get(std::string) -> nlohmann::json;
};
