
#include "../header/RegionalElection.h"
#include <iostream>
#include <fstream>

using namespace std;

RegionalElection::RegionalElection() {}

RegionalElection::RegionalElection(string id, string name, string start, string end, string reg)
    : Election(id, name, start, end), region(reg) {}

void RegionalElection::calculateResults() {
    for (int i = 0; i < getCandidateCount(); i++) {
        if (getCandidate(i) != nullptr) {
        }
    }
}

void RegionalElection::displayResults() {
    cout << "\nRegional Election Results for " << getElectionName() << " (" << region << ")" << endl;
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

void RegionalElection::saveToFile() {
    // Data is now stored in elections.txt
}

void RegionalElection::loadFromFile(string eId) {
    // Data is now loaded from elections.txt
}
