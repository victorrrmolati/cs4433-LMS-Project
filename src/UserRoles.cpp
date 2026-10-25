
#include "UserRoles.h"

using namespace std;

// Student
Student::Student(int userId, const string& username,
                 const string& passwordHash, const string& name,
                 const string& email)
    : User(userId, username, passwordHash, name, email) {
}

string Student::getRoleName() const {
    return "Student";
}

// Lecturer
Lecturer::Lecturer(int userId, const string& username,
                   const string& passwordHash, const string& name,
                   const string& email)
    : User(userId, username, passwordHash, name, email) {
}

string Lecturer::getRoleName() const {
    return "Lecturer";
}

// Head of Department
HeadOfDepartment::HeadOfDepartment(
    int userId, const string& username,
    const string& passwordHash, const string& name,
    const string& email)
    : User(userId, username, passwordHash, name, email) {
}

string HeadOfDepartment::getRoleName() const {
    return "Head of Department";
}

// System Administrator
SystemAdministrator::SystemAdministrator(
    int userId, const string& username,
    const string& passwordHash, const string& name,
    const string& email)
    : User(userId, username, passwordHash, name, email) {
}

string SystemAdministrator::getRoleName() const {
    return "System Administrator";
}

bool Student::hasPermission(Permission permission) const {
    switch (permission) {
        case Permission::ViewOwnProfile:
        case Permission::ViewOwnMarks:
        case Permission::SubmitAssignment:
            return true;

        default:
            return false;
    }
}

bool Lecturer::hasPermission(Permission permission) const {
    switch (permission) {
        case Permission::ViewOwnProfile:
        case Permission::GradeSubmissions:
        case Permission::ManageCourseAssignments:
            return true;

        default:
            return false;
    }
}

bool HeadOfDepartment::hasPermission(Permission permission) const {
    switch (permission) {
        case Permission::ViewOwnProfile:
        case Permission::ModerateDepartmentMarks:
        case Permission::ViewDepartmentReports:
            return true;

        default:
            return false;
    }
}

bool SystemAdministrator::hasPermission(Permission permission) const {
    switch (permission) {
        case Permission::ViewOwnProfile:
        case Permission::ManageUsers:
        case Permission::ManageSystemAccounts:
            return true;

        default:
            return false;
    }
}

