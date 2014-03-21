#include "FaceDetector.hpp"

FaceDetector::FaceDetector() {
    // Load Haar cascades for frontal face
    if( frontalFaceCascade.load( "haarcascade_frontalface_alt.xml" ) == 0 ) {
      cerr << "Cannot load face cascade." << endl;
		exitError = 0;
	} else {
		exitError = 1;
	}
}

FaceDetector::FaceDetector(const FaceDetector& orig) {
}

FaceDetector::~FaceDetector() {
}

int FaceDetector::isFunctional() {
	return exitError;
}

vector<Rect> FaceDetector::getFaceRectangles(Mat& frameGray) {
    vector<Rect> faceRectangles;

    // AdaBoost face detection
    frontalFaceCascade.detectMultiScale(
        frameGray, 
		faceRectangles, 
		1.1, 
		4,
		CV_HAAR_DO_ROUGH_SEARCH | CV_HAAR_SCALE_IMAGE, 
		Size( 
			60, 
			60 
		),
      Size( 
			300, 
			300 
		)
    );
	
	return faceRectangles;
}
