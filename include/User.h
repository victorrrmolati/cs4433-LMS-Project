
#ifndef USER_H
#define USER_H
using namespace std;    

#include <string>




enum class Permission {
    ViewOwnProfile,
    ViewOwnMarks,
    SubmitAssignment,
    GradeSubmissions,
    ManageCourseAssignments,
    ModerateDepartmentMarks,
    ViewDepartmentReports,
    ManageUsers,
    ManageSystemAccounts
};

class User {
protected:
    int userId;
    string username;
    string passwordHash;
    string name;
    string email;

public:
    User(int userId,
         const string& username,
         const string& passwordHash,
         const string& name,
         const string& email);

    virtual ~User() = default;

    virtual string getRoleName() const = 0;
    virtual bool hasPermission(Permission permission) const = 0;

    void logout();
    string getDetails() const;
};

#endif
