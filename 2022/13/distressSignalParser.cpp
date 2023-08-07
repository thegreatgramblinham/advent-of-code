#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class PacketParser
{
    private:
        const string LIST_START = "[";
        const string LIST_END = "]";
        const string LIST_SEPARATOR = ",";

        string _inputstr;
        int _strindex = -1;

    public:
        PacketParser(string inputstr)
        {
            _inputstr = inputstr;
        }

};

class InputParser
{
    public:
        static void count_valid_packets_in_file(string inputfile)
        {
            fstream inputfilestream;
            inputfilestream.open(inputfile, ios::in);
            if (!inputfilestream.is_open())
            {
                throw std::ios_base::failure("Could not open input file");
            }

            while(true) //yolo
            {
                string firstpacketline;
                if (!getline(inputfilestream, firstpacketline))
                {
                    cout<<">>[END]First packet could not be read."<<endl;
                    break;
                }

                string secondpacketline;
                if (!getline(inputfilestream, secondpacketline))
                {
                    cout<<">>[END]Second packet could not be read."<<endl;
                    break;
                }

                PacketParser firstlineparser(firstpacketline);
                PacketParser secondlineparser(secondpacketline);


                // Read the blank line separator. This will also be our terminator.
                if (!getline(inputfilestream, firstpacketline))
                {
                    cout<<">>[END]Blank line could not be read."<<endl;
                    break;
                }
            }
        }
};

int main()
{
    const string INPUT_FILE = "test.txt";
    InputParser::count_valid_packets_in_file(INPUT_FILE);
}