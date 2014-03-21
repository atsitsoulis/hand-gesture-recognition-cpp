#include "SkinDetector.hpp"
#include "Hand.hpp"

String handStatesStrings[4] = { 
	"UNKNOWN",
	"OPEN_PALM", 
	"SPREAD_PALM", 
	"CLOSED_PALM" 
};

SkinDetector::SkinDetector() {
}

SkinDetector::SkinDetector(const SkinDetector& orig) {
}

SkinDetector::~SkinDetector() {
}

//------------------------------------------------------------------------------
// Purpose: Statistics in YCrCb colorspace are computed for the detected face
//			region (rectangle). The statistics define approximate thresholds for
//			initializing the skin mask, which is refined using morphological
//          operations.
// In:		input RGB image, rectangle of detected face
// Returns:	image of skin mask
//------------------------------------------------------------------------------
Mat SkinDetector::initializeSkinMask(
	Mat& frame,
	Rect& faceRectangle
) {
	// Smooth image
	GaussianBlur(
		frame, 
		frame, 
		Size(
			9,
			9
		),
		1,
		1
	);

	Mat frameYCrCb;

	cvtColor(
		frame,
		frameYCrCb,
		CV_BGR2YCrCb
	);

	vector<double> meanValues;
	vector<double> stdValues;
	
	// Tighten face bbox
	faceRectangle.height *= 0.8;
	faceRectangle.width *= 0.9;
	faceRectangle.x *= 1.05;

	// Calculate face stats
	meanStdDev(
		frameYCrCb(faceRectangle),
		meanValues,
		stdValues
	);

	// Threshold frame
	vector<double> upperBounds;
	vector<double> lowerBounds;

	upperBounds.push_back(meanValues[0] + 2 * stdValues[0]);
	lowerBounds.push_back(meanValues[0] - 2 * stdValues[0]);
	upperBounds.push_back(meanValues[1] + 1 * stdValues[0]);
	lowerBounds.push_back(meanValues[1] - 0.1 * stdValues[0]); // Sensitive
	upperBounds.push_back(meanValues[2] + 1 * stdValues[0]);
	lowerBounds.push_back(meanValues[2] - 0.5 * stdValues[0]);

	Mat skinImage;
	inRange(
		frameYCrCb,
		lowerBounds,
		upperBounds,
		skinImage
	);
	
	namedWindow( 
		"Skin Image", 
		CV_WINDOW_AUTOSIZE 
	);
	imshow( 
		"Skin Image", 
		skinImage 
	);

	// Morphological operations
	Mat strElement = getStructuringElement( 
		MORPH_ELLIPSE,
      Size( 
			10, 
			20 
		),
      Point( 
			-1, 
			-1 
		) 
	);
	
	dilate(
		skinImage,
		skinImage,
		strElement,
		Point( 
			-1, 
			-1 
		),
		1
	);

	erode(
		skinImage,
		skinImage,
		strElement,
		Point( 
			-1, 
			-1 
		),
		1
	);
		
	return skinImage;
}

//------------------------------------------------------------------------------
// Purpose: Based upon the skin mask and the face rectangles, the method 
//			performs advanced morphological operations in order to extract 
//			the	hand regions and infer their gesture.
// In:		skin mask image, rectangle of detected face
// Returns:	result image depicting the detected hand regions and their gestures
//------------------------------------------------------------------------------
Mat SkinDetector::inferHandGesutres(
	Mat& skinImage, 
	Rect& faceRectangle
) {
	// Loosen and zero out face bbox
	faceRectangle.height *= 1.2;
	faceRectangle.width *= 1.2;
	faceRectangle.x *= 0.95;
	
	rectangle( 
		skinImage, 
		faceRectangle.tl(), 
		faceRectangle.br(), 
		0, 
		CV_FILLED
	);
		
	// CC Analysis
	// Find contours
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( 
		skinImage, 
		contours, 
		hierarchy, 
		CV_RETR_LIST, 
		CV_CHAIN_APPROX_SIMPLE, 
		Point(
			0, 
			0
		) 
	);

	// Eliminate too short or too long contours
	int cmin= 5000; // minimum contour length
	for( int i = 0; i < contours.size(); i++ ) {
		if( contourArea(contours[i]) < cmin ) {			
			drawContours( 
				skinImage, 
				contours, 
				i, 
				Scalar(0), 
				CV_FILLED, 
				8, 
				hierarchy, 
				0, 
				Point(
					0,
					0
				) 
			);
		}
	}
	
	// Draw contours
	Mat filledSkinImage = Mat::zeros( 
		skinImage.size(), 
		CV_8UC1
	);
	
	contours.clear();
	hierarchy.clear();
	findContours( 
		skinImage, 
		contours, 
		hierarchy, 
		CV_RETR_LIST, 
		CV_CHAIN_APPROX_SIMPLE, 
		Point(
			0, 
			0
		) 
	);
	
	// Detect hand gestures: Open palm, closed palm
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Point2f>center( contours.size() );
	vector<float>radius( contours.size() );
	vector<Hand> hands;
	int handInd = 1;
	
	for( int i = 0; i < contours.size(); i++ ) { 
		approxPolyDP( 
			Mat(contours[i]), 
			contours_poly[i], 
			3, 
			true 
		);
		
		// Fitted Bbox and circle
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		minEnclosingCircle( 
			(Mat)contours_poly[i], 
			center[i], 
			radius[i] 
		);
		
		// Hand detected
		if( (radius[i] < 1.5 * faceRectangle.height) &&
			(radius[i] > 0.5 * faceRectangle.height)
		) {
			// Show detected hand region
			drawContours( 
				filledSkinImage, 
				contours_poly, 
				i, 
				Scalar(255), 
				CV_FILLED, 
				8, 
				hierarchy, 
				0, 
				Point(
					0,
					0
				) 
			);
		
			if( contours_poly[i].size() >= 5 ) {
				vector<int> contourConvexHull;
				// Calculate convexity
				convexHull(
					contours_poly[i],
					contourConvexHull
				);

				if( contourConvexHull.size() > 3 ) {
					vector<Vec4i> contourConvexityDefect;
					convexityDefects(
						contours_poly[i],
						contourConvexHull,
						contourConvexityDefect
					);

					Point2f centerOffset = center[i];
					centerOffset.y *= 0.5;

					// Store hand objects for further processing
					Hand hand;
					hand.setCoords(
						 center[i].x,
						 center[i].y
					);
					hand.setInd(handInd);
					
					// Recognize gesture (state)
					rectangle(
						filledSkinImage,
						boundRect[i],
						Scalar(255),
						2
					);
					rectangle(
						filledSkinImage,
						fitEllipse(contours_poly[i]).boundingRect(),
						Scalar(128),
						2
					);
					
					if( (fitEllipse(contours_poly[i]).boundingRect().height / 2 >
						 1.1 * radius[i]) &&
						(radius[i] < 0.8 * faceRectangle.height)
					) {
						putText( 
							filledSkinImage, 
							handStatesStrings[CLOSED_PALM], 
							centerOffset, 
							FONT_HERSHEY_SIMPLEX,
							1, 
							Scalar(255)
						);
						
						hand.setState(CLOSED_PALM);
					} else {
						if( contourConvexityDefect[0].val[3] > 50 * radius[i] ) {
							putText( 
								filledSkinImage, 
								handStatesStrings[SPREAD_PALM], 
								centerOffset, 
								FONT_HERSHEY_SIMPLEX,
								1, 
								Scalar(255)
							);
							
							hand.setState(SPREAD_PALM);
						} else {
							putText( 
								filledSkinImage, 
								handStatesStrings[OPEN_PALM], 
								centerOffset, 
								FONT_HERSHEY_SIMPLEX,
								1, 
								Scalar(255)
							);
							
							hand.setState(OPEN_PALM);
						}
					}
					
					hands.push_back(hand);
					
					handInd++;
				}
			}
		}
	}
	
	return filledSkinImage;
} 
