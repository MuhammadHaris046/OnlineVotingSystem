
#include "../header/LocalElection.h"
#include <iostream>
#include <fstream>

using namespace std;

LocalElection::LocalElection() {}

LocalElection::LocalElection(string id, string name, string start, string end, string loc)
    : Election(id, name, start, end), locality(loc) {}

void LocalElection::calculateResults() {
    for (int i = 0; i < getCandidateCount(); i++) {
        if (getCandidate(i) != nullptr) {
        }
    }
}

void LocalElection::displayResults() {
    cout << "\nLocal Election Results for " << getElectionName() << " (" << locality << ")" << endl;
    cout << "----------------------------------------" << endl;
    
    int maxVotes = 0;
    Candidate* winner = nullptr;
    
    for (int i = 0; i < getCandidateCount(); i++) {
        Candidate* candidate = getCandidate(i);
        if (candidate != nullptr) {
            candidate->displayInfo();
            if (candidate->getVoteCount() > maxVotes) {
                maxVotes = candidate->getVoteCount();
                winner = candidate;
            }
        }
    }
    
    if (winner != nullptr) {
        cout << "\nWinner: " << winner->getName() << " (" << winner->getPartyAffiliation() 
             << ") with " << winner->getVoteCount() << " votes" << endl;
    }
}

void LocalElection::saveToFile() {
    // Data is now stored in elections.txt
}

void LocalElection::loadFromFile(string eId) {
    // Data is now loaded from elections.txt
}
