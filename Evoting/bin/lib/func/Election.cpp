
#include "../header/Election.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Election::Election() : isActive(false), candidateCount(0) {
    for (int i = 0; i < MAX_CANDIDATES; i++) {
        candidates[i] = nullptr;
    }
}

Election::Election(string id, string name, string start, string end)
    : electionId(id), electionName(name), startDate(start), endDate(end), 
      isActive(false), candidateCount(0) {
    for (int i = 0; i < MAX_CANDIDATES; i++) {
        candidates[i] = nullptr;
    }
}

Election::Election(string id, string name, string start, string end, string startT, string endT)
    : electionId(id), electionName(name), startDate(start), endDate(end), startTime(startT), endTime(endT),
      isActive(false), candidateCount(0) {
    for (int i = 0; i < MAX_CANDIDATES; i++) {
        candidates[i] = nullptr;
    }
}

Election::~Election() {
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i] != nullptr) {
            delete candidates[i];
        }
    }
}

string Election::getElectionId() const { return electionId; }
string Election::getElectionName() const { return electionName; }
bool Election::getIsActive() const { return isActive; }

string Election::getStartTime() const { return startTime; }
string Election::getEndTime() const { return endTime; }

void Election::setStartTime(const string& time) { startTime = time; }
void Election::setEndTime(const string& time) { endTime = time; }

bool Election::startElection() {
    isActive = true;
    return true;
}

bool Election::endElection() {
    isActive = false;
    return true;
}

bool Election::addCandidate(string name, string partyAffiliation) {
    if (candidateCount >= MAX_CANDIDATES) {
        return false;
    }
    
    string candidateId = electionId + "_C" + to_string(candidateCount + 1);
    candidates[candidateCount] = new Candidate(candidateId, name, partyAffiliation);
    candidateCount++;
    return true;
}

void Election::displayCandidates() {
    cout << "Candidates for election " << electionName << ":" << endl;
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i] != nullptr) {
            candidates[i]->displayInfo();
        }
    }
}

Candidate* Election::getCandidate(int index) {
    if (index >= 0 && index < candidateCount) {
        return candidates[index];
    }
    return nullptr;
}

Candidate* Election::getCandidateById(string candidateId) {
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i] != nullptr && candidates[i]->getCandidateId() == candidateId) {
            return candidates[i];
        }
    }
    return nullptr;
}

int Election::getCandidateCount() const { return candidateCount; }

bool Election::candidateExists(string candidateId) {
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i] != nullptr && candidates[i]->getCandidateId() == candidateId) {
            return true;
        }
    }
    return false;
}

void Election::saveToFile() {
    ofstream file("data/elections/" + electionId + ".txt");
    if (file.is_open()) {
        file << electionId << endl;
        file << electionName << endl;
        file << startDate << endl;
        file << endDate << endl;
        file << startTime << endl;
        file << endTime << endl;
        file << isActive << endl;
        file << candidateCount << endl;
        for (int i = 0; i < candidateCount; i++) {
            if (candidates[i] != nullptr) {
                file << candidates[i]->getCandidateId() << endl;
                file << candidates[i]->getName() << endl;
                file << candidates[i]->getPartyAffiliation() << endl;
                file << candidates[i]->getVoteCount() << endl;
            }
        }
        file.close();
    }
}

void Election::loadFromFile(string electionId) {
    ifstream file("data/elections/" + electionId + ".txt");
    if (file.is_open()) {
        getline(file, this->electionId);
        getline(file, electionName);
        getline(file, startDate);
        getline(file, endDate);
        getline(file, startTime);
        getline(file, endTime);
        file >> isActive;
        file >> candidateCount;
        file.ignore();
        for (int i = 0; i < candidateCount; i++) {
            string candidateId, name, party;
            int votes;
            getline(file, candidateId);
            getline(file, name);
            getline(file, party);
            file >> votes;
            file.ignore();
            candidates[i] = new Candidate(candidateId, name, party);
            candidates[i]->incrementVotes();
        }
        file.close();
    }
}
