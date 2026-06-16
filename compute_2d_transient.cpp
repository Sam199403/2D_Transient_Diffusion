#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>

int print_matrix(std::vector<std::vector<double>>& arr_2d){
    size_t sz = arr_2d.size(); 
    if (sz>10){
        std::cout<<"Array too big to display. Computations will proceed normally"<<std::endl;
        return 1;    
        
    }
    for (size_t i = 0;i<arr_2d.size();i++) {

        for (size_t j = 0; j < arr_2d[i].size();j++) {

            std::cout<<arr_2d[i][j]<<" "; //space after each element is printed
        }
    std::cout << "\n"; //Print new line after each row is completely printed 
    }
std::cout<<std::endl;
return 0;    
}

void apply_bc(std::vector<std::vector<double>>& arr_2d,double left_bc, double right_bc, double top_bc, double bottom_bc){
    size_t sz = arr_2d[0].size();
    // Loop to apply top wall boundary condition
    for (int j = 0; j<sz; j++){
        arr_2d[0][j] = top_bc;
    }
    // Loop to apply left wall boundary condition 
    for (int i=1; i<(sz-1) ; i++){
        
        arr_2d[i][0] = left_bc;

        
    }
    // Loop to apply right wall boundary condition 
    for (int i = 1;i<(sz - 1);i++){
        
        arr_2d[i][sz-1] = right_bc;
        
        
        
    }
    // Loop to apply bottom wall boundary condition 
    for (int j = 0; j<sz; j++){
        
        arr_2d[sz-1][j] = bottom_bc;
        
        
    }
    
}

// Function to perform numerical computation 
void compute_2d(std::vector<std::vector<double>> arr_2d,std::vector<std::vector<double>>& arr_2d_new,double beta){
    size_t sz = arr_2d.size();
    
    for (int i = 1; i<(sz - 1); i++){
        for (int j = 1;j<(sz - 1); j++){
            arr_2d_new[i][j] = arr_2d[i][j] + beta * (arr_2d[i+1][j] + arr_2d_new[i-1][j] + arr_2d[i][j+1] + arr_2d_new[i][j-1] - 4*arr_2d[i][j]);
            
        }
        
    }

    
}

double error_calc_2d(std::vector<std::vector<double>> arr_2d,std::vector<std::vector<double>> arr_2d_new){
    
    double err_sum  = 0;
    double diff_temp;
    
    size_t sz_1 = arr_2d.size();
    size_t sz_2 = arr_2d_new.size();
    
    if (sz_1 != sz_2){
        
        std::cout<<"The array dimensions do not match. Please re-check dimensions"<<std::endl;
        return 0.0;
    }
    for (int i = 0;i<sz_1;i++){
        
        for (int j = 0; j<sz_1;j++){
            
            diff_temp = 0;
            diff_temp = std::abs(arr_2d[i][j] - arr_2d_new[i][j]);
            err_sum = err_sum + diff_temp;
            
        }
    }
    return err_sum;
    
    
}

// Function to write the solution matrix into a csv file 
void write_sol_csv(const std::vector<std::vector<double>> &arr_2d,const std::string &filename){

    std::ofstream file(filename);
    for (size_t i = 0; i<arr_2d.size(); i++){

        for (size_t j = 0; j<arr_2d[0].size(); j++){
            file << arr_2d[i][j];
            file << ",";
        }
    file << '\n';    
}
file.close();
}

void save_meta_data(const std::string &filename, double alpha, double dt, double h, double beta, double err_tol, int max_iter, double top_wall_val, double bottom_wall_val, double left_wall_val, double right_wall_val){
    std::ofstream file(filename);
    file << "Thermal diffusivity (alpha): " << alpha << "\n";
    file << "Time step (dt): " << dt << "\n";
    file << "Grid spacing (h): " << h << "\n";
    file << "Beta multiplier: " << beta << "\n";
    file << "Error tolerance: " << err_tol << "\n";
    file << "Maximum iterations: " << max_iter << "\n";
    file<<top_wall_val<<" "<<bottom_wall_val<<" "<<left_wall_val<<" "<<right_wall_val<<"\n";
    file.close();
}




// Start of main function 
int main(int argc, char* argv[]){

    std::string study_name;
  
    if (argc < 2) {
        std::cout << "No string argument was provided!" << std::endl;
        std::cout<<"Study data will be stored in default directory named 'Data' in the current working directory"<<std::endl;
        study_name = "Data";
    } else {
        study_name = argv[1];
    }
std::filesystem::create_directory(study_name);

// Define domain size and number of divisions 
double dom_len = 1.0; // Length of the square domain in both x and y directions. Have to initilize as double to avoid integer division in the subsequent calculations.
int num_div; 
std::cout<<"Enter number of divisions to divide the domain into"<<std::endl;
std::cin>>num_div;

double h = dom_len / (num_div - 1); // Length of each grid cell
double alpha; 
std::cout<<"Enter the thermal diffusivity value: "<<std::endl;
std::cin>>alpha;

double dt;
std::cout<<"Enter the time step value: "<<std::endl;
std::cin>>dt;
double beta = alpha*dt/(h*h); // Stability parameter for the numerical scheme
double stab_val = (h*h)/(4*alpha); // Stability value for the numerical scheme

if (dt > stab_val){
    std::cout<<"The time step value is too large for the given grid size and thermal diffusivity. Proceeding with an appropriate time step value"<<std::endl;
    dt = stab_val * 0.9; // Use a smaller time step value
    beta = alpha * dt / (h * h);
}

std::cout<<"The beta multiplier is "<<beta<<std::endl;
// Define boundary conditions 
double top_wall_val;
double bottom_wall_val;
double left_wall_val;
double right_wall_val;

// Prompt user to enter boundary conditions 
std::cout<<"Enter the top wall temperature value: "<<std::endl; 
std::cin>>top_wall_val; 
std::cout<<"Enter the bottom wall temperature value: "<<std::endl; 
std::cin>>bottom_wall_val; 
std::cout<<"Enter the left wall temperature value: "<<std::endl; 
std::cin>>left_wall_val; 
std::cout<<"Enter the right wall temperature value: "<<std::endl; 
std::cin>>right_wall_val;
 
// Define error tolerance and maximum allowable iterations 
double err_tol = std::pow(10,-6);
int max_iter = 100000;
// Define and initialize a 2D array of size num_div X num_div . By default it initializes all elements with value 0
std::vector<std::vector<double>> T_sol(num_div,
    std::vector<double>(num_div)

);

// Initialize another 2D solution array to hold the computed results in 
std::vector<std::vector<double>> T_sol_new(num_div,
    std::vector<double>(num_div)

);

// Print the Initialized matrix
int err_code = print_matrix(T_sol);

// Apply boundary condition to the two soliution matrices  
apply_bc(T_sol,left_wall_val,right_wall_val,top_wall_val,bottom_wall_val);
apply_bc(T_sol_new,left_wall_val,right_wall_val,top_wall_val,bottom_wall_val);
std::cout<<std::endl;

// Print the initialized matrix with the applied boundary condtions 
if (err_code == 0){
std::cout<<"Initialized solution matrix after applying boundary conditions"<<std::endl;
print_matrix(T_sol);}

// Start solution computation 
double err_temp = 1;
int iter_count = 0;



int num_store = 100; // Number of iterations after which the solution will be stored in a csv file. This is done to track the convergence history of the solution.

while (err_temp > err_tol && iter_count<max_iter){
    compute_2d(T_sol,T_sol_new,beta); // Note that this function actually modifies the T_sol_new array 
    iter_count = iter_count + 1;
    err_temp = error_calc_2d(T_sol,T_sol_new);
    T_sol = T_sol_new; 
    if (iter_count % num_store == 0){
    // std::string filename = study_name + "/solution_grid_" + std::to_string(iter_count) + ".csv";
    // The following code ensures that filename has no padding and has equal number of digits to ensure easier sorting by the post-processing python script
    std::ostringstream filename_stream;
    filename_stream << study_name << "/solution_grid_" << std::setw(6) << std::setfill('0') << iter_count << ".csv";
    std::string filename = filename_stream.str();
        write_sol_csv(T_sol_new, filename);
    }

    if (iter_count == max_iter && err_temp>err_tol){
        std::cout<<"The solution did not converge within the specified number of iterations"<<std::endl;
        return 0;
        
    }
}

if (err_code == 0){
std::cout<<"The solution matrix is shown below"<<std::endl;
print_matrix(T_sol_new);
}
std::cout<<"The solution converged after "<<iter_count<<" iterations"<<std::endl;
std::cout<<"Residual error is "<<err_temp<<std::endl; 

// Write the results to a csv file 
std::string filename_final_sol = study_name + "/final_solution.csv";
write_sol_csv(T_sol_new, filename_final_sol);

// Save meta data
std::string filename_meta = study_name + "/meta_data.txt";
save_meta_data(filename_meta, alpha, dt, h, beta, err_tol, max_iter, top_wall_val, bottom_wall_val, left_wall_val, right_wall_val);

std::cout<<"The total time it took for the problem to reach steady-state is "<<iter_count*dt<<" seconds"<<std::endl;
return 0;
}