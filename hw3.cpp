#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
#include <sstream>
using namespace std;

#define INT_MAX 2147483647; 
#define INT_MIN -2147483648; 


int maximum_sub(vector<int> input, int distance, char mode) {
  /* I used A[] to explain instead of input[] 
		 I used circular array to store the tempory maximum subarray */
	int n = input.size();
	int temp;

	if (mode == 'A') {
		if (distance > 0 && distance < n) {
			int result[distance];
			int temp_A;

			/* Initialization */
			temp = INT_MIN;
			for (int i=0; i<distance; i++) {
				if (temp < input.at(i)) {
					temp = input.at(i);
				}
				result[i] = temp;
			}

			/* DP */
			for (int i=distance; i<n; i++) {
				temp_A = INT_MIN;
				for (int j=0; j<3; j++) {
					if (temp_A < input.at(i)) {  // Compared to A[i]
						temp_A = input.at(i);
					} else if (temp_A < result[(i-1)%(distance)]) {  // Compared to result[i-1]
						temp_A = result[(i-1)%distance];
					} else if (temp_A < result[(i-distance)%distance]+input.at(i)) {  // Compared to result[i-K] + A[i]
						temp_A = result[(i-distance)%distance] + input.at(i);
					}
				}
				if (temp_A > temp) {
					temp = temp_A;
				}
				result[i%distance] = temp_A;
			}
		} else {
			temp = *max_element(input.begin(), input.end());
		}
	} else if (mode == 'C') {
		if (distance > 0 && distance < n) {
			int result_max[distance];
			int result_min[distance];
			int temp_C_max, temp_C_min;

			/* Initialization */
			temp = INT_MIN;
			temp_C_min = INT_MAX;
			for (int i=0; i<distance; i++) {
				if (temp < input.at(i)) {
					temp = input.at(i);	
				}
				if (temp_C_min > input.at(i)) {
					temp_C_min = input.at(i);
				}
				result_max[i] = temp;
				result_min[i] = temp_C_min;
			}

			/* DP */
			for (int i=distance; i<n; i++) {
				temp_C_max = INT_MIN;
				temp_C_min = INT_MAX;
				for (int j=0; j<4; j++) {
					if (temp_C_max < input.at(i)) {  // Compare to A[i]
						temp_C_max = input.at(i);
					} else if (temp_C_max < result_max[(i-1)%distance]) {  // Compare to result_max[i-1]
						temp_C_max = result_max[(i-1)%distance];
					} else if (temp_C_max < result_max[(i-distance)%distance]*input.at(i)) {  // compare to result_max[i-distance] * A[i]
						temp_C_max = result_max[(i-distance)%distance] * input.at(i);
					} else if (temp_C_max < result_min[(i-distance)%distance]*input.at(i)) {  // compare to result_min[i-distance] * A[i]
						temp_C_max = result_min[(i-distance)%distance] * input.at(i);
					}
					if (temp_C_min > input.at(i)) {
						temp_C_min = input.at(i);
					} else if (temp_C_min > result_min[(i-1)%distance]) {
						temp_C_min = result_min[(i-1)%distance];
					} else if (temp_C_min > result_min[(i-distance)%distance]*input.at(i)) {
						temp_C_min = result_min[(i-distance)%distance] * input.at(i);
					} else if (temp_C_min > result_max[(i-distance)%distance]*input.at(i)) {
						temp_C_min = result_max[(i-distance)%distance] * input.at(i);
					}
				}
				if (temp_C_max > temp) {
					temp = temp_C_max;
				}
				result_max[i%distance] = temp_C_max;
				result_min[i%distance] = temp_C_min;
			}
		} else {
			temp = *max_element(input.begin(), input.end());
		}
	} else if (mode == 'B') {
		if (distance > 0 && distance < n) {
			int temp_B;
			int result[distance+1];

			/* Initialization */
			result[0] = input.at(0);
			for (int i=1; i<distance+1; i++) {
				temp_B = INT_MIN;
				for (int j=0; j<3; j++) {
					if (temp_B < input.at(i)) {
						temp_B = input.at(i);
					} else if (temp_B < result[i-1]) {
						temp_B = result[i-1];
					} else if (temp_B < result[i-1]+input.at(i)) {
						temp_B = result[i-1] + input.at(i);
					}
				}
				result[i] = temp_B;
			}
			temp = INT_MIN;
			for (int i=0; i<distance+1; i++) {
				if (result[i] >= temp) {
					temp = result[i];
				}
			}
		
			/* DP */
			for (int i=distance+1; i<n; i++) {
				if (result[(i-1)%(distance+1)]-input.at(i-distance-1) == 0) {
					result[(i-distance)%(distance+1)] = input.at(i-distance);
					for (int j=i-distance+1; j<i+1; j++) {
						temp_B = INT_MIN;
						for (int k=0; k<3; k++) {
							if (temp_B < input.at(j)) {
								temp_B = input.at(j);
							} else if (temp_B < result[(j-1)%(distance+1)]) {
								temp_B = result[(j-1)%(distance+1)];
							} else if (temp_B < result[(j-1)%(distance+1)]+input.at(j)) {
								temp_B = result[(j-1)%(distance+1)]+input.at(j);
							}
						}
						if (temp_B > temp) {
							temp = temp_B;
						}
						result[j%(distance+1)] = temp_B;
					}
				} else if (result[(i-1)%(distance+1)]-input.at(i-distance-1) < result[(i-1)%(distance+1)]) {
					result[(i-1)%(distance+1)] = result[(i-1)%(distance+1)] - input.at(i-distance-1);
					temp_B = INT_MIN;
					for (int k=0; k<3; k++) {
						if (temp_B < input.at(i)) {
							temp_B = input.at(i);
						} else if (temp_B < result[(i-1)%(distance+1)]) {
							temp_B = result[(i-1)%(distance+1)];
						} else if (temp_B < result[(i-1)%(distance+1)]+input.at(i)) {
							temp_B = result[(i-1)%(distance+1)]+input.at(i);
						}
					}
					if (temp_B > temp) {
						temp = temp_B;
					}
					result[i%(distance+1)] = temp_B;	
				} else if (result[(i-1)%(distance+1)]-input.at(i-distance-1) > result[(i-1)%(distance+1)]) {
					temp_B = INT_MIN;
					for (int k=0; k<3; k++) {
						if (temp_B < input.at(i)) {
							temp_B = input.at(i);
						} else if (temp_B < result[(i-1)%(distance+1)]) {
							temp_B = result[(i-1)%(distance+1)];
						} else if (temp_B < result[(i-1)%(distance+1)]+input.at(i)) {
							temp_B = result[(i-1)%(distance+1)]+input.at(i);
						}
					}
					if (temp_B > temp) {
						temp = temp_B;
					}
					result[i%(distance+1)] = temp_B;
				}
			}
		}
	} else {
		temp = *max_element(input.begin(), input.end());
	}
	return temp;
}



int main(int argc, char* argv[]) {
  fstream input(argv[1]);
  ofstream output(argv[2]);
  vector<int> input_arr;
  char mode;
  int number, distance, max_value;
  bool finished_input = false;
  vector<int> tmp;
  string line;

  while (getline(input, line)) {
    if (line[0] == 'A') {
		  istringstream in(line);
		  in >> mode >> distance;
      max_value = maximum_sub(input_arr, distance, mode);
			//cout << "mode A: maximum value is " << max_value << endl;
      output << max_value << endl;
	  } else if (line[0] == 'B') {
      istringstream in(line);
      in >> mode >> distance;
      max_value = maximum_sub(input_arr, distance, mode);
			//cout << "mode B: maximum value is " << max_value << endl;
      output << max_value << endl;
    } else if (line[0] == 'C') {
      istringstream in(line);
      in >> mode >> distance;
      max_value = maximum_sub(input_arr, distance, mode);
			//cout << "mode C: maximum value is " << max_value << endl;
      output << max_value << endl;
    } else {
      istringstream in(line);
			input_arr.reserve(line.size());
      if (input_arr.empty() == false) {
        input_arr.clear();
        input_arr.swap(tmp);
      }
      while ( in >> number) {
      	input_arr.push_back(number);
      }
    }
  }
  input.close();
  output.close();
  return 0;
}