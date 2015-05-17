
#include "detect_identifier.hpp"

using namespace std;
using namespace cv;
//RNG rng(12345);


bool compare_two_symbols_contours (smbl_contour c1, smbl_contour c2)
{
	return (c1.x < c2.x);
}


Identifier_detector::Identifier_detector():rng(12345)
{
	cout << "Cascade file name: \""<< identifier_cascade_name << "\"" << endl;

    //-- 1. Load the cascade
    if( !identifier_cascade.load( identifier_cascade_name ) )
    {
    	cout << error_tag<<" Error loading identifier cascade\n" << endl;
    	throw IDENTIFIER_ERROR_LOADING_CASCADE;
    };
    
    
    //vector<tmpl_inf> templates;//берем системную переменную

    //vector<tmpl_inf> symbols_templates;
    if (loadTemplates(&templates, symbols_files, symbols_files_len, template_dir) < 0 )
    {
    	cout << error_tag << "Error loading symbols templates (\""<<template_dir<<"\")" << endl;
    	throw IDENTIFIER_TEMPLATE_LOAD_ERROR;
    }
	//processTemplates(&templates);
    
    //vector<tmpl_inf> digit_templates;
    if (loadTemplates(&templates, digit_files, digit_files_len, template_dir) < 0 )
    {
    	cout << error_tag << "Error loading digit templates (\""<<template_dir<<"\")" << endl;
    	throw IDENTIFIER_TEMPLATE_LOAD_ERROR;
    }
	processTemplates(&templates);
}

int Identifier_detector::loadTemplates(vector<tmpl_inf> *templates, const char ** file, int file_len, const char *file_dir)
/*
*загружает фотографии с шаблонами, обрабатывает их
* возвращает число успешно обработанных шаблонов
*/
{
	int success_load = 0;
	//int templates_amount = templates_file_names.size();

	if (file_len == 0)
		return 0;


	char real_file [1024];
	char *pfile = real_file;
	if (file_dir != NULL)
	{
		snprintf (real_file, sizeof(real_file), "%s/", file_dir);
		pfile += strlen(file_dir);
	}


	for (int i = 0; i < file_len; i++)
	{
		strcpy(pfile, file[i]);
		cout << real_file << endl;
		tmpl_inf templ;
		templ.file_name = real_file;
		
		templ.templ = imread(templ.file_name, CV_LOAD_IMAGE_COLOR);   // Read the file
    	if(! templ.templ.data )                              // Check for invalid input
    		cout << error_tag << "Error loading template \""<< file[i] <<"\""<< endl;
    	else
    		success_load++;
    	(*templates).push_back(templ);
	}

	if (success_load == 0)
		return -1;
	return success_load;
}

int Identifier_detector::processTemplates(vector<tmpl_inf> *templates)
/* 
 * для каждого шаблона производим
 * 1. бинаризацию
 * 2. выделение главного контура - самый большой по размеру
 */
{
	cout << "******************** processTemplates ********************** " << endl;
	int templates_size = (*templates).size();
	for (int i = 0; i < templates_size; i++)
	{
		Mat frame_gray;
		cvtColor( (*templates)[i].templ, frame_gray, COLOR_BGR2GRAY );
		//equalizeHist( frame_gray, frame_gray );
		blur( frame_gray, frame_gray, Size(3,3) );

		//imshow("OLOLOl 123", frame_gray);

	  	vector<vector<Point> > contours;
	  	Mat frame_bin;
	    adaptiveThreshold(frame_gray, frame_bin, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, 0);
	    //imshow("OLOLOLO", frame_bin);
	    findContours( frame_bin, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

		Mat drawing = Mat::zeros( frame_bin.size(), CV_8UC3 );
		cout << " amount => " << contours.size() << endl;
		/*
		 * Получаем размеры изображения (выделенной области, содержащей идентификатор)
		 */
		Size identifier_size = frame_bin.size();
		int identifier_height = identifier_size.height, identifier_width = identifier_size.width;
		cout << "identifier_width: " << identifier_width<< ", identifier_height: "<<identifier_height << endl;
		cout << "contours amount => "<<contours.size() <<endl;

		int max_contour = 0, prev_max_contour = 0, max_area = 0;
		int contours_size = contours.size();
		for( int j = 0; j < contours_size; j++ )
		{
			vector<Point> contour = contours[j];
			// находим минимальный прямоугольник, который содержит рассматриваемый контур
			RotatedRect rotated_rect = minAreaRect(contour);

			// Проверка ширины, высоты и заполненной области найденного прямоугольника
			float width = rotated_rect.size.width;
			float height = rotated_rect.size.height;
			float area = width * height;
			if (width < 40 || height < 60)
				continue;

			if (area > max_area)
			{
				max_area = area;
				prev_max_contour = max_contour;
				max_contour = j;
			}

			cout << "contour "<< j << ": width = " << width << ", height = " << height << ", area = "<< area << endl;

	    	//possible_symbols.push_back(contour);
	    	// отображем отобранные контуры, для DEBUG
	    	
	    	/*Mat drawing1 = Mat::zeros( frame_bin.size(), CV_8UC3 );
		    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		    drawContours( drawing1, contours, j, color, 2, 8, noArray(), 0, Point() );
		    char ss[1024];
		    snprintf (ss, sizeof (ss), "%i.contours.%s", j, (*templates)[i].file_name.c_str());
		    namedWindow( ss, CV_WINDOW_AUTOSIZE );
		    imshow( ss, drawing1 );*/
		    
		}
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours( drawing, contours, prev_max_contour, color, 2, 8, noArray(), 0, Point() );
		drawContours( drawing, contours, max_contour, color, 2, 8, noArray(), 0, Point() );
		//namedWindow( (*templates)[i].file_name.c_str(), CV_WINDOW_AUTOSIZE );
		//imshow((*templates)[i].file_name.c_str(), drawing);

		//Rect rect;
		//rect = boundingRect(contours[max_contour]);
		//Mat frame_bin1;
		//adaptiveThreshold(frame_gray, frame_bin1, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, 0);
		//Mat roi (frame_bin1, rect);
		//(*templates)[i].contour = roi;
		
		(*templates)[i].contours.push_back(contours[prev_max_contour]);
		(*templates)[i].contours.push_back(contours[max_contour]);
	}
	return 1;
}

int Identifier_detector::run_detector()
{
	VideoCapture capture ("127.0.0.1:5000/video_feed");
	if ( ! capture.isOpened() )
	{ 
		cout << "Error opening video capture" << endl;
		return -1; 
	}
	Mat frame;
	while ( capture.read(frame) )
    {
        if( frame.empty() )
        {
            cout << error_tag << "Error loading image \""<< frame <<"\""<< endl;
            break;
        }
        imshow( "video", frame);
        //-- 3. Apply the classifier to the frame
        //detectIdentifier( frame );

        //-- bail out if escape was pressed
        int c = waitKey(10);
        if( (char)c == 27 ) { break; }
    }

    //const char * const test_image = "image337.bmp";
    /*const char * const test_image = "127.0.0.1:5000/video_feed";
    Mat frame1 = imread(test_image, CV_LOAD_IMAGE_COLOR);   // Read the file
    if(! frame1.data )                              // Check for invalid input
    {
    	cout << error_tag << "Error loading image \""<< test_image <<"\""<< endl;
    	//throw 5;
        return -1;
    }
    imshow("video", frame1);*/

    //detectIdentifier( frame1 );

    cvWaitKey(0);
    return 0;
}

void Identifier_detector::detectIdentifier( Mat frame)
{
    std::vector<Rect> identifiers;
    Mat frame_gray;

    // бинаризация изображения - конвертация в оттенки черного
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    //equalizeHist( frame_gray, frame_gray );
    blur( frame_gray, frame_gray, Size(3,3) );

    //-- Detect identifiers
    identifier_cascade.detectMultiScale( frame_gray, identifiers, 1.1, 2, 0, Size(100, 100) );

    //для всех найденных идентификаторов вырезаем область с идентификаторов и распознаем идентификационный номер
    int identifiers_size = identifiers.size();
    for( size_t i = 0; i < identifiers_size; i++ )
    {
        Mat identifier_roi = frame_gray( identifiers[i] );
        imshow( window_name_with_id_ROI, identifier_roi );
        char *extracted_id = NULL;
        extractIdentifier(identifier_roi, &extracted_id); //, templates);
    }
    
}

void Identifier_detector::extractIdentifier( Mat frame, char **result/*, vector<tmpl_inf> *templates*/)
//вариант с бинаризацией (по хорошему только для чистых идентификаторов)
{

    char window_name1[] = "extract countrs 1";

    //***********************************************************************************************************************************************
    //бинаризация изображения - перевод изображения в чернобелое

    //TODO: здесь надо бы вставить проверку - если не в чб, то перевести в чб
    //cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    //equalizeHist( frame_gray, frame_gray );
    
    //Mat frame_bin;
    //adaptiveThreshold(frame, frame_bin, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, 0);
    //imshow(window_name1, frame_bin);

    Mat canny_output;
  	vector<vector<Point> > contours;
  	vector<Vec4i> hierarchy;
  	int thresh = 100;
	int max_thresh = 255;

	//vector<vector<Point> > possible_symbols;
	//vector<Mat> possible_symbols;
	vector<smbl_contour> possible_symbols;

  	/// Detect edges using canny
  	//Canny( frame, canny_output, thresh, thresh*2, 3 );
  	/// Find contours
  	//findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  	Mat frame_bin;
    adaptiveThreshold(frame, frame_bin, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, 0);
    imshow(window_name1, frame_bin);
    findContours( frame_bin, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	/// Draw contours
	Mat drawing = Mat::zeros( frame_bin.size(), CV_8UC3 );
	//Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	cout << " amount => " << contours.size() << endl;
	/*
	 * Получаем размеры изображения (выделенной области, содержащей идентификатор)
	 */
	Size identifier_size = frame_bin.size();
	int identifier_height = identifier_size.height, identifier_width = identifier_size.width;
	cout << "identifier_width: " << identifier_width<< ", identifier_height: "<<identifier_height << endl;


	int contours_size = contours.size();
	float frame_bin_area = frame_bin.rows * frame_bin.cols;
	float possible_symbol_area = frame_bin_area / 2; 
	cout  << "contours size => " << contours_size << endl;
	for( int i = 0; i < contours_size; i++ )
	{
		vector<Point> contour = contours[i];

		// находим минимальный прямоугольник, который содержит рассматриваемый контур
		RotatedRect rotated_rect = minAreaRect(contour);

		// Проверка ширины, высоты и заполненной области найденного прямоугольника
		float width = rotated_rect.size.width;
		float height = rotated_rect.size.height;
		float area = width * height;

		if (width < min_symbol_width - 20 || height < min_symbol_height - 20)
			continue;

		// Получаем коэффициент, отражающий 
		cout << "Filled area: "<< (fabs(contourArea(contour)) / area)<<endl;
		if (fabs(contourArea(contour)) / area < min_fill_area)
    		continue;

    	Mat drawing1 = Mat::zeros( frame_bin.size(), CV_8UC3 );
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing1, contours, i, color, 2, 8, noArray(), 0, Point() );
        char ss[100];
        snprintf(ss, sizeof(ss), "olololo %i", i);
        imshow (ss, drawing1);

    	//Площадь, занимаемая символом всегда меньше площади исходного изображения (идентификатора)
    	if (area > possible_symbol_area)
    		continue;

    	// Проверка отношения границ найденного прямоугольника
		float ratio = (width < height) ? (float) width/height : height/width;
		cout << "RATIO => " << ratio << endl;		
		if (ratio < min_ratio || ratio > max_ratio)
    		continue;

    	cout << "contour "<< i << ": width = "<<width<<", height = "<<height << ", area = "<<area << endl;
    	cout << "contour total => " << arcLength(contour, true) << endl;
    	if (arcLength(contour, true) > 1000)
    		continue;

    	
		// Проверка не повернутого прямоугольника
		Rect rect;
		rect = boundingRect(contour);
		cout  << "rect.x => " << rect.x  << ",   rect.y => " <<rect.y<< endl;
		ratio = (float)rect.size().height / (float)rect.size().width;
		cout << "ordinary rect => " << ratio << endl;
		//if ( ratio < 1 || ratio > 2 )
		//    continue;

    	//TODO: проверка числа черных и белых пикселей
    	int white_pixels = 0;
		int black_pixels = 0;
		
		// these lines - fixing bug with copying image after findContours()
		Mat test;
    	adaptiveThreshold(frame, test, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15, 0);
		Mat roi(test, rect);


		int rows = roi.rows;
		int cols = roi.cols;
		for(int k = 0; k < rows; k++)
		{
		    for(int j = 0; j < cols; j++)
		        {
		         	unsigned char val = roi.at<unsigned char>(k,j);
		         	//cout << "current val => " << (int) val << endl;
		            if (val == 0)
		               black_pixels++;
		            else if (val > 200)
		               white_pixels++;
		        }
		}
		cout << "white => " << white_pixels << ", black => " << black_pixels << endl;
		ratio = (float) black_pixels / (float) white_pixels;
		cout << "Black and white pixels => "<< ratio << endl;
		//TODO - выявить основные критерии вырезки по черным и белым пикселям
		//if ((ratio > 0.7) || (ratio < 0.2))
		//    continue;

		smbl_contour tmp (contour, rect.x, rect.y);
    	possible_symbols.push_back(tmp);
	}

	cout  << "Possible symbols size => " << possible_symbols.size() << endl;

	sort(possible_symbols.begin(), possible_symbols.end(), compare_two_symbols_contours);

	for (int i = 0; i < possible_symbols.size(); i++)
	{
		vector<vector<Point> > qq;
		qq.push_back(possible_symbols[i].contour);
		Mat drawing = Mat::zeros( frame_bin.size(), CV_8UC3 );

		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, qq, 0, color, 2, 8, noArray(), 0, Point() );

		char ss[100];
		snprintf (ss, sizeof(ss), "sorted %i", i);
		imshow( ss, drawing );
	}


	int possible_symbols_size = possible_symbols.size();
	for (int j = 0; j < possible_symbols_size; j++)
	{
		cout << " ******************************* iteration " << j << endl;
		char ss[1000];
		//Mat drawing1 = Mat::zeros( frame_bin.size(), CV_8UC3 );
	    //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	    //drawContours( drawing1, possible_symbols, j, color, 2, 8, noArray(), 0, Point() );

		double max_match_res = 1000;
		int max_match_contour_idx = 0, max_match_subcontour_idx = 0;

		int templates_size = templates.size();
		for (int i = 0; i < templates_size; i++)
		{
			int templates_contours_size = templates[i].contours.size();
			for (int k = 0; k < templates_contours_size; k++)
			{
				snprintf (ss, sizeof (ss), "Templ%i", i);
	    		//namedWindow( ss, CV_WINDOW_AUTOSIZE );
				//imshow( ss, templates[i].contour );

				double match0 = matchShapes(possible_symbols[j].contour, templates[i].contours[k], CV_CONTOURS_MATCH_I2, 0);
				cout << "match" << i <<" => "<< match0 << endl;
				if (match0 < 0.5) continue;
				if (match0 <  max_match_res)
				{
					max_match_res = match0;
					max_match_contour_idx = i;
					max_match_subcontour_idx = k;
				}
			}
		}

		cout  << "res match => "<< max_match_res << endl;
		snprintf (ss, sizeof(ss), "%i.RES", j);
		imshow( ss, templates[max_match_contour_idx].templ);
	}
	//необходимо подготовить шаблоны для сравнения

}
