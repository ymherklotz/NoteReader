// A4 Paper Dimensionsa
#define HEIGHT 2481
#define WIDTH 3510
#define BOXHEIGHT 5
#define BOXWIDTH 32
#define BOXBORDER 10
#define BORDER 20
#define GRIDNUM 1
#define DOTSIZE 20

#include <opencv2/core/core.hpp>

void initGrid(cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&);
void addBox(cv::Mat&, int, int);
void colourRect(cv::Mat &, cv::Vec3b colour);
