#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

using namespace std;

struct Edge {
    int to; //your destinaiton
    int ETA; //time left to get there
    bool block;// road blocked Y or N
};
struct Student {
    string name;
    int id;
    int Dorm;// where they are or live (dorm)
    vector<string> classes;//the classes taken
};

struct Class {
    string code;// like COT3100
    int building;//which classroom
    string begin;
    string end;
};

class CampusCompass {

private:
    unordered_map<int, vector<Edge>> map; // to see all posible ways from one point
    set<int> nodes;// all the ids on the graph
    unordered_map<int, Student> students; // each student looked from the IDs
    unordered_map<string, Class> classes;// each class by the code

    bool isValidName(const string& name) const; // for names ot only be letters and space
    bool isValidID(const string& id) const; // ID only 8 digits long "albert 12345678"
    bool isValidClassCode(const string& code) const;// class code 3 upper and 4 num COT3100

    unordered_map<int, int> dijkstra(int src) const; //dijkstra is for short time in open roads
    bool bfs(int src, int dst) const; // see if you can go in the open roads
    vector<int> shortesPath(int src, int dst) const;
    int minSpantree(const set<int>& subNodes) const;

    void AddStudent(const string& line);
    void DelStudent(const string& line);
    void DropClass(const string& line);
    void ChangeClass(const string& line);
    void DelClass(const string& line);
    void ToggleRoads(const string& line);
    void RoadStatus(const string& line);
    void Connection(const string& line);
    void ShortPath(const string& line);
    void PrintZone(const string& line);
    void CheckSchedule(const string& line);

public:
    CampusCompass(); //empty
    bool ParseCSV(const string& edges_filepath, const string& classes_filepath);// loads the csv
    bool ParseCommand(const string& command);
};