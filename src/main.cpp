/*  Recognition of palm gestures (open, closed, spread) based on skin detection
 * 
 *  Algorithm:
 *  1)  Face detection using OpenCV's Viola-Jones method.
 *  2)  Skin detection based on color statistics gathered from the face region.
 *  3)  Hands (palms) are detected through their skin color (the best ones are
 *      selected and refined with morphological operations).
 *  4)  The three supported gestures are recognized according to empirical rules
 *      based on the convexity defects of the palms' shape.
 * 
 *  Usage:
 *      The algorithm is completely automatic. It just assumes the presence of
 *      a (frontal) face and that the person's palms are naked, so that they can 
 *      be detected through the color of their skin. 'Esc' terminates the 
 *      program.
 */

#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include "FaceDetector.hpp"
#include "SkinDetector.hpp"
#include "Hand.hpp"

using namespace std;
using namespace cv;

RNG rng(12345);

int main(
    int argc, 
    char** argv
) {
	CvCapture* capture;
	Mat frame;
	Mat frameGray;
	vector<Rect> faceRectangles;
	FaceDetector fd;
	int pressedKey;
	double totalDuration = 0.0;
	double mainFunctionCallCount = 0.0;
	
	// Open capturing device
	capture = cvCaptureFromCAM( -1 );
	if( capture ) {
		while( true ) {
			frame = cvQueryFrame( capture );
			if( !frame.empty() ) { 
				cvtColor(
					frame,
					frameGray,
					CV_BGR2GRAY
				);
				if( fd.isFunctional() ) {
					faceRectangles = fd.getFaceRectangles(frameGray);
					for( 
						int faceInd = 0; 
						faceInd < faceRectangles.size(); 
						faceInd++ 
					) {
						// Start timer
						double duration;
						duration = static_cast<double>(getTickCount());
						
						// Create a Skin Detector and initialize skin mask
						SkinDetector sd;
						Mat skinImage;
						skinImage = sd.initializeSkinMask(
							frame,
							faceRectangles[faceInd]
						);
						
						// Use the skin mask to find hand regions and their 
                        // gestures
						Mat handsImage;
						handsImage = sd.inferHandGesutres(
							skinImage, 
							faceRectangles[faceInd]
						);
						
						// Stop timer
						duration = static_cast<double>(getTickCount())-duration;
						duration /= getTickFrequency(); // elapsed time in ms
						totalDuration += duration;
						mainFunctionCallCount++;
						
						// Visualization
						namedWindow( 
							"Hands Image", 
							CV_WINDOW_AUTOSIZE 
						);
						imshow( 
							"Hands Image", 
							handsImage 
						);
						moveWindow(
							"Hands Image",
							600,
							50
						);
					}

                    // Exit with 'Esc' key
					pressedKey = waitKey(10);
					if( pressedKey == 27 ) { 
						cout << "Average execution time for whole algorithm: " << 
							totalDuration / mainFunctionCallCount << endl;
						return 0; 
					}
				} else {
					return 1;
				}
			} else {
				 cerr << "Problem with capturing device." << endl;
				 break; 
			}

		}
	}
  
	 return 0;
}
