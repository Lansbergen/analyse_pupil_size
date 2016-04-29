// video_analysis.cpp (64-bit)
//-----------------------------------------------------------------------//
//
// Program:
// This program was written for trying out openCV functionallity and to
// test image processing algorithms using opencCV.
//
// Software written by Simon Lansbergen, 
// 
//	(c) April 2016. SL
//
//------------------------------------------------------------------------//
//
//		Revision:
//			- fully working and tested. see algorithm part for steps.
//			- can be compiled in release mode.
//			
//			
//
//	last edited - 29-4-2016, SL
//
//------------------------------------------------------------------------//
//

#include "iostream"
#include "sstream"
#include "fstream"
#include "math.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/core.hpp"

#include <read_config_file.h>

using namespace cv;    // Namespace for using openCV objects.
using namespace std;   // Namespace for using cout.


int main(int argc, char* argv[])
{

	system("CLS");

	//------------------------//
	// Configuration variable //
	//------------------------//

	//------------------------//
	// Configuration variable //
	//------------------------//

	const char* open_config = "config_analysepupilsize.cfg";

	//-------------------//
	// Defualt variables //
	//-------------------//

	// default hard coded settings if Config.cfg file is not present or in-complete/commented
	bool show_screen_info = 1;
	bool save_video = 0;
	bool save_radius = 0;
	double threshold_ = 20;
	int thres_mode = 0;
	ThresholdTypes thres_type = THRESH_BINARY;
	const char* thres_name = "THRESH_BINARY";
	double fps;
	const char* save_path = "c:/output.avi";
	const char* save_path_num = "c:/output.txt";
	string sav_pat = "c:/output.avi";
	string sav_pat_num = "c:/output.txt";;
	string filename = "D:/Dropbox/NIN/Video_software/matlab_code/video analysis/practice_pics/vid_eye.avi";
	Size ROI_dimensions = Size(50, 50);
	bool ROI_start_auto = 1;
	Size ROI_start;
	int ROI_start_x;
	int ROI_start_y;
	int width_SE = 3;
	int heigth_SE = 3;
	Size SE_morph = Size(width_SE, heigth_SE);
	int heigth_blur = 3;
	int width_blur = 3;
	Size blur_dimensions = Size(width_SE, heigth_SE);
	int itterations_close = 3;
	int threshold1_canny = 100;
	int	threshold2_canny = 50;
	int aperture_canny = 3;
	double pupil_aspect_ratio = 1.5;
	int pupil_min = 15;
	int pupil_max = 55;
	bool original_image = 0;
	bool blurred_image = 0;
	bool thresholded_image = 0;
	bool closed_image = 0;
	bool canny_image = 0;
	bool end_result_image = 1;
	bool show_ost = 0;
	double size_text = 0.5;
	
	//---------------------//
	// Remaining variables //
	//---------------------//

	// video writer output
	VideoWriter roi_end_result;

	// images for processing
	Mat eye;
	Mat thres;
	Mat roi_eye_rgb;
	Mat close;
	Mat blur;
	Mat canny;
	Mat roi_eye;
	Mat frame;

	// variables for numerical output
	ofstream output_end_result;
	ostringstream strs, ost1, ost2, ost3;
	string radius, size_roi, frame_rate, output_file;

	
	//-------------------------//
	// read configuration file //
	//-------------------------//

	// get config.cfg from cmd-line if entered correctly
	if (argc == 3)
	{
		if (isdigit(*argv[1]) || isdigit(*argv[2])) {
			puts("Wrong input argument for [open vid] or [config.cfg] ?");
		}

		else if (isalpha(*argv[1]) && isalpha(*argv[2])) {
			// get entered config.cfg file and path
			open_config = argv[2];

			cout << endl << endl << "*** Reading configuration file : " << open_config << " ***";

		}
		else {
			puts("Not a number: Wrong input argument for [open vid]or [config.cfg] ?");
			return 1;
		}

	}
	else {
		cout << endl << endl << "*** Reading system configuration file : " << open_config << " ***";
	}

	// read config file
	ifstream ifile(open_config);
	if (ifile) { // The file exists, and is open for input

		ConfigFile cfg(open_config);

		// check for existance and replace defualt value
		if (cfg.keyExists("show_screen_info") == true) { // get screen info on/off
			show_screen_info = cfg.getValueOfKey<bool>("show_screen_info");
		}
		if (cfg.keyExists("save_video") == true) { // get video save_file info
			save_video = cfg.getValueOfKey<bool>("save_video");
		}
		if (cfg.keyExists("save_radius") == true) { // get numerical save_file info
			save_radius = cfg.getValueOfKey<bool>("save_radius");
		}
		if (cfg.keyExists("threshold") == true) { // get threshold value
			threshold_ = cfg.getValueOfKey<double>("threshold");
		}
		if (cfg.keyExists("save_path_vid") == true) { // get video output file name & path
			sav_pat = cfg.getValueOfKey<string>("save_path_vid");
			save_path = sav_pat.c_str();
		}
		if (cfg.keyExists("save_path_num") == true) { // get numerical output file name & path
			sav_pat_num = cfg.getValueOfKey<string>("save_path_num");
			save_path_num = sav_pat_num.c_str();
		}
		if (cfg.keyExists("open_file") == true) { // get original stream file name & path
			filename = cfg.getValueOfKey<string>("open_file");
		}
		if (cfg.keyExists("height_roi") == true && cfg.keyExists("width_roi") == true) { // get heigth & width ROI
			int ROI_heigth = cfg.getValueOfKey<int>("height_roi");
			int ROI_width = cfg.getValueOfKey<int>("width_roi");
			ROI_dimensions = Size(ROI_width, ROI_heigth);
		}
		if (cfg.keyExists("ROI_start_x") == true && cfg.keyExists("ROI_start_y") == true) { // get x and y starting point for ROI
			ROI_start_y = cfg.getValueOfKey<int>("ROI_start_y");
			ROI_start_x = cfg.getValueOfKey<int>("ROI_start_x");
			ROI_start = Size(ROI_start_x, ROI_start_y);
			ROI_start_auto = false;
		}
		if (cfg.keyExists("width_SE") == true && cfg.keyExists("heigth_SE") == true) { // get dimensions SE
			width_SE = cfg.getValueOfKey<int>("width_SE");
			heigth_SE = cfg.getValueOfKey<int>("heigth_SE");
			SE_morph = Size(width_SE, heigth_SE);
		}
		if (cfg.keyExists("heigth_blur") == true && cfg.keyExists("width_blur") == true) { // get dimensions Gaussian blur
			heigth_blur = cfg.getValueOfKey<int>("heigth_blur");
			width_blur = cfg.getValueOfKey<int>("width_blur");
			blur_dimensions = Size(width_blur, heigth_blur);
		}
		if (cfg.keyExists("thres_mode") == true) { // get threshold method
			thres_mode = cfg.getValueOfKey<int>("thres_mode");
			switch (thres_mode) {
			case 0:
				thres_type = THRESH_BINARY;
				thres_name = "THRESH_BINARY";
				break;
			case 1:
				thres_type = THRESH_BINARY_INV;
				thres_name = "THRESH_BINARY_INV";
				break;
			case 2:
				thres_type = THRESH_TRUNC;
				thres_name = "THRESH_TRUNC";
				break;
			case 3:
				thres_type = THRESH_TOZERO;
				thres_name = "THRESH_TOZERO";
				break;
			case 4:
				thres_type = THRESH_TOZERO_INV;
				thres_name = "THRESH_TOZERO_INV";
				break;
			default:
				thres_type = THRESH_BINARY;
				thres_name = "THRESH_BINARY";
			}
		}
		if (cfg.keyExists("itterations_close") == true) { // get number of itterations for closing operation
			itterations_close = cfg.getValueOfKey<int>("itterations_close");
		}
		if (cfg.keyExists("threshold1_canny") == true && cfg.keyExists("threshold2_canny") == true
			&& cfg.keyExists("aperture_canny") == true) { // get parameters Canny filter algorithm
			threshold1_canny = cfg.getValueOfKey<int>("threshold1_canny");
			threshold2_canny = cfg.getValueOfKey<int>("threshold2_canny");
			aperture_canny = cfg.getValueOfKey<int>("aperture_canny");
		}
		if (cfg.keyExists("pupil_aspect_ratio") == true) { // get aspect ratio threshold accepted ellipse
			pupil_aspect_ratio = cfg.getValueOfKey<double>("pupil_aspect_ratio");
		}
		if (cfg.keyExists("pupil_min") == true) {  // get minimal accepted pupil radius
			pupil_min = cfg.getValueOfKey<int>("pupil_min");
		}
		if (cfg.keyExists("pupil_max") == true) {  // get maximal accepted pupil radius
			pupil_max = cfg.getValueOfKey<int>("pupil_max");
		}
		if (cfg.keyExists("original_image") == true) { // info: stream original stream to display
			original_image = cfg.getValueOfKey<bool>("original_image");
		}
		if (cfg.keyExists("blurred_image") == true) { // info: stream blurred stream to display
			blurred_image = cfg.getValueOfKey<bool>("blurred_image");
		}
		if (cfg.keyExists("thresholded_image") == true) { // info: thresholded_image blurred stream to display
			thresholded_image = cfg.getValueOfKey<bool>("thresholded_image");
		}
		if (cfg.keyExists("closed_image") == true) { // info: closed_image blurred stream to display
			closed_image = cfg.getValueOfKey<bool>("closed_image");
		}
		if (cfg.keyExists("canny_image") == true) { // info: canny_image stream to display
			canny_image = cfg.getValueOfKey<bool>("canny_image");
		}
		if (cfg.keyExists("end_result_image") == true) { // info: end_result_image blurred stream to display
			end_result_image = cfg.getValueOfKey<bool>("end_result_image");
		}
		if (cfg.keyExists("show_ost") == true) {	// put text on screen info
			show_ost = cfg.getValueOfKey<bool>("show_ost");
		}
		if (cfg.keyExists("size_text") == true) { // get text size for on screen text
			size_text = cfg.getValueOfKey<double>("size_text");
		}


		cout << endl << endl << " *** Configuration file loaded ***" << endl;

	}
	else {
		cout << endl << endl << " *** No configuration file found ***" << endl;
		cout << " *** Default, internal parameters loaded ***" << endl;
	}

	//------------------------//
	// Handle input arguments //
	//------------------------//

	cout << endl << endl << "Program has " << (argc - 1) << " extra input arguments:" << endl;
	for (int i = 1; i < argc; ++i) cout << argv[i] << endl;


	// input: none
	if (argc <= 1)	// actual count (starts at 1)
	{
		cout << endl << " - No additional input - " << endl << endl << endl;
		}
	// input: [open vid]
	else if (argc == 2)
	{
		if (isdigit(*argv[1])) {	// index starts at 0
			puts("Not a number: Wrong input argument for [open vid] ?");
		}
		else if (isalpha(*argv[1])) {
			// atoi = Ascii To Int -> stops converting at the first non int
			filename = argv[1];
		}
		else {
			puts("Not a number: Wrong input argument for [open vid] ?");
			return 1;
		}
	}
	// input: [open vid] [config.cfg]
	else if (argc == 3)
	{
		if (isdigit(*argv[1]) || isdigit(*argv[2])) {	// index starts at 0
			puts("Not a number: Wrong input argument for [open vid] or [config.cfg] ?");
		}
		else if (isalpha(*argv[1]) && isalpha(*argv[2])) {
			// atoi = Ascii To Int -> stops converting at the first non int
			filename = argv[1];
		}
		else {
			puts("Not a number: Wrong input argument for [open vid] or [config.cfg] ?");
			return 1;
		}
	}
	// return 1 (exit)
	else
	{
		cout << endl << " *** To many input arguments *** " << endl;
		return 1;
	}

	//-----------------------//
	// Show loaded variables //
	//-----------------------//

	if (show_screen_info == true) {

		cout << endl << endl;
		cout << "*------------------------------------------------------*" << endl;
		cout << "*****               Program Parameters             *****" << endl;
		cout << "*------------------------------------------------------*" << endl;
		cout << "*" << endl;
		cout << "* Save video output                   : " << save_video << endl;
		cout << "* Save radius numerical output        : " << save_radius << endl;
		cout << "* Save path video output              : " << save_path << endl;
		cout << "* Open path original stream           : " << filename << endl;
		cout << "* Save path numerical output          : " << save_path_num << endl;
		cout << endl;
		cout << "* Heigth and width ROI                : " << ROI_dimensions << endl;
		if (ROI_start_auto == false) {
			cout << "* Anchor coordinate [X,Y] ROI \n  manually set                        : " << ROI_start << endl;
		}
		else { cout << "* Anchor coordinate [X,Y] ROI set automatically" << endl; }
		cout << endl;
		cout << "* Value of threshold                  : " << threshold_ << endl;
		cout << "* Threshold mode                      : (" << thres_mode << ") " << thres_name << endl;
		cout << "* Size Gaussian blur filter           : " << blur_dimensions << endl;
		cout << "* Size structuring element \n  for morphological closing           : " << SE_morph << endl;
		cout << "* Total itterations closing operation : " << itterations_close << endl;
		cout << "* First threshold canny filter        : " << threshold1_canny << endl;
		cout << "* Second threshold canny filter       : " << threshold2_canny << endl;
		cout << "* Size aperture kernel canny filter   : " << aperture_canny << endl;
		cout << "* Threshold aspect ratio ellipse      : " << pupil_aspect_ratio << endl;
		cout << "* Minimum radius accepted ellipse     : " << pupil_min << endl;
		cout << "* Maximum radius accepted ellipse     : " << pupil_max << endl;
		cout << endl;
		cout << "* Show original stream on display     : " << original_image << endl;
		cout << "* Show blurred stream on display      : " << blurred_image << endl;
		cout << "* Show thresholded stream on display  : " << thresholded_image << endl;
		cout << "* Show morph closed stream on display : " << closed_image << endl;
		cout << "* Show Canny filter stream on display : " << canny_image << endl;
		cout << "* Show end result stream on display   : " << end_result_image << endl;
		cout << endl;
		cout << "* Show text on end result stream      : " << show_ost << endl;
		cout << "* Size text on screen                 : " << size_text << endl;
		cout << "*" << endl;
		cout << "*------------------------------------------------------*" << endl;
		cout << "*******     Program by S.E Lansbergen, 2016     ******** " << endl;
		cout << "*------------------------------------------------------*" << endl;

	}


		
	//--------------------//
	// Setup main program //
	//--------------------//

	// open video stream
	VideoCapture capture(filename);

	// test if stream exsist
	if (!capture.isOpened())
		throw "Error when reading stream_avi";


	capture >> frame;
	frame.reshape(1);
	Size size_vid = frame.size();

	// show dimensions [X,Y] video stream
	cout << endl << "Dimensions video file :" << endl;
	cout << endl << "Width : " << size_vid.width << endl << "Heigth : " << size_vid.height << endl;


	// get frame rate video file
	fps = capture.get(CV_CAP_PROP_FPS);
	cout << endl << "Frame rate video file : " << fps << endl;



	// pause - hit any key - to continue
	cout << endl << "Hit key to continue" << endl;
	system("pause");


	//--------------------------------//
	//  Set ROI and other variables   //
	//--------------------------------//

	// automatically calculate start point of ROI
	if (ROI_start_auto == true) {
		Size size(size_vid.width, size_vid.height);
		ROI_start_x = (size.width / 2) - (ROI_dimensions.width / 2);
		ROI_start_y = (size.height / 2) - (ROI_dimensions.height / 2);
	}
	
	// set ROI
	Rect roi(ROI_start_x, ROI_start_x, ROI_dimensions.width, ROI_dimensions.height);

	// set Structuring Element
	Mat SE = getStructuringElement(MORPH_ELLIPSE, SE_morph, Point(-1, -1));

	// contour variables
	vector<vector<Point> > contours;
	
	// set variables for on screen text output
	if (show_ost == true) {
		// text for ROI size
		ost1 << "ROI size: " << ROI_dimensions.width << "x" << ROI_dimensions.height;
		size_roi = ost1.str();
		// text for frame rate
		ost2 << "Frame rate : " << fps << " Hz";
		frame_rate = ost2.str();
		// text with save path & name
		ost3 << save_path;
		output_file = ost3.str();
	}

	//--------------------------------//
	// Open avi and txt output stream //
	//--------------------------------//

	// open video writer object: End Result
	if (save_video == true) {

		roi_end_result.open(save_path, CV_FOURCC('M', 'P', 'E', 'G'), fps, ROI_dimensions, true);

		//if the VideoWriter file is not initialized successfully, exit the program.
		if (!roi_end_result.isOpened())
		{
			cout << "ERROR: Failed to write the video" << endl;
			return -1;
		}
	}

	// open outstream to write end result (radius)
	if (save_radius == true) {
		output_end_result.open(save_path_num);
	}


	//---------------------//
	// Main Algorithm Loop //
	//---------------------//

	// initiate main loop with algorithm
	for (; ; )
	{

		capture >> frame;

		if (frame.empty())
			break;

		// Step 1
		//
		// convert to propper GRAY scaled image
		cvtColor(frame, frame, COLOR_RGB2GRAY);

		// Step 2
		//
		// take ROI from eye original
		eye = frame;
		roi_eye = eye(roi);
		// make RGB copy of ROI for end result
		if (!roi_eye.channels() == 3) {
			cvtColor(roi_eye, roi_eye_rgb, CV_GRAY2RGB);
		}
		else {
			cvtColor(roi_eye, roi_eye_rgb, CV_GRAY2RGB);
			//roi_eye_rgb = roi_eye;
		}

		// Step 3
		//
		// apply Gaussian blur to ROI
		GaussianBlur(roi_eye, blur, blur_dimensions, 0, 0, BORDER_DEFAULT);

		// Step 4
		//
		// Convert to binary image by thresholding it
		threshold(blur, thres, threshold_, 255, thres_type);

		// Step 5
		//
		// Morphological closing (erosion and dilation)
		morphologyEx(thres, close, MORPH_CLOSE, SE, Point(-1, -1), itterations_close);

		// Step 6
		//
		// Canny algorithm
		Canny(close, canny, threshold1_canny, threshold2_canny, aperture_canny, true);

		// Step 7
		//
		// find contour algorithm
		findContours(canny, contours, RETR_LIST, CHAIN_APPROX_NONE);

		// Step 8
		// 
		// Fit ellipse and draw on image
		double test_width(0), test_height(0);
		int flag(0), radius_output;

		// Loop to draw circle on video image
		for (int i = 0; i < contours.size(); i++)
		{

			size_t count = contours[i].size();
			if (count < 6)
				continue;
			Mat pointsf;
			Mat(contours[i]).convertTo(pointsf, CV_32F);
			RotatedRect box = fitEllipse(pointsf);
			if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * pupil_aspect_ratio)
				continue;

			// sets min and max width and heigth of the box in which the ellipse is fitted
			// only these are used in the video and numerical output
			if (MAX(box.size.width, box.size.height) > pupil_min && MAX(box.size.width, box.size.height) < pupil_max) {

				flag++;  // counts 1 at first itteration

						 // adds found all width and height in all itterations
				test_width = test_width + box.size.width;
				test_height = test_height + box.size.height;

				//drawContours(roi_eye_rgb, contours, (int)i, Scalar::all(255), 1, 8);
				ellipse(roi_eye_rgb, box, Scalar(0, 0, 255), 1, LINE_AA);
				ellipse(roi_eye_rgb, box.center, box.size*0.5f, box.angle, 0, 360, Scalar(0, 255, 255), 1, LINE_AA);

			}
		}


		// devides width and heigth with total number of found ellipses to get average value
		//
		if (test_width != NAN && test_height != NAN
			&& test_width != 0 && test_height != 0) {
			test_width = test_width / flag;
			test_height = test_height / flag;

			// add radius to streamstring for output to video frame
			//
			strs << "Radius : " << sqrt(test_width*test_width + test_height*test_height) << " pix";
			radius = strs.str();
			radius_output = sqrt(test_width*test_width + test_height*test_height);
		}
		else
		{
			// add radius to streamstring for output to video frame
			//
			strs << "Radius : " << 0 << " pix";
			radius = strs.str();
			radius_output = 0;
		}


		// put streamstring to video frame
		if (show_ost == true) {

			putText(roi_eye_rgb, frame_rate, cvPoint(30, 20),
				FONT_HERSHEY_COMPLEX_SMALL, size_text, cvScalar(0, 0, 255), 1, CV_AA);

			putText(roi_eye_rgb, size_roi, cvPoint(30, 35),
				FONT_HERSHEY_COMPLEX_SMALL, size_text, cvScalar(0, 0, 255), 1, CV_AA);

			putText(roi_eye_rgb, output_file, cvPoint(30, 55),
				FONT_HERSHEY_COMPLEX_SMALL, (size_text + 0.1), cvScalar(0, 0, 255), 1, CV_AA);
		}


		//------------------------------//
		// Store radius & video streams //
		//------------------------------//

		// store radius in output file
		if (save_radius == true) {
			output_end_result << radius_output << endl;
		}

		// write the end result into file
		if (save_video == true) {
			roi_end_result.write(roi_eye_rgb);
		}
		

		//--------------------//
		// show video streams //
		//--------------------//

		if (original_image == true) { imshow("original", eye); }
		if (blurred_image == true) { imshow("Gaussian blur", blur); }
		if (thresholded_image == true) { imshow("Thresholded", thres); }
		if (closed_image == true) { imshow("Morphological closing", close); }
		if (canny_image == true) { imshow("Canny filter", canny); }
		if (end_result_image == true) { imshow("Result (ROI)", roi_eye_rgb); }



		// wait before new frame
		waitKey(20); // waits to display frame

		
	} // end algorithm main loop


	  // close numerical output stream
	if (save_radius == true) { output_end_result.close(); }


	waitKey(0); // key press to close window
				// releases and window destroy are automatic in C++ interface


	return 0;

}