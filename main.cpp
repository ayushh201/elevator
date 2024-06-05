#include<bits/stdc++.h>
#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <fstream>
using namespace std;
class passenger;
int passengerID = 0;
void clearScreen();
int generateRandomInt(int min, int max);
void plotArt(vector<vector<passenger>> waiting, vector<int> lifts, int maxFloor, vector<int> storageModule);
void printPMap(vector<vector<passenger>> pMap);
void mergeSort(vector<int> &vec, int const begin, int const end);
void mergeSort(vector<int> &vec, int const begin, int const end);
void merge(vector<int> &vec, int const left, int const mid, int const right);
void sleepForMilliseconds(int milliseconds);
vector<int> removeIndices(const vector<int> &original, const vector<int> &indicesToRemove)
{
    vector<int> result;
    for (int i = 0; i < original.size(); i++)
    {
        if (find(indicesToRemove.begin(), indicesToRemove.end(), i) == indicesToRemove.end())
        {
            result.push_back(original[i]);
        }
    }
    return result;
}
class Elevator
{
public:
    int currentFloor;
    int direction;
    int storage;
    vector<int> destinations;
    int waitTime;
    int prevDirection;

public:
    Elevator();
};

Elevator::Elevator()
{
    currentFloor = 0;
    direction = 0;
    waitTime = 0;
    prevDirection = 0;
    storage = 0;
}

class SCAN : public Elevator
{
};

class LOOK : public Elevator
{
};

class ShortestPath : public Elevator
{
public:
    int destination;
    int oppositeDestination;
    vector<passenger> assigned;
    ShortestPath()
    {
        destination = -1;
        oppositeDestination = -1;
    }
};

class passenger
{
public:
    int currentFloor;
    int destination;
    int travelling; // 0 means not travelling, 1 means that he is travelling and -1 means he has reached his destination.
    int direction;
    int UID;
    passenger(int floors);
    void printPassenger();
    friend void plotArt(vector<vector<passenger>> waiting, vector<int> lifts, int maxFloor, vector<int> storageModule);
};
passenger::passenger(int floors)
{
    floors = floors;
    currentFloor = generateRandomInt(1, floors - 2);
    destination = generateRandomInt(1, floors - 2);
    travelling = 0;
    if (currentFloor == destination)
    {
        destination = currentFloor / 2;
        if (currentFloor == destination) // Edge case if both are 0
            currentFloor = destination + floors - 1;
    }
    direction = abs(-currentFloor + destination) / (destination - currentFloor);
    UID = passengerID;
    passengerID++;
}
// Add this method to the passenger class
void passenger::printPassenger()
{
    ofstream details("Details.txt", ios::app);
    details << "Current Floor is " << currentFloor << " and he has to go to " << destination << endl;
    details.close();
}
int generateRandomInt(int min, int max)
{
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> distrib(min, max);

    return distrib(gen);
}

class elevatorSystem
{
public:
    float score;

protected:
    vector<vector<passenger>> human;
    int liftNumber;
    int maxPeople;
    int floors;
    vector<vector<passenger>> waiting;

public:
    elevatorSystem() {}
    virtual void run() = 0;
};
class LOOKSystem : public elevatorSystem
{
    vector<LOOK> lifts;
    vector<int> liftCoords;

public:
    LOOKSystem(vector<vector<passenger>> huma, int liftNumbe, int floor, int maxHumans)
    {
        maxPeople = maxHumans;
        human = huma;
        liftNumber = liftNumbe;
        floors = floor;
        for (int i = 0; i < liftNumber; i++)
        {
            LOOK ob;
            lifts.push_back(ob);
            liftCoords.push_back(0);
        }
        waiting.resize(floors);
        score = 0;
    }
    void run()
    {
        vector<int> vec(lifts.size(), 0);
        liftCoords.resize(lifts.size());
        // We have the list of lifts
        // We also have the PMap of all the people here
        // So we will sequentially generate data and run the draw function directly from here
        plotArt(waiting, liftCoords, floors, vec); // Initial Plot
        for (int tickNumber = 0; tickNumber < human.size(); tickNumber++)
        {
            // Adding the people from pmap to waiting
            for (int i = 0; i < human[tickNumber].size(); i++)
            {
                passenger currentHuman = human[tickNumber][i];
                waiting[currentHuman.currentFloor].push_back(currentHuman);
            }

            // Calculating what directions the lifts will move wrt the new passengers
            for (int i = 0; i < lifts.size(); i++)
            {
                // THis is the most basic algorithm
                // This lift will just go to the top then at the bottom and repeat
                if (lifts[i].waitTime != 0)
                {
                    lifts[i].prevDirection = lifts[i].direction;
                    lifts[i].direction = 0;
                }
                else if (lifts[i].prevDirection != 0)
                {
                    lifts[i].direction = lifts[i].prevDirection;
                    lifts[i].prevDirection = 0;
                }
                if (lifts[i].currentFloor == 0)
                {
                    lifts[i].direction = 1;
                }
                if (lifts[i].currentFloor == floors - 1)
                {
                    lifts[i].direction = -1;
                }
                // The condition that makes LOOK unique
                if (lifts[i].destinations.size() == 0)
                {
                    if (lifts[i].direction == 1 && lifts[i].currentFloor != 0)
                    {
                        // If we do not find any lift going up, we turn
                        bool found = false;
                        for (int j = lifts[i].currentFloor; j < floors; j++)
                        {
                            if (waiting[j].size() != 0)
                            {
                                found = true;
                            }
                        }
                        if (!found)
                            lifts[i].direction = -1;
                    }
                    else if (lifts[i].direction == -1 && lifts[i].currentFloor != floors - 1)
                    {
                        // If we do not find any lift going down we turn
                        bool found = false;
                        for (int j = lifts[i].currentFloor; j >= 0; j--)
                        {
                            if (waiting[j].size() != 0)
                            {
                                found = true;
                            }
                        }
                        if (!found)
                            lifts[i].direction = 1;
                    }
                }
            }
            // moving the lifts in the specified direction
            for (int i = 0; i < lifts.size(); i++)
            {
                lifts[i].currentFloor += lifts[i].direction;
            }

            // picking up passengers
            for (int i = 0; i < lifts.size(); i++)
            {
                if (waiting[lifts[i].currentFloor].size() != 0)
                {
                    vector<passenger> temp;
                    // We will add their destinations in the respective lifts, and remove the people from waiting vector.
                    for (int j = 0; j < waiting[lifts[i].currentFloor].size(); j++)
                    {
                        if (lifts[i].direction == waiting[lifts[i].currentFloor][j].direction && lifts[i].storage < maxPeople)
                        {
                            lifts[i].destinations.push_back(waiting[lifts[i].currentFloor][j].destination);
                            lifts[i].storage++;
                            score++;
                            lifts[i].waitTime += 10;
                        }
                        else
                            temp.push_back(waiting[lifts[i].currentFloor][j]);
                    }
                    // We will save the elements of waiting somewhere else
                    // Then clear this
                    // Then paste the safe elements back here

                    waiting[lifts[i].currentFloor].clear();
                    waiting[lifts[i].currentFloor] = temp;
                    temp.clear();
                    // removing from vector
                }
            }

            // Checking if the passengers have reached and adding that to the score
            for (int i = 0; i < lifts.size(); i++)
            {
                // Get the current floor of the i-th elevator
                int checker = lifts[i].currentFloor;

                // Count the number of times the current floor appears in the destination list of the i-th elevator
                // and add it to the score
                int temp = count(lifts[i].destinations.begin(), lifts[i].destinations.end(), checker);
                score += 5 * temp;
                lifts[i].storage -= temp;
                lifts[i].waitTime = 5 * temp;

                // Remove all occurrences of the current floor from the destination list of the i-th elevator
                lifts[i].destinations.erase(remove(lifts[i].destinations.begin(), lifts[i].destinations.end(), checker), lifts[i].destinations.end());
            }
            // finding the number of people waiting
            int noOfWaiting = 0;
            for (int i = 0; i < waiting.size(); i++)
            {
                noOfWaiting += waiting[i].size();
            }
            score -= noOfWaiting / 2;
            // finding the number of people on the elevator
            int noOnLift = 0;
            for (int i = 0; i < lifts.size(); i++)
            {
                noOnLift += lifts[i].storage;
            }
            score -= noOnLift;
            // Number of lifts beigh used
            int freeLifts = 0;
            for (int i = 0; i < lifts.size(); i++)
            {
                if (lifts[i].direction == 0 && lifts[i].waitTime == 0)
                    freeLifts++;
            }
            score += 5 * freeLifts;

            // Generating liftCoords
            for (int i = 0; i < lifts.size(); i++)
            {
                liftCoords[i] = lifts[i].currentFloor;
                vec[i] = lifts[i].storage;
            }
            // Plot after all the calculations
            plotArt(waiting, liftCoords, floors, vec);
        }
    }
};
class SCANSystem : public elevatorSystem
{
    vector<SCAN> lifts;
    vector<int> liftCoords;

public:
    SCANSystem(vector<vector<passenger>> huma, int liftNumbe, int floor, int maxHumans)
    {
        maxPeople = maxHumans;
        human = huma;
        liftNumber = liftNumbe;
        floors = floor;
        for (int i = 0; i < liftNumber; i++)
        {
            SCAN ob;
            lifts.push_back(ob);
            liftCoords.push_back(0);
        }
        waiting.resize(floors);
        score = 0;
    }
    void run()
    {
        vector<int> vec(lifts.size(), 0);
        liftCoords.resize(lifts.size());
        // We have the list of lifts
        // We also have the PMap of all the people here
        // So we will sequentially generate data and run the draw function directly from here
        plotArt(waiting, liftCoords, floors, vec); // Initial Plot
        for (int tickNumber = 0; tickNumber < human.size(); tickNumber++)
        {
            // Adding the people from pmap to waiting
            for (int i = 0; i < human[tickNumber].size(); i++)
            {
                passenger currentHuman = human[tickNumber][i];
                waiting[currentHuman.currentFloor].push_back(currentHuman);
            }

            // Calculating what directions the lifts will move wrt the new passengers
            for (int i = 0; i < lifts.size(); i++)
            {
                // THis is the most basic algorithm
                // This lift will just go to the top then at the bottom and repeat
                if (lifts[i].waitTime != 0)
                {
                    lifts[i].prevDirection = lifts[i].direction;
                    lifts[i].direction = 0;
                }
                else if (lifts[i].prevDirection != 0)
                {
                    lifts[i].direction = lifts[i].prevDirection;
                    lifts[i].prevDirection = 0;
                }
                if (lifts[i].currentFloor == 0)
                {
                    lifts[i].direction = 1;
                }
                if (lifts[i].currentFloor == floors - 1)
                {
                    lifts[i].direction = -1;
                }
            }
            // moving the lifts in the specified direction
            for (int i = 0; i < lifts.size(); i++)
            {
                lifts[i].currentFloor += lifts[i].direction;
            }

            // picking up passengers
            for (int i = 0; i < lifts.size(); i++)
            {
                if (waiting[lifts[i].currentFloor].size() != 0)
                {
                    vector<passenger> temp;
                    // We will add their destinations in the respective lifts, and remove the people from waiting vector.
                    for (int j = 0; j < waiting[lifts[i].currentFloor].size(); j++)
                    {
                        if (lifts[i].direction == waiting[lifts[i].currentFloor][j].direction && lifts[i].storage < maxPeople)
                        {
                            lifts[i].destinations.push_back(waiting[lifts[i].currentFloor][j].destination);
                            lifts[i].storage++;
                            lifts[i].waitTime += 10;
                            score++;
                        }
                        else
                            temp.push_back(waiting[lifts[i].currentFloor][j]);
                    }
                    // We will save the elements of waiting somewhere else
                    // Then clear this
                    // Then paste the safe elements back here

                    waiting[lifts[i].currentFloor].clear();
                    waiting[lifts[i].currentFloor] = temp;
                    temp.clear();
                    // removing from vector
                }
            }

            // Checking if the passengers have reached and adding that to the score
            for (int i = 0; i < lifts.size(); i++)
            {
                // Get the current floor of the i-th elevator
                int checker = lifts[i].currentFloor;

                // Count the number of times the current floor appears in the destination list of the i-th elevator
                // and add it to the score
                int temp = count(lifts[i].destinations.begin(), lifts[i].destinations.end(), checker);
                score += 5 * temp;
                lifts[i].storage -= temp;
                lifts[i].waitTime = 5 * temp;

                // Remove all occurrences of the current floor from the destination list of the i-th elevator
                lifts[i].destinations.erase(remove(lifts[i].destinations.begin(), lifts[i].destinations.end(), checker), lifts[i].destinations.end());
            }
            // finding the number of people waiting
            int noOfWaiting = 0;
            for (int i = 0; i < waiting.size(); i++)
            {
                noOfWaiting += waiting[i].size();
            }
            score -= noOfWaiting / 2;
            // finding the number of people on the elevator
            int noOnLift = 0;
            for (int i = 0; i < lifts.size(); i++)
            {
                noOnLift += lifts[i].storage;
            }
            score -= noOnLift;
            // Number of lifts beigh used
            int freeLifts = 0;
            for (int i = 0; i < lifts.size(); i++)
            {
                if (lifts[i].direction == 0 && lifts[i].waitTime == 0)
                    freeLifts++;
            }
            score += 5 * freeLifts;

            // Generating liftCoords
            for (int i = 0; i < lifts.size(); i++)
            {
                liftCoords[i] = lifts[i].currentFloor;
                vec[i] = lifts[i].storage;
            }
            // Plot after all the calculations
            plotArt(waiting, liftCoords, floors, vec);
        }
    }
};

class shortestPathSystem : public elevatorSystem
{
    vector<ShortestPath> lifts;
    vector<int> liftCoords;

public:
    shortestPathSystem(vector<vector<passenger>> huma, int liftNumbe, int floor, int maxHumans)
    {
        maxPeople = maxHumans;
        human = huma;
        liftNumber = liftNumbe;
        floors = floor;
        for (int i = 0; i < liftNumber; i++)
        {
            ShortestPath ob;
            lifts.push_back(ob);
            liftCoords.push_back(0);
        }
        waiting.resize(floors);
        score = 0;
    }
    void run()
    {
        vector<passenger> priority;
        vector<int> vec(lifts.size(), 0);
        liftCoords.resize(lifts.size());
        // We have the list of lifts
        // We also have the PMap of all the people here
        // So we will sequentially generate data and run the draw function directly from here
        plotArt(waiting, liftCoords, floors, vec); // Initial Plot
        for (int tickNumber = 0; tickNumber < human.size(); tickNumber++)
        {
            // Adding the people from pmap to waiting
            for (int i = 0; i < human[tickNumber].size(); i++)
            {
                passenger currentHuman = human[tickNumber][i];
                waiting[currentHuman.currentFloor].push_back(currentHuman);
            }
            // First we add the new people to the priority stack
            for (int i = 0; i < human[tickNumber].size(); i++)
            {
                passenger currentHuman = human[tickNumber][i];
                priority.push_back(currentHuman);
            }
            // Clearing lifts assigned
            // As they are reassigned everytime
            for (int i = 0; i < lifts.size(); i++)
            {
                lifts[i].assigned.clear();
            }
            // Assigning lifts to all the people of the lift
            for (int i = 0; i < priority.size(); i++)
            {
                int index = -1;
                // We will calculate the best lift for each person
                // Finding the time it takes
                int distance = 1000000;
                for (int j = 0; j < lifts.size(); j++)
                {
                    if (lifts[j].storage + lifts[j].assigned.size() < maxPeople)
                    {
                        int temp;
                        // priority[i] is the human and lift[i] is the lift we are calculating distance between
                        if (priority[i].direction == lifts[j].direction)
                        {
                            temp = abs(priority[i].currentFloor - lifts[j].currentFloor);
                            if (temp < distance)
                            {
                                distance = temp;
                                index = j;
                            }
                        }
                        else
                        {
                            if (lifts[j].direction == 1) // we are going up
                            {
                                temp = abs(lifts[j].currentFloor - floors) + abs(floors - priority[i].currentFloor);
                            }
                            else // we are going down
                            {
                                temp = abs(lifts[j].currentFloor) + abs(priority[i].currentFloor);
                            }
                            if (temp < distance)
                            {
                                distance = temp;
                                index = j;
                            }
                        }
                    }
                }

                // Now distance contains the min distance there is
                // We also need to know the index of that LIft
                // That is stored in index
                // So now we can assign lifts[index] to the passenger
                if (index != -1)
                    lifts[index].assigned.push_back(priority[i]);
            }
            // Now, on the basis of the data, we can determine the destination and the opposite destination for each lift
            for (int i = 0; i < lifts.size(); i++)
            {
                int temp = 0, temp1 = 0;
                lifts[i].oppositeDestination = -1;
                for (int j = 0; j < lifts[i].assigned.size(); j++)
                {
                    // Finding distance in same direction
                    if (lifts[i].destination == -1 || ((lifts[i].assigned[j].destination - lifts[i].currentFloor) * lifts[i].direction) > 0)
                    {
                        // cout << "Here";
                        int temp2 = abs(lifts[i].currentFloor - lifts[i].assigned[j].destination);
                        if (temp < temp2)
                        {
                            temp = temp2;
                            lifts[i].destination = lifts[i].assigned[j].destination;
                        }
                    }
                    else
                    {
                        int temp2 = abs(lifts[i].currentFloor - lifts[i].assigned[j].destination);
                        if (temp1 < temp2)
                        {
                            temp1 = temp2;
                            lifts[i].oppositeDestination = lifts[i].assigned[j].destination;
                        }
                    }
                }
                // running the same stats for people aready sitting in the lift
                for (int j = 0; j < lifts[i].destinations.size(); j++)
                {
                    // Finding distance in same direction
                    if (lifts[i].destination == -1 || ((lifts[i].destinations[j] - lifts[i].currentFloor) * lifts[i].direction) > 0)
                    {
                        // cout << "Here";
                        int temp2 = abs(lifts[i].currentFloor - lifts[i].destinations[j]);
                        if (temp < temp2)
                        {
                            temp = temp2;
                            lifts[i].destination = lifts[i].assigned[j].destination;
                        }
                    }
                    else
                    {
                        int temp2 = abs(lifts[i].currentFloor - lifts[i].destinations[j]);
                        if (temp1 < temp2)
                        {
                            temp1 = temp2;
                            lifts[i].oppositeDestination = lifts[i].destinations[j];
                        }
                    }
                }
            }
            // Calculating what directions the lifts will move wrt the new passengers
            for (int i = 0; i < lifts.size(); i++)
            {
                // This lift will just go to the top then at the bottom and repeat
                if (lifts[i].waitTime != 0)
                {
                    lifts[i].prevDirection = lifts[i].direction;
                    lifts[i].direction = 0;
                }
                else if (lifts[i].prevDirection != 0)
                {
                    lifts[i].direction = lifts[i].prevDirection;
                    lifts[i].prevDirection = 0;
                }

                if (lifts[i].currentFloor == floors - 1)
                {
                    lifts[i].direction = 0;
                }
                else if (lifts[i].currentFloor == 0)
                {
                    lifts[i].direction = 0;
                }
                if (lifts[i].destination == lifts[i].currentFloor && lifts[i].oppositeDestination != -1)
                {
                    lifts[i].direction = -1 * lifts[i].direction;
                    lifts[i].destination = lifts[i].oppositeDestination;
                    lifts[i].oppositeDestination = -1;
                }
                else if (lifts[i].destination == lifts[i].currentFloor)
                {
                    lifts[i].direction = 0;
                    lifts[i].destination = -1;
                }
                else if (lifts[i].direction == 0 && lifts[i].waitTime == 0 && lifts[i].prevDirection == 0 && lifts[i].destination != -1)
                {
                    if (lifts[i].destination != -1)
                        lifts[i].direction = (lifts[i].destination - lifts[i].currentFloor) / abs(lifts[i].destination - lifts[i].currentFloor);
                }
            }
            // moving the lifts in the specified direction
            for (int i = 0; i < lifts.size(); i++)
            {
                lifts[i].currentFloor += lifts[i].direction;
            }

            // picking up passengers
            for (int i = 0; i < lifts.size(); i++)
            {
                vector<passenger> doNotRemove;
                // Now we check through passenger list of each lift
                for (int j = 0; j < lifts[i].assigned.size(); j++)
                {
                    if (lifts[i].assigned[j].currentFloor == lifts[i].currentFloor)
                    {
                        // means that the person assigned is in this floor

                        int temp = lifts[i].assigned[j].UID;
                        // Now we will scan through all the people standing on this floor and see whose UID matches to the one we have
                        for (int k = 0; k < priority.size(); k++)
                        {
                            if (priority[k].UID == temp)
                            {
                                // Lets get this sorted
                                lifts[i].destinations.push_back(priority[k].destination);
                                lifts[i].storage++;
                                lifts[i].waitTime += 10;
                                score++;
                                // I need to remove it from both waiting and priority
                                // We know its one so it will help
                                // I do not know what is the index of this element in waiting
                                // SO we will find it and erase that
                                for (int l = 0; l < waiting[lifts[i].currentFloor].size(); l++)
                                {
                                    if (waiting[lifts[i].currentFloor][l].UID == priority[k].UID)
                                    {
                                        waiting[lifts[i].currentFloor].erase(waiting[lifts[i].currentFloor].begin() + l);
                                        break;
                                    }
                                }
                                // I also have to remove it from lift[i].assigned...

                                priority.erase(priority.begin() + k);
                            }
                        }
                    }
                    else
                        doNotRemove.push_back(lifts[i].assigned[j]);

                    // Now we scan through the people at this floor for their UIDs and if it matches, we put that person in our lift
                }
                lifts[i].assigned.clear();
                lifts[i].assigned = doNotRemove;
            }

            // Checking if the passengers have reached and adding that to the score
            for (int i = 0; i < lifts.size(); i++)
            {
                // Get the current floor of the i-th elevator
                int checker = lifts[i].currentFloor;

                // Count the number of times the current floor appears in the destination list of the i-th elevator
                // and add it to the score
                int temp = count(lifts[i].destinations.begin(), lifts[i].destinations.end(), checker);
                score += 5 * temp;
                lifts[i].storage -= temp;
                lifts[i].waitTime = 5 * temp;

                // Remove all occurrences of the current floor from the destination list of the i-th elevator
                lifts[i].destinations.erase(remove(lifts[i].destinations.begin(), lifts[i].destinations.end(), checker), lifts[i].destinations.end());
            }
            // finding the number of people waiting
            int noOfWaiting = 0;
            for (int i = 0; i < waiting.size(); i++)
            {
                noOfWaiting += waiting[i].size();
            }
            score -= noOfWaiting / 2;
            // finding the number of people on the elevator
            int noOnLift = 0;
            for (int i = 0; i < lifts.size(); i++)
            {
                noOnLift += lifts[i].storage;
            }
            score -= noOnLift;
            // Number of lifts beigh used
            int freeLifts = 0;
            for (int i = 0; i < lifts.size(); i++)
            {
                if (lifts[i].direction == 0 && lifts[i].waitTime == 0)
                    freeLifts++;
            }
            score += 5 * freeLifts;
            // Generating liftCoords
            for (int i = 0; i < lifts.size(); i++)
            {
                liftCoords[i] = lifts[i].currentFloor;
                vec[i] = lifts[i].storage;
            }
            // Plot after all the calculations
            plotArt(waiting, liftCoords, floors, vec);
        }
    }
};

class building
{
protected:
    int floors;
    int noOfElevators;
    int currentTick;
    int currentAlgorithm;
    int maxHumans;

    vector<vector<passenger>> pMap;

public:
    building(int maxFloors, int Elevators, vector<vector<passenger>> pMp, int maxPeople);
    void startSimulation();
};
building::building(int maxFloors, int Elevators, vector<vector<passenger>> pMp, int maxPeople)
{
    floors = maxFloors;
    noOfElevators = Elevators;
    pMap = pMp;
    currentTick = 0;
    currentAlgorithm = 0;
    maxHumans = maxPeople;
}

void building::startSimulation()
{
    cout << "The SCAN Algorithm";
    sleepForMilliseconds(1000);
    clearScreen();
    SCANSystem A(pMap, noOfElevators, floors, maxHumans);
    A.run();
    clearScreen();
    cout << "Now the LOOK Algorithm\n";
    sleepForMilliseconds(1000);
    clearScreen();
    LOOKSystem B(pMap, noOfElevators, floors, maxHumans);
    B.run();
    clearScreen();
    cout << "The Shortest Path Algorithm\n";
    sleepForMilliseconds(1000);
    clearScreen();
    shortestPathSystem D(pMap, noOfElevators, floors, maxHumans);
    D.run();
    cout << "Scan -> " << A.score << endl;
    cout << "Look -> " << B.score << endl;
    // cout << "ShortestTime -> " << C.score << endl;
    cout << "ShortestPath -> " << D.score << endl;
}
// Other Functions

void sleepForMilliseconds(int milliseconds)
{
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

vector<vector<passenger>> pMapGenerator(int secs, int floors, int maxSpawnRate)
{
    int ticks = secs * 5;
    vector<vector<passenger>> pMap;
    for (int i = 0; i < ticks; i++)
    {
        int spawnRate = generateRandomInt(0, maxSpawnRate);
        vector<passenger> alpha;
        for (int j = 0; j < spawnRate; j++)
        {
            passenger temp(floors);
            alpha.push_back(temp);
        }
        pMap.push_back(alpha);
    }
    return pMap;
}

void merge(vector<int> &vec, int const left, int const mid, int const right)
{
    auto const subArrayOne = mid - left + 1;
    auto const subArrayTwo = right - mid;

    // Create temporary vectors
    vector<int> leftArray(subArrayOne), rightArray(subArrayTwo);

    // Copy data to temporary vectors
    for (auto i = 0; i < subArrayOne; i++)
        leftArray[i] = vec[left + i];
    for (auto j = 0; j < subArrayTwo; j++)
        rightArray[j] = vec[mid + 1 + j];

    auto indexOfSubArrayOne = 0, indexOfSubArrayTwo = 0;
    int indexOfMergedArray = left;

    // Merge the temporary vectors back into vec
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo)
    {
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo])
        {
            vec[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else
        {
            vec[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }

    // Copy the remaining elements of leftArray, if there are any
    while (indexOfSubArrayOne < subArrayOne)
    {
        vec[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }

    // Copy the remaining elements of rightArray, if there are any
    while (indexOfSubArrayTwo < subArrayTwo)
    {
        vec[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
}

void mergeSort(vector<int> &vec, int const begin, int const end)
{
    if (begin >= end)
        return; // Returns recursively

    auto mid = begin + (end - begin) / 2;
    mergeSort(vec, begin, mid);
    mergeSort(vec, mid + 1, end);
    merge(vec, begin, mid, end);
}

// Function to print pMap
void printPMap(vector<vector<passenger>> pMap)
{
    ofstream details("Details.txt", ios::app);
    int j;
    for (int i = 0; i < pMap.size(); i++)
    {
        details << "Tick " << i << ":" << endl;
        for (j = 0; j < pMap[i].size(); j++)
        {
            pMap[i][j].printPassenger();
        }
        if (j < 1)
        {
            details << "No spawns in this tick" << endl;
        }
    }
    details.close();
}

void plotArt(vector<vector<passenger>> waiting, vector<int> lifts, int maxFloor, vector<int> storageModule)
{
    // We have 3 things
    // waiting, which contains the people that are currently standing
    // lifts, that contains the coordinates of the lifts
    // The max Height

    // Lets draw its ASCII Representation
    for (int i = maxFloor - 1; i >= 0; i--)
    {
        // 14 ||   |    |   [2]   |    |  || The passengers
        // 13 ||   |    |    |   [4]   |  ||

        // Printing Line by line
        cout << setw(3) << i;
        cout << " || ";
        for (int j = 0; j < lifts.size(); j++)
        {
            cout << " ";
            if (i == lifts[j])
            {
                cout << "[" << storageModule[j] << "]";
            }
            else
            {
                cout << " | ";
            }
            cout << " ";
        }
        // Now printing all the passengers
        cout << " ";
        for (int j = 0; j < waiting[i].size(); j++)
        {
            if (waiting[i][j].direction == 1)
                cout << "A";
            else
                cout << "V";
        }
        cout << endl;
    }
    sleepForMilliseconds(200);
    clearScreen();
}
void clearScreen()
{
    system("cls");
}
void displayGuide()
{
    int choice;
    while (true)
    {
        cout << "Press 1 for SCAN guide\n";
        cout << "Press 2 for LOOK guide\n";
        cout << "Press 3 for Shortest Path guide\n";
        cout << "Press 4 to Continue\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "SCAN: The lift starts by going up, when it reaches the top, it goes down, and repeat, serving requests on its way.\n";
            cout << "Press 0 to go back or 4 to Continue: ";
            cin >> choice;
            if (choice == 0)
                continue;
            else if (choice == 4)
                return;
            break;
        case 2:
            cout << "LOOK: Similar to SCAN, but changes direction if it does not find any passengers in its current direction.\n";
            cout << "Press 0 to go back or 4 to Continue: ";
            cin >> choice;
            if (choice == 0)
                continue;
            else if (choice == 4)
                return;
            break;
        case 3:
            cout << "Shortest Path: Calculates which lift will take the minimum time to reach and goes accordingly.\n";
            cout << "Press 0 to go back or 4 to Continue: ";
            cin >> choice;
            if (choice == 0)
                continue;
            else if (choice == 4)
                return;
            break;
        case 4:
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}
int main()
{
    cout << "Welcome to the Elevator Simulation" << endl;
    cout << "In this simulation program, we will be comparing 3 popular Elevator Algorithms" << endl;
    sleepForMilliseconds(4000);
    displayGuide();
    cout << "Before We begin, let us set up the starting conditions for our simulation" << endl;
    int maxFloors, noOfElevators, maxSpawnRate, duration, maxPeople;
    try
    {
        cout << "How many Floors Will the building have? (min 5)" << endl;
        cin >> maxFloors;
        if (maxFloors < 5)
        {
            throw std::invalid_argument("Number of floors cannot be less than 5");
        }

        cout << "How many Elevators will be there? (Max 10)" << endl;
        cin >> noOfElevators;
        if (noOfElevators > 10)
        {
            throw std::invalid_argument("Number of elevators cannot be more than 10");
        }

        cout << "What should be the duration of the algorithm in seconds ";
        cin >> duration;

        cout << "Enter the maximum number of people that can fit in one lift (max 8)" << endl;
        cin >> maxPeople;
        if (maxPeople > 8)
        {
            throw std::invalid_argument("Maximum number of people cannot be more than 8");
        }

        maxSpawnRate = 1;
        if (maxSpawnRate > 10)
        {
            throw std::invalid_argument("Maximum spawn rate cannot be more than 30");
        }
    }
    catch (const std::invalid_argument &e)
    {
        cout << "Invalid input: " << e.what() << "\nSetting to default value.\n";
        if (maxFloors < 5)
            maxFloors = 5;
        if (noOfElevators > 10)
            noOfElevators = 10;
        if (maxPeople > 8)
            maxPeople = 8;
        if (maxSpawnRate > 10)
            maxSpawnRate = 30;
    }
    vector<vector<passenger>> pMap = pMapGenerator(duration, maxFloors, maxSpawnRate);
    building wtc(maxFloors, noOfElevators, pMap, maxPeople);
    ofstream details("Details.txt");
    details << "Elevator Simulation Travel Log" << endl;
    details << endl
            << "The details of Passenger Spawns with ticks are given below: " << endl;
    details.close();
    printPMap(pMap);
    wtc.startSimulation();
}