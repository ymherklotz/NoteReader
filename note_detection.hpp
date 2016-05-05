#include <opencv2/core/core.hpp>

// constant definitions
#define HEIGHT 220
#define NOTEHEIGHT 90
#define START 393     // FIRST LINE: 393; SECOND LINE: 685
#define THRESHOLD 10
#define LINEAPPROXIMATION 3
#define STAVENUM 6
#define TOPLIMIT 25
#define BOTLIMIT 14
#define NOTEBIT 5
#define NOTEDIFF 120

// all the functions used
void drawRowInt(cv::Mat&, int*);
void findHorizLines(cv::Mat&, int*);
void drawColInt(cv::Mat&, int*);
void findVertLines(cv::Mat&, int*);
void findPeaks(int*, int, int*, int&);
void findNotes(int*, int&, int*, int&);
void drawNoteLoc(cv::Mat&, int*, int&);
void extractNotes(int*, int&, int*, int&);
bool inside(int*, int, int);
void findPitch(cv::Mat&, int*, int*, int&, int*, int*);
void getNotePitch(int*, int*, int*, int&);
void findRests(int*, int*, int*, int(*)[2], int&, int&);
void getKey(int, char, bool*);
