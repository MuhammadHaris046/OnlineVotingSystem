
#ifndef ELECTION_H
#define ELECTION_H

#include "Candidate.h"
#include <string>

class Election {
protected:
    std::string electionId;
    std::string electionName;
    std::string startDate;
    std::string endDate;
    bool isActive;
    int candidateCount;
    static const int MAX_CANDIDATES = 20;
    Candidate* candidates[MAX_CANDIDATES];
    std::string startTime;
    std::string endTime;

public:
    Election();
    Election(std::string id, std::string name, std::string start, std::string end);
    Election(std::string id, std::string name, std::string start, std::string end, std::string startT, std::string endT);
    virtual ~Election();

    std::string getElectionId() const;
    std::string getElectionName() const;
    bool getIsActive() const;
    
    virtual bool startElection();
    virtual bool endElection();
    
    bool addCandidate(std::string name, std::string partyAffiliation);
    void displayCandidates();
    Candidate* getCandidate(int index);
    Candidate* getCandidateById(std::string candidateId);
    int getCandidateCount() const;
    
    virtual void calculateResults() = 0;
    virtual void displayResults() = 0;
    
    virtual void saveToFile() = 0;
    virtual void loadFromFile(std::string electionId) = 0;
    
    bool candidateExists(std::string candidateId);

    std::string getStartTime() const;
    std::string getEndTime() const;
    void setStartTime(const std::string& time);
    void setEndTime(const std::string& time);
};

#endif // ELECTION_H
