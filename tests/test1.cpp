#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <functional>

#include <SHAUN/parser.hpp>
#include <SHAUN/printer.hpp>
#include <SHAUN/sweeper.hpp>

using namespace std;

bool parsing_ok()
{
    try
    {
        // Parse file
        shaun::object obj1 = shaun::parse_file("../resources/test_tangente.shaun");

        // Generate shaun file
        stringstream output1;
        shaun::printer pr1(&output1);
        pr1.visit(obj1);

        // Parse generated file
        shaun::object obj2 = shaun::parse(output1.str());
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

bool object_primitive_attributes()
{
    shaun::object o;
    o.add("x", 10);
    o.add("y", 10.0);
    o.add("b", true);
    o.add("s", "hello");

    ofstream file("../resources/test2_out.shaun");
    shaun::printer pr(&file);
    pr.visit(o);
    file.close();

    try
    {
        shaun::object o = shaun::parse_file("../resources/test2_out.shaun");
        return o.get<shaun::string>("s") == "hello";
    }
    catch (shaun::parse_error e)
    {
        std::cout << e << std::endl;
        return false;
    }
    catch (...)
    {
      return false;
    }
}

bool get_with_default()
{
    shaun::object o;
    o.add("x", 10);
    o.add("y", shaun::null());

    return o.get_with_default<std::string>("default", "monattribut") == "default" 
        && (double)o.get_with_default(0, "x") == 10
        && (double)o.get_with_default(58, "y") == 58;
}

bool list_push_back_prim()
{
    shaun::list l;
    l.push_back(10);
    l.push_back(10.0);
    l.push_back("hello there");
    l.push_back(std::string("how are you ?"));
    l.push_back(true);

    return l[0].type() == shaun::Type::number
        && l[1].type() == shaun::Type::number
        && l[2].type() == shaun::Type::string
        && l[3].type() == shaun::Type::string
        && l[4].type() == shaun::Type::boolean;
}

bool sweeper_ok()
{
    shaun::list l;
    l.push_back(10);
    l.push_back(10.0);
    l.push_back("hello there");
    l.push_back(std::string("how are you ?"));
    l.push_back(true);

    shaun::object obj;
    obj.add("list", l);

    try
    {
        shaun::sweeper sw(obj);
        bool isValid = !sw.is_null();
        bool subSweepValid = !sw("list").is_null();
        bool typeValid = sw("list")[0].type() == shaun::Type::number;
        bool valueValid = /*(shaun::string)*/sw("list")[2].value<shaun::string>() == "hello there";
        return isValid && subSweepValid && typeValid && valueValid;
    }
    catch (shaun::parse_error e)
    {
        std::cout << e << std::endl;
        return false;
    }
    catch (...)
    {
      return false;
    }
}

bool null_parsing_ok()
{
  try
  {
    shaun::object o = shaun::parse("{ attr: null}");
    return o.get<shaun::shaun>("attr").type() == shaun::Type::null;
  }
  catch (shaun::parse_error e)
  {
    std::cout << e << std::endl;
    return false;
  }
  catch (...)
  {
    return false;
  }
}

bool sweeper_with_default()
{
  try
  {
    shaun::object o;
    o.add("x", 10);
    o.add("y", true);
    shaun::sweeper sw(o);

    return sw("x").with_default<int>(5) == 10
        && sw("non").with_default<std::string>("yeah") == "yeah"
        && sw("y").with_default<bool>(false) == true
        && sw("x").with_default<std::string>("hello") == "hello";
  }
  catch (shaun::exception& e)
  {
    std::cout << e << std::endl;
    return false;
  }
  catch (...)
  {
    std::cout << "exception occured\n";
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
        MAKE_TEST(parsing_ok),
        MAKE_TEST(object_primitive_attributes),
        MAKE_TEST(get_with_default),
        MAKE_TEST(list_push_back_prim),
        MAKE_TEST(sweeper_ok),
        MAKE_TEST(null_parsing_ok),
        MAKE_TEST(sweeper_with_default)
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
        cout << failedNum << " " << ((failedNum>1)?"tests":"test") << " failed" << endl;
    }
    else
    {
        cout << "All tests successful" << endl;
    }
}
