#ifndef constants_H
#define constants_H
#include "string"

using namespace std;

// contains all the constants used in the program
const int initialResources = 0;
const int timeout = 10000;
const string emptySpace = " ";
const string welcomeMsg = "Welcome To Gagaland BE";
const string numberOfServicesInTheSystem = "Number of services in the system";
const string numberOfWorkerThreads = "Number of worker threads";
const string priorityLevelAndResourcesAssigned = "Priority level and Resources assigned for Worker Thread";
const string transactionTypeAndResourcesRequired = "Transaction Type and Resources Required";
const string serviceLabel = "SERVER";
const string numberOfRequest = "Number of Requests";
const string request = "Request";
const string transactionTypeLabel = "Transaction Type";
const string resourceRequiredLabel = "Resource Required";
const string openBracket = "<";
const string closeBracket = ">";
const string one = "1";
const string hypen = "-";
const string openingSquareBracket = "[";
const string closingSquareBracket = "]";
const string invalidInput = "Invalid Input";
const string outOfMemory = "Out of memory";
const string requestScheduleForService = "Requests Schedule";
const string verticalBar = "|";
const string requestRejectedForService = "Rejected Requests";
const string scheduleIsEmpty = "Schedule is empty";
const string avgWaitingTimeLabel = "Average Waiting Time(in ms)";
const string avgTurnAroundTimeLabel = "Average Turn Around Time(in ms)";
const string newLine = "\n";
const string scheduleTableHeader = "Request No\t\tThread No";
const string tabSpace = "\t\t\t";
const string waitingListUpdated = "Waiting List Updated";
const string waitingIsEmpty = "No Request is in waiting now";
const string zero = "0";
const string noRequestToSchedule = "No Request For This Server To Schedule";

#endif