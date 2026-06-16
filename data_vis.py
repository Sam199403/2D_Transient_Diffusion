from pathlib import Path 
import sys
import numpy as np 
import matplotlib.pyplot as plt
import time 

if len(sys.argv) > 1:
    folder_name = sys.argv[1]
else:
    folder_name = "Data" # Default folder name if not provided as a command-line argument


data_dir = Path(folder_name) # data_dir contains folder object for Data, containing the csv files

csv_files = sorted(data_dir.glob("solution_grid_*.csv")) 
# Note: glob is a method of Path objects that returns a list of all the files in the directory....
# that match the specified pattern. In this case, it looks for files that start with "solution_grid_" and end with ".csv".

T_0 = np.genfromtxt(csv_files[0], delimiter=',') # This reads the first csv file in the list of csv files and stores it as a 2D array in T_0
T_0 = T_0[:,:-1] # This removes the last column of T_0


# print(f"T_0 shape: {T_0.shape}")
# print(f"T_0 data type: {T_0.dtype}")

fig, ax = plt.subplots() 
len_domain = 1
num_points = T_0.shape[0]


x = np.linspace(0,len_domain,num_points)
y = np.linspace(0,len_domain,num_points)
xv,yv = np.meshgrid(x,y)

def plot_helper(T):
    contour_plot = plt.contourf(xv,yv,T,cmap = 'coolwarm',levels = 10)
    
    
    plt.xlabel("X Co-ordinate")
    plt.ylabel("Y Co-ordinate")
    
    return contour_plot


cntr = plot_helper(T_0)
plt.colorbar(cntr)
plt.gca().invert_yaxis()
total_frames = len(csv_files)
print(f"Total frames in animation: {total_frames}")

desired_duration = float(input("Enter the desired duration of the animation in seconds: "))

# iter_frame = 0
choice_var = 0

while choice_var == 0:
    start_time = time.perf_counter()
    for csv_file in csv_files:
        T = np.genfromtxt(csv_file, delimiter=',')
        T = T[:,:-1]
        cntr = plot_helper(T)
        # iter_frame = iter_frame + 1
        # print(iter_frame)
        
        plt.pause(0.0001) # pause duration computed from number of files and fps
        if time.perf_counter() - start_time > desired_duration:
            print("Desired duration reached, stopping animation.")
            break
        
    print("Animation complete.")
    print("Do you want to change time duration and run animation again?")
    choice = input("Enter 0 to continue or any other key to exit: ")
    if choice == "0":
        desired_duration = float(input("Enter the desired duration of the animation in seconds: "))
        iter_frame = 0
    else:
        choice_var = 1
        print("Exiting animation loop.")

# plt.show()