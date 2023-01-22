#include <csv_reader.h>


struct csv
{
    int time[10];
    int temp[10];
};


void read_record(struct csv* row )
{
    // File pointer
    std::fstream fin;
  
    // Open an existing file
    fin.open("../curva_reflow.csv", std::ios::in);
  
    
    // Read the Data from the file
    // as String Vector
    std::string line, word;
    std::getline(fin, line);
    int count = 0;
    while (std::getline(fin, line)) {
  
        std::stringstream s(line);
  
        std::getline(s, word, ',');
        (*row).time[count] = std::stoi(word);
        std::getline(s, word, ',');
        (*row).temp[count] = std::stoi(word);
  
        count++;
    }
    if (count == 0)
        printf("Record not found\n");
    return ;
}
