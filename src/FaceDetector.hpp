#ifndef FACEDETECTOR_HPP
#define	FACEDETECTOR_HPP

#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/ml/ml.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

class FaceDetector {
public:
	FaceDetector();
	FaceDetector(const FaceDetector& orig);
	virtual ~FaceDetector();
	int isFunctional();
	vector<Rect> getFaceRectangles(Mat&);
private:
	int exitError;
	CascadeClassifier frontalFaceCascade;
	Mat frame;
};

#endif	/* FACEDETECTOR_HPP */

