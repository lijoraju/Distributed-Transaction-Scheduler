#include <iostream>
#include "constants.h"
#include "map"
#include "Service.h"
#include "utility.h"
#include "queue"
#include "climits"

using namespace std;

#define BURST_TIME 1 // in ms

int numOfServices;            // n in question
map<int, Service *> services; // num of servers
queue<Request> requests;      // initial queue before assigning to a particular service
int numOfRequests;            // num of requests received for scheduling

int initializeServicesAndWorkerThreads();
void getRequestsForScheduling();
void assignRequestToService();
void performProcessScheduling();
pair<WorkerThread, bool> mapRequestToThread(WorkerThread threads[], int n, Request request);
void displayScheduleDetails(int serviceNo, vector<Request> schedule, vector<Request> rejectedRequestsList);
void calculateWaitingAndTurnAroundTimeForRequest(Request &request);
void printScheduleTable(map<int, int> table);

int main()
{
    if (initializeServicesAndWorkerThreads())
    {
        getRequestsForScheduling();
        assignRequestToService();
        performProcessScheduling();
    }
    return 0;
}

/**
 * @brief Intializing the services and threads in the system
 * Each service will be a separate server and it has its own worker threads
 * @return int {0 - error and 1 - success}
 */
int initializeServicesAndWorkerThreads()
{
    int numOfWorkerThreadsVal;
    string str;
    int priorityVal;
    int assignedResourcesVal;
    int totalResources;
    displayInConsole(welcomeMsg);
    displayInConsole(numberOfServicesInTheSystem);
    cin >> numOfServices;

    // validating num of services i/p
    if (!validateInput(numOfServices))
    {
        displayInConsole(invalidInput);
        return 0;
    }

    for (int i = 1; i <= numOfServices; i++)
    {
        totalResources = 0;
        str.clear();
        str = serviceLabel + emptySpace + to_string(i);
        displayInConsole(str);
        displayInConsole(numberOfWorkerThreads);
        cin >> numOfWorkerThreadsVal;

        // validating num of threads i/p
        if (!validateInput(numOfWorkerThreadsVal))
        {
            displayInConsole(invalidInput);
            exit(1);
        }

        services[i] = new Service(i, numOfWorkerThreadsVal); // creating new service class object

        if (services[i] == NULL)
        {
            displayInConsole(outOfMemory);
            exit(1);
        }

        for (int j = 0; j < numOfWorkerThreadsVal; j++)
        {
            str.clear();
            str = priorityLevelAndResourcesAssigned + emptySpace + to_string(j + 1);
            displayInConsole(str);
            cin >> priorityVal >> assignedResourcesVal;

            // validating priority and resources assigned i/p
            if (!(validateInput(priorityVal) && validateInput(assignedResourcesVal)))
            {
                displayInConsole(invalidInput);
                exit(1);
            }
            // setting priority and resources for service class thread
            services[i]->setPriorityAndResourcesAssignedForWorkerThread(j, priorityVal, assignedResourcesVal);
            totalResources += assignedResourcesVal;
        }
        services[i]->setTotalResources(totalResources); // updating total resources for service class
        services[i]->sortWorkerThreads();               // sorting threads based on assigned resources ASC
    }

    return 1;
}

/**
 * @brief Get the Requests For Scheduling object
 * Second step after service intialization.
 * User will input request one by one <type of service,resource required>
 */
void getRequestsForScheduling()
{
    displayInConsole(numberOfRequest);
    cin >> numOfRequests;
    int transactionType;
    int resourcesRequired;
    Request req;
    string str;

    for (int i = 1; i <= numOfRequests; i++)
    {
        str.clear();
        str = request + emptySpace + to_string(i) + emptySpace + openBracket + transactionTypeLabel + openingSquareBracket + one + hypen + to_string(numOfServices) + closingSquareBracket + emptySpace + resourceRequiredLabel + closeBracket;
        displayInConsole(str);
        cin >> transactionType >> resourcesRequired;

        if (!(validateTransactionType(transactionType, numOfServices) && validateInput(resourcesRequired)))
        {
            displayInConsole(invalidInput);
            exit(1);
        }

        req = {i, transactionType, resourcesRequired, initialResources, clock()}; // creating Request Struct obj
        requests.push(req);                                                       // Inserting into the initial queue before assigning to a service server
    }
}

/**
 * @brief Based on request type a request is assigned to
 * the particular service that it belong
 */
void assignRequestToService()
{
    int type;
    while (!requests.empty())
    {
        type = requests.front().transactionType;
        services[type]->insertIncomingRequestToQueue(requests.front()); // inserting to the corresponding service queue before mapping to a thread
        requests.pop();
    }
}

/**
 * @brief Perform request scheduling for each server
 * monitors the waiting and rejected requests for each
 * server separately
 */
void performProcessScheduling()
{
    Request request;
    WorkerThread thread;
    vector<Request> rejectedRequests;
    vector<Request> schedule;
    pair<WorkerThread, bool> mappingStatus;
    bool isWaiting;
    int resourcesFurtherRequired;
    int newResourcesObtained;
    int waitingTime;

    for (int i = 1; i <= numOfServices; i++)
    {
        rejectedRequests.clear();
        schedule.clear();
        displayInConsole(newLine + serviceLabel + emptySpace + to_string(i));

        if (services[i]->isRequestQueueEmpty())
        {
            displayInConsole(noRequestToSchedule);
            continue;
        }

        while (!services[i]->isRequestQueueEmpty())
        {
            request = services[i]->removeRequestFromQueueFront(); // getting the request from queue front for scheduling
            if (request.resourcesRequired > services[i]->getResourcesInfo().first)
            {
                rejectedRequests.push_back(request); // resource requirement can't be satisfied so request is rejected
            }
            else
            {
                mappingStatus = mapRequestToThread(services[i]->threads, services[i]->getNumOfWorkerThreads(), request); // contains the mapped thread and waiting status
                thread = mappingStatus.first;
                isWaiting = mappingStatus.second;
                services[i]->updateScheduleTable(request.requestNo, thread.threadNo); // schedule table is updated
                if (isWaiting == false)
                {
                    // no wait request can be scheduled
                    request.resourcesObtained = request.resourcesRequired;
                    calculateWaitingAndTurnAroundTimeForRequest(request);
                    schedule.push_back(request);
                    services[i]->releaseResources(thread.threadNo, request.resourcesObtained);
                    services[i]->increaseAvailableResourcesBy(request.resourcesObtained);
                }
                else if (isWaiting)
                {
                    // request need to wait till required resources become available
                    request.resourcesObtained = thread.assignedResources;
                    services[i]->releaseResources(thread.threadNo, request.resourcesObtained);
                    services[i]->addToWaitingList(request, thread);
                }
            }
        }

        while (!services[i]->isWaitingListEmpty())
        {
            // scheduling the waiting requests once the required resources become available
            PendingRequest penReq = services[i]->removeFromWaitingList();
            resourcesFurtherRequired = penReq.request.resourcesRequired - penReq.request.resourcesObtained;
            newResourcesObtained = services[i]->assignResourcesToWorkerThread(penReq.thread.threadNo, resourcesFurtherRequired);
            penReq.request.resourcesObtained += newResourcesObtained;
            services[i]->releaseResources(penReq.thread.threadNo, newResourcesObtained);
            if (penReq.request.resourcesObtained == penReq.request.resourcesRequired)
            {
                // waiting is over request can be scheduled now
                calculateWaitingAndTurnAroundTimeForRequest(penReq.request);
                schedule.push_back(penReq.request);
                services[i]->increaseAvailableResourcesBy(penReq.request.resourcesObtained);
            }
            else if (penReq.request.resourcesObtained < penReq.request.resourcesRequired)
            {
                waitingTime = clock() - penReq.request.arrivalTime;
                if (waitingTime < timeout)
                {
                    // request need to be rejected since it waiting time crossed the timeout and release any acquired resources
                    services[i]->addToWaitingList(penReq.request, penReq.thread);
                }
                else
                {
                    // still need to wait till some resources will become available
                    services[i]->updateScheduleTable(penReq.request.requestNo, -1);
                    services[i]->increaseAvailableResourcesBy(penReq.request.resourcesObtained);
                    rejectedRequests.push_back(penReq.request);
                }
            }
        }

        displayScheduleDetails(i, schedule, rejectedRequests);
        printScheduleTable(services[i]->getScheduleTable()); // printing the schedule table
    }
}

/**
 * @brief Mapping of request to a the best thread possible based on the
 *  resource availability and resource requirement
 * @param threads {WorkerThread}
 * @param n {num of threads available for the service}
 * @param request {Request}
 * @return pair<WorkerThread, bool> {mapping status}
 */
pair<WorkerThread, bool> mapRequestToThread(WorkerThread threads[], int n, Request request)
{
    int threadIndex;
    bool isWaiting;

    isWaiting = true;

    for (int i = 0; i < n; i++)
    {
        if (request.resourcesRequired <= threads[i].assignedResources)
        {
            // enough resources are availabe with the thread to satisfy the request
            threadIndex = i;
            isWaiting = false;
            break;
        }
    }
    if (isWaiting)
    {
        // no thread can satisfy the request currently
        int currentMinResourceRequired = INT_MAX;
        int minResourceRequired;

        for (int i = 0; i < n; i++)
        {
            minResourceRequired = request.resourcesRequired - threads[i].assignedResources;
            if (currentMinResourceRequired > minResourceRequired)
            {
                currentMinResourceRequired = minResourceRequired;
                threadIndex = i;
            }
        }
    }

    return {threads[threadIndex], isWaiting};
}

/**
 * @brief Display the schedule details for a particular server
 * schedule, rejected requests, avg waiting time and avg turn around time are displayed
 * @param serviceNo {int}
 * @param schedule {vector<Request>}
 * @param rejectedRequestsList vector<Request>
 */
void displayScheduleDetails(int serviceNo, vector<Request> schedule, vector<Request> rejectedRequestsList)
{
    displayInConsole(newLine + requestScheduleForService);
    vector<Request>::iterator request;
    string str;
    float avgWaitingTime = 0;
    float avgTurnAroundTime = 0;
    int totalRequests = schedule.size() + rejectedRequestsList.size();

    // calculating the avg waiting time and avg turn around time
    for (request = schedule.begin(); request < schedule.end(); request++)
    {
        str += verticalBar + to_string((*request).requestNo);
        avgWaitingTime += (*request).waitingTime;
        avgTurnAroundTime += (*request).turnAroundTime;
    }

    avgWaitingTime /= totalRequests;    // avg WT
    avgTurnAroundTime /= totalRequests; // avg TAT

    // printing the schedule
    if (schedule.size() > 0)
    {
        displayInConsole(str + verticalBar);
    }
    else
    {
        displayInConsole(scheduleIsEmpty);
    }

    // printing the rejected requests due to resource unavailability or thread unavailability
    if (rejectedRequestsList.size() > 0)
    {
        displayInConsole(newLine + requestRejectedForService + emptySpace + to_string(rejectedRequestsList.size()));
        str.clear();

        for (request = rejectedRequestsList.begin(); request < rejectedRequestsList.end(); request++)
        {
            str += to_string((*request).requestNo) + emptySpace;
        }
        displayInConsole(str);
    }
    else
    {
        displayInConsole(newLine + requestRejectedForService + emptySpace + zero);
    }

    displayInConsole(newLine + avgWaitingTimeLabel + emptySpace + to_string(avgWaitingTime));       // printing avg WT
    displayInConsole(newLine + avgTurnAroundTimeLabel + emptySpace + to_string(avgTurnAroundTime)); // printing avg TAT
}

/**
 * @brief Calculating the waiting time and turn around time for
 * request and updating the request obj
 * @param request {Request}
 */
void calculateWaitingAndTurnAroundTimeForRequest(Request &request)
{
    request.waitingTime = clock() - request.arrivalTime;       // request WT
    request.turnAroundTime = request.waitingTime + BURST_TIME; // request TAT
}

/**
 * @brief Printing the schedule table
 * table header request_no thread_no
 * @param table {request_no, thread_no}
 */
void printScheduleTable(map<int, int> table)
{
    // Two column table column 1 Request No. and column 2 Thread No.
    displayInConsole(newLine + scheduleTableHeader);
    for (auto row : table)
    {
        displayInConsole(to_string(row.first) + tabSpace + to_string(row.second));
    }
}