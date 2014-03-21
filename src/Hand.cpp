#include "Hand.hpp"

Hand::Hand() {
}

Hand::Hand(const Hand& orig) {
}

Hand::~Hand() {
}

void Hand::setCoords(
	int xIn, 
	int yIn
){
    // Hand position (centroid of palm)
	x = xIn;
	y = yIn;
}

void Hand::setInd(
	int indIn
){
    // Hand identifier
	ind = indIn;
}

void Hand::setState(
	int stateIn
){
    // Gesture (state) of hands (open, closed, etc.)
	state = stateIn;
}
