#pragma once

#include "SQLiteCpp/Database.h"
#include "Result.hpp"

#include <stdexcept>

namespace db {

class Repository final {
 public:
    explicit Repository(SQLite::Database* db) : db_(db) {
        if (db_ == nullptr) {
            throw std::invalid_argument("Database cannot be null pointer");
        }
    }

 private:
    tools::Result<void> CreateDatabase() noexcept;

    SQLite::Database* db_;
};

}  // namespace db
