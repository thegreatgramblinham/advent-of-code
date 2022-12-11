#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>

using namespace std;

class Monke
{
    private:
        const int CONST_ITEM_DIVISOR = 3;

        size_t _inspectionsdone = 0;

        void inspectitem(size_t& item)
        {
            const string ADDITION_OPERATOR = "+ ";
            const string MULTIPLICATION_OPERATOR = "* ";

            if (operationstr.substr(0, ADDITION_OPERATOR.length()) == ADDITION_OPERATOR)
            {
                size_t param = parseoperatorparameter(operationstr.substr(ADDITION_OPERATOR.length()), item);
                cout << ">Inspecting item \'"+to_string(item)+"\' to ADD \'"+to_string(param)+"]\'"<<endl;
                item += param;
            }
            else if (operationstr.substr(0, MULTIPLICATION_OPERATOR.length()) == MULTIPLICATION_OPERATOR)
            {
                size_t param = parseoperatorparameter(operationstr.substr(MULTIPLICATION_OPERATOR.length()), item);
                cout << ">Inspecting item \'"+to_string(item)+"\' to MULTBY \'"+to_string(param)+"]\'"<<endl;
                item *= param;
            }
            else
            {
                throw std::invalid_argument("Unexpected starting character in operation string:\'"+operationstr+"\'");
            }
            _inspectionsdone++;
        }

        size_t parseoperatorparameter(string operparamstr, size_t curritemvalue)
        {
            const string OLD_VARIABLE = "old";
            if(operparamstr.substr(0, OLD_VARIABLE.length()) == OLD_VARIABLE)
            {
                //cout << ">Parsed \'"+operparamstr+"\' as operator parameter." << endl;
                return curritemvalue;
            }
            else
            {
                //cout << ">Attempting to parse \'"+operparamstr+"\' as operator parameter." << endl;
                return stoul(operparamstr);
            }
        }

        bool testitem(size_t item)
        {
            return item%testdivisor==0;
        }

    public:
        list<size_t> itemqueue;
        string operationstr;
        size_t testdivisor;

        int truemonkekey;
        int falsemonkekey;

        void processturn(map<int, Monke>& monkemap, size_t currentmonke)
        {
            cout<<">==Starting monke["+to_string(currentmonke)+"] turn.=="<<endl;
            while (!itemqueue.empty())
            {
                size_t item = itemqueue.front();
                itemqueue.pop_front();

                inspectitem(item);
                cout<<">Dividing item ("+to_string(item)+") by constant \'"+to_string(CONST_ITEM_DIVISOR)+"\' ";
                item /= CONST_ITEM_DIVISOR;
                cout<<"to get \'"+to_string(item)+"\', rounded down."<<endl;
                bool result = testitem(item);

                if (result)
                {
                    const map<int, Monke>::iterator& truemonkeit = monkemap.find(truemonkekey);
                    if (truemonkeit == monkemap.end())
                        throw std::out_of_range("True monke not contained within map.");

                    cout<<">[True] condition met. Item is divisible by "+to_string(testdivisor)
                        +". Passing item to monke["+to_string(truemonkekey)+"]"<<endl;
                    truemonkeit->second.itemqueue.push_back(item);
                }
                else
                {
                    const map<int, Monke>::iterator& falsemonkeit = monkemap.find(falsemonkekey);
                    if (falsemonkeit == monkemap.end())
                        throw std::out_of_range("False monke not contained within map.");

                    cout<<">[False] condition met. Item is not divisible by "+to_string(testdivisor)
                        +". Passing item to monke["+to_string(falsemonkekey)+"]"<<endl;
                    falsemonkeit->second.itemqueue.push_back(item);
                }
            }
        }

        size_t get_inspection_count()
        {
            return _inspectionsdone;
        }
};

class MonkeParser
{
    private:
        static string trim(string str)
        {
            size_t first = str.find_first_not_of(' ');

            // Handle the empty/whitespace string.
            if (first == string::npos)
                return "";

            size_t last = str.find_last_not_of(' ');
            return str.substr(first, (last-first+1));
        }

        static string remove_prefix_by_colon(string str)
        {
            const string VALUE_SEPARATOR = ":";

            size_t valuesepindex = str.find(VALUE_SEPARATOR);
            if (valuesepindex == string::npos)
                throw std::invalid_argument("String not in expected form.");

            string strnoprefix = str.substr(valuesepindex+1);
            strnoprefix = trim(strnoprefix);
            return strnoprefix;
        }

    public:
        static map<int, Monke> parse_monkques(string inputfile)
        {
            fstream inputfilestream;
            inputfilestream.open(inputfile, ios::in);
            if (!inputfilestream.is_open())
            {
                throw std::ios_base::failure("Could not open input file");
            }

            const string VALUE_SEPARATOR = ":";
            const string NEW_MONKE_PREFIX = "Monkey";
            const string STARTING_ITEMS_PREFIX = "Starting items:";
            const string OPERATION_PREFIX = "Operation: new = old ";
            const string TEST_PREFIX = "Test: divisible by ";
            const string IF_TRUE_PREFIX = "If true: throw to monkey ";
            const string IF_FALSE_PREFIX = "If false: throw to monkey ";

            map<int, Monke> returnmap;

            int currmonkeindex = -1;
            Monke* currmonke;

            string currline;
            while (getline(inputfilestream, currline))
            {
                currline = trim(currline);

                if (currline.empty())
                    continue;

                // Monkey
                if (currline.substr(0, NEW_MONKE_PREFIX.length()) == NEW_MONKE_PREFIX)
                {
                    if (currmonkeindex >= 0)
                    {
                        cout<<">Parsed monke["+to_string(currmonkeindex)+"]"<<endl;
                        returnmap.insert(pair<int, Monke>(currmonkeindex, *currmonke));
                    }

                    Monke newmonke;
                    currmonke = &newmonke;
                    currmonkeindex++;
                    continue;
                }
                // Starting items
                else if (currline.substr(0, STARTING_ITEMS_PREFIX.length()) == STARTING_ITEMS_PREFIX)
                {
                    string itemstr = remove_prefix_by_colon(currline);

                    const string ITEM_SEPARATOR = ",";
                    size_t nextSep = itemstr.find(ITEM_SEPARATOR);
                    while (nextSep != string::npos)
                    {
                        string singleitemstr = itemstr.substr(0, nextSep);
                        cout << ">Attempting to parse \'"+singleitemstr+"\' as item." << endl;
                        size_t item = stoul(singleitemstr);
                        (*currmonke).itemqueue.push_back(item);
                        itemstr = itemstr.substr(nextSep+2);
                        nextSep = itemstr.find(ITEM_SEPARATOR);
                    }

                    //Record our last item (or if there was only one item)
                    cout << ">Attempting to parse \'"+itemstr+"\' as item." << endl;
                    size_t item = stoul(itemstr);
                    (*currmonke).itemqueue.push_back(item);
                }
                // Operation
                else if (currline.substr(0, OPERATION_PREFIX.length()) == OPERATION_PREFIX)
                {
                    string operationstr = currline.substr(OPERATION_PREFIX.length());
                    (*currmonke).operationstr = operationstr;
                }
                // Test
                else if (currline.substr(0, TEST_PREFIX.length()) == TEST_PREFIX)
                {
                    string teststr = currline.substr(TEST_PREFIX.length());
                    cout << ">Attempting to parse \'"+teststr+"\' as test divisor." << endl;
                    (*currmonke).testdivisor = stoul(teststr);
                }
                // True Monke
                else if (currline.substr(0, IF_TRUE_PREFIX.length()) == IF_TRUE_PREFIX)
                {
                    string truestr = currline.substr(IF_TRUE_PREFIX.length());
                    cout << ">Attempting to parse \'"+truestr+"\' as true monke key." << endl;
                    (*currmonke).truemonkekey = stoi(truestr);
                }
                // False Monke
                else if (currline.substr(0, IF_FALSE_PREFIX.length()) == IF_FALSE_PREFIX)
                {
                    string falsestr = currline.substr(IF_FALSE_PREFIX.length());
                    cout << ">Attempting to parse \'"+falsestr+"\' as false monke key." << endl;
                    (*currmonke).falsemonkekey = stoi(falsestr);
                }
            }

            // Insert the final monke
            cout<<">Parsed monke["+to_string(currmonkeindex)+"]"<<endl;
            returnmap.insert(pair<int, Monke>(currmonkeindex, *currmonke));
            return returnmap;
        }
};

class MonkeRoundManager
{
    private:
        static void printmonkestate(size_t roundnumber, map<int, Monke>& monkestate)
        {
            cout<<">[At the end of round "+to_string(roundnumber)+"]"<<endl;
            for (size_t i = 0; i < monkestate.size(); i++)
            {
                map<int, Monke>::iterator monkeit = monkestate.find(i);
                if (monkeit == monkestate.end())
                    throw std::invalid_argument("Map does not contain expect monke at index: "+to_string(i));
                cout<<">> Monke["+to_string(i)+"] holds: "+longlisttostring(monkeit->second.itemqueue)<<endl;
            }
        }

        static string longlisttostring(list<size_t> lst)
        {
            const string SEPARATOR = ", ";
            string returnstr;
            for(const size_t& item : lst)
            {
                returnstr += to_string(item)+SEPARATOR;
            }
            return returnstr.substr(0, returnstr.length()-2);
        }

    public:
        static void run(map<int, Monke>& monkquestoprocess, size_t roundcnt)
        {
            for (size_t currround = 0; currround < roundcnt; currround++)
            {
                for (size_t i = 0; i < monkquestoprocess.size(); i++)
                {
                    map<int, Monke>::iterator monkeit = monkquestoprocess.find(i);
                    if (monkeit == monkquestoprocess.end())
                        throw std::invalid_argument("Map does not contain expect monke at index: "+to_string(i));
                    monkeit->second.processturn(monkquestoprocess, i);
                }
                printmonkestate(currround+1, monkquestoprocess);
            }

            cout<<">>All \'"+to_string(roundcnt)+"\' rounds complete."<<endl;
            list<size_t> inspections;
            for (size_t i = 0; i < monkquestoprocess.size(); i++)
            {
                map<int, Monke>::iterator monkeit = monkquestoprocess.find(i);
                if (monkeit == monkquestoprocess.end())
                    throw std::invalid_argument("Map does not contain expect monke at index: "+to_string(i));

                size_t inspectioncount = monkeit->second.get_inspection_count();
                cout<<">> Monke["+to_string(i)+"] inspected items \'"+to_string(inspectioncount)+"\' times."<<endl;
                inspections.push_back(inspectioncount);
            }

            inspections.sort();
            const size_t MONKE_BIZ_FACTOR = 2;
            size_t totalbiz = 1;
            for (size_t i = 0; i < MONKE_BIZ_FACTOR; i++)
            {
                size_t largestcnt = inspections.back();
                totalbiz*=largestcnt;
                inspections.pop_back();
            }
            cout<<">>Total monkey business is: "+to_string(totalbiz)<<endl;
        }
};

int main()
{
    const string INPUT_FILE = "monkey_rules.txt";
    map<int, Monke> monkquemap = MonkeParser::parse_monkques(INPUT_FILE);
    cout << ">Completed monke parse with \'"+to_string(monkquemap.size())+"\' monkques." << endl;

    cout<<">Begining keep away game."<<endl;
    const size_t ROUNDS = 20;
    MonkeRoundManager::run(monkquemap, ROUNDS);
}

