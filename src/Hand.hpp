#ifndef HAND_HPP
#define	HAND_HPP

class Hand {
public:
	Hand();
	Hand(const Hand& orig);
	virtual ~Hand();
	void setCoords(
		int,
		int
	);
	void setInd(
		int
	);
	void setState(
		int
	);
private:
	int x;
	int y;
	int ind;
	int state;
};

#endif	/* HAND_HPP */

