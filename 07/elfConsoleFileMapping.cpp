#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;

class FileNode
{
    public:
        //Properties
        string name;
        size_t filesize;
};

class DirectoryNode
{
    public:
        //Properties
        string name;
        DirectoryNode* parentNode;
        list<DirectoryNode> subdirectories;
        list<FileNode> files;
};

class FileStructureBuilder
{
    const string COMMAND_START_PREFIX = "$";
    const string CHANGE_DIR_PREFIX = "cd";
    const string CD_OUT_SUFFIX = "..";
    const string CD_ROOT_SUFFIX = "/";
    const string LIST_COMMAND = "ls";
    const string DIR_PREFIX = "dir";

    private:
        string _inputfilename;

    public:
        //Constructor
        FileStructureBuilder(string inputfilename)
        {
            _inputfilename = inputfilename;
        }

        //Public Methods
        DirectoryNode buildfilesystem()
        {
            fstream consolefilestream;
            consolefilestream.open(_inputfilename, ios::in);

            if (!consolefilestream.is_open())
            {
                throw std::ios_base::failure("Failed to open console file.");
            }

            // Create our root node.
            DirectoryNode root;
            root.name = "[root]";

            DirectoryNode* currentnode = &root;

            int inputlinesread = 0;
            int dircnt = 0;
            int filecnt = 0;
            int cds = 0;

            string currentline;
            while(getline(consolefilestream, currentline))
            {
                if (currentline.substr(0, COMMAND_START_PREFIX.length()) == COMMAND_START_PREFIX)
                {
                    bool foundcd = false;
                    string currentparsingstr;
                    //Handle command - starting at 1 to skip "$"
                    for (size_t i = 1; i < currentline.length(); i++)
                    {
                        char currentchar = currentline[i];
                        if ((currentchar == '\n') || (currentchar == ' '))
                        {
                            continue;
                        }

                        currentparsingstr += currentchar;

                        // Look for a "cd" command.
                        if (!foundcd && currentparsingstr.substr(
                                0, CHANGE_DIR_PREFIX.length()) == CHANGE_DIR_PREFIX)
                        {
                            foundcd = true;
                            currentparsingstr = "";
                        }
                    }

                    // If we've been issued a cd command, then our target will
                    // reside in currentparsingstr.
                    if (foundcd)
                    {
                        if (currentparsingstr.substr(0, CD_ROOT_SUFFIX.length()) == CD_ROOT_SUFFIX)
                        {
                            cout << ">Moving dir to root\n";
                            currentnode = &root;
                        }
                        else if (currentparsingstr.substr(0, CD_OUT_SUFFIX.length()) == CD_OUT_SUFFIX)
                        {
                            DirectoryNode& curr = *currentnode;
                            cout << ">Moving dir from \""+curr.name+"\""+" to parent\n";
                            currentnode = curr.parentNode;
                        }
                        else
                        {
                            // Otherwise we move our current node to a like-named node
                            // in the current directory.
                            bool locationupdated = false;
                            string namesfound;
                            DirectoryNode& curr = *currentnode;

                            cout << ">Moving dir from \""+curr.name+"\""+" to \""+currentparsingstr+"\"\n";
                            for (DirectoryNode& searchnode : curr.subdirectories)
                            {
                                namesfound += searchnode.name+";";
                                if (currentparsingstr == searchnode.name)
                                {
                                    currentnode = &searchnode;
                                    locationupdated = true;
                                    break;
                                }
                            }

                            if (!locationupdated)
                            {
                                throw std::range_error("No subdirectory exists for \""+currentparsingstr
                                    +"\" out of : "+namesfound);
                            }
                        }
                        cds++;
                    }
                }
                else
                {
                    if (currentline.substr(0, DIR_PREFIX.length()) == DIR_PREFIX)
                    {
                        // We are creating directory.

                        DirectoryNode subdir;
                        // Excluding "dir ", so starting at index 4, get the name.
                        // Which is -5 from the total str lengh long.
                        subdir.name = currentline.substr(
                            DIR_PREFIX.length()+1, currentline.length()-(DIR_PREFIX.length()+1));
                        subdir.parentNode = currentnode;

                        DirectoryNode& curr = *currentnode;
                        curr.subdirectories.push_back(subdir);

                        cout << ">Created dir \""+subdir.name+"\" under parent \""+curr.name+"\"\n";
                        dircnt++;
                    }
                    else
                    {
                        // We are creating a file.

                        // Find the space divider between size and name.
                        size_t spacediv = currentline.find(' ');

                        string filesizestr = currentline.substr(0, spacediv);
                        string filename = currentline.substr(spacediv+1, currentline.length()-spacediv-1);

                        FileNode file;
                        file.name = filename;
                        file.filesize = stoul(filesizestr);

                        cout << ">Created file \""+file.name+"\" with size: "+filesizestr+"\n";
                        DirectoryNode& curr = *currentnode;
                        curr.files.push_back(file);
                        filecnt++;
                    }
                }

                inputlinesread++;
            }

            consolefilestream.close();
            cout << ">> Lines read: "+ to_string(inputlinesread)+"\n";
            cout << ">> Directories created: "+ to_string(dircnt)+"\n";
            cout << ">> Files created: "+ to_string(filecnt)+"\n";
            cout << ">> Directory changes made: "+ to_string(cds)+"\n";
            return root;
        }
};

class DirectoryHelper
{
    public:
        static size_t calc_total_dir_size(DirectoryNode* dir)
        {
            DirectoryNode& currentdir = *dir;

            //Get the total for all sub dirs
            size_t subdirstotal = 0;
            for (DirectoryNode& subdir : currentdir.subdirectories)
            {
                size_t size = calc_total_dir_size(&subdir);
                subdirstotal += size;
            }

            size_t filetotal = 0;
            //Get the total file size of the current dir.
            for (FileNode& file : currentdir.files)
            {
                filetotal += file.filesize;
            }

            return subdirstotal + filetotal;
        }
};

// Puzzle 1 Solution
class ThresholdDirectorySumCalculator
{
    private:
        int _threshold;
        size_t _thresholdsummation;

        // Sum all directory values that are less than or equal to a given threshold.
        void directory_size_threshold_summation(DirectoryNode* dir, int threshold)
        {
            DirectoryNode& currentdir = *dir;

            for (DirectoryNode& subdir : currentdir.subdirectories)
            {
                // Depth first
                directory_size_threshold_summation(&subdir, threshold);
            }

            cout << ">Visiting dir \""+currentdir.name+"\"\n";
            size_t totalSummation = 0;

            // Sum the files.
            size_t filesummationtotal = 0;
            for (FileNode& file : currentdir.files)
            {
                filesummationtotal += file.filesize;
            }
            cout << ">["+currentdir.name+"] has total file size:"+to_string(filesummationtotal)+"\n";
            totalSummation += filesummationtotal;

            // Sum the sub directories.
            int subdirtotal = 0;
            size_t totaldirsummation = 0;
            for (DirectoryNode& subdir : currentdir.subdirectories)
            {
                size_t dirsum = DirectoryHelper::calc_total_dir_size(&subdir);
                totaldirsummation += dirsum;
                subdirtotal++;
            }
            totalSummation += totaldirsummation;
            cout << ">["+currentdir.name+"] has "+to_string(subdirtotal)+" subdir(s) of total size:"
                +to_string(totaldirsummation)+"\n";
            cout << ">["+currentdir.name+"] has total size: "+to_string(totalSummation)+"\n";

            // If our total for this node is within the threshold, count it.
            if (totalSummation <= _threshold)
            {
                _thresholdsummation += totalSummation;
            }
        }

    public:
        ThresholdDirectorySumCalculator(int threshold)
        {
            _threshold = threshold;
        }

        size_t run(DirectoryNode* root)
        {
            _thresholdsummation = 0;
            directory_size_threshold_summation(root, _threshold);
            return _thresholdsummation;
        }
};

// Puzzle 2 Solution
class SmallestFileToDeleteCalculator
{
    private:
        size_t _freespace = 0;
        size_t _filesystemsize = 0;
        size_t _currsmallestdir = 0;

        void find_smallest_directory_to_delete(DirectoryNode* dir, size_t freespacereq)
        {
            DirectoryNode& currentdir = *dir;

            for (DirectoryNode& subdir : currentdir.subdirectories)
            {
                // Depth first
                find_smallest_directory_to_delete(&subdir, freespacereq);
            }

            size_t currdirsize = DirectoryHelper::calc_total_dir_size(dir);

            size_t potentialfreespace = _freespace + currdirsize;
            if ((potentialfreespace >= freespacereq) && (currdirsize < _currsmallestdir))
            {
                cout << ">Found a smaller size. Replacing \'"+to_string(_currsmallestdir)+"\' with \'"
                    +to_string(currdirsize)+"\'\n";
                _currsmallestdir = currdirsize;
            }
        }

    public:
        SmallestFileToDeleteCalculator(size_t filesystemsize)
        {
            _filesystemsize = filesystemsize;
        }

        size_t run(DirectoryNode* dir, size_t freespacereq)
        {
            size_t rootsize = DirectoryHelper::calc_total_dir_size(dir);
            _currsmallestdir = rootsize;
            _freespace = _filesystemsize - rootsize;
            cout << ">Free space available: \'"+to_string(_freespace)+" units\'\n";
            cout << ">Need to free up \'"+to_string(freespacereq-_freespace)+" units\'\n";
            find_smallest_directory_to_delete(dir, freespacereq);
            return _currsmallestdir;
        }
};

int main()
{
    const string CONSOLE_FILE = "directory_console_logs.txt";

    FileStructureBuilder fsbuilder(CONSOLE_FILE);
    DirectoryNode rootdir = fsbuilder.buildfilesystem();

    const int NODE_THRESHOLD = 100000;
    ThresholdDirectorySumCalculator dirsum(NODE_THRESHOLD);

    size_t total = dirsum.run(&rootdir);
    cout << ">>Total summation under \'"+to_string(NODE_THRESHOLD)+"\': "+to_string(total)+"\n";
    cout << "================\n";

    const int FILE_SYSTEM_SIZE = 70000000;
    SmallestFileToDeleteCalculator rmcalc(FILE_SYSTEM_SIZE);

    const int FREE_SPACE_REQ = 30000000;
    size_t rmdirsize = rmcalc.run(&rootdir, FREE_SPACE_REQ);

    cout << ">>Smallest directory to remove for \'"+to_string(FREE_SPACE_REQ)+" units\': "
        +to_string(rmdirsize)+"\n";
}

