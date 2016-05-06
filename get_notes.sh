echo -ne "\e[0;37m[ \e[0;31m....\e[0;37m ] Converting\e[1;36m \"$1\"\e[0;37m to\e[1;35m \"./program_files/score.jpg\"\r" 
convert -interlace none -density 150x150 -quality 50 "$1" "./program_files/score.jpg";
echo -ne "\e[0;37m[ \e[0;32mDone\e[0;37m ] Converting\e[1;36m \"$1\"\e[0;37m to\e[1;35m \"./program_files/score.jpg\"\n" 
echo -ne "\e[0;37m[ \e[0;31m....\e[0;37m ] Compiling\e[1;36m \"./note_detection.cpp\"\r" 
g++ note_detection.cpp -o note_detection -fpermissive `pkg-config --libs opencv`;
echo -ne "\e[0;37m[ \e[0;32mDone\e[0;37m ] Compiling\e[1;36m \"./note_detection.cpp\"\n" 
echo -ne "\e[0;37m[ \e[0;31m....\e[0;37m ] Compiling \e[1;36m\"./draw_grid.cpp\"\r" 
g++ draw_grid.cpp -o draw_grid -fpermissive `pkg-config --libs opencv`;
echo -ne "\e[0;37m[ \e[0;32mDone\e[0;37m ] Compiling \e[1;36m\"./draw_grid.cpp\"\n" 
echo -ne "\e[0;37m[ \e[0;31m....\e[0;37m ] executing\e[1;36m \"./note_detection\"\e[0;37m on\e[1;35m \"./program_files/score.jpg\"\r" 
./note_detection "./program_files/score.jpg" 0;
echo -ne "\e[0;37m[ \e[0;32mDone\e[0;37m ] executing\e[1;36m \"./note_detection\"\e[0;37m on\e[1;35m \"./program_files/score.jpg\"\n" 
mv "./program_files/noteBits.txt" "./program_files/noteBits1.txt";
./note_detection "./program_files/score.jpg" 1;
mv "./program_files/noteBits.txt" "./program_files/noteBits2.txt";
echo -ne "\e[0;37m[ \e[0;31m....\e[0;37m ] drawing the grid\r" 
./draw_grid
echo -ne "\e[0;37m[ \e[0;32mDone\e[0;37m ] drawing the grid\n" 
