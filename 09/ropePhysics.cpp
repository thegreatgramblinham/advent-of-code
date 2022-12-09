#include <iostream>
#include <fstream>
#include <string>
#include <set>

using namespace std;

// Puzzle 1 solution
class TailLocationCalculator
{
    public:
        set<int> calc_all_tail_locations(string inputfile)
        {
            fstream headmoveinstructions;
            headmoveinstructions.open(inputfile, ios::in);

            if (!headmoveinstructions.is_open())
            {
                throw std::ios_base::failure("Could not open input file.");
            }

            int linecount = 0;
            string currentLine;
            while(getline(headmoveinstructions, currentLine))
            {

            }

            cout << ">Processed \'"+to_string(linecount)+"\' input lines.";
            headmoveinstructions.close();
        }
};

int main()
{

}