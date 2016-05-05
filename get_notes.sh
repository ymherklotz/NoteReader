convert -interlace none -density 150x150 -quality 50 "$1" "./score.jpg";
g++ note_detection.cpp -o note_detection -fpermissive `pkg-config --libs opencv`;
g++ draw_grid.cpp -o draw_grid -fpermissive `pkg-config --libs opencv`;
./note_detection score.jpg 0;
mv "./noteBits.txt" "./noteBits1.txt";
./note_detection score.jpg 1;
mv "./noteBits.txt" "./noteBits2.txt";
./draw_grid
