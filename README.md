Hand-Gesture-Recognition
========================

Recognition of palm gestures (open, closed, spread) based on face and skin detection.
 
Algorithm:
1)  Face detection using OpenCV's Viola-Jones method.
2)  Skin detection based on color statistics gathered from the face region.
3)  Hands (palms) are detected through their skin color (the best ones are
    selected and refined with morphological operations).
4)  The three supported gestures are recognized according to empirical rules
    based on the convexity defects of the palms' shape.

Usage:
    The algorithm is completely automatic. It just assumes the presence of
    a (frontal) face and that the person's palms are naked, so that they can 
    be detected through the color of their skin. 'Esc' terminates the 
    program.
