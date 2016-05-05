convert -interlace none -density 150x150 -quality 50 "$1" "./program_files/score.jpg";
g++ note_detection.cpp -o note_detection -fpermissive `pkg-config --libs opencv`;
g++ draw_grid.cpp -o draw_grid -fpermissive `pkg-config --libs opencv`;
./note_detection "./program_files/score.jpg" 0;
mv "./program_files/noteBits.txt" "./program_files/noteBits1.txt";
./note_detection "./program_files/score.jpg" 1;
mv "./program_files/noteBits.txt" "./program_files/noteBits2.txt";
./draw_grid
