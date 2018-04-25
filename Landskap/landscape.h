#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <vector>
using namespace std;

// the size of the landscape
#define LANDSCAPESIZE 5
// (LANDSCAPESIZE*LANDSCAPESIZE-1)/2
#define LANDSCAPECENTER 12

namespace LandscapeHelpers {
	const int MAXDEPTH=14;
	const int EDGEPOS=1<<MAXDEPTH;
	const int SLOPEOFFSET=10; //Number slope is shifted to get int presicion
	const int DEFAULTSEED=1000;
	/** Landpoint version 3: this contains only
	*	left, right up down pointers. The exact
	*	interpretations of these has to be controlled by
	*	the patch using them. 
	*	For interior points they represent children
	*	For edges they represent children and partners
	*	For ends some of these will not be defined! */
	class LandPoint {		
		int height;
		int slope[2];

		//int downSlope;
		//int rightSlope;

		enum DIRECTION {
			DOWN=0, RIGHT=1, UP=2, LEFT=3
		};
		/** down, right, up, left */
		LandPoint* dir[4];

		bool edge; // used for succefull destruction

		
	public:
		LandPoint* getLeft() {return dir[3];}
		LandPoint* getRight() {return dir[1];}
		LandPoint* getUp() {return dir[2];}
		LandPoint* getDown(){return dir[0];}
		LandPoint* getDir(int i) {return dir[i];}

		void setLeft(LandPoint* n) { dir[3]=n; }
		void setRight(LandPoint* n) { dir[1]=n; }
		void setUp(LandPoint* n) { dir[2]=n; }
		void setDown(LandPoint* n) { dir[0]=n; }
		void setChild(int i, LandPoint* n) { dir[i]=n; }

		const LandPoint* cLeft() const {return dir[3];}
		const LandPoint* cRight() const {return dir[1];}
		const LandPoint* cUp() const {return dir[2];}
		const LandPoint* cDown() const {return dir[0];}
		const LandPoint* cDir(int i) const {return dir[i];}

		LandPoint* copy() const;

		int getHeight() const {return height;}
		int getRightSlope() const {return slope[1]; }
		int getDownSlope() const { return slope[0]; }
		int getSlope(int i) const { return slope[i]; }
		
		/** Fills arrays with coordinate data.
		*	@param cd distance to next collumn
		*	@param rd distance to next row
		*	@param cc collumn coordinates
		*	@param rc row coordinates
		*	@param h heigth data */
		void getHeightMap(int x, int y, int cd, int rd,  
							vector<int>& cc, vector<int>&rc, vector<int>& h);
		void getInnerHeightMap(vector<int>& h);

		LandPoint(int h, int ds, int rs, bool leftEdge) : 
		height(h), edge(leftEdge) {slope[0]=ds; slope[1]=rs;
		dir[0]=dir[1]=dir[2]=dir[3]=NULL;}
		
		/** Destructor that deletes all children 
		*	if edge is true, or the rest of the 
		*	row otherwise */
		~LandPoint();
	};

	struct PointGenData {
		int coord[2];
		//int collumn;
		//int row;

		LandPoint* dirs[4];
		//LandPoint* down;
		//LandPoint* right;
		//LandPoint* up;
		//LandPoint* left;

		const LandPoint* oldPoint;
		
		// 0=false, 1=true
		char lowIsParent[2];
		//bool upIsParent;
		//bool leftIsParent;
		short stripDir;

		/** height of point in row/collumn 
		*	Distance to parent is 1 << DepthDesc */ 
		signed char dynDepthDesc;
		signed char statDepthDesc;
	};

	/** Generates the heights of points
	*	primarily a advanced radom generator */
	class HeightGenerator {
		/** should be a high number to allow for a high number of 
		*	different patches */
		//static const int PATCHSEEDMOD=11113;

		///** A high odd number */
		//static const int PATCHSEEDFACTOR=23849;

		/** How much the calculated slope point should be weighted */
		float SLOPEWEIGHT;

		/** how much the randomComponent should matter
		*   making it a power of 2 might reduce randomility*/
		float RANDOMWEIGHT;

		/** constant seed for the world */
		int seed;

		/** the x and y coordinate of the patch (patchcoordinate<<maxdepth) */
		int patchx, patchy;

		///** Either number either calculated from seed and ID, 
		//*	or directly given to the class */
		//int patchSeed;

		/** gets the random component of a point in the current patch*/
		int getRandom(int depth, int x, int y);

		static HeightGenerator* singleton;

	public:
		/** sets the coordinate of the patches in patchsizes */
		void setPatch(int x, int y);
		///** sets patchID and recalculates patchseed */
		//void setPatchID(int id);
		///** sets patchSeed directly */
		//void setPatchSeed(int s) {patchSeed=s;}
		/** makes a new generator with the given seed */
		HeightGenerator(int s) : seed(s|1), RANDOMWEIGHT(0.02f), SLOPEWEIGHT(0.17f) {;}

		/** Main method */
		int getHeight(const PointGenData& data);

		static HeightGenerator* getInstance() {
			if(!singleton) {
				singleton=new HeightGenerator(DEFAULTSEED);
			} 
			return singleton;
		}
	};

	class LandPatch {
		LandPoint* base;
		LandPoint* oldPatch;
		int posx, posy;

		/** how deeply all collumns/rows are generated */
		short mDeep[2];
	
		/** Helps find the depth in the tree of a point
		*	&input n the row or collumn of the point
		*   @return MAXDEPTH-depth of the point
		*   @return -1 on error */
		int getHeight(int n) const;

		/** Generates a strip between the given edge points. 
		*	WARNING: does not test valid input! Strictly private */
		int generateStrip( LandPoint* low, LandPoint*high, bool lowIsParent, 
							 unsigned int coord, short stripDir, const LandPoint* old);

		/** generates a point, and all it's children */
		int generateStrip(PointGenData& pg);

		/** recursive strip generation down to a given deep */
		int generateDeep(LandPoint* low, LandPoint* high, bool lowIsParent,
						 int coord, int deltaPos, int stripDir, int restDeep);

		/** Flips old and new point. Nice for generating internal temporary points. 
		*	Remember to flip back. */
		void flip();

		/** Remakes theee corners from the data in oldPatch */
		void reinitiate();
	public:
		/** Copies the current landscape to old, and initiates a new start */
		void renew();
		
		/** initiates generation down to a given deep in the given direction */
		int generateDeep(int dir, int deep);

		/** initiates generation to the given deep in both directions 
		*	Probably unused */ 
		int generateDeep(int  deep) {
			generateDeep(0, deep);
			generateDeep(1, deep);
		}

		/** initiates stripGeneration  */
		int generateStrip(unsigned long coord, short stripDir);

		/** generates new Row and Columns in the patch 
		*   @return Errorcode
		*   0: allOK
		*   -1: Invallid input 
		*	-2: Unable to generate helper collumn 
		*   -3: Already made */
		int generateRow(unsigned long row) {return generateStrip(row, 1);}
		int generateCol(unsigned long col) {return generateStrip(col, 0);}
		
		/** generates a new ponint in the patch */
		void generatePoint(unsigned long c, unsigned long r) {
			generateRow(r);
			generateCol(c);
		}

		const LandPoint* cGetBase() const {return base;}

		
		/** returns the point currently the closest to the given point
		*	Optional parameters dx, dy is pointers to 
		*	integers to be filled with how much the point misses */
		LandPoint* getClosestPoint(int x, int y, int* dx, int* dy);
		LandPoint* getClosestPoint(int x, int y) {
			return getClosestPoint(x, y, NULL, NULL);
		}


		/** Gets the height at a point using the
		*	indicated method for aproximation
		*	@param method 
		*	0 for closest point
		*	1 Interpolated from slope of closest 
		*	2+ get precise height */
		int getHeight(int x, int y, short method);
		
		/** Gets presise height */
		int getPreciseHeight(int x, int y);

		/** Gets height from point based on the slope of it's closest neighbour */
		int getInterpolatedHeight(int x, int y);

		void getHeightMap(vector<int>& colcoords, 
							vector<int>& rowcoords,
							vector<int>& heights) {
			base->getHeightMap(0, 0, EDGEPOS>>1, EDGEPOS>>1, 
								colcoords, rowcoords, heights); 
		}
		
		/** makes a new landpatch with the given heights in the corners
		*	@param ul=upper left (0,0)
		*	@param ur=upper right (x,0)
		*	@param ll=lower left (0,x)
		*	@param lr=lower right (x,x) */
		LandPatch(int ul, int ur, int ll, int lr, int x, int y);
		
		~LandPatch() {
			if(base) delete base;
			if(oldPatch) delete oldPatch;
		}
	};

	/** Wrapper class for safe height map handling 
	*	implements a dohnut topology*/
	class HeightGrid {
		int* heights;
		int width, depth;
		bool owner;
	public:
		/** Relatively safe getter */
		int get(int x, int y) {
			return(heights[x%width+(y%depth)*width]);	
		}
		
		/** Relatively safe setter */
		void set(int x, int y, int height) {
			heights[x%width+(y%depth)*width]=height;
		}
		
		/** Gives full access to the raw data. 
		*	Use with causon as it is a standard array */
		int* getStream() { return heights; }
		int getWidth() { return width; }
		int getDepth() { return depth; }

		HeightGrid(int w, int d, int* map) 
			: width(w), depth(d), heights(map), owner(false){}

		HeightGrid(int w,int d) 
			:width(w), depth(d), owner(true){
			heights=new int[w*d];
		}
		~HeightGrid() { if(owner) delete heights; }
	};

};

/** Contains all information of landscapes */
class Landscape {

	int offset[2]; //where the upper left of the landscape is in the world(in landpatches)
	LandscapeHelpers::LandPatch* landscape[LANDSCAPESIZE*LANDSCAPESIZE]; //The current landscape of 5*5 patches
	LandscapeHelpers::HeightGrid* baseGrid; //the base behaviour of the terrain (doughnut)

	
	/** thread and effectivity handeling */
	bool requested; // an outside thread want to do something
	bool updated;   // the landscape has been updated
	bool busy;      // lock - no read or write
	bool ready;		// request accepted, wainting for read before doing more
	bool outdated;  // The landscape should be refreshed due to new world sit.

	/** called by all constructors */
	void initialize();
	
	/** Deletes all patches and reinitializes the landscape 
	*	practical for handeling position leaps */
	void refresh();

	/** generate a point in one of the patches 
	*	param patch The current index of the patch */
	void generatePoint(int patch, int x, int y);

public:
	bool isUpdated() { return updated; }
	void request() { requested=true; }
	bool isReady() { return ready; }

	void worstCaseTest();
	/** generates every point to the given depth in the given patch */
	void fullGrid(int index, int depth);

	void getHeightMap(int index, vector<int>& colcoords, 
						vector<int>& rowcoords,
						vector<int>& heights) {
		landscape[index]->getHeightMap(colcoords, rowcoords, heights); 
	}

	/** Generates a height 0 landscape */
	Landscape();
	/** Generates a lansdscape with constant height */
	Landscape(int height);
	Landscape(LandscapeHelpers::HeightGrid* bg);
	Landscape(LandscapeHelpers::HeightGrid* bg, int x, int y);
	~Landscape();
};



#endif