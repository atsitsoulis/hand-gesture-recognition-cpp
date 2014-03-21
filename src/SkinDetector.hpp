#ifndef SKINDETECTOR_HPP
#define	SKINDETECTOR_HPP

#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

enum handState { 
	UNKNOWN,
	OPEN_PALM, 
	SPREAD_PALM,
	CLOSED_PALM
};

using namespace std;
using namespace cv;

class SkinDetector {
public:
	SkinDetector();
	SkinDetector(const SkinDetector& orig);
	virtual ~SkinDetector();
	Mat initializeSkinMask(
		Mat&, 
		Rect&
	);
	Mat inferHandGesutres(
		Mat&,
		Rect&
	);
private:

};

#endif	/* SKINDETECTOR_HPP */

