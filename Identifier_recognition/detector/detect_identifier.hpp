#ifndef _DETECT_IDENTIFIER_HPP_
#define _DETECT_IDENTIFIER_HPP_


#include <iostream>

#include <string.h>
#include <stdio.h>

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <opencv/cv.h>
#include "opencv/highgui.h"

#include "../../RasberryPi/main_application/error_codes.hpp"

using namespace std;
using namespace cv;

#define BASIC_TEMPLATES_AMOUNT 36
#define BASIC_SYMBOL_TEMPLATES_AMOUNT 26
#define BASIC_DIGIT_TEMPLATES_AMOUNT 10

//Касательно распознавалки сайт http://habrahabr.ru/company/recognitor/blog/225913/

/** Global variables */

static const char error_tag[] = "[ERROR]";
static String identifier_cascade_name = "../Haar/haarcascade\\cascade.xml";
//CascadeClassifier identifier_cascade;

static const char window_name[] = "Capture - Identifier detection";
static const char window_name_with_id_ROI[] = "Detected identifier";
static const char template_dir[] = "../Templates/tanulic/";

static const int min_symbol_width = 55;
static const int min_symbol_height = 85;
static const float min_fill_area = 0.2;
static const float min_ratio = 0.45;
static const float max_ratio = 0.97;

//vector<String> templates_file_names;
//static const char* names[] = { "../data/pic1.png", "../data/pic2.png", "../data/pic3.png", "../data/pic4.png", "../data/pic5.png", "../data/pic6.png", 0 };
static const char *symbols_files [] = { "s0.bmp", "s1.bmp", "s2.bmp", "s3.bmp", "s4.bmp", "s5.bmp", "s6.bmp", "s7.bmp", "s8.bmp", "s9.bmp", "s10.bmp",
										"s11.bmp", "s12.bmp", "s13.bmp", "s14.bmp", "s15.bmp", "s16.bmp", "s17.bmp", "s18.bmp", "s19.bmp", "s20.bmp",
										"s21.bmp", "s22.bmp", "s23.bmp", "s24.bmp", "s25.bmp"};
static const int symbols_files_len = 26;
static const char *digit_files [] = {"n0.bmp", "n1.bmp", "n2.bmp", "n3.bmp", "n4.bmp", "n5.bmp", "n6.bmp", "n7.bmp", "n8.bmp", "n9.bmp"};
static const int digit_files_len = 10;

typedef struct template_information
{
	Mat templ;
	Mat gray;
	String file_name;
	vector<vector<Point> > contours;
	//Mat contour;
} tmpl_inf;


typedef struct symbol_contour
{
	symbol_contour(vector<Point> _contour, int _x, int _y)
	{
		contour = _contour;
		x = _x;
		y = _y;
	}
	int x;
	int y;
	vector<Point> contour;
} smbl_contour;

class Identifier_detector
{
    public:
    	void detectIdentifier( Mat frame);
		void extractIdentifier( Mat frame, char **result); //, vector<tmpl_inf> *templates);
		int loadTemplates(vector<tmpl_inf> *templates, const char ** file, int file_len, const char *file_dir);
		int processTemplates(vector<tmpl_inf> *templates);



		int run_detector(); //main
		Identifier_detector();

    private:
    	//bool compare_two_symbols_contours (smbl_contour c1, smbl_contour c2);


    public:

    private:
    	CascadeClassifier identifier_cascade;
    	RNG rng;
    	vector<tmpl_inf> templates;

};

#endif // _DETECT_IDENTIFIER_HPP_
