
#ifndef SQLITE_DATABASE_H
#define SQLITE_DATABASE_H

#include <sqlite3.h>
#include <string>

using namespace std;

class SQLiteDatabase {
private:
    sqlite3* db;

public:
    explicit SQLiteDatabase(const string& databasePath);

    ~SQLiteDatabase();

    // Prevent accidental copying of the database connection.
    SQLiteDatabase(const SQLiteDatabase&) = delete;
    SQLiteDatabase& operator=(const SQLiteDatabase&) = delete;

    sqlite3* getConnection() const;
    void initializeSchema(const std::string& schemaPath);
};

#endif
