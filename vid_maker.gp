# Make a heatmap frame for every CSV in a target folder, then encode the frames as MP4.
# Run with:
#   gnuplot -c gnu_script.gp study_1
#
# Or run without the folder argument and enter the folder name when prompted:
#   gnuplot gnu_script.gp

data_dir = exists("ARG1") ? ARG1 : ""

if (strlen(data_dir) == 0) {
    data_dir = system('powershell -NoProfile -Command "Read-Host ''Target folder''"')
}

frame_dir = data_dir . "/frames"
video_out = data_dir . "/animation.mp4"
meta_file = data_dir . "/meta_data.txt"
fps = 30

if (strlen(data_dir) == 0) {
    print "No target folder was provided."
    exit
}

set datafile separator comma
set datafile missing ""

# Create/refresh the frame directory.
system(sprintf('if not exist "%s" mkdir "%s"', frame_dir, frame_dir))
system(sprintf('del /q "%s\\frame_*.png" 2>NUL', frame_dir))

# On Windows, dir /b returns the matching file names without the folder prefix.
files = system(sprintf('dir /b "%s\\solution_grid_*.csv"', data_dir))
n_files = words(files)

if (n_files == 0) {
    print sprintf("No CSV files found in %s", data_dir)
    exit
}

last_meta_line = system(sprintf('powershell -NoProfile -Command "Get-Content -LiteralPath ''%s'' -Tail 1"', meta_file))
n_meta_values = words(last_meta_line)

if (n_meta_values == 0) {
    print sprintf("No numeric values found in the last line of %s", meta_file)
    exit
}

cbar_min = real(word(last_meta_line, 1))
cbar_max = cbar_min

do for [i=2:n_meta_values] {
    value = real(word(last_meta_line, i))
    cbar_min = value < cbar_min ? value : cbar_min
    cbar_max = value > cbar_max ? value : cbar_max
}

print sprintf("Using color range [%g:%g] from %s", cbar_min, cbar_max, meta_file)

set terminal pngcairo size 900,900 enhanced font "Arial,12"
set view map
set size ratio -1
set border linewidth 1
set tics out
set xlabel "x index"
set ylabel "y index"
set cblabel "Temperature"
set palette rgb 33,13,10
set cbrange [cbar_min:cbar_max]
set yrange [] reverse

do for [i=1:n_files] {
    csv_file = sprintf("%s/%s", data_dir, word(files, i))
    png_file = sprintf("%s/frame_%06d.png", frame_dir, i)

    set output png_file
    set title sprintf("Temperature Distribution - %s", word(files, i))

    # CSVs have a trailing comma, so the final matrix column is empty.
    # The expression below converts empty/NaN cells to undefined and plots only real values.
    plot csv_file matrix using 1:2:($3 == $3 ? $3 : 1/0) with image notitle
}

unset output
set terminal qt

system(sprintf('ffmpeg -y -framerate %d -i "%s\\frame_%%06d.png" -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2" -pix_fmt yuv420p "%s"', fps, frame_dir, video_out))

print sprintf("Saved video to %s", video_out)
