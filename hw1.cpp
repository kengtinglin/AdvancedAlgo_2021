#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

class student
{
    public:
        int id;
        int behavior;
        int grade;
};

class company
{
    public:
        int id;
        int behavior;
        int grade;
        int salary;
};

class result
{
    public:
        int student_id;
        int company_id;
};

bool compare_company(company c1, company c2)
{
    if (c1.salary == c2.salary && c1.id > c2.id)
    {
        return c1.salary < c2.salary;
    }
    else
    {
        return c1.salary > c2.salary; 
    }
}

bool compare_result(result r1, result r2)
{
    return r1.student_id < r2.student_id;
}


int main(int argc, char* argv[])  
{
    //read data from files and open an output data
    student student_tmp;
    company company_tmp;
    result result_tmp;
    vector <company> company_list;
    vector <vector <vector <result> > > student_table(101, vector <vector <result> >(101, vector<result>()));
    vector <result> result_list;
    fstream student_file(argv[1]);
    fstream company_file(argv[2]);
    ofstream output_file(argv[3]);
    
    while (student_file >> student_tmp.id >> student_tmp.behavior >> student_tmp.grade)
    {
        result_tmp.student_id = student_tmp.id;
        result_tmp.company_id = 0;
        student_table[student_tmp.behavior][student_tmp.grade].push_back(result_tmp);
    }
    
    while (company_file >> company_tmp.id >> company_tmp.behavior >> company_tmp.grade >> company_tmp.salary)
    {
        company_list.push_back(company_tmp);
    }

    // To sort company by salary in decreasing order
    sort(company_list.begin(), company_list.end(), compare_company);
    
    // To let company choose the student
    for (int i = company_list.size() - 1; i >= 0; i--)
    {
        for (int j = company_list[i].behavior; j < 101; j++)
        {
            for (int k = company_list[i].grade; k < 101; k++)
            {
                for (int l = 0; l < student_table[j][k].size(); l++)
                {
                    student_table[j][k][l].company_id = company_list[i].id;
                }
            }
        }
    }
    
    // To store the result
    for (int i = 0; i < 101; i++)
    {
        for (int j = 0; j < 101; j++)
        {
            if (student_table[i][j].size() != 0)
            {
                for (int k = 0; k < student_table[i][j].size(); k++)
                result_list.push_back(student_table[i][j][k]);
            }
        }
    }
    
    
    // To sort the result by student id
    sort(result_list.begin(), result_list.end(), compare_result);
    for (int i = 0; i < result_list.size(); i++)
    {
        output_file << result_list[i].student_id << ": " << result_list[i].company_id << "\n";
    }
    
    student_file.close();
    company_file.close();
    output_file.close();
    return 0;
}
