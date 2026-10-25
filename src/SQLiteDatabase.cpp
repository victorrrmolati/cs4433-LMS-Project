
#include "SQLiteDatabase.h"

#include <stdexcept>
#include <fstream>
#include <sstream>

using namespace std;

SQLiteDatabase::SQLiteDatabase(const string& databasePath)
    : db(nullptr) {

    int result = sqlite3_open(databasePath.c_str(), &db);

    if (result != SQLITE_OK) {
        string errorMessage = db
            ? sqlite3_errmsg(db)
            : "Unable to allocate SQLite connection";

        if (db != nullptr) {
            sqlite3_close(db);
            db = nullptr;
        }

        throw runtime_error(
            "Failed to open database: " + errorMessage
        );
    }

    // Enforce foreign-key constraints for this connection.
    char* errorMessage = nullptr;

    result = sqlite3_exec(
        db,
        "PRAGMA foreign_keys = ON;",
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {
        string message = errorMessage
            ? errorMessage
            : sqlite3_errmsg(db);

        sqlite3_free(errorMessage);
        sqlite3_close(db);
        db = nullptr;

        throw runtime_error(
            "Failed to enable foreign keys: " + message
        );
    }
}

SQLiteDatabase::~SQLiteDatabase() {
    if (db != nullptr) {
        sqlite3_close(db);
    }
}

sqlite3* SQLiteDatabase::getConnection() const {
    return db;
}

void SQLiteDatabase::initializeSchema(const std::string& schemaPath) {
    std::ifstream file(schemaPath);
    if (!file) throw std::runtime_error("Cannot open schema file: " + schemaPath);
    std::stringstream contents; contents << file.rdbuf();
    char* error = nullptr;
    int rc = sqlite3_exec(db, contents.str().c_str(), nullptr, nullptr, &error);
    if (rc != SQLITE_OK) {
        std::string message = error ? error : sqlite3_errmsg(db);
        sqlite3_free(error);
        throw std::runtime_error("Schema initialization failed: " + message);
    }
}
