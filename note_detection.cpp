#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "note_detection.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	// getting input for the file
	char* imageName = argv[1];

	int n;
	
	if(argc > 2) {
	    char* input = argv[2];
	    n = int(input[0])-48;
	} else {
		n = 0;
	}

	// initialising matrices
	Mat image, imagebw;
	Mat cut, cut2, cutbw, notes, notesbw;
	
	// Arrays that will contain the different components
	int rowInt[HEIGHT];
	int barLineLoc[THRESHOLD];
	int staveLoc[STAVENUM];
	int finalNotes[THRESHOLD*4];
	int outPitch[THRESHOLD*4];
	int noteType[THRESHOLD*4];
	int notePitch[THRESHOLD*4];
	int noteRests[THRESHOLD*4][2];
	bool notesBool[THRESHOLD*4][NOTEBIT];

	// Initialisation of the counting variables for the arrays
	int barLineLocSize;
	int staveLocSize;
	int noteLocSize;
	int noteNum;
	int noteRestNum;

	// Making filestreams for text output
	ofstream noteStream;

	// opening images into Mat
	image = imread(imageName, 1);
	imagebw = imread(imageName, 0);

	// Array that will contain cols
	int colInt[imagebw.cols];
	int noteLoc[imagebw.cols];

	if(!image.data) {
		printf("No image data n");
		return -1;
	}

	const int start = START+n*(685-393);
	// cuts parts of the matrix and then copies them
	Mat cut_off(image, Rect(0, start, image.cols, HEIGHT));
	Mat cut_offbw(imagebw, Rect(0, start, image.cols, HEIGHT));
	Mat note_cut(image, Rect(0, start, image.cols, NOTEHEIGHT));
	Mat note_cutbw(imagebw, Rect(0, start, image.cols, NOTEHEIGHT));

	// duplicates the images so that they can be changed without changing
	// the original
	cut_off.copyTo(cut);
	cut.copyTo(cut2);
	cut_offbw.copyTo(cutbw);
	note_cut.copyTo(notes);
	note_cutbw.copyTo(notesbw);

	// finds the lengths of the vertical lines and horizontal pixels
	findVertLines(cut_offbw, rowInt);
	findHorizLines(cut_offbw, colInt);

	// Finds the locations and size of the barLines
	findPeaks(rowInt, HEIGHT, staveLoc, staveLocSize);
	staveLoc[5] = staveLoc[4] - staveLoc[0] + staveLoc[1];
	findPeaks(colInt, imagebw.cols, barLineLoc, barLineLocSize);

	// finds the notes
	findNotes(colInt, imagebw.cols, noteLoc, noteLocSize);

	// extract the single notes
	extractNotes(noteLoc, noteLocSize, finalNotes, noteNum);

	// This section will find the pitch of the notes
	findPitch(notesbw, staveLoc, finalNotes, noteNum, outPitch, noteType);

	getNotePitch(staveLoc, outPitch, notePitch, noteNum);
	
	findRests(notePitch, finalNotes, noteType, noteRests, noteNum, noteRestNum);

	for(int i = 0; i < noteRestNum; ++i) {
		getKey(noteRests[i][1], 'c', notesBool[i]);
	}
	
	// draws them to a file so that we can view and debug them
	drawRowInt(cut, rowInt);
	drawColInt(cut2, colInt);
	drawNoteLoc(notes, finalNotes, noteNum);

	// Writing all the outputs to files for logging
	imwrite("./program_files/cut_out.jpg", cut_offbw);
	imwrite("./program_files/cut_hist_horiz.jpg", cut);
	imwrite("./program_files/cut_hist_vert.jpg", cut2);
	imwrite("./program_files/note_detected.jpg", notes);
	imwrite("./program_fiels/pitch_detect.jpg", notesbw);

	// Creating text files with the data for external access
	noteStream.open("./program_files/note.txt");
	for(int i = 0; i < noteRestNum; ++i) {
		noteStream << noteRests[i][0] << " " << noteRests[i][1] << endl;
	}  
	noteStream.close();

	noteStream.open("./program_files/noteBits.txt");
	noteStream << noteRestNum << endl;
	for(int i = 0; i < noteRestNum; ++i) {
		for(int j = 0; j < NOTEBIT; ++j) {
			noteStream << notesBool[i][j];
		}
		noteStream << endl;
	}
	noteStream.close();

	return 0;
}

void findVertLines(Mat &image, int* rowInt) {
	// goes through the image from left to right and counts number of
	// black pixels
	for(int y = 0; y < image.rows; ++y) {
		int count = 0;
		for(int x = 0; x < image.cols; ++x) {
			if(image.at<uchar>(Point(x, y)) < 20) {
				++count;
			}
		}
		rowInt[y] = count;
	}
}

void findHorizLines(Mat &image, int* colInt) {
	// Same but from top to bottom
	for(int x = 0; x < image.cols; ++x) {
		int count = 0;
		for(int y = 0; y < image.rows/2; ++y) {
			if(image.at<uchar>(Point(x, y)) < 20) {
				++count;
			}
		}
		colInt[x] = count;
	}
}

void drawRowInt(Mat &image, int* rowInt) {
	// uses the array to draw the amount of pixels
	for(int y = 0; y < image.rows; ++y) {
		for(int x = 0; x < image.cols; ++x) {
			if(x < rowInt[y]) {
				image.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 255);
			}
		}
	}
}

void drawColInt(Mat &image, int* colInt) {
	for(int x = 0; x < image.cols; ++x) {
		for(int y = 0; y < image.rows; ++y) {
			if(y < colInt[x]) {
				image.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 255);
			}
		}
	}
}

void findPeaks(int* inputPeaks, int inLen, int* output, int &outLen) {
	// calculates any peaks with a specific threshold
	int max = inputPeaks[0];
	int min, prev;
	for(int i = 0; i < inLen; ++i) {
		if(inputPeaks[i] > max) {
			max = inputPeaks[i];
		}
	}
	// sets the min according to a percentage of the max so that
	// it scales correctly
	min = max - max / THRESHOLD;
	prev = 0;
	outLen = 0;
	for(int i = 0; i < inLen; ++i) {
		// if there wasn't a peak before and the input is larger
		if(inputPeaks[i] > min && i-1 > prev) {
			output[outLen++] = i;
			// updates the previous peak
			prev = i + THRESHOLD / 2;
		}
	}
}

void findNotes(int* inputPeaks, int &inLen, int* output, int &outLen) {
	// finds the notes between a certain threshold
	outLen = 0;
	for(int i = 0; i < inLen; ++i) {
		if(inputPeaks[i] < TOPLIMIT && inputPeaks[i] > BOTLIMIT) {
			output[outLen++] = i;
		}
	}
}

void drawNoteLoc(Mat &image, int* noteLoc, int &noteLocSize) {
	// draws notes according to input
	for(int y = 0; y < image.rows; ++y) {
		for(int x = 0; x < image.cols; ++x) {
			if(inside(noteLoc, noteLocSize, x)) {
				image.at<Vec3b>(Point(x, y)) = Vec3b(255, 0, 0);
			}
		}
	}
}

void extractNotes(int* input, int &inLen, int *tmpArr, int &outLen) {
	int i = 0;
	int output[inLen];
	outLen = 0;
	while(i < inLen) {
	    int count = 0;
		for(int j = input[i]; j < input[i]+THRESHOLD; ++j) {
			if(inside(&input[i], THRESHOLD, j)) {
				++count;
			}
		}
		if(count > THRESHOLD-4) {
			output[outLen++] = input[i];
			i += THRESHOLD;
		}
		++i;
	}
	int tmpLen = 0;
	bool changed = false;
	for(int i = 0; i < outLen-1; ++i) {
		if(output[i+1] - output[i] > THRESHOLD*2) {
			if(changed) {
			    tmpArr[tmpLen++] = output[i]-10;
				changed = false;
			} else {
				tmpArr[tmpLen++] = output[i]-2;
			}
		} else {
			changed = true;
		}
	}
	if(changed) {
	    tmpArr[tmpLen++] = output[outLen-1]-10;
	} else {
	    tmpArr[tmpLen++] = output[outLen-1]-2;
	}
	outLen = tmpLen;
}

bool inside(int* arr, int arrSize, int num) {
	for(int x = 0; x < arrSize; ++x) {
		if(arr[x] == num) {
			return true;
		}
	}
	return false;
}

void findPitch(Mat &inputLine, int* stave, int* inNote, int &inLen, int* outPitch, int* noteType) {
	// Check where the first note is
	// define pixels compared to the stave lines to detect

	for(int i = 0; i < inLen; ++i) {
		int totalCount[2] = {0, 0};
		int count2 = 0;
		bool tail = false;
		int average = 0;
		int avgCount = 0;
		for(int j = 0; j < 25; ++j) {
		    int count = 0;
			for(int y = 0; y < inputLine.rows; ++y) {
				if(inputLine.at<uchar>(Point(inNote[i]+j, y)) < 127 && !inside(stave, STAVENUM, y) && !inside(stave, STAVENUM, y-1) && !inside(stave, STAVENUM, y-2)) {
					++count;
					inputLine.at<uchar>(Point(inNote[i]+j, y)) = 127;
				}
			}
			if(count > totalCount[0] && count < 15) {
				totalCount[0] = count;
				totalCount[1] = j;
			} else if (count > 15) {
				tail = true;
			}
			if(count < 15) {
				count2 += count;
			}
		}
		
        for(int y = 0; y < inputLine.rows; ++y) {
		    if(inputLine.at<uchar>(Point(inNote[i]+totalCount[1], y)) < 128 && !inside(stave, STAVENUM, y) && !inside(stave, STAVENUM, y-1) && !inside(stave, STAVENUM, y-2)) {
				average += y;
				++avgCount;
			}
		}
		average /= avgCount;
		outPitch[i] = average;
        for(int y = 0; y < inputLine.rows; ++y) {
			inputLine.at<uchar>(Point(inNote[i]+totalCount[1], y)) = 0;
		}
		if(count2 > 90 && !tail) {
			noteType[i] = 2;
		} else if(count2 > 90 && tail){
			noteType[i] = 0;
		} else {
			noteType[i] = 1;
		}
	}
}

void getNotePitch(int *stave, int *loc, int *pitch, int &noteNum) {
	for(int i = 0; i < noteNum; ++i) {
		for(int j = 0; j < STAVENUM; ++j) {
			int test[5] = {loc[i]-2, loc[i]-1, loc[i], loc[i]+1, loc[i]+2};
			if(inside(test, 5, stave[j])) {
				if(i > 0 && pitch[i-1] == 11-2*j) {
				    pitch[i] = 31;
				} else {
					pitch[i] = 11-2*j;
				}
			} else {
				if(inside(test, 5, stave[j]+(stave[j+1]-stave[j])/2)) {
				    if(i > 0 && pitch[i-1] == 10-2*j) {
					    pitch[i] = 31;
				    } else {
					    pitch[i] = 10-2*j;
					}
				}
			}
		}
	}
}

void findRests(int *notePitch, int *noteLoc, int *noteType, int (*finalNotes)[2], int &noteNum, int &restNoteNum) {
    restNoteNum = 0;
	for(int i = 0; i < noteNum; ++i) {
		if(noteType[i] == 0) {
			finalNotes[restNoteNum][1] = notePitch[i];
			finalNotes[restNoteNum++][0] = noteType[i];
			if(i < noteNum-1 && (noteLoc[i+1] - noteLoc[i]) > NOTEDIFF) {
			    finalNotes[restNoteNum][1] = 0;
			    finalNotes[restNoteNum++][0] = -1;
			}
		} else if(noteType[i] == 1) {
			finalNotes[restNoteNum][1] = notePitch[i];
			finalNotes[restNoteNum++][0] = noteType[i];
			finalNotes[restNoteNum][1] = notePitch[i];
			finalNotes[restNoteNum++][0] = noteType[i];
		    if(i < noteNum-1 && (noteLoc[i+1] - noteLoc[i]) > 2*NOTEDIFF) {
			    finalNotes[restNoteNum][1] = 0;
			    finalNotes[restNoteNum++][0] = -1;
			}
		} else if(noteType[i] == 2) {
			finalNotes[restNoteNum][1] = notePitch[i];
			finalNotes[restNoteNum++][0] = noteType[i];
			finalNotes[restNoteNum][1] = notePitch[i];
			finalNotes[restNoteNum++][0] = noteType[i];
			finalNotes[restNoteNum][1] = notePitch[i];
			finalNotes[restNoteNum++][0] = noteType[i];
			finalNotes[restNoteNum][1] = notePitch[i];
			finalNotes[restNoteNum++][0] = noteType[i];
		    if(i < noteNum-1 && (noteLoc[i+1] - noteLoc[i]) > 4*NOTEDIFF) {
			    finalNotes[restNoteNum][1] = 0;
			    finalNotes[restNoteNum++][0] = -1;
			}
		} else {
			cout << "Error" << endl;	
		}
	}
}

void getKey(int note, char key, bool *output) {
	bool c_maj[11][5] ={{false, false, true, true, true}, 
					{false, true, false, false, true},
					{false, true, false, true, true},
					{false, true, true, false, false},
					{false, true, true, true, false},
					{true, false, false, false, false},
					{true, false, false, true, false},
					{true, false, false, true, true},
					{true, false, true, false, true},
					{true, false, true, true, true},
						{true, true, false, false, false}};
	bool d_maj[11][5] ={{false, true, false, false, false},
					{false, true, false, false, true}, 
					{false, true, false, true, true},
					{false, true, true, false, true},
					{false, true, true, false, false},
					{true, false, false, false, false},
					{true, false, false, true, false},
					{true, false, true, false, false},
					{true, false, true, false, true},
					{true, false, true, true, true},
					{true, true, false, false, true}};
	bool e_maj[11][5] ={{false, true, false, false, false},
					{false, true, false, true, false},
					{false, true, false, true, true},
					{false, true, true, false, true},
					{false, true, true, true, true},
					{true, false, false, false, false},
					{true, false, false, true, false},
					{true, false, true, false, false},
					{true, false, true, true, false},
					{true, false, true, true, true},
					{true, true, false, false, true}};
	bool f_maj[11][5] ={{false, false, true, true, true},
					{false, true, false, false, true},
					{false, true, false, true, true},
					{false, true, true, false, false},
					{false, true, true, true, false},
					{true, false, false, false, false},
					{true, false, false, false, true},
					{true, false, false, true, true},
					{true, false, true, false, true},
					{true, false, true, true, true},
					{true, true, false, false, false}};
	bool r_maj[11][5] ={{false, true, false, false, false},
					{false, true, false, true, false},
					{false, true, true, false, false},
					{false, true, true, false, true},
					{false, true, true, true, true},
					{true, false, false, false, true},
					{true, false, false, true, false},
					{true, false, true, false, false},
					{true, false, true, true, false},
					{true, true, false, false, false},
					{true, true, false, false, true}};
	bool g_maj[11][5] ={{false, false, true, true, true},
					{false, true, false, false, true},
					{false, true, false, true, true},
					{false, true, true, false, true},
					{false, true, true, true, false}, 
					{true, false, false, false, false},
					{true, false, false, true, false},
					{true, false, false, true, true},
					{true, false, true, false, true},
					{true, false, true, true, true},
					{true, true, false, false, true}};
	bool a_maj[11][5] ={{false, true, false, false, false},
					{false, true, false, false, true},
					{false, true, false, true, true},
					{false, true, true, false, true},
					{false, true, true, true, true},
					{true, false, false, false, false}, 
					{true, false, false, true, false},
					{true, false, true, false, false},
					{true, false, true, false, true},
					{true, false, true, true, true},
					{true, true, false, false, true}};
	bool b_maj[11][5] ={{false, true, false, false, false},
					{false, true, false, true, false},
					{false, true, false, true, true},
					{false, true, true, false, true},
					{false, true, true, true, true},
					{true, false, false, false, true},
					{true, false, false, true, false}, 
					{true, false, true, false, false},
					{true, false, true, true, false},
					{true, false, true, true, true},
					{true, true, false, false, true}};
	if (note == 0) {
		bool rest[5] = {false, false, false, false, false};
		for(int i = 0; i < 5; ++i) {
			output[i] = rest[i];
		}
	}
	else if (note == 31) {
		bool repeat[5] = {true, true, true, true, true};
		for(int i = 0; i < 5; ++i) {
			output[i] = repeat[i];
		}
	}
	else
	{
		bool key_arr[11][5];
		for(int i = 0; i < 11; ++i) {
			for(int j = 0; j < 5; ++j) {
		        switch (key) {
			        case 'c':
				        key_arr[i][j] = c_maj[i][j];
				        break;
			        case 'd':
				        key_arr[i][j] = d_maj[i][j];
				        break;
			        case 'e':
				        key_arr[i][j] = e_maj[i][j];
				        break;
			        case 'f':
				        key_arr[i][j] = f_maj[i][j];
		         		break;
	         		case 'r':
				        key_arr[i][j] = r_maj[i][j];
        				break;
        			case 'g':
				        key_arr[i][j] = g_maj[i][j];
        				break;
	        		case 'a':
				        key_arr[i][j] = a_maj[i][j];
	        			break;
        			case 'b':
				        key_arr[i][j] = b_maj[i][j];
	        			break;
				}
			}
		}
		for(int i = 0; i < 5; ++i) {
			output[i] = key_arr[note-1][i];
		}
	}
}
