#ifndef Service_H
#define Service_H
#include "string"
#include "queue"
#include "map"

using namespace std;

struct WorkerThread
{
    int threadNo;
    int priority;
    int assignedResources;
};
typedef struct WorkerThread WorkerThread;

struct Request
{
    int requestNo;
    int transactionType;
    int resourcesRequired;
    int resourcesObtained;
    clock_t arrivalTime;
    int waitingTime;
    int turnAroundTime;
};
typedef struct Request Request;

struct PendingRequest
{
    Request request;
    WorkerThread thread;
};
typedef struct PendingRequest PendingRequest;

class Compare
{
public:
    bool operator()(PendingRequest r1, PendingRequest r2)
    {
        int minResR1;
        int minResR2;

        if (r1.thread.priority < r2.thread.priority)
        {
            return true;
        }
        else if (r1.thread.priority == r2.thread.priority)
        {
            minResR1 = r1.request.resourcesRequired - r1.thread.assignedResources;
            minResR2 = r2.request.resourcesRequired - r2.thread.assignedResources;

            if (minResR1 <= minResR2)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
};

class Service
{
private:
    int type;
    int numOfWorkerThreads;
    int totalResources;
    int availableResources;
    queue<Request> requests;
    map<int, int> scheduleTable;
    priority_queue<PendingRequest, vector<PendingRequest>, Compare> waitingList;

public:
    WorkerThread *threads;
    Service(int typeVal, int numOfWorkerThreadsVal);
    ~Service();
    int getType();
    int getNumOfWorkerThreads();
    void setPriorityAndResourcesAssignedForWorkerThread(int index, int priorityVal, int resourcesVal);
    void insertIncomingRequestToQueue(Request request);
    void setTotalResources(int resources);
    pair<int, int> getResourcesInfo();
    void sortWorkerThreads();
    bool isRequestQueueEmpty();
    Request removeRequestFromQueueFront();
    void increaseAvailableResourcesBy(int resources);
    void decreaseAvailableResourcesBy(int resources);
    void releaseResources(int threadIndex, int numOfResourcesReleased);
    void updateScheduleTable(int requestNo, int threadNo);
    bool isWaitingListEmpty();
    void addToWaitingList(Request request, WorkerThread thread);
    PendingRequest removeFromWaitingList();
    int assignResourcesToWorkerThread(int threadIndex, int numOfResources);
    map<int, int> getScheduleTable();
    void printWaitingList();
};

#endif