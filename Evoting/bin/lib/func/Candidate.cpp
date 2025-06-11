
#include "../header/Candidate.h"
#include <iostream>
#include <fstream>

using namespace std;

Candidate::Candidate() : voteCount(0) {}

Candidate::Candidate(string id, string n, string party)
    : candidateId(id), name(n), partyAffiliation(party), voteCount(0) {}

string Candidate::getCandidateId() const { return candidateId; }
string Candidate::getName() const { return name; }
string Candidate::getPartyAffiliation() const { return partyAffiliation; }
int Candidate::getVoteCount() const { return voteCount; }

void Candidate::incrementVotes() { voteCount++; }

void Candidate::displayInfo() const {
    cout << "ID: " << candidateId 
         << " | Name: " << name 
         << " | Party: " << partyAffiliation 
         << " | Votes: " << voteCount << endl;
}

void Candidate::saveToFile(string electionId) {
    // Data is now stored in elections.txt
}

void Candidate::loadFromFile(string electionId, string candId) {
    // Data is now loaded from elections.txt
}
