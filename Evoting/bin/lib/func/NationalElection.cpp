
#include "../header/NationalElection.h"
#include <iostream>
#include <fstream>

using namespace std;

NationalElection::NationalElection() : regions(0) {}

NationalElection::NationalElection(string id, string name, string start, string end, 
                                 string country, int reg)
    : Election(id, name, start, end), country(country), regions(reg) {}

void NationalElection::calculateResults() {
    for (int i = 0; i < getCandidateCount(); i++) {
        if (getCandidate(i) != nullptr) {
        }
    }
}

void NationalElection::displayResults() {
    cout << "\nNational Election Results for " << getElectionName() << " (" << country << ")" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Number of Regions: " << regions << endl;
    
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

void NationalElection::saveToFile() {
    // Data is now stored in elections.txt
}

void NationalElection::loadFromFile(string eId) {
    // Data is now loaded from elections.txt
}
