
#include "../header/Voter.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Voter::Voter() : hasVoted(false), votedElectionsCount(0) {}

Voter::Voter(string username, string password, string name, string id)
    : User(username, password, name, id), hasVoted(false), votedElectionsCount(0) {}

bool Voter::login() {
    cout << "Voter " << getName() << " logged in successfully." << endl;
    return true;
}

bool Voter::hasVotedInElection(string electionId) {
    for (int i = 0; i < votedElectionsCount; i++) {
        if (votedElections[i] == electionId) {
            return true;
        }
    }
    return false;
}

void Voter::addVotedElection(string electionId) {
    if (votedElectionsCount < 10) {
        votedElections[votedElectionsCount++] = electionId;
    }
}

bool Voter::checkVoteStatus(string electionId) {
    return hasVotedInElection(electionId);
}

bool Voter::castVote(string electionId, string candidateId) {
    if (!hasVotedInElection(electionId)) {
        addVotedElection(electionId);
        return true;
    }
    return false;
}

void Voter::saveToFile() {
    // Voting history is now stored in the users.txt file
    ifstream inFile("data/users.txt");
    ofstream outFile("data/users_temp.txt");
    
    if (inFile.is_open() && outFile.is_open()) {
        string line;
        bool found = false;
        
        while (getline(inFile, line)) {
            stringstream ss(line);
            string role, fileId;
            getline(ss, role, ',');
            getline(ss, fileId, ',');
            
            if (role == "voter" && fileId == id) {
                // Update the voter's record with voting history
                outFile << "voter," << id << "," << username << "," << password << "," 
                       << name << "," << votedElectionsCount;
                for (int i = 0; i < votedElectionsCount; i++) {
                    outFile << "," << votedElections[i];
                }
                outFile << endl;
                found = true;
            } else {
                outFile << line << endl;
            }
        }
        
        inFile.close();
        outFile.close();
        
        if (found) {
            remove("data/users.txt");
            rename("data/users_temp.txt", "data/users.txt");
        } else {
            remove("data/users_temp.txt");
        }
    }
}

void Voter::loadFromFile(string userId) {
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
            
            if (role == "voter" && id == userId) {
                this->username = username;
                this->password = password;
                this->name = name;
                this->id = id;
                
                // Load voting history
                string votedCount;
                getline(ss, votedCount, ',');
                votedElectionsCount = stoi(votedCount);
                
                for (int i = 0; i < votedElectionsCount; i++) {
                    getline(ss, votedElections[i], ',');
                }
                break;
            }
        }
        file.close();
    }
}
