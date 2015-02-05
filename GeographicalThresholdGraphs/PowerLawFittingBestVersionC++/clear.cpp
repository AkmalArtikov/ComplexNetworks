#include <iostream>
#include <fstream>
#include <string>

void clear_file(std::string file_name)
{
    std::ofstream ofs;
    ofs.open(file_name.c_str(), std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

int main()
{
    clear_file("temp_degrees/temp.txt");
    clear_file("temp_degrees/temp_good.txt");
    clear_file("temp_degrees/temp_bad.txt");
    clear_file("temp_degrees/cumulative_temp.txt");
    clear_file("temp_degrees/cumulative_temp_good.txt");

    return 0;
}