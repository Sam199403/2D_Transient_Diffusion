set terminal qt

set datafile separator comma

set title "Temperature Distribution"

set view map
set pm3d map
set colorbox

set cbrange [0:20]

set size ratio -1

splot 'vid_exp/solution_grid_000100.csv' matrix notitle