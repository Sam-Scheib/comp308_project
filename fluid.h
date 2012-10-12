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
	float** velocities;

public:
	Fluid(int, int);
	~Fluid();
	void calculateSurface();
	void displayFluid();
};

