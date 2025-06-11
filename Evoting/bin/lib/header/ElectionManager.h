
#ifndef ELECTION_MANAGER_H
#define ELECTION_MANAGER_H

#include "Election.h"
#include "LocalElection.h"
#include "NationalElection.h"
#include "RegionalElection.h"
#include <string>

class ElectionManager {
private:
    static const int MAX_ELECTIONS = 20;
    Election* elections[MAX_ELECTIONS];
    int electionCount;

public:
    ElectionManager();
    ~ElectionManager();

    bool createElection(std::string type, std::string name, std::string start, std::string end, std::string region = "", int regions = 0, std::string startTime = "", std::string endTime = "");
    void displayActiveElections();
    void displayAllElections();
    Election* getElection(std::string electionId);
    bool addCandidateToElection(std::string electionId, std::string name, std::string party);
    void viewElectionResults(std::string electionId);
    void loadAllElections();
    bool electionExists(std::string electionId);
};

#endif // ELECTION_MANAGER_H
