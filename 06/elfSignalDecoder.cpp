#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <set>

using namespace std;
int main()
{
    const string SIGNAL_FILE = "elf_communication_signal.txt";

    //Read in the signal string
    fstream signalfilestream;
    signalfilestream.open(SIGNAL_FILE, ios::in);

    if (!signalfilestream.is_open())
    {
        throw std::ios_base::failure("Failed to open signal file.");
    }

    string signalstring;
    string currentline;
    int linesRead = 0;
    while(getline(signalfilestream, currentline))
    {
        signalstring += currentline;
        linesRead++;
    }

    cout << "Lines Read:" << linesRead;
    cout << "\n";

    const int UNIQUE_CHAR_COUNT = 14;//4; //Part 1

    list<char> lastfourcharslist;
    size_t messagestartposition;
    for (size_t i = 0; i < signalstring.length(); i++)
    {
        char currentchar = signalstring[i];
        lastfourcharslist.push_back(currentchar);

        if (lastfourcharslist.size() == UNIQUE_CHAR_COUNT+1)
        {
            lastfourcharslist.pop_front();
            set<char> currcharset;

            for(const auto& prevChar : lastfourcharslist)
            {
                currcharset.insert(prevChar);
            }

            if (currcharset.size() == UNIQUE_CHAR_COUNT)
            {
                messagestartposition = i;
                break;
            }
        }
    }

    cout << "Message begins at: " << messagestartposition+1;
}