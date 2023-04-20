#include "iostream"
#include "utility"

using namespace std;

/**
 * @brief Utility fuction for displaying output
 * @param msg {output in string}
 */
void displayInConsole(string msg)
{
    cout << msg << endl;
}

/**
 * @brief Utility fuction for validating the service type
 * @param type {service no}
 * @param limit {available services limit}
 * @return true {valid}
 * @return false {invalid}
 */
bool validateTransactionType(int type, int limit)
{
    if (type <= 0)
        return false;
    return type <= limit;
}

/**
 * @brief Utility fuction for validating numerical input to the program
 * @param inputVal {integer}
 * @return true {valid}
 * @return false {invalid}
 */
bool validateInput(int inputVal)
{
    return inputVal > 0;
}