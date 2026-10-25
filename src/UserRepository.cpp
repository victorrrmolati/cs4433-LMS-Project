
#include "UserRepository.h"

#include <stdexcept>
#include <sqlite3.h>

using namespace std;

UserRepository::UserRepository(SQLiteDatabase& database)
    : database(database) {
}

int UserRepository::createUser(
    const string& username,
    const string& passwordHash,
    const string& name,
    const string& email,
    const string& role,
    int departmentId
) {
    // Validate basic required values.
    if (username.empty() || passwordHash.empty() ||
        name.empty() || email.empty()) {
        throw invalid_argument(
            "Username, password hash, name and email are required."
        );
    }

    // Validate the role against the four supported user types.
    if (role != "Student" &&
        role != "Lecturer" &&
        role != "HeadOfDepartment" &&
        role != "SystemAdministrator") {
        throw invalid_argument("Invalid user role.");
    }

    const char* sql =
        "INSERT INTO Users "
        "(username, password_hash, name, email, role, department_id) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* statement = nullptr;
    sqlite3* connection = database.getConnection();

    int result = sqlite3_prepare_v2(
        connection, sql, -1, &statement, nullptr
    );

    if (result != SQLITE_OK) {
        throw runtime_error(sqlite3_errmsg(connection));
    }

    // Bind values instead of inserting user input into the SQL string.
    sqlite3_bind_text(
        statement, 1, username.c_str(), -1, SQLITE_TRANSIENT
    );
    sqlite3_bind_text(
        statement, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT
    );
    sqlite3_bind_text(
        statement, 3, name.c_str(), -1, SQLITE_TRANSIENT
    );
    sqlite3_bind_text(
        statement, 4, email.c_str(), -1, SQLITE_TRANSIENT
    );
    sqlite3_bind_text(
        statement, 5, role.c_str(), -1, SQLITE_TRANSIENT
    );

    if (departmentId > 0) {
        sqlite3_bind_int(statement, 6, departmentId);
    } else {
        sqlite3_bind_null(statement, 6);
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE) {
        string error = sqlite3_errmsg(connection);
        sqlite3_finalize(statement);
        throw runtime_error("Could not create user: " + error);
    }

    int newUserId = static_cast<int>(
        sqlite3_last_insert_rowid(connection)
    );

    sqlite3_finalize(statement);

    return newUserId;
}

bool UserRepository::usernameExists(const string& username) {
    const char* sql =
        "SELECT 1 FROM Users WHERE username = ? LIMIT 1;";

    sqlite3_stmt* statement = nullptr;
    sqlite3* connection = database.getConnection();

    int result = sqlite3_prepare_v2(
        connection, sql, -1, &statement, nullptr
    );

    if (result != SQLITE_OK) {
        throw runtime_error(sqlite3_errmsg(connection));
    }

    sqlite3_bind_text(
        statement, 1, username.c_str(), -1, SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);

    bool exists = (result == SQLITE_ROW);

    if (result != SQLITE_ROW && result != SQLITE_DONE) {
        string error = sqlite3_errmsg(connection);
        sqlite3_finalize(statement);
        throw runtime_error("Could not check username: " + error);
    }

    sqlite3_finalize(statement);

    return exists;
}


bool UserRepository::getUserByUsername(
    const string& username,
    int& userId,
    string& name,
    string& email,
    string& role
) {
    const char* sql =
        "SELECT user_id, name, email, role "
        "FROM Users WHERE username = ? LIMIT 1;";

    sqlite3* connection = database.getConnection();
    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        connection, sql, -1, &statement, nullptr
    );

    if (result != SQLITE_OK) {
        throw runtime_error(
            "Could not prepare user lookup: " +
            string(sqlite3_errmsg(connection))
        );
    }

    result = sqlite3_bind_text(
        statement, 1, username.c_str(), -1, SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK) {
        string error = sqlite3_errmsg(connection);
        sqlite3_finalize(statement);
        throw runtime_error("Could not bind username: " + error);
    }

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW) {
        userId = sqlite3_column_int(statement, 0);

        const unsigned char* nameValue =
            sqlite3_column_text(statement, 1);
        const unsigned char* emailValue =
            sqlite3_column_text(statement, 2);
        const unsigned char* roleValue =
            sqlite3_column_text(statement, 3);

        name = nameValue
            ? reinterpret_cast<const char*>(nameValue) : "";
        email = emailValue
            ? reinterpret_cast<const char*>(emailValue) : "";
        role = roleValue
            ? reinterpret_cast<const char*>(roleValue) : "";

        sqlite3_finalize(statement);
        return true;
    }

    if (result == SQLITE_DONE) {
        sqlite3_finalize(statement);
        return false;
    }

    string error = sqlite3_errmsg(connection);
    sqlite3_finalize(statement);

    throw runtime_error("Could not retrieve user: " + error);
}


#include "PasswordHasher.h"
#include <iostream>

bool UserRepository::authenticate(const string& username, const string& password,
                                  int& userId, string& name, string& email, string& role) {
    const char* sql = "SELECT user_id, password_hash, name, email, role FROM Users WHERE username = ? LIMIT 1;";
    sqlite3_stmt* st = nullptr; sqlite3* db = database.getConnection();
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) throw runtime_error(sqlite3_errmsg(db));
    sqlite3_bind_text(st, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(st);
    if (rc != SQLITE_ROW) { sqlite3_finalize(st); if (rc != SQLITE_DONE) throw runtime_error(sqlite3_errmsg(db)); return false; }
    string hash = reinterpret_cast<const char*>(sqlite3_column_text(st, 1));
    bool ok = false;
    try { ok = PasswordHasher::verifyPassword(password, hash); } catch (...) { ok = false; }
    if (ok) {
        userId = sqlite3_column_int(st, 0);
        name = reinterpret_cast<const char*>(sqlite3_column_text(st, 2));
        email = reinterpret_cast<const char*>(sqlite3_column_text(st, 3));
        role = reinterpret_cast<const char*>(sqlite3_column_text(st, 4));
    }
    sqlite3_finalize(st); return ok;
}

void UserRepository::listUsers() {
    const char* sql = "SELECT user_id, username, name, email, role FROM Users ORDER BY user_id;";
    sqlite3_stmt* st=nullptr; sqlite3* db=database.getConnection();
    if(sqlite3_prepare_v2(db,sql,-1,&st,nullptr)!=SQLITE_OK) throw runtime_error(sqlite3_errmsg(db));
    cout << "\nID | Username | Name | Email | Role\n";
    while(sqlite3_step(st)==SQLITE_ROW) {
        cout << sqlite3_column_int(st,0) << " | "
        << reinterpret_cast<const char*>(sqlite3_column_text(st,1)) << " | "
        << reinterpret_cast<const char*>(sqlite3_column_text(st,2)) << " | "
        << reinterpret_cast<const char*>(sqlite3_column_text(st,3)) << " | "
        << reinterpret_cast<const char*>(sqlite3_column_text(st,4)) << "\n";
    }
    sqlite3_finalize(st);
}
void UserRepository::updateUserEmail(int userId, const string& email) {
    if(email.find('@')==string::npos) throw invalid_argument("Enter a valid email address.");
    const char* sql="UPDATE Users SET email=? WHERE user_id=?;"; sqlite3_stmt* st=nullptr; sqlite3* db=database.getConnection();
    if(sqlite3_prepare_v2(db,sql,-1,&st,nullptr)!=SQLITE_OK) throw runtime_error(sqlite3_errmsg(db));
    sqlite3_bind_text(st,1,email.c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_int(st,2,userId);
    int rc=sqlite3_step(st); sqlite3_finalize(st); if(rc!=SQLITE_DONE) throw runtime_error(sqlite3_errmsg(db));
    if(sqlite3_changes(db)==0) throw runtime_error("No user updated; check the ID.");
}
void UserRepository::deleteUser(int userId) {
    const char* sql="DELETE FROM Users WHERE user_id=?;"; sqlite3_stmt* st=nullptr; sqlite3* db=database.getConnection();
    if(sqlite3_prepare_v2(db,sql,-1,&st,nullptr)!=SQLITE_OK) throw runtime_error(sqlite3_errmsg(db));
    sqlite3_bind_int(st,1,userId); int rc=sqlite3_step(st); sqlite3_finalize(st);
    if(rc!=SQLITE_DONE) throw runtime_error(sqlite3_errmsg(db));
    if(sqlite3_changes(db)==0) throw runtime_error("No user deleted; check the ID.");
}

int UserRepository::countUsers() {
    sqlite3_stmt* st=nullptr; sqlite3* db=database.getConnection();
    if(sqlite3_prepare_v2(db,"SELECT COUNT(*) FROM Users;",-1,&st,nullptr)!=SQLITE_OK) throw runtime_error(sqlite3_errmsg(db));
    int rc=sqlite3_step(st); if(rc!=SQLITE_ROW){string e=sqlite3_errmsg(db);sqlite3_finalize(st);throw runtime_error(e);} int count=sqlite3_column_int(st,0);sqlite3_finalize(st);return count;
}
void UserRepository::assignDemoUsersToDepartment(int departmentId) {
    sqlite3_stmt* st=nullptr; sqlite3* db=database.getConnection();
    const char* sql="UPDATE Users SET department_id=? WHERE username IN ('admin01','lecturer01','hod01','student01') AND department_id IS NULL;";
    if(sqlite3_prepare_v2(db,sql,-1,&st,nullptr)!=SQLITE_OK) throw runtime_error(sqlite3_errmsg(db));
    sqlite3_bind_int(st,1,departmentId);int rc=sqlite3_step(st);if(rc!=SQLITE_DONE){string e=sqlite3_errmsg(db);sqlite3_finalize(st);throw runtime_error(e);}sqlite3_finalize(st);
}
void UserRepository::migrateLegacyDemoPasswords() {
    sqlite3* db=database.getConnection();
    // Demo-only migration for the supplied coursework database; production systems need a user-driven reset flow.
    const char* roleSql="UPDATE Users SET role='SystemAdministrator' WHERE role='System Administrator'; UPDATE Users SET role='HeadOfDepartment' WHERE role='Head of Department';";
    sqlite3_exec(db,roleSql,nullptr,nullptr,nullptr);
    struct Pair { const char* username; const char* password; } pairs[]={{"admin01","Admin@12345"},{"lecturer01","Teach@12345"},{"hod01","Moderate@12345"},{"student01","Student@12345"}};
    for(const auto& x:pairs){sqlite3_stmt* q=nullptr;if(sqlite3_prepare_v2(db,"SELECT password_hash FROM Users WHERE username=?;",-1,&q,nullptr)!=SQLITE_OK)throw runtime_error(sqlite3_errmsg(db));sqlite3_bind_text(q,1,x.username,-1,SQLITE_TRANSIENT);int rc=sqlite3_step(q);string hash;if(rc==SQLITE_ROW&&sqlite3_column_text(q,0))hash=(const char*)sqlite3_column_text(q,0);sqlite3_finalize(q);if(rc==SQLITE_ROW&&hash.rfind("$argon2id$",0)!=0){string fresh=PasswordHasher::hashPassword(x.password);sqlite3_stmt* u=nullptr;if(sqlite3_prepare_v2(db,"UPDATE Users SET password_hash=? WHERE username=?;",-1,&u,nullptr)!=SQLITE_OK)throw runtime_error(sqlite3_errmsg(db));sqlite3_bind_text(u,1,fresh.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(u,2,x.username,-1,SQLITE_TRANSIENT);int urc=sqlite3_step(u);sqlite3_finalize(u);if(urc!=SQLITE_DONE)throw runtime_error(sqlite3_errmsg(db));}}
}
