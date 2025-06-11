
#include "../header/VotingSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <ctime>
#include <algorithm>
#include <vector>
#include <tuple>
#include <iomanip>

using namespace std;

VotingSystem::VotingSystem() : currentUser(nullptr) {}

VotingSystem::~VotingSystem() {
    if (currentUser != nullptr) {
        delete currentUser;
    }
}

bool VotingSystem::registerVoter(string username, string password, string name, string dob) {
    // Calculate age based on DOB
    int birthYear = stoi(dob.substr(0, 4));
    int birthMonth = stoi(dob.substr(5, 2));
    int birthDay = stoi(dob.substr(8, 2));

    int currentYear = 2025;
    int currentMonth = 5;
    int currentDay = 5;

    int age = currentYear - birthYear;
    if (currentMonth < birthMonth || (currentMonth == birthMonth && currentDay < birthDay)) {
        age--;
    }

    if (age < 18) {
        cout << "You must be at least 18 years old to register as a voter." << endl;
        return false;
    }

    stringstream ss;
    ss << "V" << time(nullptr);
    string id = ss.str();

    ofstream file("data/users.txt", ios::app);
    if (file.is_open()) {
        file << "voter," << id << "," << username << "," << password << "," << name << "," << dob << endl;
        file.close();
        return true;
    }
    return false;
}

bool VotingSystem::registerAdmin(string username, string password, string name) {
    stringstream ss;
    ss << "A" << time(nullptr);
    string id = ss.str();

    ofstream file("data/users.txt", ios::app);
    if (file.is_open()) {
        file << "admin," << id << "," << username << "," << password << "," << name << endl;
        file.close();
        return true;
    }
    return false;
}

bool VotingSystem::loginUser(string username, string password, string role) {
    if (currentUser != nullptr) {
        delete currentUser;
        currentUser = nullptr;
    }

    ifstream file("data/users.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string fileRole, id, fileUsername, filePassword, name;

            getline(ss, fileRole, ',');
            getline(ss, id, ',');
            getline(ss, fileUsername, ',');
            getline(ss, filePassword, ',');
            getline(ss, name, ',');

            if (fileRole == role && fileUsername == username && filePassword == password) {
                if (role == "voter") {
                    currentUser = new Voter(username, password, name, id);
                } else if (role == "admin") {
                    currentUser = new Administrator(username, password, name, id);
                }
                file.close();
                return true;
            }
        }
        file.close();
    }
    return false;
}

void VotingSystem::logout() {
    if (currentUser != nullptr) {
        delete currentUser;
        currentUser = nullptr;
        cout << "Logged out successfully." << endl;
    }
}

bool VotingSystem::createElection(string type, string name, string start, string end, string region, int regions, string startTime, string endTime) {
    if (!currentUser || dynamic_cast<Administrator*>(currentUser) == nullptr) {
        cout << "Only administrators can create elections." << endl;
        return false;
    }

    // Convert type to lowercase for case-insensitive comparison
    string lowerType = type;
    transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    
    // Validate election type
    if (lowerType != "local" && lowerType != "national" && lowerType != "regional") {
        cout << "Invalid election type. Must be local, national, or regional." << endl;
        return false;
    }

    // Validate dates and times
    try {
        // Parse start date
        int startYear, startMonth, startDay;
        sscanf(start.c_str(), "%d-%d-%d", &startYear, &startMonth, &startDay);
        
        // Parse end date
        int endYear, endMonth, endDay;
        sscanf(end.c_str(), "%d-%d-%d", &endYear, &endMonth, &endDay);

        // Basic date validation
        if (startYear < 2025 || endYear < 2025 || 
            startMonth < 1 || startMonth > 12 || endMonth < 1 || endMonth > 12 ||
            startDay < 1 || startDay > 31 || endDay < 1 || endDay > 31) {
            cout << "Invalid date format. Use YYYY-MM-DD with valid dates." << endl;
            return false;
        }

        // Validate start date is not after end date
        if (startYear > endYear || 
            (startYear == endYear && startMonth > endMonth) ||
            (startYear == endYear && startMonth == endMonth && startDay > endDay)) {
            cout << "Start date cannot be after end date." << endl;
            return false;
        }

        // Validate time format if provided
        if (!startTime.empty() && !endTime.empty()) {
            int startHour, startMin, endHour, endMin;
            if (sscanf(startTime.c_str(), "%d:%d", &startHour, &startMin) != 2 ||
                sscanf(endTime.c_str(), "%d:%d", &endHour, &endMin) != 2 ||
                startHour < 0 || startHour > 23 || endHour < 0 || endHour > 23 ||
                startMin < 0 || startMin > 59 || endMin < 0 || endMin > 59) {
                cout << "Invalid time format. Use HH:MM (24-hour format)" << endl;
                return false;
            }

            // If same day, check times
            if (startYear == endYear && startMonth == endMonth && startDay == endDay) {
                if (startHour > endHour || (startHour == endHour && startMin >= endMin)) {
                    cout << "Start time must be before end time on the same day." << endl;
                    return false;
                }
            }
        }
    } catch (const exception& e) {
        cout << "Invalid date/time format." << endl;
        return false;
    }

    // Validate national election requirements
    if (lowerType == "national" && (region.empty() || regions <= 0)) {
        cout << "National elections require a country name and at least one region." << endl;
        return false;
    }

    // Validate regional/local election requirements
    if ((lowerType == "regional" || lowerType == "local") && region.empty()) {
        cout << (lowerType == "regional" ? "Regional" : "Local") << " elections require a region name." << endl;
        return false;
    }

    // Use ElectionManager to create the election
    return electionManager.createElection(type, name, start, end, region, regions, startTime, endTime);
}

void VotingSystem::displayElectionInfo(const string& targetId) {
    ifstream file("data/elections.txt");
    string line;
    bool found = false;
    
    if (file.is_open()) {
        while (getline(file, line)) {
            if (line == targetId) {
                found = true;
                string type, name, start, end, startTime, endTime, region, regions, status;
                
                getline(file, type);
                getline(file, name);
                getline(file, start);
                getline(file, end);
                getline(file, startTime);
                getline(file, endTime);
                getline(file, region);
                getline(file, regions);
                getline(file, status);
                
                cout << "\nElection Details:" << endl;
                cout << "---------------" << endl;
                cout << "ID: " << targetId << endl;
                cout << "Name: " << name << endl;
                cout << "Type: " << type << endl;
                if (!region.empty()) {
                    cout << "Region: " << region << endl;
                }
                cout << "Start Date: " << start << endl;
                cout << "Start Time: " << startTime << endl;
                cout << "End Date: " << end << endl;
                cout << "End Time: " << endTime << endl;
                cout << "Status: " << status << endl;
                
                string candidateCount;
                getline(file, candidateCount);
                int numCandidates = stoi(candidateCount);
                
                if (numCandidates > 0) {
                    cout << "\nCandidates:" << endl;
                    cout << "----------" << endl;
                    
                    for (int i = 0; i < numCandidates; i++) {
                        string candidateName, partyAffiliation, votes;
                        getline(file, candidateName);
                        getline(file, partyAffiliation);
                        getline(file, votes);
                        
                        cout << "ID: " << (i + 1) << endl;
                        cout << "Name: " << candidateName << endl;
                        cout << "Party: " << partyAffiliation << endl;
                        cout << string(20, '-') << endl;
                    }
                }
                break;
            }
            // Skip to next election record
            while (getline(file, line) && line != "----------------------------------------") {}
        }
        file.close();
        
        if (!found) {
            cout << "Election not found!" << endl;
        }
    }
}

bool VotingSystem::addCandidate(string targetId, string name, string partyAffiliation) {
    ifstream inFile("data/elections.txt");
    ofstream outFile("data/elections_temp.txt");
    bool found = false;
    bool success = false;
    string line;
    
    if (inFile.is_open() && outFile.is_open()) {
        while (getline(inFile, line)) {
            outFile << line << endl;  // Write election ID
            
            if (line == targetId) {
                found = true;
                string type, elecName, start, end, startTime, endTime, region, regions, status;
                
                // Copy and write the next 8 lines unchanged
                for (int i = 0; i < 8; i++) {
                    getline(inFile, line);
                    outFile << line << endl;
                }
                
                // Get and check status
                getline(inFile, status);
                if (status == "Active" || status == "Ended") {
                    outFile << status << endl;
                    cout << "Cannot add candidates after the election has started or ended." << endl;
                    success = false;
                } else {
                    outFile << status << endl;
                    // Get current candidate count
                    string candidateCountStr;
                    getline(inFile, candidateCountStr);
                    int candidateCount = stoi(candidateCountStr);
                    
                    if (candidateCount >= 20) {
                        cout << "Maximum number of candidates reached." << endl;
                        success = false;
                    } else {
                        // Write updated candidate count
                        outFile << (candidateCount + 1) << endl;
                        
                        // Copy existing candidates
                        for (int i = 0; i < candidateCount; i++) {
                            getline(inFile, line); outFile << line << endl;  // name
                            getline(inFile, line); outFile << line << endl;  // party
                            getline(inFile, line); outFile << line << endl;  // votes
                        }
                        
                        // Add new candidate
                        outFile << name << endl;
                        outFile << partyAffiliation << endl;
                        outFile << "0" << endl;  // Initial vote count
                        success = true;
                    }
                }
                
                // Write separator
                outFile << "----------------------------------------" << endl;
                
                // Skip rest of current election in input file
                while (getline(inFile, line) && line != "----------------------------------------") {}
                
            } else {
                // Copy entire election record unchanged
                while (getline(inFile, line) && line != "----------------------------------------") {
                    outFile << line << endl;
                }
                outFile << "----------------------------------------" << endl;
            }
        }
        
        inFile.close();
        outFile.close();
        
        if (found) {
            if (success) {
                remove("data/elections.txt");
                rename("data/elections_temp.txt", "data/elections.txt");
                cout << "Candidate added successfully!" << endl;
                return true;
            } else {
                remove("data/elections_temp.txt");
                return false;
            }
        } else {
            remove("data/elections_temp.txt");
            cout << "Election not found." << endl;
            return false;
        }
    }
    return false;
}

bool VotingSystem::startElection(string targetId) {
    cout << "Starting election:" << endl;
    displayElectionInfo(targetId);
    
    ifstream inFile("data/elections.txt");
    ofstream outFile("data/elections_temp.txt");
    bool found = false;
    bool success = false;
    string line;
    
    if (inFile.is_open() && outFile.is_open()) {
        while (getline(inFile, line)) {
            outFile << line << endl;  // Write election ID
            
            if (line == targetId) {
                found = true;
                // Copy next 8 lines unchanged (type through regions)
                for (int i = 0; i < 8; i++) {
                    getline(inFile, line);
                    outFile << line << endl;
                }
                
                // Handle status line
                getline(inFile, line);
                if (line == "Inactive") {
                    outFile << "Active" << endl;
                    success = true;
                } else {
                    outFile << line << endl;
                }
                
                // Copy rest of this election record
                while (getline(inFile, line) && line != "----------------------------------------") {
                    outFile << line << endl;
                }
                outFile << "----------------------------------------" << endl;
            } else {
                // Copy the rest of the current election record
                while (getline(inFile, line) && line != "----------------------------------------") {
                    outFile << line << endl;
                }
                outFile << "----------------------------------------" << endl;
            }
        }
        
        inFile.close();
        outFile.close();
        
        if (found && success) {
            remove("data/elections.txt");
            rename("data/elections_temp.txt", "data/elections.txt");
            cout << "\nElection started successfully!" << endl;
            return true;
        } else {
            remove("data/elections_temp.txt");
            if (!found) {
                cout << "\nError: Election not found!" << endl;
            } else {
                cout << "\nError: Election is not in Inactive state!" << endl;
            }
            return false;
        }
    }
    return false;
}

bool VotingSystem::endElection(string targetId) {
    cout << "Ending election:" << endl;
    displayElectionInfo(targetId);
    
    ifstream inFile("data/elections.txt");
    ofstream outFile("data/elections_temp.txt");
    bool found = false;
    bool success = false;
    string line;
    
    if (inFile.is_open() && outFile.is_open()) {
        while (getline(inFile, line)) {
            outFile << line << endl;  // Write election ID
            
            if (line == targetId) {
                found = true;
                // Copy next 8 lines unchanged (type through regions)
                for (int i = 0; i < 8; i++) {
                    getline(inFile, line);
                    outFile << line << endl;
                }
                
                // Handle status line
                getline(inFile, line);
                if (line == "Active") {
                    outFile << "Ended" << endl;
                    success = true;
                } else {
                    outFile << line << endl;
                }
                
                // Copy rest of this election record
                while (getline(inFile, line) && line != "----------------------------------------") {
                    outFile << line << endl;
                }
                outFile << "----------------------------------------" << endl;
            } else {
                // Copy the rest of the current election record
                while (getline(inFile, line) && line != "----------------------------------------") {
                    outFile << line << endl;
                }
                outFile << "----------------------------------------" << endl;
            }
        }
        
        inFile.close();
        outFile.close();
        
        if (found && success) {
            remove("data/elections.txt");
            rename("data/elections_temp.txt", "data/elections.txt");
            cout << "\nElection ended successfully!" << endl;
            return true;
        } else {
            remove("data/elections_temp.txt");
            if (!found) {
                cout << "\nError: Election not found!" << endl;
            } else {
                cout << "\nError: Election is not in Active state!" << endl;
            }
            return false;
        }
    }
    return false;
}

void VotingSystem::viewResults(string targetId) {
    ifstream file("data/elections.txt");
    string line;
    bool found = false;
    
    if (file.is_open()) {
        while (getline(file, line)) {
            if (line == targetId) {
                string type, name, start, end, startTime, endTime, region, regions, status;
                
                getline(file, type);
                getline(file, name);
                getline(file, start);
                getline(file, end);
                getline(file, startTime);
                getline(file, endTime);
                getline(file, region);
                getline(file, regions);
                getline(file, status);
                
                cout << "\n=========================================" << endl;
                cout << "           Election Results              " << endl;
                cout << "=========================================" << endl;
                cout << "Election Name: " << name << endl;
                cout << "Type: " << type << endl;
                if (!region.empty()) {
                    cout << "Region: " << region << endl;
                }
                cout << "Status: " << status << endl;
                cout << "Duration: " << start << " " << startTime << " to " << end << " " << endTime << endl;
                cout << "-----------------------------------------" << endl;
                
                if (status != "Ended") {
                    cout << "\nResults are only available after the election has ended." << endl;
                    file.close();
                    return;
                }
                
                string candidateCount;
                getline(file, candidateCount);
                
                // Safely convert candidate count
                int numCandidates;
                try {
                    candidateCount.erase(remove_if(candidateCount.begin(), candidateCount.end(), ::isspace), candidateCount.end());
                    numCandidates = candidateCount.empty() ? 0 : stoi(candidateCount);
                } catch (const std::exception& e) {
                    numCandidates = 0;
                }
                
                if (numCandidates == 0) {
                    cout << "\nNo candidates were registered for this election." << endl;
                    found = true;
                    break;
                }
                
                vector<tuple<string, string, int>> candidates; // name, party, votes
                int totalVotes = 0;
                int maxVotes = 0;
                vector<int> winnerIndices;
                
                // First pass: collect data and calculate totals
                for (int i = 0; i < numCandidates; i++) {
                    string candidateName, partyAffiliation, votes;
                    getline(file, candidateName);
                    getline(file, partyAffiliation);
                    getline(file, votes);
                    
                    // Safely convert vote count
                    int voteCount = 0;
                    try {
                        votes.erase(remove_if(votes.begin(), votes.end(), ::isspace), votes.end());
                        voteCount = votes.empty() ? 0 : stoi(votes);
                    } catch (const std::exception& e) {
                        voteCount = 0;
                    }
                    
                    totalVotes += voteCount;
                    
                    if (voteCount > maxVotes) {
                        maxVotes = voteCount;
                        winnerIndices.clear();
                        winnerIndices.push_back(i);
                    } else if (voteCount == maxVotes) {
                        winnerIndices.push_back(i);
                    }
                    
                    candidates.push_back(make_tuple(candidateName, partyAffiliation, voteCount));
                }
                
                // Display results
                cout << "\nVoting Summary:" << endl;
                cout << "Total votes cast: " << totalVotes << endl;
                cout << "-----------------------------------------" << endl;
                cout << "\nDetailed Results:" << endl;
                
                // Display each candidate's results
                for (int i = 0; i < candidates.size(); i++) {
                    string name = get<0>(candidates[i]);
                    string party = get<1>(candidates[i]);
                    int votes = get<2>(candidates[i]);
                    double percentage = totalVotes > 0 ? (votes * 100.0) / totalVotes : 0;
                    
                    cout << "\nCandidate " << (i + 1) << ":" << endl;
                    cout << "Name: " << name << endl;
                    cout << "Party: " << party << endl;
                    cout << "Votes: " << votes << " (" << fixed << setprecision(2) << percentage << "%)" << endl;
                    if (find(winnerIndices.begin(), winnerIndices.end(), i) != winnerIndices.end()) {
                        cout << "Status: WINNER" << endl;
                    }
                    cout << string(20, '-') << endl;
                }
                
                // Display winner(s)
                cout << "\nElection Outcome:" << endl;
                if (winnerIndices.size() == 1) {
                    string name = get<0>(candidates[winnerIndices[0]]);
                    string party = get<1>(candidates[winnerIndices[0]]);
                    int votes = get<2>(candidates[winnerIndices[0]]);
                    cout << "Winner: " << name << " (" << party << ")" << endl;
                    cout << "Winning votes: " << votes << " (" << fixed << setprecision(2) 
                         << (votes * 100.0) / totalVotes << "%)" << endl;
                } else if (winnerIndices.size() > 1) {
                    cout << "Election resulted in a tie between:" << endl;
                    for (int idx : winnerIndices) {
                        string name = get<0>(candidates[idx]);
                        string party = get<1>(candidates[idx]);
                        cout << "- " << name << " (" << party << ")" << endl;
                    }
                    cout << "Tied votes: " << maxVotes << " (" << fixed << setprecision(2) 
                         << (maxVotes * 100.0) / totalVotes << "%)" << endl;
                }
                
                found = true;
                break;
            }
            // Skip to next election record
            while (getline(file, line) && line != "----------------------------------------") {}
        }
        file.close();
        
        if (!found) {
            cout << "Election not found!" << endl;
        }
    }
}

void VotingSystem::viewAllElections() {
    cout << "\nAll Elections:" << endl;
    cout << "-------------" << endl;
    
    ifstream file("data/elections.txt");
    string line;
    
    if (file.is_open()) {
        bool hasElections = false;
        while (getline(file, line)) {
            // Skip empty lines or separator lines
            if (line.empty() || line == "----------------------------------------") {
                continue;
            }
            
            string electionId = line;
            string type, name, start, end, startTime, endTime, region, regions, status;
            
            // Read the election details
            if (!(getline(file, type) && 
                getline(file, name) && 
                getline(file, start) && 
                getline(file, end) && 
                getline(file, startTime) && 
                getline(file, endTime) && 
                getline(file, region) && 
                getline(file, regions) && 
                getline(file, status))) {
                break; // Break if we can't read all fields
            }
            
            hasElections = true;
            cout << "ID: " << electionId << " | Name: " << name << " | Type: " << type << " | Status: " << status << endl;
            
            // Skip any remaining data until separator
            while (getline(file, line) && line != "----------------------------------------") {}
        }
        file.close();
        
        if (!hasElections) {
            cout << "No elections found." << endl;
        }
    } else {
        cout << "No elections found." << endl;
    }
}

void VotingSystem::viewElections() {
    cout << "\nActive Elections:" << endl;
    cout << "----------------" << endl;
    
    ifstream file("data/elections.txt");
    string line;
    bool found = false;
    
    if (file.is_open()) {
        while (getline(file, line)) {
            string electionId = line;
            string type, name, start, end, startTime, endTime, region, regions, status;
            
            getline(file, type);
            getline(file, name);
            getline(file, start);
            getline(file, end);
            getline(file, startTime);
            getline(file, endTime);
            getline(file, region);
            getline(file, regions);
            getline(file, status);
            
            if (status == "Active") {
                found = true;
                cout << "ID: " << electionId << " | Name: " << name << " | Type: " << type << endl;
            }
            
            // Skip remaining election data until separator
            while (getline(file, line) && line != "----------------------------------------") {}
        }
        file.close();
        
        if (!found) {
            cout << "No active elections found." << endl;
        }
    }
}

void VotingSystem::displayRunningElections() {
    cout << "\nRunning Elections:" << endl;
    cout << "-----------------" << endl;
    
    ifstream file("data/elections.txt");
    string line;
    bool found = false;
    
    if (file.is_open()) {
        while (getline(file, line)) {
            string electionId = line;
            string type, name, start, end, startTime, endTime, region, regions, status;
            
            getline(file, type);
            getline(file, name);
            getline(file, start);
            getline(file, end);
            getline(file, startTime);
            getline(file, endTime);
            getline(file, region);
            getline(file, regions);
            getline(file, status);
            
            if (status == "Active") {
                found = true;
                cout << "ID: " << electionId << " | Name: " << name << " | Type: " << type << endl;
                cout << "Start: " << start << " " << startTime << " | End: " << end << " " << endTime << endl;
                if (!region.empty()) {
                    cout << "Region: " << region << endl;
                }
                cout << string(50, '-') << endl;
            }
            
            // Skip remaining election data until separator
            while (getline(file, line) && line != "----------------------------------------") {}
        }
        file.close();
        
        if (!found) {
            cout << "No elections are currently running." << endl;
        }
    }
}

bool VotingSystem::castVote(string electionId, string candidateId) {
    ifstream inFile("data/elections.txt");
    ofstream outFile("data/elections_temp.txt");
    bool found = false;
    bool success = false;
    string line;
    
    if (inFile.is_open() && outFile.is_open()) {
        while (getline(inFile, line)) {
            outFile << line << endl;  // Write election ID
            
            if (line == electionId) {
                found = true;
                string type, name, start, end, startTime, endTime, region, regions, status;
                
                // Copy next 8 lines unchanged (type through regions)
                for (int i = 0; i < 8; i++) {
                    getline(inFile, line);
                    outFile << line << endl;
                }
                
                // Get and check status
                getline(inFile, status);
                if (status != "Active") {
                    cout << "This election is not currently active." << endl;
                    success = false;
                    outFile << status << endl;
                } else {
                    outFile << status << endl;
                    
                    // Get current date and time
                    time_t now = time(0);
                    tm localTime = *localtime(&now);
                    
                    // Parse start date and time
                    tm startDateTime = {0};
                    int startYear, startMonth, startDay, startHour = 0, startMin = 0;
                    start.erase(remove_if(start.begin(), start.end(), ::isspace), start.end());
                    sscanf(start.c_str(), "%d-%d-%d", &startYear, &startMonth, &startDay);
                    sscanf(startTime.c_str(), "%d:%d", &startHour, &startMin);
                    startDateTime.tm_year = startYear - 1900;
                    startDateTime.tm_mon = startMonth - 1;
                    startDateTime.tm_mday = startDay;
                    startDateTime.tm_hour = startHour;
                    startDateTime.tm_min = startMin;
                    startDateTime.tm_sec = 0;
                    
                    // Parse end date and time
                    tm endDateTime = {0};
                    int endYear, endMonth, endDay, endHour = 0, endMin = 0;
                    end.erase(remove_if(end.begin(), end.end(), ::isspace), end.end());
                    sscanf(end.c_str(), "%d-%d-%d", &endYear, &endMonth, &endDay);
                    sscanf(endTime.c_str(), "%d:%d", &endHour, &endMin);
                    endDateTime.tm_year = endYear - 1900;
                    endDateTime.tm_mon = endMonth - 1;
                    endDateTime.tm_mday = endDay;
                    endDateTime.tm_hour = endHour;
                    endDateTime.tm_min = endMin;
                    endDateTime.tm_sec = 0;
                    
                    time_t startTimeT = mktime(&startDateTime);
                    time_t endTimeT = mktime(&endDateTime);
                    
                    if (difftime(now, startTimeT) < 0) {
                        cout << "Voting has not started yet for this election." << endl;
                        success = false;
                    } else if (difftime(now, endTimeT) > 0) {
                        cout << "Voting has ended for this election." << endl;
                        success = false;
                    } else if (hasVoted(electionId)) {
                        cout << "You have already voted in this election." << endl;
                        success = false;
                    } else {
                        // Get candidate count and check if voter is a candidate
                        string candidateCountStr;
                        getline(inFile, candidateCountStr);
                        int candidateCount = stoi(candidateCountStr);
                        
                        // Store current file position to return after checking candidates
                        streampos filePos = inFile.tellg();
                        bool isCandidate = false;
                        
                        // Check if the current voter is a candidate
                        for (int i = 0; i < candidateCount; i++) {
                            string candidateName, partyAffiliation, votes;
                            getline(inFile, candidateName);
                            getline(inFile, partyAffiliation);
                            getline(inFile, votes);
                            
                            if (candidateName == currentUser->getName()) {
                                isCandidate = true;
                                break;
                            }
                        }
                        
                        // Return to stored position
                        inFile.seekg(filePos);
                        
                        if (isCandidate) {
                            cout << "As a candidate in this election, you are not allowed to vote." << endl;
                            success = false;
                            outFile << candidateCountStr << endl;
                            // Copy remaining candidates unchanged
                            for (int i = 0; i < candidateCount; i++) {
                                string line;
                                for (int j = 0; j < 3; j++) {  // 3 lines per candidate
                                    getline(inFile, line);
                                    outFile << line << endl;
                                }
                            }
                        } else {
                            outFile << candidateCountStr << endl;
                            bool validNumber = true;
                            int candidateIndex = -1;
                            
                            try {
                                // Validate that candidateId is a number
                                for(char c : candidateId) {
                                    if(!isdigit(c)) {
                                        validNumber = false;
                                        break;
                                    }
                                }
                                
                                if(validNumber) {
                                    candidateIndex = stoi(candidateId) - 1;
                                }
                            } catch(const std::invalid_argument& e) {
                                validNumber = false;
                            } catch(const std::out_of_range& e) {
                                validNumber = false;
                            }

                            if (!validNumber) {
                                cout << "Invalid candidate ID. Please enter a number." << endl;
                                success = false;
                                // Copy remaining candidates unchanged
                                for (int i = 0; i < candidateCount; i++) {
                                    string line;
                                    for (int j = 0; j < 3; j++) {
                                        getline(inFile, line);
                                        outFile << line << endl;
                                    }
                                }
                            } else if (candidateIndex < 0 || candidateIndex >= candidateCount) {
                                cout << "Invalid candidate ID." << endl;
                                success = false;
                                // Copy remaining candidates unchanged
                                for (int i = 0; i < candidateCount; i++) {
                                    string line;
                                    for (int j = 0; j < 3; j++) {  // 3 lines per candidate
                                        getline(inFile, line);
                                        outFile << line << endl;
                                    }
                                }
                            } else {
                                // Copy candidates before target, incrementing vote for target
                                for (int i = 0; i < candidateCount; i++) {
                                    string candidateName, partyAffiliation, votes;
                                    getline(inFile, candidateName);
                                    getline(inFile, partyAffiliation);
                                    getline(inFile, votes);
                                    
                                    outFile << candidateName << endl;
                                    outFile << partyAffiliation << endl;
                                    if (i == candidateIndex) {
                                        try {
                                            outFile << (stoi(votes) + 1) << endl;  // Increment vote
                                        } catch(const std::exception& e) {
                                            outFile << "1" << endl;  // If votes is invalid, set to 1
                                        }
                                    } else {
                                        outFile << votes << endl;
                                    }
                                }
                                
                                // Record vote in voter's history
                                dynamic_cast<Voter*>(currentUser)->addVotedElection(electionId);
                                success = true;
                            }
                        }
                    }
                }
                
                // Write separator
                outFile << "----------------------------------------" << endl;
                
                // Skip rest of current election in input file
                while (getline(inFile, line) && line != "----------------------------------------") {}
                
            } else {
                // Copy entire election record unchanged
                while (getline(inFile, line) && line != "----------------------------------------") {
                    outFile << line << endl;
                }
                outFile << "----------------------------------------" << endl;
            }
        }
        
        inFile.close();
        outFile.close();
        
        if (found) {
            if (success) {
                remove("data/elections.txt");
                rename("data/elections_temp.txt", "data/elections.txt");
                cout << "Vote cast successfully!" << endl;
                return true;
            } else {
                remove("data/elections_temp.txt");
                return false;
            }
        } else {
            remove("data/elections_temp.txt");
            cout << "Election not found." << endl;
            return false;
        }
    }
    return false;
}

bool VotingSystem::hasVoted(string electionId) {
    if (currentUser != nullptr) {
        Voter* voter = dynamic_cast<Voter*>(currentUser);
        if (voter != nullptr) {
            return voter->hasVotedInElection(electionId);
        }
    }
    return false;
}
