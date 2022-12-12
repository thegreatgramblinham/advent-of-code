#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

class TopoMap
{
    private:
        const string ROW_COL_SEP = ";";

        map<string, size_t> _map;

        size_t _maxrow = 0;
        size_t _maxcol = 0;

        size_t _startrow = 0;
        size_t _startcol = 0;

        size_t _endrow = 0;
        size_t _endcol = 0;

        string buildkey(int rowval, int colval)
        {
            return to_string(rowval)+ROW_COL_SEP+to_string(colval);
        }

    public:
        size_t getrowcnt()
        {
            return _maxrow+1;
        }

        size_t getcolcnt()
        {
            return _maxcol+1;
        }

        size_t getstartrow()
        {
            return _startrow;
        }

        size_t getstartcol()
        {
            return _startcol;
        }

        size_t getendrow()
        {
            return _endrow;
        }

        size_t getendcol()
        {
            return _endcol;
        }

        void setloc(size_t row, size_t col, char val)
        {
            size_t weighttoapply = size_t(val);
            if (val == 'S')
            {
                weighttoapply = size_t('a');
                _startrow = row;
                _startcol = col;
            }

            if (val == 'E')
            {
                weighttoapply = size_t('a');
                _endrow = row;
                _endcol = col;
            }

            _map.insert(pair<string, size_t>(buildkey(row, col), weighttoapply));
            if (row > _maxrow)
            {
                _maxrow = row;
            }
            if (col > _maxcol)
            {
                _maxcol = col;
            }
        }

        size_t getlocweight(size_t row, size_t col)
        {
            const map<string, size_t>::iterator& it = _map.find(buildkey(row, col));

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

class TopoMapParser
{
    public:
        static TopoMap parse_map(string inputfile)
        {
            TopoMap topomap;

            fstream topofilestream;
            topofilestream.open(inputfile, ios::in);

            int currentLineIndex = 0;
            string currentline;
            while (getline(topofilestream, currentline))
            {
                for (size_t i = 0; i < currentline.length(); i++)
                {
                    char val = currentline[i];
                    topomap.setloc(currentLineIndex, i, val);
                }
                currentLineIndex++;
            }
            return topomap;
        }
};

class PathfinderNode
{
    public:
        size_t shortestpath = -1; // Init to maxval via wrap;
        bool pathfound = false;
};

class TopoMapPathfinder
{
    private:
        TopoMap* _inputmap;
        map<string, PathfinderNode> _shortestpathmap;

        string buildkey(int rowval, int colval)
        {
            const string ROW_COL_SEP = ";";
            return to_string(rowval)+ROW_COL_SEP+to_string(colval);
        }

        bool canmoveup(int currrow, int currcol)
        {
            if (currrow+1 >= _inputmap->getrowcnt())
                return false;
            size_t currweight = _inputmap->getlocweight(currrow, currcol);
            size_t upweight = _inputmap->getlocweight(currrow+1, currcol);
            // cout<<to_string(getnode(currrow+1, currcol)->shortestpath)<<endl;
            // cout<<to_string(getnode(currrow, currcol)->shortestpath)<<endl;
            return upweight <= currweight+1
                //&& !(getnode(currrow+1, currcol)->pathfound)
                && (getnode(currrow+1, currcol)->shortestpath) > (getnode(currrow, currcol)->shortestpath);
        }

        bool canmovedown(int currrow, int currcol)
        {
            if (currrow-1 < 0)
                return false;
            size_t currweight = _inputmap->getlocweight(currrow, currcol);
            size_t downweight = _inputmap->getlocweight(currrow-1, currcol);
            return downweight <= currweight+1
                //&& !(getnode(currrow-1, currcol)->pathfound)
                && (getnode(currrow-1, currcol)->shortestpath) > (getnode(currrow, currcol)->shortestpath);
        }

        bool canmoveleft(int currrow, int currcol)
        {
            if (currcol-1 < 0)
                return false;
            size_t currweight = _inputmap->getlocweight(currrow, currcol);
            size_t leftweight = _inputmap->getlocweight(currrow, currcol-1);
            return leftweight <= currweight+1
                //&& !(getnode(currrow, currcol-1)->pathfound)
                && (getnode(currrow, currcol-1)->shortestpath) > (getnode(currrow, currcol)->shortestpath);
        }

        bool canmoveright(int currrow, int currcol)
        {
            if (currcol+1 >= _inputmap->getcolcnt())
                return false;
            size_t currweight = _inputmap->getlocweight(currrow, currcol);
            size_t rightweight = _inputmap->getlocweight(currrow, currcol+1);
            return rightweight <= currweight+1
                //&& !(getnode(currrow, currcol+1)->pathfound)
                && (getnode(currrow, currcol+1)->shortestpath) > (getnode(currrow, currcol)->shortestpath);
        }

        PathfinderNode* getnode(int row, int col)
        {
            string key = buildkey(row,col);
            map<string, PathfinderNode>::iterator pathit = _shortestpathmap.find(key);
            if (pathit == _shortestpathmap.end())
            {
                // If the node does not exist, create it.
                PathfinderNode newNode;
                _shortestpathmap.insert(pair<string,PathfinderNode>(key, newNode));
                pathit = _shortestpathmap.find(key);
            }
            return &pathit->second;
        }

        void visitnode(int row, int col, PathfinderNode* prev)
        {
            cout<<">Beginning visit to ["+to_string(row)+", "+to_string(col)+"]"<<endl;
            PathfinderNode* currentnode = getnode(row, col);
            size_t currentNodeWeight = _inputmap->getlocweight(row, col);

            // Record the path we arrived at this node, if it's the shortest.
            if (prev == nullptr)
            {
                currentnode->shortestpath = 0;
                currentnode->pathfound = true;
                cout<<">[START] Recorded start at ["+to_string(row)+", "+to_string(col)
                    +"] with shortest path \'"+to_string(currentnode->shortestpath)+"\'"<<endl;
            }
            else if (currentnode->shortestpath > prev->shortestpath+1)
            {
                cout<<">Current shortest path is \'"+to_string(currentnode->shortestpath)
                    +"\'. Prev shortest path is \'"+to_string(prev->shortestpath)+"\'."<<endl;
                currentnode->shortestpath = prev->shortestpath+1;
                cout<<">Replacing shortest path at ["+to_string(row)+", "+to_string(col)+"]. It is now: "
                    +to_string(currentnode->shortestpath)<<endl;

                // If we have a valid move a node that has found its shortest
                // path, then we have found our shortest path.
                bool abovehasfoundpath = canmovedown(row+1, col);
                bool belowhasfoundpath = canmoveup(row-1, col);
                bool lefthasfoundpath = canmoveright(row, col+1);
                bool righthasfoundpath = canmoveleft(row, col-1);

                // currentnode->pathfound = false; //!abovehasfoundpath && !belowhasfoundpath
                //     //&& !lefthasfoundpath && !righthasfoundpath;
                // if(currentnode->pathfound)
                //     cout<<">[FOUND] shortest path at ["+to_string(row)+", "+to_string(col)+"] which is: "
                //     +to_string(currentnode->shortestpath)<<endl;
            }

            bool validupmove = canmoveup(row, col) && (getnode(row+1, col) != prev);
            cout<<">Has a vaild move down to ["+to_string(row+1)+", "+to_string(col)+"] returns: "
                +to_string(validupmove)<<endl;
            bool validdownmove = canmovedown(row, col) && (getnode(row-1, col) != prev);
            cout<<">Has a vaild move up to ["+to_string(row-1)+", "+to_string(col)+"] returns: "
                +to_string(validdownmove)<<endl;
            bool validleftmove = canmoveleft(row, col) && (getnode(row, col-1) != prev);
            cout<<">Has a vaild move left to ["+to_string(row)+", "+to_string(col-1)+"] returns: "
                +to_string(validleftmove)<<endl;
            bool validrightmove = canmoveright(row, col) && (getnode(row, col+1) != prev);
            cout<<">Has a vaild move right to ["+to_string(row)+", "+to_string(col+1)+"] returns: "
                +to_string(validrightmove)<<endl;

            // base case
            if (!validupmove && !validdownmove && !validleftmove && !validrightmove)
            {
                currentnode->pathfound = true;
                cout<<">[FOUND] shortest path at ["+to_string(row)+", "+to_string(col)+"] which is: "
                    +to_string(currentnode->shortestpath)<<endl;
                cout<<">[BASE] No valid moves from  ["+to_string(row)+", "+to_string(col)+"]"<<endl;
            }

            // Recurse only on nodes we have a valid move to.
            if (validupmove)
                visitnode(row+1, col, currentnode);
            if (validdownmove)
                visitnode(row-1, col, currentnode);
            if (validleftmove)
                visitnode(row, col-1, currentnode);
            if (validrightmove)
                visitnode(row, col+1, currentnode);
        }

    public:
        TopoMapPathfinder(TopoMap& inputmap)
        {
            _inputmap = &inputmap;
        }

        void run()
        {
            cout<<">Starting traversal on map of \'"+to_string((*_inputmap).getrowcnt())
                +"\' rows and \'"+to_string((*_inputmap).getcolcnt())+"\' colunms"<<endl;
            visitnode(_inputmap->getstartrow(), _inputmap->getstartcol(), nullptr);

            size_t endrow = _inputmap->getendrow();
            size_t endcol = _inputmap->getendcol();
            map<string, PathfinderNode>::iterator goalit = _shortestpathmap.find(buildkey(endrow, endcol));
            if (goalit == _shortestpathmap.end())
                throw std::out_of_range("Goal node is somehow not in map.");

            cout<<">>[DONE]Traversal complete. Shortest path to goal at ["
                +to_string(endrow)+", "+to_string(endcol)+"] is: "
                +to_string(goalit->second.shortestpath)+" steps."<<endl;
        }
};

int main()
{
    const string INPUT_FILE = "test.txt";
    TopoMap map = TopoMapParser::parse_map(INPUT_FILE);
    TopoMapPathfinder pathfinder(map);
    pathfinder.run();
}
