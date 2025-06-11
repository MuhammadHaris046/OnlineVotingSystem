
#include "../header/ElectionManager.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>

using namespace std;

ElectionManager::ElectionManager() : electionCount(0) {
    for (int i = 0; i < MAX_ELECTIONS; i++) {
        elections[i] = nullptr;
    }
    loadAllElections();
}

ElectionManager::~ElectionManager() {
    for (int i = 0; i < electionCount; i++) {
        if (elections[i] != nullptr) {
            delete elections[i];
        }
    }
}

bool ElectionManager::createElection(string type, string name, string start, string end, 
                                  string region, int regions, string startTime, string endTime) {
    if (electionCount >= MAX_ELECTIONS) {
        return false;
    }

    string electionId = "E" + to_string(time(nullptr));
    
    if (type == "local") {
        elections[electionCount] = new LocalElection(electionId, name, start, end, region);
    }
    else if (type == "national") {
        elections[electionCount] = new NationalElection(electionId, name, start, end, region, regions);
    }
    else if (type == "regional") {
        elections[electionCount] = new RegionalElection(electionId, name, start, end, region);
    }
    else {
        return false;
    }
    
    elections[electionCount]->setStartTime(startTime);
    elections[electionCount]->setEndTime(endTime);
    
    // Append to elections.txt
    ofstream file("data/elections.txt", ios::app);
    if (file.is_open()) {
        file << electionId << endl;
        file << type << endl;
        file << name << endl;
        file << start << endl;
        file << end << endl;
        file << startTime << endl;
        file << endTime << endl;
        file << region << endl;
        file << regions << endl;
        file << "Inactive" << endl;  // Status
        file << "0" << endl;         // Number of candidates
        file << "----------------------------------------" << endl;
        file.close();
        
        electionCount++;
        return true;
    }
    
    delete elections[electionCount];
    elections[electionCount] = nullptr;
    return false;
}

void ElectionManager::displayActiveElections() {
    cout << "\nActive Elections:" << endl;
    cout << "----------------" << endl;
    
    for (int i = 0; i < electionCount; i++) {
        if (elections[i] != nullptr && elections[i]->getIsActive()) {
            cout << "ID: " << elections[i]->getElectionId() 
                     << " | Name: " << elections[i]->getElectionName() << endl;
        }
    }
}

void ElectionManager::displayAllElections() {
    cout << "\nAll Elections:" << endl;
    cout << "-------------" << endl;
    
    for (int i = 0; i < electionCount; i++) {
        if (elections[i] != nullptr) {
            cout << "ID: " << elections[i]->getElectionId() 
                     << " | Name: " << elections[i]->getElectionName()
                     << " | Status: " << (elections[i]->getIsActive() ? "Active" : "Inactive") 
                     << endl;
        }
    }
}

Election* ElectionManager::getElection(string electionId) {
    for (int i = 0; i < electionCount; i++) {
        if (elections[i] != nullptr && elections[i]->getElectionId() == electionId) {
            return elections[i];
        }
    }
    return nullptr;
}

bool ElectionManager::addCandidateToElection(string electionId, string name, string party) {
    Election* election = getElection(electionId);
    if (election != nullptr) {
        if (election->getIsActive()) {
            cout << "Cannot add candidates to an active election." << endl;
            return false;
        }
        return election->addCandidate(name, party);
    }
    return false;
}

void ElectionManager::viewElectionResults(string electionId) {
    Election* election = getElection(electionId);
    if (election != nullptr) {
        election->calculateResults();
        election->displayResults();
    }
}

void ElectionManager::loadAllElections() {
    ifstream file("data/elections.txt");
    string line;
    
    if (file.is_open()) {
        while (getline(file, line) && electionCount < MAX_ELECTIONS) {
            string electionId = line;
            string type, name, start, end, startTime, endTime, region, regionsStr, status;
            
            getline(file, type);
            getline(file, name);
            getline(file, start);
            getline(file, end);
            getline(file, startTime);
            getline(file, endTime);
            getline(file, region);
            getline(file, regionsStr);
            getline(file, status);
            
            int regions = 0;
            try {
                if (!regionsStr.empty()) {
                    // Remove any whitespace
                    regionsStr.erase(remove_if(regionsStr.begin(), regionsStr.end(), ::isspace), regionsStr.end());
                    if (!regionsStr.empty()) {
                        regions = stoi(regionsStr);
                    }
                }
            } catch (const std::exception& e) {
                // If conversion fails, keep regions as 0
                regions = 0;
            }
            
            if (type == "local") {
                elections[electionCount] = new LocalElection(electionId, name, start, end, region);
            }
            else if (type == "national") {
                elections[electionCount] = new NationalElection(electionId, name, start, end, region, regions);
            }
            else if (type == "regional") {
                elections[electionCount] = new RegionalElection(electionId, name, start, end, region);
            }
            
            if (elections[electionCount] != nullptr) {
                elections[electionCount]->setStartTime(startTime);
                elections[electionCount]->setEndTime(endTime);
                if (status == "Active") {
                    elections[electionCount]->startElection();
                }
                
                // Load candidates
                string candidateCount;
                getline(file, candidateCount);
                
                try {
                    candidateCount.erase(remove_if(candidateCount.begin(), candidateCount.end(), ::isspace), candidateCount.end());
                    int numCandidates = candidateCount.empty() ? 0 : stoi(candidateCount);
                    
                    for (int i = 0; i < numCandidates; i++) {
                        string candidateName, partyAffiliation, votes;
                        getline(file, candidateName);
                        getline(file, partyAffiliation);
                        getline(file, votes);
                        string candidateId = electionId + "_C" + to_string(i + 1);
                        
                        Candidate* candidate = new Candidate(candidateId, candidateName, partyAffiliation);
                        try {
                            votes.erase(remove_if(votes.begin(), votes.end(), ::isspace), votes.end());
                            int voteCount = votes.empty() ? 0 : stoi(votes);
                            for (int v = 0; v < voteCount; v++) {
                                candidate->incrementVotes();
                            }
                        } catch (const std::exception& e) {
                            // If vote count conversion fails, assume 0 votes
                        }
                        elections[electionCount]->addCandidate(candidateName, partyAffiliation);
                    }
                } catch (const std::exception& e) {
                    // If candidate count conversion fails, skip candidates
                }
                electionCount++;
            }
            
            // Skip to next election record
            while (getline(file, line) && line != "----------------------------------------") {}
        }
        file.close();
    }
}

bool ElectionManager::electionExists(string electionId) {
    for (int i = 0; i < electionCount; i++) {
        if (elections[i] != nullptr && elections[i]->getElectionId() == electionId) {
            return true;
        }
    }
    return false;
}
