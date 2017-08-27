#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <functional>

#include <SHAUN/parser.hpp>
#include <SHAUN/printer.hpp>

using namespace std;

bool test1()
{
    try
    {
        // Parse file
        shaun::parser p;
        ifstream file("../resources/test.shaun");
        stringstream buffer; 
        buffer << file.rdbuf();
        shaun::object obj1 = p.parse(buffer.str());

        // Generate shaun file
        stringstream output1;
        shaun::printer pr1(&output1);
        pr1.visit(obj1);

        // Parse generated file
        shaun::object obj2 = p.parse(output1.str());
        // Regenerate shaun file
        stringstream output2;
        shaun::printer pr2(&output2);
        pr2.visit(obj2);

        // Compare both files
        return output1.str() == output2.str();
    }
    catch (shaun::parse_error e)
    {
        std::cout << e << std::endl;
        return false;
    }
}

#define MAKE_TEST(name) (UnitTest(#name, name))

class UnitTest
{
public:
    UnitTest(const string &name, const function<bool(void)> &test) : _name{name}, _test{test} {}
    const string &getName() const {return _name;}
    const function<bool(void)> &getTest() const {return _test;}
private:
    string _name;
    function<bool(void)> _test;
};

int main(void)
{
    const vector<UnitTest> unitTests = {
        MAKE_TEST(test1)
    };

    int failedNum = 0;
    for (const auto &test : unitTests)
    {
        const bool result = test.getTest()();
        cout << "Test " << test.getName() << (result?" successful":" failed") << endl;
        failedNum += result?0:1;
    }

    cout << "-----------------------" << endl;
    if (failedNum)
    {
        cout << failedNum << " tests failed" << endl;
    }
    else
    {
        cout << "All tests successful" << endl;
    }
}
