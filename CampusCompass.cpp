#include "CampusCompass.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <climits>
using namespace std;

CampusCompass::CampusCompass() {}
bool CampusCompass::ParseCSV(const string& edges_filepath, const string& classes_filepath) { //loads data and takes both csv
    ifstream fileEdges(edges_filepath);
    if (!fileEdges.is_open()) return false;
    string lines;
    getline(fileEdges, lines);
    while (getline(fileEdges, lines)) {
        if (lines.empty()) continue;
        if (!lines.empty() && lines.back() == '\r') lines.pop_back();
        stringstream ss(lines);
        string part;
        vector<string> parts;
        while (getline(ss, part, ',')) { //to split using commas
            parts.push_back(part);
        }
        if (parts.size() < 5) continue;
        int origin      = stoi(parts[0]);
        int destination = stoi(parts[1]);
        int ETA         = stoi(parts[4]);
        nodes.insert(origin); //save both as nodes
        nodes.insert(destination);
        map[origin].push_back({destination, ETA, false});
        map[destination].push_back({origin, ETA, false});
    }
    fileEdges.close();
    ifstream fileClasses(classes_filepath);
    if (!fileClasses.is_open()) return false;
    getline(fileClasses, lines);
    while (getline(fileClasses, lines)) {
        if (lines.empty()) continue;
        if (!lines.empty() && lines.back() == '\r') lines.pop_back();
        stringstream ss(lines);
        string part;
        vector<string> parts;
        while (getline(ss, part, ',')) {
            parts.push_back(part);
        }
        if (parts.size() < 4) continue;
        Class course;
        course.code     = parts[0];
        course.building = stoi(parts[1]);
        course.begin    = parts[2];
        course.end      = parts[3];
        classes[course.code] = course;
    }
    fileClasses.close();
    return true;
}
bool CampusCompass::isValidName(const string& name) const {//just letters and spaces nothing else
    if (name.empty()) return false;
    for (char checkLetters : name) {
        if (!isalpha(checkLetters) && checkLetters != ' ') return false;
    }
    return true;
}
bool CampusCompass::isValidID(const string& id) const { //can only be 8 digits
    if (id.size() != 8) return false;
    for (char checkNumbers : id) {
        if (!isdigit(checkNumbers)) return false;
    }
    return true;
}
bool CampusCompass::isValidClassCode(const string& code) const { //only 3 uppercase and 4 numbers COT3100
    if (code.size() != 7) return false;
    for (int i = 0; i < 3; i++) {
        if (!isupper(code[i])) return false;
    }
    for (int i = 3; i < 7; i++) {
        if (!isdigit(code[i])) return false;
    }
    return true;
}
unordered_map<int, int> CampusCompass::dijkstra(int src) const { // no blocked roads and looks for fastest way possible
    unordered_map<int, int> distance;
    for (int n : nodes) distance[n] = INT_MAX;
    distance[src] = 0; // beggining
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> next; //closest is first always
    next.push({0, src});
    while (!next.empty()) {
        auto [currentTime, currentNode] = next.top();
        next.pop();
        if (currentTime > distance[currentNode]) continue;//better way here
        auto it = map.find(currentNode);
        if (it == map.end()) continue;
        for (const Edge& e : it->second) {
            if (e.block) continue;// here road is blcoked so its a skip
            int newETA = distance[currentNode] + e.ETA;
            if (newETA < distance[e.to]) {
                distance[e.to] = newETA;
                next.push({newETA, e.to});
            }
        }
    }
    return distance;
}
bool CampusCompass::bfs(int src, int dst) const { //to check if you can get there with open roads
    if (src == dst) return true;
    unordered_map<int, bool> seen;
    queue<int> nodesToVisit;
    nodesToVisit.push(src);
    seen[src] = true;
    while (!nodesToVisit.empty()) {
        int currentNode = nodesToVisit.front();
        nodesToVisit.pop();
        auto it = map.find(currentNode);
        if (it == map.end()) continue;
        for (const Edge& e : it->second) {
            if (e.block) continue;// if blocked skip
            if (e.to == dst) return true;
            if (!seen[e.to]) {
                seen[e.to] = true;
                nodesToVisit.push(e.to);
            }
        }
    }
    return false;// when there is no way to get there
}
vector<int> CampusCompass::shortesPath(int src, int dst) const {// dijkstra but also remembers the roads you take, and gives list of stops too
    unordered_map<int, int> distance;
    unordered_map<int, int> parent; //where you come from
    for (int n : nodes) {
        distance[n] = INT_MAX;
        parent[n]   = -1; //-1 is nowhere
    }
    distance[src] = 0;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> next;
    next.push({0, src});
    while (!next.empty()) {
        auto [currentTime, currentNode] = next.top();
        next.pop();
        if (currentTime > distance[currentNode]) continue;
        auto it = map.find(currentNode);
        if (it == map.end()) continue;
        for (const Edge& e : it->second) {
            if (e.block) continue;
            int newTime = distance[currentNode] + e.ETA;
            if (newTime < distance[e.to]) {
                distance[e.to] = newTime;
                parent[e.to] = currentNode;
                next.push({newTime, e.to});
            }
        }
    }
    if (distance[dst] == INT_MAX) return {}; //no way to get there
    vector<int> newPath; //go backwards from the end to the start then flip it
    for (int step = dst; step != -1; step = parent[step]) {
        newPath.push_back(step);
    }
    reverse(newPath.begin(), newPath.end());
    return newPath;
}
int CampusCompass::minSpantree(const set<int>& subNodes) const { //cheapest way to get to a place
    if (subNodes.empty()) return 0;
    unordered_map<int, vector<pair<int,int>>> localMap;// only use all the roads inbetween our group
    for (int currentNode : subNodes) {
        auto it = map.find(currentNode);
        if (it == map.end()) continue;
        for (const Edge& e : it->second) {
            if (!e.block && subNodes.count(e.to)) {
                localMap[currentNode].push_back({e.to, e.ETA});
            }
        }
    }
    unordered_map<int, bool> inTree; //get cheapest edge next
    unordered_map<int, int> best;
    for (int n : subNodes) {
        best[n]   = INT_MAX;
        inTree[n] = false;
    }
    int root = *subNodes.begin();
    best[root] = 0;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> next;
    next.push({0, root});
    int total = 0;
    while (!next.empty()) {
        auto [cost, currentNode] = next.top();
        next.pop();
        if (inTree[currentNode]) continue;
        inTree[currentNode] = true;
        total += cost;
        for (auto [neighbor, edgeCost] : localMap[currentNode]) {
            if (!inTree[neighbor] && edgeCost < best[neighbor]) {
                best[neighbor] = edgeCost;
                next.push({edgeCost, neighbor});
            }
        }
    }
    return total;
}
void CampusCompass::AddStudent(const string& line) {// gets a new student with dorm and class
    size_t start  = line.find('"'); //gets name from inside the quotes
    size_t finish = line.find('"', start + 1);
    if (start == string::npos || finish == string::npos) {
        cout << "unsuccessful\n";
        return;
    }
    string name = line.substr(start + 1, finish - start - 1);
    if (!isValidName(name)) {
        cout << "unsuccessful\n";
        return;
    }
    stringstream ss(line.substr(finish + 1));
    string studentID;
    int DormID, totalClasses;
    ss >> studentID >> DormID >> totalClasses;
    if (!isValidID(studentID)) {
        cout << "unsuccessful\n";
        return;
    }
    int ID = stoi(studentID);
    if (students.count(ID)) { //no two or more students with the same id
        cout << "unsuccessful\n";
        return;
    }
    if (totalClasses < 1 || totalClasses > 6) { //min 1 class max 6
        cout << "unsuccessful\n";
        return;
    }
    vector<string> schedule; // see the code for the class and checks it
    set<string> alreadyThere;// no duplicates
    for (int i = 0; i < totalClasses; i++) {
        string course;
        if (!(ss >> course)) {
            cout << "unsuccessful\n";
            return;
        }
        if (!isValidClassCode(course) || !classes.count(course)) {
            cout << "unsuccessful\n";
            return;
        }
        if (alreadyThere.count(course)) { // no two classes the same
            cout << "unsuccessful\n";
            return;
        }
        alreadyThere.insert(course);
        schedule.push_back(course);
    }
    Student newGator;
    newGator.name    = name;
    newGator.id      = ID;
    newGator.Dorm    = DormID;
    newGator.classes = schedule;
    students[ID]     = newGator;
    cout << "successful\n";
}
void CampusCompass::DelStudent(const string& line) { //looks for student by id and removes them
    stringstream ss(line);
    string cmd, studentID;
    ss >> cmd >> studentID;
    if (!isValidID(studentID)) {
        cout << "unsuccessful\n";
        return;
    }
    int ID = stoi(studentID);
    if (!students.count(ID)) {
        cout << "unsuccessful\n";
        return;
    }
    students.erase(ID);
    cout << "successful\n";
}
void CampusCompass::DropClass(const string& line) { //drops a class from the student
    stringstream ss(line);
    string cmd, studentID, course;
    ss >> cmd >> studentID >> course;
    if (!isValidID(studentID)) {
        cout << "unsuccessful\n";
        return;
    }
    int ID = stoi(studentID);
    if (!students.count(ID)) {
        cout << "unsuccessful\n";
        return;
    }
    if (!isValidClassCode(course) || !classes.count(course)) {
        cout << "unsuccessful\n";
        return;
    }
    Student& s = students[ID];
    auto it = find(s.classes.begin(), s.classes.end(), course);
    if (it == s.classes.end()) {
        cout << "unsuccessful\n";
        return;
    }
    s.classes.erase(it);
    if (s.classes.empty()) { // no class left = remove
        students.erase(ID);
    }
    cout << "successful\n";
}
void CampusCompass::ChangeClass(const string& line) { //drops a class for another class like in swap in one.uf
    stringstream ss(line);
    string cmd, studentID, currentClass, newClass;
    ss >> cmd >> studentID >> currentClass >> newClass;
    if (!isValidID(studentID)) {
        cout << "unsuccessful\n";
        return;
    }
    int ID = stoi(studentID);
    if (!students.count(ID)) {
        cout << "unsuccessful\n";
        return;
    }
    if (!isValidClassCode(currentClass) || !isValidClassCode(newClass)) {
        cout << "unsuccessful\n";
        return;
    }
    if (!classes.count(newClass)) { //class needs to be offered by uf (exist)
        cout << "unsuccessful\n";
        return;
    }
    Student& s = students[ID];
    auto it = find(s.classes.begin(), s.classes.end(), currentClass); // to swap a class you need to have a preexisting one
    if (it == s.classes.end()) {
        cout << "unsuccessful\n";
        return;
    }
    if (find(s.classes.begin(), s.classes.end(), newClass) != s.classes.end()) { // you cant rpelace a class with the same
        cout << "unsuccessful\n";
        return;
    }
    *it = newClass;
    cout << "successful\n";
}
void CampusCompass::DelClass(const string& line) {// drops a class from all students
    stringstream ss(line);
    string cmd, course;
    ss >> cmd >> course;
    if (!isValidClassCode(course)) {
        cout << "unsuccessful\n";
        return;
    }
    if (!classes.count(course)) {
        cout << "unsuccessful\n";
        return;
    }
    int totalStudents = 0;
    vector<int> emptyStudents; //students with no classes
    for (auto& [ID, s] : students) {
        auto it = find(s.classes.begin(), s.classes.end(), course);
        if (it != s.classes.end()) {
            s.classes.erase(it);
            totalStudents++;
            if (s.classes.empty()) {
                emptyStudents.push_back(ID);
            }
        }
    }
    if (totalStudents == 0) {
        cout << "unsuccessful\n";
        return;
    }
    for (int ID : emptyStudents) { //removes students with no class
        students.erase(ID);
    }
    cout << totalStudents << "\n";
}
void CampusCompass::ToggleRoads(const string& line) {// to open or close roads
    stringstream ss(line);
    string cmd;
    int totalRoads;
    ss >> cmd >> totalRoads;
    for (int i = 0; i < totalRoads; i++) {
        int from, to;
        ss >> from >> to;
        auto it = map.find(from); // TFF from from to to
        if (it != map.end()) {
            for (Edge& e : it->second) {
                if (e.to == to) { e.block = !e.block; break; }
            }
        }
        auto it2 = map.find(to);// TFF from to to from
        if (it2 != map.end()) {
            for (Edge& e : it2->second) {
                if (e.to == from) { e.block = !e.block; break; }
            }
        }
    }
    cout << "successful\n";
}
void CampusCompass::RoadStatus(const string& line) { //to see if road is open closed or dne
    stringstream ss(line);
    string cmd;
    int from, to;
    ss >> cmd >> from >> to;
    auto it = map.find(from);
    if (it == map.end()) {
        cout << "DNE\n";
        return;
    }
    for (const Edge& e : it->second) {
        if (e.to == to) {
            cout << (e.block ? "closed" : "open") << "\n";
            return;
        }
    }
    cout << "DNE\n";
}
void CampusCompass::Connection(const string& line) {// checks if possible to go from a to b
    stringstream ss(line);
    string cmd;
    int from, to;
    ss >> cmd >> from >> to;
    cout << (bfs(from, to) ? "successful" : "unsuccessful") << "\n";
}
void CampusCompass::ShortPath(const string& line) { //prints how long to walk to all classes
    stringstream ss(line);
    string cmd, studentID;
    ss >> cmd >> studentID;
    int ID = stoi(studentID);
    const Student& s = students[ID];
    auto distance = dijkstra(s.Dorm);// runs dijkstra from the dorm
    vector<string> classByOrder = s.classes;//to sort them a to z
    sort(classByOrder.begin(), classByOrder.end());
    cout << "Time For Shortest Edges: " << s.name << "\n";
    for (const string& course : classByOrder) {
        int buildingRoom = classes[course].building;
        int ETA = (distance.count(buildingRoom) && distance[buildingRoom] != INT_MAX) ? distance[buildingRoom] : -1;
        cout << course << ": " << ETA << "\n";
    }
    cout.flush();
}
void CampusCompass::PrintZone(const string& line) { // looks for the cheapest road from dorm to all classes
    stringstream ss(line);
    string cmd, studentID;
    ss >> cmd >> studentID;
    int ID = stoi(studentID);
    const Student& s = students[ID];
    set<int> stops;// all nodes from all shortest path
    for (const string& course : s.classes) {
        int end = classes[course].building;
        vector<int> route = shortesPath(s.Dorm, end);
        for (int n : route) {
            stops.insert(n);
        }
    }
    int total = minSpantree(stops);// runs MST on nodes
    cout << "Student Zone Cost For " << s.name << ": " << total << "\n";
    cout.flush();
}
void CampusCompass::CheckSchedule(const string& line) { // looks if the student can go from calss to class
    stringstream ss(line);
    string cmd, studentID;
    ss >> cmd >> studentID;
    int ID = stoi(studentID);
    const Student& s = students[ID];
    if (s.classes.size() <= 1) {// min 2 classes to compare
        cout << "unsuccessful\n";
        return;
    }
    vector<Class> classByOrder;// sort from earliest class to latest
    for (const string& course : s.classes) {
        classByOrder.push_back(classes[course]);
    }
    sort(classByOrder.begin(), classByOrder.end(), [](const Class& a, const Class& b) {
        return a.begin < b.begin;
    });
    auto toMinutes = [](const string& t) -> int {// to make them in min
        int h = stoi(t.substr(0, 2));
        int m = stoi(t.substr(3, 2));
        return h * 60 + m;
    };
    cout << "Schedule Check for " << s.name << ":\n";
    for (int i = 0; i + 1 < (int)classByOrder.size(); i++) {
        int timeBetweenClasses = toMinutes(classByOrder[i+1].begin) - toMinutes(classByOrder[i].end);
        auto distance          = dijkstra(classByOrder[i].building);
        int ETA                = (distance.count(classByOrder[i+1].building)) ? distance[classByOrder[i+1].building] : INT_MAX;
        bool doableInTime      = (ETA != INT_MAX && timeBetweenClasses >= ETA);
        cout << classByOrder[i].code << " - " << classByOrder[i+1].code
             << ": " << (doableInTime ? "successful" : "unsuccessful") << "\n";
    }
    cout.flush();
}
bool CampusCompass::ParseCommand(const string& command) {// reads the first word to see which function
    if (command.empty()) {
        cout << "unsuccessful\n";
        return false;
    }
    string cleaning = command; //deletes unnecesary spaces
    size_t start    = cleaning.find_first_not_of(" \t\r\n");
    size_t finish   = cleaning.find_last_not_of(" \t\r\n");
    if (start == string::npos) {
        cout << "unsuccessful\n";
        return false;
    }
    cleaning = cleaning.substr(start, finish - start + 1);
    stringstream ss(cleaning);
    string cmd;
    ss >> cmd;
    if      (cmd == "insert")             AddStudent(cleaning);
    else if (cmd == "remove")             DelStudent(cleaning);
    else if (cmd == "dropClass")          DropClass(cleaning);
    else if (cmd == "replaceClass")       ChangeClass(cleaning);
    else if (cmd == "removeClass")        DelClass(cleaning);
    else if (cmd == "toggleEdgesClosure") ToggleRoads(cleaning);
    else if (cmd == "checkEdgeStatus")    RoadStatus(cleaning);
    else if (cmd == "isConnected")        Connection(cleaning);
    else if (cmd == "printShortestEdges") ShortPath(cleaning);
    else if (cmd == "printStudentZone")   PrintZone(cleaning);
    else if (cmd == "verifySchedule")     CheckSchedule(cleaning);
    else {
        cout << "unsuccessful\n";
        return false;
    }
    return true;
}