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
                        if (!foundcd && currentparsingstr.substr(0, CHANGE_DIR_PREFIX.length()) == CHANGE_DIR_PREFIX)
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
                                //DirectoryNode searchnode = *nodeptr;
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
                                throw std::range_error("No subdirectory exists for \""+currentparsingstr+"\" out of : "+namesfound);
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
                        subdir.name = currentline.substr(4, currentline.length()-4);
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

            cout << ">> Lines read: "+ to_string(inputlinesread)+"\n";
            cout << ">> Directories created: "+ to_string(dircnt)+"\n";
            cout << ">> Files created: "+ to_string(filecnt)+"\n";
            cout << ">> Directory changes made: "+ to_string(cds)+"\n";
            return root;
        }
};

int main()
{
    const string CONSOLE_FILE = "directory_console_logs.txt";

    FileStructureBuilder fsbuilder(CONSOLE_FILE);
    DirectoryNode rootdir = fsbuilder.buildfilesystem();

    
}