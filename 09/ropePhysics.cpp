#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <set>
#include <cmath>

using namespace std;

class Loc
{
    public:
        int row = 0;
        int col = 0;
};

class TailLocationCalculator
{
    private:
        const string TAIL_LOC_DELIMITER = ";";

        Loc _head;
        list<Loc> _taillocs;
        set<string> _prevtaillocs;

        bool iswithinone(int x, int y)
        {
            return abs(x-y) <= 1;
        }

        bool isadj(Loc& point1, Loc& point2)
        {
            cout << "Comparing ["+to_string(point1.row)+", "+to_string(point1.col)
                +"] to ["+to_string(point2.row)+", "+to_string(point2.col)+"]" << endl;

            bool withinrow = iswithinone(point1.row, point2.row);
            bool withincol = iswithinone(point1.col, point2.col);
            return withinrow && withincol;
        }

        void moveheadleft()
        {
            _head.col--;
            cout << ">Moving head left to ["+to_string(_head.row)+", "+to_string(_head.col)+"]"<< endl;
            updatetail();
        }

        void moveheadright()
        {
            _head.col++;
            cout << ">Moving head right to ["+to_string(_head.row)+", "+to_string(_head.col)+"]"<< endl;
            updatetail();
        }

        void moveheadup()
        {
            _head.row++;
            cout << ">Moving head up to ["+to_string(_head.row)+", "+to_string(_head.col)+"]"<< endl;
            updatetail();
        }

        void moveheaddown()
        {
            _head.row--;
            cout << ">Moving head down to ["+to_string(_head.row)+", "+to_string(_head.col)+"]"<< endl;
            updatetail();
        }

        void updatetail()
        {
            Loc* prev = &_head;

            int tailcnt = 0;
            for (Loc& tailpart : _taillocs)
            {
                if (isadj(*prev, tailpart))
                {
                    cout << ">Tail part["+to_string(tailcnt)+"] is adj at ["
                        +to_string(tailpart.row)+", "+to_string(tailpart.col)+"]"<< endl;
                    cout << ">Tail update complete." << endl;
                    return;
                }

                // Otherwise move one space toward the prev.
                int rowdist = prev->row-tailpart.row;
                int coldist = prev->col-tailpart.col;

                if (rowdist > 0) tailpart.row++;
                if (rowdist < 0) tailpart.row--;
                if (coldist > 0) tailpart.col++;
                if (coldist < 0) tailpart.col--;
                cout << ">Moving tail part["+to_string(tailcnt)+"] to follow ["
                    +to_string(tailpart.row)+", "+to_string(tailpart.col)+"]"<< endl;

                if (tailcnt == _taillocs.size()-1)
                    recordtailloc(tailpart);

                tailcnt++;
                prev = &tailpart;
            }

            cout << ">Tail update complete." << endl;
        }

        void recordtailloc(Loc& tailloc)
        {
            string taillocstr = to_string(tailloc.row)+";"+to_string(tailloc.col);
            _prevtaillocs.insert(taillocstr);
        }

        void processinstruction(string instructionline)
        {
            char direction = instructionline[0];
            cout << "======{"+instructionline+"}======" << endl;
            int repeat = stoi(instructionline.substr(2, instructionline.length()-2));

            for (int i = 0; i < repeat; i++)
            {
                switch (direction)
                {
                    case 'U':
                        moveheadup();
                        break;
                    case 'D':
                        moveheaddown();
                        break;
                    case 'L':
                        moveheadleft();
                        break;
                    case 'R':
                        moveheadright();
                        break;
                    default:
                        throw std::out_of_range(
                            "Direction \'"+to_string(instructionline[0])+"\' could not be processed.");
                }
            }
        }

        void run(string inputfile)
        {
            fstream headmoveinstructions;
            headmoveinstructions.open(inputfile, ios::in);

            if (!headmoveinstructions.is_open())
            {
                throw std::ios_base::failure("Could not open input file.");
            }

            // Record the starting location
            recordtailloc(_head);

            int linecount = 0;
            string currentLine;
            while(getline(headmoveinstructions, currentLine))
            {
                processinstruction(currentLine);
                linecount++;
            }

            cout << ">Processed \'"+to_string(linecount)+"\' input lines."<< endl;
            headmoveinstructions.close();
        }

    public:
        int calc_all_tail_location_count(string inputfile, size_t tailknotcount)
        {
            _prevtaillocs.clear();
            _head.row = 0;
            _head.col = 0;

            _taillocs.clear();
            for (size_t i = 0; i < tailknotcount; i++)
            {
                Loc newtailloc;
                _taillocs.push_back(newtailloc);
            }

            run(inputfile);

            return _prevtaillocs.size();
        }
};

int main()
{
    const string INPUT_FILE = "rope_head_moves.txt";

    TailLocationCalculator tailloccalc;

    const int PUZZLE_1_SEGMENTS = 1;
    int uniquesingletaillocs = tailloccalc.calc_all_tail_location_count(INPUT_FILE, PUZZLE_1_SEGMENTS);
    const int PUZZLE_2_SEGMENTS = 9;
    int unique9taillocs = tailloccalc.calc_all_tail_location_count(INPUT_FILE, PUZZLE_2_SEGMENTS);

    cout << ">> The last rope tail has moved to \'"+to_string(uniquesingletaillocs)
        +"\' unique locations with \'"+to_string(PUZZLE_1_SEGMENTS)+"\' segments."<< endl;
    cout << ">> The last rope tail has moved to \'"+to_string(unique9taillocs)
        +"\' unique locations with \'"+to_string(PUZZLE_2_SEGMENTS)+"\' segments."<< endl;
}