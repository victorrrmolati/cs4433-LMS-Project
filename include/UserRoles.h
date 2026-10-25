
#ifndef USER_ROLES_H
#define USER_ROLES_H

#include "User.h"

using namespace std;

class Student : public User {
public:
    Student(int userId, const string& username,
            const string& passwordHash, const string& name,
            const string& email);

    string getRoleName() const override;
    bool hasPermission(Permission permission) const override;
};

class Lecturer : public User {
public:
    Lecturer(int userId, const string& username,
             const string& passwordHash, const string& name,
             const string& email);

    string getRoleName() const override;
    bool hasPermission(Permission permission) const override;
};

class HeadOfDepartment : public User {
public:
    HeadOfDepartment(int userId, const string& username,
                     const string& passwordHash, const string& name,
                     const string& email);

    string getRoleName() const override;
    bool hasPermission(Permission permission) const override;
};

class SystemAdministrator : public User {
public:
    SystemAdministrator(int userId, const string& username,
                        const string& passwordHash, const string& name,
                        const string& email);

    string getRoleName() const override;
    bool hasPermission(Permission permission) const override;
};

#endif
