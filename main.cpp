#include <iostream>
#include "CampusCompass.h"

using namespace std;

int main() {
    // initialize your main project object
    CampusCompass compass;

    // ingest CSV data
    compass.ParseCSV("data/edges.csv", "data/classes.csv");

    // read number of commands then process each one
    int no_of_lines;
    string command;
    cin >> no_of_lines;
    cin.ignore(); //ignore newline that first cin left over

    for (int i = 0; i < no_of_lines; i++) {
        getline(cin, command);
        compass.ParseCommand(command);
    }
    cout << endl;
    cout.flush();
    return 0;
}