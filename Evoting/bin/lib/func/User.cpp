
#include "../header/User.h"

using namespace std;

User::User() {}

User::User(string uname, string pwd, string n, string uid)
    : username(uname), password(pwd), name(n), id(uid) {}

User::~User() {}

string User::getUsername() const { return username; }
string User::getPassword() const { return password; }
string User::getName() const { return name; }
string User::getID() const { return id; }

bool User::authenticate(string inputUsername, string inputPassword) const {
    return (username == inputUsername && password == inputPassword);
}
