/**
 *
 * Fluid simulation module
 *
 * by Sam Scheib
 *
 */

class Fluid {

private:
	int rows, cols;
	float** heights;

public:
	Fluid(int, int);
	~Fluid();
	void displaySurface();
};

