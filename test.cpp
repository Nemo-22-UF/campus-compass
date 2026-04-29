// #include <catch2/catch_test_macros.hpp>
// #include <iostream>
// #include <sstream>
// #include "CampusCompass.h"
// using namespace std;
// string runCommands(CampusCompass& c, vector<string> cmds) { //do the commands and gives all that was printed
//     ostringstream buffer;
//     streambuf* oldCout = cout.rdbuf(buffer.rdbuf()); //sends the output to buffer
//     for (const string& cmd : cmds) {
//         c.ParseCommand(cmd);
//     }
//     cout.rdbuf(oldCout); //lets output back to normal
//     return buffer.str();
// }
// TEST_CASE("bad commands print unsuccessful", "[invalid]") {//test 1 all bad commands print unsuccessful
//     CampusCompass c;
//     c.ParseCSV("data/edges.csv", "data/classes.csv");
//     REQUIRE(runCommands(c, {"insert \"A11y\" 45679999 1 1 COP3530"}) == "unsuccessful\n");//name has a number in it
//     REQUIRE(runCommands(c, {"insert \"Alice\" 1234567 1 1 COP3530"}) == "unsuccessful\n");//id is only 7 digits needs 8
//     REQUIRE(runCommands(c, {"fly 10000001"}) == "unsuccessful\n");//totally unknown command
//     REQUIRE(runCommands(c, {"insert \"Bob\" 12345678 1 1 cop3530"}) == "unsuccessful\n");//lowercase class code not allowed
//     REQUIRE(runCommands(c, {"insert \"Carol\" 87654321 1 2 COP3530"}) == "unsuccessful\n"); //says 2 classes but only gave 1
//     REQUIRE(runCommands(c, {"insert \"Eve\" 22222222 1 2 COP3530 COP3530"}) == "unsuccessful\n");//same class twice not allowed
// }
// TEST_CASE("edge cases", "[edge]") {// test 2 all the edge cases that could make the code fail
//     CampusCompass c;
//     c.ParseCSV("data/edges.csv", "data/classes.csv");
//     REQUIRE(runCommands(c, {"remove 99999999"}) == "unsuccessful\n");// if not added remove him
//     REQUIRE(runCommands(c, {"dropClass 99999999 COP3530"}) == "unsuccessful\n");// dropping class from a perosn who is not n UF
//     runCommands(c, {"insert \"Alice\" 10000001 1 1 COP3530"});// if adding a student twice
//     REQUIRE(runCommands(c, {"insert \"Alice\" 10000001 1 1 COP3530"}) == "unsuccessful\n");
//     REQUIRE(runCommands(c, {"checkEdgeStatus 1 99"}) == "DNE\n");// if road dne
// }
// TEST_CASE("dropClass removeClass remove replaceClass", "[class_mgmt]") {// test 3 doing more tests on dropclass, remoce class
//     CampusCompass c;
//     c.ParseCSV("data/edges.csv", "data/classes.csv");
//     runCommands(c, {
//         "insert \"Brandon\" 45679999 20 2 COP3530 MAC2311",
//         "insert \"Brian\" 35459999 21 3 COP3530 CDA3101 MAC2311"
//     });
//     REQUIRE(runCommands(c, {"dropClass 45679999 COP3530"}) == "successful\n");// drop a class from brandon
//     REQUIRE(runCommands(c, {"replaceClass 45679999 MAC2311 MAC2312"}) == "successful\n");// replace a class
//     REQUIRE(runCommands(c, {"removeClass COP3530"}) == "1\n"); // only he has a the class so it sould only be 1
//     REQUIRE(runCommands(c, {"remove 35459999"}) == "successful\n");// removing
//     REQUIRE(runCommands(c, {"remove 35459999"}) == "unsuccessful\n");//if removing again it shoudl failed w the same person
// }
// TEST_CASE("printShortestEdges road gets blocked", "[shortest]") { // test 4 prints when road close first the class is reachable then we block all roads from the dorm
//     CampusCompass c;
//     c.ParseCSV("data/edges.csv", "data/classes.csv");
//     runCommands(c, {"insert \"Natalie\" 20000001 1 1 COP3530"});// studnet lives at node 1 and the class was at node 14
//     ostringstream buf1;
//     streambuf* old1 = cout.rdbuf(buf1.rdbuf());
//     c.ParseCommand("printShortestEdges 20000001");// should be possible so there should not be no -1
//     cout.rdbuf(old1);
//     REQUIRE(buf1.str().find("-1") == string::npos);
//     runCommands(c, {"toggleEdgesClosure 3 1 2 1 4 1 50"});// closes all roads to remain in node 1
//     ostringstream buf2;
//     streambuf* old2 = cout.rdbuf(buf2.rdbuf());
//     c.ParseCommand("printShortestEdges 20000001"); // now the class is not possible so it should say -1
//     cout.rdbuf(old2);
//     REQUIRE(buf2.str().find("-1") != string::npos);
// }