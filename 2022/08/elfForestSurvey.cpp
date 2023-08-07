#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

class TreeGrid
{
    private:
        const string ROW_COL_SEP = ";";
        const int MIN_COL = 0;
        const int MIN_ROW = 0;

        map<string, int> _map;

        int _maxrow = 0;
        int _maxcol = 0;

        void checkparams(int rowval, int colval)
        {
            if (rowval < MIN_ROW)
            {
                throw std::out_of_range("Row value cannot be less than zero");
            }
            if (colval < MIN_COL)
            {
                throw std::out_of_range("Col value cannot be less than zero");
            }
        }

        string buildkey(int rowval, int colval)
        {
            return to_string(rowval)+ROW_COL_SEP+to_string(colval);
        }

    public:
        int getrowcnt()
        {
            return _maxrow;
        }

        int getcolcnt()
        {
            return _maxcol;
        }

        void setval(int rowval, int colval, int val)
        {
            checkparams(rowval, colval);
            _map.insert(pair<string, int>(buildkey(rowval, colval), val));
            if (rowval+1 > _maxrow)
            {
                _maxrow = rowval+1;
            }
            if (colval+1 > _maxcol)
            {
                _maxcol = colval+1;
            }
        }

        int getval(int rowval, int colval)
        {
            checkparams(rowval, colval);
            const map<string, int>::iterator& it = _map.find(buildkey(rowval, colval));

            if (it == _map.end())
            {
                return -1;
            }
            else
            {
                return it->second;
            }
        }
};

class TreeGridBuilder
{
    private:
        string _inputfile;

    public:
        TreeGridBuilder(string inputfile)
        {
            _inputfile = inputfile;
        }

        TreeGrid buildgrid()
        {
            fstream forestfilestream;
            forestfilestream.open(_inputfile, ios::in);

            if (!forestfilestream.is_open())
            {
                throw std::ios_base::failure("Failed to open input file.");
            }

            TreeGrid grid;

            int linecount = 0;
            string currentline;
            while(getline(forestfilestream, currentline))
            {
                for (size_t i = 0; i < currentline.length(); i++)
                {
                    char currchar = currentline[i];
                    int parseddigit = stoi(to_string(currchar));
                    grid.setval(linecount, i, parseddigit);
                }
                linecount++;
            }

            forestfilestream.close();
            return grid;
        }
};

class TreeCalculatorHelper
{
    public:
        static bool issearchtreetaller(int currrow, int currcol,
            int searchrow, int searchcol, TreeGrid& inputgrid)
        {
            int currtreeheight = inputgrid.getval(currrow, currcol);
            int searchtree = inputgrid.getval(searchrow, searchcol);
            return (searchtree >= currtreeheight);
        }
};

//Puzzle 1 solution
class ViewableTreeCalculator
{
    private:
        static bool visup(int row, int col, TreeGrid& inputgrid)
        {
            //cout << ">Checking visup() on ["+to_string(row)+","+to_string(col)+"]\n";
            bool foundgreaterorequal = false;
            for (int i = row-1; i >= 0; i--)
            {
                if(TreeCalculatorHelper::issearchtreetaller(row, col, i, col, inputgrid))
                {
                    foundgreaterorequal = true;
                    break;
                }
            }
            return !foundgreaterorequal;
        }

        static bool visdown(int row, int col, TreeGrid& inputgrid)
        {
            //cout << ">Checking visdown() on ["+to_string(row)+","+to_string(col)+"]\n";
            bool foundgreaterorequal = false;
            for (int i = row+1; i < inputgrid.getrowcnt(); i++)
            {
                if(TreeCalculatorHelper::issearchtreetaller(row, col, i, col, inputgrid))
                {
                    foundgreaterorequal = true;
                    break;
                }
            }
            return !foundgreaterorequal;
        }

        static bool visleft(int row, int col, TreeGrid& inputgrid)
        {
            //cout << ">Checking visleft() on ["+to_string(row)+","+to_string(col)+"]\n";
            bool foundgreaterorequal = false;
            for (int i = col-1; i >= 0; i--)
            {
                if(TreeCalculatorHelper::issearchtreetaller(row, col, row, i, inputgrid))
                {
                    foundgreaterorequal = true;
                    break;
                }
            }
            return !foundgreaterorequal;
        }

        static bool visright(int row, int col, TreeGrid& inputgrid)
        {
            //cout << ">Checking visright() on ["+to_string(row)+","+to_string(col)+"]\n";
            bool foundgreaterorequal = false;
            for (int i = col+1; i < inputgrid.getcolcnt(); i++)
            {
                if(TreeCalculatorHelper::issearchtreetaller(row, col, row, i, inputgrid))
                {
                    foundgreaterorequal = true;
                    break;
                }
            }
            return !foundgreaterorequal;
        }

    public:
        static int calc_viewable_tree_quantity(TreeGrid& inputgrid)
        {
            cout << ">Beginning height calculation on grid of \'"
                +to_string(inputgrid.getrowcnt())+"\' rows and \'"
                +to_string(inputgrid.getcolcnt())+"\' columns\n";

            int viewabletreecnt = 0;
            for (size_t i = 0; i < inputgrid.getrowcnt(); i++)
            {
                for (size_t j = 0; j < inputgrid.getcolcnt(); j++)
                {
                    // Visit each tree.
                    int currtreeheight = inputgrid.getval(i, j);
                    if (currtreeheight < 0)
                    {
                        continue;
                    }

                    bool up = visup(i, j, inputgrid);
                    bool down = visdown(i, j, inputgrid);
                    bool left = visleft(i, j, inputgrid);
                    bool right = visright(i, j, inputgrid);

                    if (up || down || left || right)
                    {
                        viewabletreecnt++;
                    }
                }
            }
            return viewabletreecnt;
        }
};

//Puzzle 2 solution
class ScenicScoreCalculator
{
    private:
        static int visscoreup(int row, int col, TreeGrid& inputgrid)
        {
            //cout << ">Checking visscoreup() on ["+to_string(row)+","+to_string(col)+"]\n";
            int totalscore = 0;
            for (int i = row-1; i >= 0; i--)
            {
                totalscore++;
                if(TreeCalculatorHelper::issearchtreetaller(row, col, i, col, inputgrid))
                {
                    break;
                }
            }
            return totalscore;
        }

        static int visscoredown(int row, int col, TreeGrid& inputgrid)
        {
            //cout << ">Checking visscoredown() on ["+to_string(row)+","+to_string(col)+"]\n";
            int totalscore = 0;
            for (int i = row+1; i < inputgrid.getrowcnt(); i++)
            {
                totalscore++;
                if(TreeCalculatorHelper::issearchtreetaller(row, col, i, col, inputgrid))
                {
                    break;
                }
            }
            return totalscore;
        }

        static int visscoreleft(int row, int col, TreeGrid& inputgrid)
        {
            //cout << ">Checking visscoreleft() on ["+to_string(row)+","+to_string(col)+"]\n";
            int totalscore = 0;
            for (int i = col-1; i >= 0; i--)
            {
                totalscore++;
                if(TreeCalculatorHelper::issearchtreetaller(row, col, row, i, inputgrid))
                {
                    break;
                }
            }
            return totalscore;
        }

        static int visscoreright(int row, int col, TreeGrid& inputgrid)
        {
            //cout << ">Checking visscoreright() on ["+to_string(row)+","+to_string(col)+"]\n";
            int totalscore = 0;
            for (int i = col+1; i < inputgrid.getcolcnt(); i++)
            {
                totalscore++;
                if(TreeCalculatorHelper::issearchtreetaller(row, col, row, i, inputgrid))
                {
                    break;
                }
            }
            return totalscore;
        }

    public:
        static int calc_highest_scenic_score(TreeGrid& inputgrid)
        {
            cout << ">Beginning scenic score calculation on grid of \'"
                +to_string(inputgrid.getrowcnt())+"\' rows and \'"
                +to_string(inputgrid.getcolcnt())+"\' columns\n";

            size_t highestscore = 0;

            int viewabletreecnt = 0;
            for (size_t i = 0; i < inputgrid.getrowcnt(); i++)
            {
                for (size_t j = 0; j < inputgrid.getcolcnt(); j++)
                {
                    // Visit each tree.
                    int currtreeheight = inputgrid.getval(i, j);
                    if (currtreeheight < 0)
                    {
                        continue;
                    }

                    int upscore = visscoreup(i, j, inputgrid);
                    int downscore = visscoredown(i, j, inputgrid);
                    int leftscore = visscoreleft(i, j, inputgrid);
                    int rightscore = visscoreright(i, j, inputgrid);

                    int treescore = upscore*downscore*leftscore*rightscore;
                    if (treescore > highestscore)
                    {
                        highestscore = treescore;
                    }
                }
            }
            return highestscore;
        }
};

int main()
{
    const string INPUT_FILE = "tree_grid.txt";
    TreeGridBuilder gridbuilder(INPUT_FILE);
    TreeGrid treegrid = gridbuilder.buildgrid();
    cout << ">Grid build completed with \'"+to_string(treegrid.getrowcnt())
        +"\' rows and \'"+to_string(treegrid.getcolcnt())+"\' columns\n";

    int viewablecnt = ViewableTreeCalculator::calc_viewable_tree_quantity(treegrid);
    cout << ">>There are \'"+to_string(viewablecnt)+"\' viewable trees from outside the grid\n";

    int highestscenicscore = ScenicScoreCalculator::calc_highest_scenic_score(treegrid);
    cout << ">>The tree with the best scenic score is \'"+to_string(highestscenicscore)+"\'\n";
}