#include <iostream>
#include <fstream>
#include <string>
#include <set>

using namespace std;

class CommandProcessor
{
    private:
        const string NO_OPERATION_COMMAND = "noop";
        const string ADD_X_COMMAND = "addx";

        size_t _cycle;
        int _xregister;
        fstream _commandinputstream;

        set<int>* _cyclesofinterest;
        int _cyclesofinterestsum;

        const int CRT_ROW_PIXELS = 40;
        const int CRT_COL_PIXELS = 6;
        char _crt[40][6];

        void executeaddx(int registermodifier)
        {
            const int CYCLE_COST = 2;
            for (size_t i = 0; i < CYCLE_COST; i++)
            {
                cout << ">Cycle["+to_string(_cycle+1)+"] begin, xregister=\'"+to_string(_xregister)+"\'" << endl;
                checkinterestcycle();
                checkcrtcrossover();

                if (i+1 == CYCLE_COST)
                    _xregister+=registermodifier;

                cout << ">Cycle["+to_string(_cycle+1)+"] complete, xregister=\'"+to_string(_xregister)+"\'" << endl;
                _cycle++;
            }
        }

        void executenoop()
        {
            cout << ">Cycle["+to_string(_cycle+1)+"] begin, xregister=\'"+to_string(_xregister)+"\'" << endl;
            checkinterestcycle();
            checkcrtcrossover();
            cout << ">Cycle["+to_string(_cycle+1)+"] complete, xregister=\'"+to_string(_xregister)+"\'" << endl;
            _cycle++;
        }

        void checkinterestcycle()
        {
            set<int>& cycleset = *_cyclesofinterest;
            if (cycleset.find(_cycle+1) != cycleset.end())
            {
                cout << ">Cycle["+to_string(_cycle+1)+"] is a cycle of iterest. Adding xregister=\'"+to_string(_xregister)+"\'" << endl;
                _cyclesofinterestsum += _xregister * (_cycle+1);
            }
        }

        bool iswithinone(int x, int y)
        {
            return abs(x-y) <= 1;
        }

        void checkcrtcrossover()
        {
            int row = (_cycle)%CRT_ROW_PIXELS;
            int col = (_cycle)/CRT_ROW_PIXELS;

            if (iswithinone(_xregister, row))
            {
                _crt[row][col] = '#';
            }
            else
            {
                _crt[row][col] = '.';
            }
        }

        void runloop(string inputfile)
        {
            fstream _commandinputstream;
            _commandinputstream.open(inputfile, ios::in);
            if(!_commandinputstream.is_open())
            {
                throw std::ios_base::failure("Could not open input file.");
            }
            cout << ">Opened input file: "+inputfile << endl;

            string currentcommand;
            while(getline(_commandinputstream, currentcommand))
            {
                if (currentcommand.substr(0, ADD_X_COMMAND.length()) == ADD_X_COMMAND)
                {
                    const int addxend = ADD_X_COMMAND.length()+1;
                    string modifierstr = currentcommand.substr(addxend, currentcommand.length()-addxend);
                    int registermodifier = stoi(modifierstr);
                    cout << ">Read addx\'"+to_string(registermodifier)+"\' command" << endl;
                    executeaddx(registermodifier);

                }
                else if (currentcommand.substr(0, NO_OPERATION_COMMAND.length()) == NO_OPERATION_COMMAND)
                {
                    cout << ">Read noop command" << endl;
                    executenoop();
                }
            }
        }

        void printcrtdisplay()
        {
            for (size_t i = 0; i < CRT_COL_PIXELS; i++)
            {
                string crtline;
                for (size_t j = 0; j < CRT_ROW_PIXELS; j++)
                {
                    crtline += _crt[j][i];
                }
                cout << crtline << endl;
            }
        }

    public:
        void run(string inputfile, set<int>& cyclesofinterest)
        {
            _cycle = 0;
            _xregister = 1;
            _cyclesofinterestsum = 0;
            _cyclesofinterest = &cyclesofinterest;
            runloop(inputfile);
            cout << ">Cycles of interest sum: "+to_string(_cyclesofinterestsum) << endl;
            printcrtdisplay();
        }
};

int main()
{
    const string INPUT_FILE = "command_input.txt";
    CommandProcessor proccessor;
    set<int> cyclestosum = {20, 60, 100, 140, 180, 220};
    proccessor.run(INPUT_FILE, cyclestosum);
}