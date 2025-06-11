
#include "../header/Administrator.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Administrator::Administrator() {}

Administrator::Administrator(string username, string password, string name, string id)
    : User(username, password, name, id) {}

bool Administrator::login() {
    cout << "Administrator " << getName() << " logged in successfully." << endl;
    return true;
}

void Administrator::createElection(string type) {
}

void Administrator::addCandidate(string electionId, string candidateName, string partyAffiliation) {
}

void Administrator::viewResults(string electionId) {
}

void Administrator::viewAllElections() {
}

void Administrator::saveToFile() {
    // No need to save individual files - all data is in users.txt
}

void Administrator::loadFromFile(string userId) {
    ifstream file("data/users.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string role, id, username, password, name;
            
            getline(ss, role, ',');
            getline(ss, id, ',');
            getline(ss, username, ',');
            getline(ss, password, ',');
            getline(ss, name, ',');
            
            if (role == "admin" && id == userId) {
                this->username = username;
                this->password = password;
                this->name = name;
                this->id = id;
                break;
            }
        }
        file.close();
    }
}
