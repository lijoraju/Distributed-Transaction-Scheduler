#include "service.h"
#include "queue"
#include "algorithm"
#include "utility.h"
#include "constants.h"

// Service class constructor
Service::Service(int typeVal, int numOfWorkerThreadsVal)
{
    type = typeVal;
    numOfWorkerThreads = numOfWorkerThreadsVal;
    threads = (WorkerThread *)malloc(sizeof(WorkerThread) * numOfWorkerThreads);
}

Service::~Service()
{
}

// Service class getting type get
int Service::getType()
{
    return type;
}

// Service class getting num of worker threads
int Service::getNumOfWorkerThreads()
{
    return numOfWorkerThreads;
}

// Service class setting worker thread priority and resource
void Service::setPriorityAndResourcesAssignedForWorkerThread(int index, int priorityVal, int resourcesVal)
{
    threads[index].threadNo = index + 1;
    threads[index].priority = priorityVal;
    threads[index].assignedResources = resourcesVal;
    availableResources = 0;
}

// Service class inserting incoming request to queue before scheduling or mapping to a thread
void Service::insertIncomingRequestToQueue(Request request)
{
    requests.push(request);
}

// Service class setting total resources
void Service::setTotalResources(int resources)
{
    totalResources = resources;
}

// Service class increasing available resources
void Service::increaseAvailableResourcesBy(int resources)
{
    availableResources += resources;
}

// Service class decreasing available resources
void Service::decreaseAvailableResourcesBy(int resources)
{
    availableResources -= resources;
}

// Service class getting resource info
pair<int, int> Service::getResourcesInfo()
{
    return {totalResources, availableResources};
}

// Service class sorting worker threads on assigned resources
void Service::sortWorkerThreads()
{
    sort(threads, threads + numOfWorkerThreads, [](const WorkerThread &thread1, const WorkerThread &thread2)
         { return thread1.assignedResources < thread2.assignedResources; });
}

// Service class checking if request queue is empty
bool Service::isRequestQueueEmpty()
{
    return requests.empty();
}

// Service class removing request from a queue front
Request Service::removeRequestFromQueueFront()
{
    Request request = requests.front();
    requests.pop();
    return request;
}

// Service class releasing resources after successful allocation and scheduling of request
void Service::releaseResources(int threadIndex, int numOfResourcesReleased)
{
    threads[threadIndex].assignedResources -= numOfResourcesReleased;
}

// Service class updating schedule table with request no and its assigned thread no
void Service::updateScheduleTable(int requestNo, int threadNo)
{
    if (threadNo == -1)
    {
        scheduleTable.erase(requestNo);
    }
    else
    {
        scheduleTable[requestNo] = threadNo;
    }
}

// Service class checking if waiting list is empty
bool Service::isWaitingListEmpty()
{
    return waitingList.empty();
}

// Service class adding a request and its assigned thread to waiting list
void Service::addToWaitingList(Request request, WorkerThread thread)
{
    waitingList.push({request, thread});
    printWaitingList();
}

// Service class removing a pending request from the waiting list based on priority of threads
PendingRequest Service::removeFromWaitingList()
{
    PendingRequest penReq = waitingList.top();
    waitingList.pop();
    printWaitingList();
    return penReq;
}

// Service class assigning resources to a worker thread to process a request
int Service::assignResourcesToWorkerThread(int threadIndex, int numOfResources)
{
    int resources = min(numOfResources, availableResources);
    threads[threadIndex].assignedResources += resources;
    decreaseAvailableResourcesBy(resources);
    return resources;
}

// Service class getting schedule table
map<int, int> Service::getScheduleTable()
{
    return scheduleTable;
}

// Service class print the waiting list after every insertion or deletion
void Service::printWaitingList()
{
    vector<PendingRequest> buffer;
    PendingRequest penReq;
    string str;
    displayInConsole(waitingListUpdated);

    if (waitingList.empty())
    {
        displayInConsole(waitingIsEmpty);
        return;
    }

    str.clear();
    while (!waitingList.empty())
    {
        str += to_string(waitingList.top().request.requestNo) + emptySpace;
        buffer.push_back(waitingList.top());
        waitingList.pop();
    }

    displayInConsole(str);

    for (int i = 0; i < buffer.size(); i++)
    {
        waitingList.push({buffer[i].request, buffer[i].thread});
    }
}