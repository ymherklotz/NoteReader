#include <iostream>
#include <fstream>
#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "draw_grid.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	Mat firstGrid, secondGrid, thirdGrid, redPix1, redPix2;
	Mat A4Grid(HEIGHT, WIDTH, CV_8UC3, Scalar(255, 255, 255));

	int noteNum;
	
	fstream inputBits;

	// get input
	inputBits.open("./program_files/noteBits1.txt");
	inputBits >> noteNum;

	int notes[BOXWIDTH][BOXHEIGHT];
	string tmpStr;

	for(int i = 0; i < noteNum; ++i) {
		inputBits >> tmpStr;
		for(int j = 0; j < BOXHEIGHT; ++j) {
			notes[i][j] = int(tmpStr[j])-48;
		}
	}
	inputBits.close();
	
	int tmpNote = noteNum;
	inputBits.open("./program_files/noteBits2.txt");
	inputBits >> noteNum;

	for(int i = tmpNote; i < noteNum+tmpNote; ++i) {
		inputBits >> tmpStr;
		for(int j = 0; j < BOXHEIGHT; ++j) {
			notes[i][j] = int(tmpStr[j])-48;
		}
	}
	inputBits.close();

	initGrid(A4Grid, firstGrid, secondGrid, thirdGrid, redPix1, redPix2);
	for(int y = 0; y < BOXHEIGHT; ++y) {
		for(int x = 0; x < BOXWIDTH; ++x) {
			if(x < tmpNote+noteNum) {
			    if(!(notes[x][y] == 1)) {
				    addBox(firstGrid, x, y);
			    }
			} else {
				addBox(firstGrid, x, y);
			}
		}
	}

	colourRect(redPix1, Vec3b(0, 0, 255));

	imwrite("./program_files/A4Grid.jpg", A4Grid);
	return 0;
}

void initGrid(Mat &A4image, Mat &row1, Mat &row2, Mat &row3, Mat &redPix1, Mat &redPix2) {
	row1 = A4image(Rect(BORDER, BORDER, WIDTH-2*BORDER, HEIGHT/GRIDNUM-2*BORDER));
	//row2 = A4image(Rect(BORDER, HEIGHT/GRIDNUM+BORDER/2, WIDTH-2*BORDER, HEIGHT/GRIDNUM-2*BORDER));
	//row3 = A4image(Rect(BORDER, 2*(HEIGHT/GRIDNUM)+BORDER/2, WIDTH-2*BORDER, HEIGHT/GRIDNUM-2*BORDER));

	redPix1 = A4image(Rect(2*BORDER, 2*BORDER, DOTSIZE, DOTSIZE));
	redPix2 = A4image(Rect(WIDTH-DOTSIZE-BORDER, 2*BORDER, DOTSIZE, DOTSIZE));
	
	colourRect(row1, Vec3b(0, 0, 0));
	//colourRect(row2, Vec3b(0, 0, 0));
	//colourRect(row3, Vec3b(0, 0, 0));
	colourRect(redPix1, Vec3b(0, 0, 255));
	colourRect(redPix2, Vec3b(0, 0, 255));
}

void addBox(Mat &rect, int xC, int yC) {
	Mat rectCut = rect(Rect(BORDER, BORDER, rect.cols-2*BORDER, rect.rows-2*BORDER));

	double xLd = (double(rectCut.cols)-BOXBORDER*(BOXWIDTH-1))/BOXWIDTH;
	double yLd = (double(rectCut.rows)-BOXBORDER*(BOXHEIGHT-1))/BOXHEIGHT;
	double xd = double(xC)*(xLd+BOXBORDER);
	double yd = double(yC)*(yLd+BOXBORDER);

	int xL = int(xLd);
	int yL = int(yLd);
	int x = int(xd);
	int y = int(yd);
	
	Mat box = rectCut(Rect(x, y, xL, yL));
	
	colourRect(box, Vec3b(255, 255, 255));
}

void colourRect(Mat &inRect, Vec3b colour) {
	for(int y = 0; y < inRect.rows; ++y) {
		for(int x = 0; x < inRect.cols; ++x) {
			inRect.at<Vec3b>(Point(x, y)) = colour;
		}
	}
}
