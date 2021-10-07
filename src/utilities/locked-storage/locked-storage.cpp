#include "locked-storage.hpp"

LockedStorage* LockedStorage::s_instance { nullptr };
std::mutex LockedStorage::s_mutex;
nlohmann::json LockedStorage::s_storage;

/**
 * @brief Retrieves the instance of the LockedStorage  
 */
auto LockedStorage::GetInstance() -> LockedStorage*
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_instance == nullptr) {
        s_instance = new LockedStorage;
    }
    return s_instance;
}

/**
 * @brief Get the value from the storage
 * @param field name from the field whre the requested value is
 */
auto LockedStorage::get(const std::string field) -> nlohmann::json
{
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_storage[field];
}

/**
 * @brief Write a value to the storage
 * @param field Location to write
 * @param value Value to be writed 
 */
template <typename T>
void LockedStorage::set(const std::string field, T value)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    s_storage[field] = value;
}
