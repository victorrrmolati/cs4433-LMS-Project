
#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "SQLiteDatabase.h"

#include <string>

using namespace std;

class UserRepository {
private:
    SQLiteDatabase& database;

public:
    explicit UserRepository(SQLiteDatabase& database);

    int createUser(
        const string& username,
        const string& passwordHash,
        const string& name,
        const string& email,
        const string& role,
        int departmentId = 0
    );

    bool usernameExists(const string& username);
    bool authenticate(const string& username, const string& password,
                      int& userId, string& name, string& email, string& role);
    void listUsers();
    void updateUserEmail(int userId, const string& email);
    void deleteUser(int userId);
    int countUsers();
    void assignDemoUsersToDepartment(int departmentId);
    void migrateLegacyDemoPasswords();

    bool getUserByUsername(
    const string& username,
    int& userId,
    string& name,
    string& email,
    string& role
);
};

#endif
