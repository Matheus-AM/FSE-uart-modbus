#include <fstream>
#include <sstream>
#include <stdio.h>

struct csv
{
    int time[10];
    int temp[10];
};

void read_record(struct csv* row );