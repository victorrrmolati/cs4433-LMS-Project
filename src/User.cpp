
#include "User.h"

#include <sstream>

using namespace std;

User::User(int userId,
           const string& username,
           const string& passwordHash,
           const string& name,
           const string& email)
    : userId(userId),
      username(username),
      passwordHash(passwordHash),
      name(name),
      email(email) {
}

void User::logout() {
    
}

string User::getDetails() const {
    stringstream details;

    details << "User ID: " << userId << '\n';
    details << "Username: " << username << '\n';
    details << "Name: " << name << '\n';
    details << "Email: " << email << '\n';
    details << "Role: " << getRoleName() << '\n';

    return details.str();
}
