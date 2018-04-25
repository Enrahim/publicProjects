#include "landscape.h"

using namespace LandscapeHelpers;

HeightGenerator* HeightGenerator::singleton(NULL);

int LandPatch::getHeight(int n) const {
	for(int i=0; i<MAXDEPTH+1; i++) {
		if((1<<i)&n) return i;
	}
	return -1;
}

void LandPoint::getHeightMap(int x, int y, int cd, int rd, 
							vector<int>& cc, vector<int>&rc, vector<int>& h) {
	if(x==0) {
		if(y==0) {
			h.push_back(height);
			cc.push_back(x);
			rc.push_back(y);
			if(getLeft()) getLeft()->getHeightMap(cd, 0, cd>>1, rd, cc, rc, h);
			getRight()->getHeightMap(cd<<1,0,cd,rd, cc, rc,h);
			if(getUp()) getUp()->getHeightMap(0, rd, cd, rd>>1, cc, rc, h);
			getDown()->getHeightMap(0,rd<<1, cd, rd, cc, rc, h);
		} else {
			if(getUp())getUp()->getHeightMap(0,y-rd,cd,rd>>1, cc, rc,h);
			rc.push_back(y);
			h.push_back(height);
			if(getLeft()) getLeft()->getInnerHeightMap(h);
			getRight()->getInnerHeightMap(h);
			if(getDown()) getDown()->getHeightMap(0,y+rd,cd,rd>>1, cc, rc, h);
		} 
	} else { // ( asumes y=0 )
		if(getLeft()) getLeft()->getHeightMap(x-cd, y, cd>>1, rd, cc, rc, h);
		cc.push_back(x);
		h.push_back(height);
		if(getRight()) getRight()->getHeightMap(x+cd, y, cd>>1, rd, cc, rc, h);
	}
}

void LandPoint::getInnerHeightMap(vector<int>& h) {
	if(getLeft()) getLeft()->getInnerHeightMap(h);
	h.push_back(height);
	if(getRight()) getRight()->getInnerHeightMap(h);
}

//void HeightGenerator::setPatchID(int id) {
//	if(patchID==id) return;
//	patchID=id;
//	patchSeed=id*seed*PATCHSEEDFACTOR //%PATCHSEEDMOD;
//}

void HeightGenerator::setPatch(int x, int y)
{ 
	patchx=x<<MAXDEPTH;
	patchy=y<<MAXDEPTH;
}

int HeightGenerator::getRandom(int depth, int x, int y) {
	return ((199345*(patchx+x)+134449*(patchy+y))%((1<<depth)-1));//-(1<<(depth-1));
}

int HeightGenerator::getHeight(const PointGenData& data) {
	int depth[2];
	depth[data.stripDir]=data.dynDepthDesc;
	depth[1-data.stripDir]=data.statDepthDesc;
	int depthMax=(depth[0]>depth[1] ? 0 : 1);
	int depthDif=depth[depthMax]-depth[1-depthMax];

	int baseHeight[2]={0,0};
	for (int i=0; i<2; i++) {
		if(data.dirs[i]==NULL) continue;
		long long lowDelta = data.dirs[i+2]->getSlope(i)*(1<<depth[i]);
		long long highDelta = -data.dirs[i]->getSlope(i)*(1<<depth[i]);
		baseHeight[i]=(data.dirs[i]->getHeight()+data.dirs[i+2]->getHeight())/2;
		baseHeight[i]=(baseHeight[i]+SLOPEWEIGHT*(lowDelta+highDelta));
	}
	int resHeight=0;
	if(depthMax==MAXDEPTH) resHeight=baseHeight[1-depthMax];
	else resHeight=(baseHeight[depthMax]+(baseHeight[1-depthMax]<<depthDif))/((1<<depthDif)+1);
	resHeight+=RANDOMWEIGHT*getRandom(depth[1-depthMax], data.coord[0], data.coord[1]);
	return resHeight;
}

int LandPatch::generateDeep(LandPoint* low, LandPoint* high, bool lowIsParent, 
							int coord, int deltaPos, int stripDir, int restDeep) {
	generateStrip(low, high, lowIsParent, coord, stripDir, NULL);
	if(restDeep==0) return 0;
	LandPoint* current=NULL;
	if(lowIsParent) {
		if(coord==EDGEPOS>>1)
			current=low->getDir(1-stripDir+2);
		else
			current=low->getDir(1-stripDir);
	} else
		current=high->getDir(1-stripDir+2);
	if(!current) return -1;
	generateDeep(low, current, false, coord-deltaPos, deltaPos>>1, stripDir, restDeep-1);
	generateDeep(current, high, true, coord+deltaPos, deltaPos>>1, stripDir, restDeep-1);
}

int LandPatch::generateDeep(int dir, int deep) {
	if(mDeep[dir]>=deep) return 0;
	HeightGenerator::getInstance()->setPatch(posx, posy);
	generateDeep(base, base->getDir(1-dir), true, EDGEPOS>>1, EDGEPOS>>2, dir, deep-1);
	mDeep[dir]=deep;
	return 0;
}

int LandPatch::generateStrip(unsigned long coord, short stripDir) {
	int height=getHeight(coord);
	if(height==-1) return 1;
	//bool lowIsParent = (0 < (coord&(1<<(height+1))));
	//int lowNr=coord-(1<<height);
	//int highNr=coord+(1<<height);
	bool lowIsParent=true;

	int higher=1-stripDir;
	int lower=higher+2;

	LandPoint* lowStart=base;
	LandPoint* highStart=base->getDir(higher);
	//const LandPoint* lowOld=0;
	//const LandPoint* highOld=0;


	int depthDescriptor=EDGEPOS>>1;
	unsigned int searchNr=depthDescriptor;

	const LandPoint* searchOld=NULL;
	if(oldPatch) searchOld=oldPatch->cDir(lower);

	LandPoint* searchPoint;
	if(!(searchPoint=base->getDir(lower))&&searchNr!=coord) {
		generateStrip(lowStart, highStart, lowIsParent, searchNr, stripDir, searchOld);
		if(!(searchPoint=base->getDir(lower))) return -2;
	}

	while(searchNr!=coord) {
		depthDescriptor>>=1;
		int dir=higher;
		if(coord<searchNr) {
			dir=lower;
			highStart=searchPoint;
			lowIsParent=false;
			searchNr-=depthDescriptor;
		} else {
			dir=higher;
			lowStart=searchPoint;
			lowIsParent=true;
			searchNr+=depthDescriptor;
		}
		if(searchNr!=coord && !searchPoint->getDir(dir)) {
			generateStrip(lowStart, highStart, lowIsParent, searchNr, stripDir, searchOld);
			if(!searchPoint->getDir(dir)) return -2;
		}
		searchPoint=searchPoint->getDir(dir);
		if(searchOld) searchOld=searchOld->cDir(dir);
	}
	if(searchPoint) return -3;

	HeightGenerator::getInstance()->setPatch(posx,posy);

	return generateStrip(lowStart, highStart, lowIsParent, searchNr, stripDir, searchOld);
}

int LandPatch::generateStrip( LandPoint* low, LandPoint*high, bool lowIsParent, 
							 unsigned int coord, short stripDir, const LandPoint* old) {
	short higher=1-stripDir;
    PointGenData pointGen;
	pointGen.oldPoint=old;
	pointGen.coord[higher]=coord;
	pointGen.coord[stripDir]=0;
	pointGen.dirs[stripDir]=NULL;
	pointGen.dirs[stripDir+2]=NULL;
	pointGen.dirs[higher+2]=low;
	pointGen.dirs[higher]=high;
	pointGen.lowIsParent[higher]=lowIsParent;
	pointGen.lowIsParent[stripDir]=0;
	pointGen.stripDir=stripDir;
	pointGen.statDepthDesc=getHeight(coord)-1;
	pointGen.dynDepthDesc=MAXDEPTH-1;

	generateStrip(pointGen);
	return 0;
}

LandPatch::LandPatch(int ul, int ur, int ll, int lr, int x, int y) : posx(x), posy(y), oldPatch(NULL) {
	base=new LandPoint(ul, 0, 0, true);
	LandPoint* d=new LandPoint(ll,0,0, true);
	base->setChild(0,d);
	LandPoint* r=new LandPoint(ur,0,0, false);
	base->setChild(1,r);
	LandPoint* t=new LandPoint(lr, 0, 0, false);
	d->setChild(1, t);
	r->setChild(0, t);
	mDeep[0]=0;
	mDeep[1]=0;
}

LandPoint* LandPatch::getClosestPoint(int x, int y, int* dx, int *dy) {
	LandPoint* current=base;
	int pos[]={y,x};
	int* dpos[]={dy, dx};
	for(int i=0; i<2; i++) {
		LandPoint* bestLow=current;
		LandPoint* bestHigh=base->getDir(i);
		
		int coord=EDGEPOS>>1;
		if(pos[i]==0) {
			coord=0;
		} else if (pos[i]==EDGEPOS) {
			coord=EDGEPOS;
			current=base->getDir(i);
		} else {
			current=base->getDir(i+2);
		}
		
		int depthDesc=EDGEPOS>>2;
		while (current && coord!=pos[i]) {
			if(pos[i]<coord) {
				coord-=depthDesc;
				bestHigh=current;
				current=bestHigh->getDir(i);
			} else if (pos[i]>coord) {
				coord+=depthDesc;
				bestLow=current;
				current=bestLow->getDir(i+2);
			}
			depthDesc>>=1;
		}
		if(!current) {
			if(coord>=pos[i])
			{
				current=bestLow;
				if(dpos[i]) *dpos[i]=(depthDesc<<1)-(coord-pos[i]);
			}
			else {
				current=bestHigh;
				if(dpos[i]) *dpos[i]-coord-(depthDesc<<1);
			}
		}
	}
	return current;
}


int LandPatch::getHeight(int x, int y, short method) {
	switch(method) {
		case 0:
			return getClosestPoint(x, y)->getHeight();
		case 1:
			return getInterpolatedHeight(x, y);
		default: 
			return getPreciseHeight(x, y);
	}
}

int LandPatch::getPreciseHeight(int x, int y) {
	generatePoint(x,y);
	return getClosestPoint(x, y)->getHeight();
}

int LandPatch::getInterpolatedHeight(int x, int y) {
	int dx, dy;
	LandPoint* temp=getClosestPoint(x, y, &dx, &dy);
	return temp->getHeight()+(dx*temp->getRightSlope()+dy*temp->getDownSlope())/(1<<SLOPEOFFSET);
}

void LandPatch::reinitiate() {
	if(!oldPatch) return; //ERROR
	base=oldPatch->copy();
	LandPoint* nd=oldPatch->getDir(0)->copy();
	base->setChild(0, nd);
	LandPoint* nr=oldPatch->getDir(1)->copy();
	base->setChild(1, nr);
	LandPoint* last=oldPatch->getDir(0)->getDir(1)->copy();
	nd->setChild(1,last);
	nr->setChild(0,last);
}

void LandPatch::flip() {
	LandPoint* temp=oldPatch; //not time critical; safe swap
	oldPatch=base;
	base=oldPatch;
	if(!base) reinitiate();
}

void LandPatch::renew() {
	if(oldPatch) delete oldPatch;
	oldPatch=base;
	reinitiate();
}


//int LandPatch::getPreciseHeigth(int x, int y) {
//	int lowcoord=0;
//	int highcoord=EDGEPOS;
//	int curcoord=EDGEPOS>>1;
//	int distance=curcoord>>1;
//	LandPoint* current=base->getUp();
//	LandPoint* low=base;
//	LandPoint* high=base->getDown();
//	if(y==0){
//		current=low;
//		curcoord=y;
//	} else if (y==EDGEPOS) {
//		current=high;
//		curcoord=y;
//	} 
//	if(!current) {
//		generateStrip(low, high, true, curcoord, 1, NULL);
//		if(!current=base->getUp()) return 0; //ERROR
//	}
//
//	while(curcoord!=y) {
//		if(curcoord<y) {
//			curcoord+=distance;
//			distance>>=1;
//			low=current;
//			if(!current=current->getDown()) {
//				generateStrip(low, high, true, curcoord, 1, NULL);
//				if(!current=low->getDown()) return 0; //ERROR
//			}
//		} else {
//			curcoord-=distance;
//			distance>>=1;
//			high=current;
//			if(!current=current->getUp()) {
//				generateStrip(low, high, false, curcoord, 1, NULL);
//				if(!current=high->getUp()) return 0; //ERROR
//			}
//		}
//	}
//
//	curcoord=EDGEPOS>>1;
//	distance=curcoord>>1;
//	low=current;
//	high=current->getRight();
//	current=low->getLeft();
//	LandPoint* lowBase=base;
//	LandPoint* highBase=base->getRight();
//	LandPoint* currentBase=base->getLeft();
//	if(x==0){
//		current=low;
//		curcoord=x;
//	} else if (x==EDGEPOS) {
//		current=high;
//		curcoord=x;
//	} 
//	if(!current) {
//		generateStrip(lowBase, highBase, true, curcoord, 0, NULL);
//		if(!current=base->getLeft()) return 0; //ERROR
//		currentBase=lowBase->getLeft();
//	}	
//
//	while(curcoord!=x) {
//		if(curcoord<x) {
//			curcoord+=distance;
//			distance>>=1;
//			low=current;
//			lowBase=currentBase;
//			if(!current=current->getLeft()) {
//				generateStrip(lowBase, highBase, true, curcoord, 0, NULL);
//				if(!current=low->getLeft()) return 0; //ERROR
//			}
//			currentBase=lowBase->getLeft();
//
//		} else {
//			curcoord-=distance;
//			distance>>=1;
//			high=current;
//			highBase=currentBase;
//			if(!current=current->getRight()) {
//				generateStrip(lowBase, highBase, false, curcoord, 0, NULL);
//				if(!current=high->getRight()) return 0; //ERROR
//			}
//			currentBase=highBase->getDown();
//		}		
//	}
//
//	return current->getHeight();
//
	//int lowcoord[]={0,0};
	//int highcoord[]={EDGEPOS, EDGEPOS};
	//int curcoord[]={EDGEPOS>>1, EDGEPOS>>1}
	//int target[]={y,x};
	//int distance[]={EDGEPOS>>1, EDGEPOS>>1};
	//LandPoint* current[]={base->getUp(), base->getLeft()}
	//LandPoint* low[]={base, base};
	//LandPoint* high[]={base->getDown(), base->getRight()}
	//for(int sd=0; sd<2; sd++) {  //search direction
	//	if(target[sd]==0) {
	//		high[sd]=low[sd];
	//		highcoord[sd]=lowcoord[sd];
	//		continue;
	//	} else if (target[sd]==EDGEPOS) {
	//		low[sd]=high[sd];
	//		lowcoord[sd]=highcoord[sd];
	//		continue;
	//	}
	//	while(current && curcoord!=
//}
		

Landscape::Landscape() {
	offset[0]=offset[1]=0;
	baseGrid=new LandscapeHelpers::HeightGrid(1, 1);
	baseGrid->set(0,0,0);
	initialize();
}

Landscape::Landscape(int height) {
	offset[0]=offset[1]=0;
	baseGrid=new LandscapeHelpers::HeightGrid(1, 1);
	baseGrid->set(0,0, height);
	initialize();
}

Landscape::Landscape(HeightGrid* bg) : baseGrid(bg){ 
	offset[0]=offset[1]=LANDSCAPECENTER; 
	initialize();
}

Landscape::Landscape(HeightGrid* bg, int x, int y) : baseGrid(bg) {
	offset[0]=x;
	offset[1]=y;
	initialize();
}

Landscape::~Landscape() {
	for(int i=0; i<LANDSCAPESIZE*LANDSCAPESIZE; i++) {
		delete landscape[i];
	}
}

void Landscape::initialize() {
	int k=0;
	for(int i=offset[1]; i<offset[0]+LANDSCAPESIZE;i++) {
		for(int j=offset[0]; j<offset[0]+LANDSCAPESIZE; j++) {
			landscape[k++]=new LandPatch(
				baseGrid->get(j, i),
				baseGrid->get(j+1,i),
				baseGrid->get(j, i+1),
				baseGrid->get(j+1,i+1),
				j, i);
		}
	}
}

void Landscape::refresh() {
	for(int i=0; i<LANDSCAPESIZE*LANDSCAPESIZE; i++) {
		delete landscape[i];
	}
	initialize();
}

void Landscape::generatePoint(int patch, int x, int y) {
	int base=patch/LANDSCAPESIZE;
	for(int i=0; i<LANDSCAPESIZE; i++) {
		landscape[(base+i)%LANDSCAPESIZE]->generateRow(y);
	}
	for(int i=patch%LANDSCAPESIZE; i<LANDSCAPESIZE*LANDSCAPESIZE; i+=LANDSCAPESIZE) {
		landscape[i]->generateCol(x);
	}
}
	

void Landscape::worstCaseTest() {
	refresh();
	for(int i=0; i<LANDSCAPESIZE*LANDSCAPESIZE; i++) {
		generatePoint(i,1,1);
	}
}

void Landscape::fullGrid(int patch, int depth) {
	if(depth>MAXDEPTH) depth=MAXDEPTH;

	int base=patch/LANDSCAPESIZE;
	for(int i=-base; i<LANDSCAPESIZE-base; i++) {
		landscape[patch+i]->generateDeep(1, depth);
	}
	for(int i=patch%LANDSCAPESIZE; i<LANDSCAPESIZE*LANDSCAPESIZE; i+=LANDSCAPESIZE) {
		landscape[i]->generateDeep(0,depth);
	}
}


LandPoint* LandPoint::copy() const {
	return new LandPoint(height, slope[0], slope[1], edge);
}

LandPoint::~LandPoint() {
	if(edge&&dir[0]) delete dir[0];
	if(dir[1]) delete dir[1];
	if(edge&&dir[2]) delete dir[2];
	if(dir[3]) delete dir[3];
}


int LandPatch::generateStrip(PointGenData& pg) {
	LandPoint* madePoint;
	const LandPoint* lowOld=0;
	const LandPoint* highOld=0;
	int stripDir=pg.stripDir;
	int orthoDir=1-stripDir;
	
	
	if(pg.oldPoint) {
		madePoint=pg.oldPoint->copy();
		lowOld=pg.oldPoint->cDir(stripDir+2);
		highOld=pg.oldPoint->cDir(stripDir);
	} else {
		int h=HeightGenerator::getInstance()->getHeight(pg);

		int hdif=pg.dirs[orthoDir]->getHeight()-pg.dirs[orthoDir]->getHeight();
		int ss=(hdif<0 ? -(((-hdif)<<SLOPEOFFSET)>>pg.statDepthDesc) :
				((hdif << SLOPEOFFSET) >> pg.statDepthDesc));

		int ds=0;
		if(pg.dirs[stripDir]) {
			hdif=pg.dirs[stripDir]->getHeight()-pg.dirs[stripDir]->getHeight();
			ds=(hdif<0 ? -(((-hdif)<<SLOPEOFFSET)>>pg.dynDepthDesc) :
					((hdif << SLOPEOFFSET) >> pg.dynDepthDesc));
		}
		bool edge=(pg.coord[1]==0);
		madePoint=(stripDir ? new LandPoint(h, ss, ds, edge) : new LandPoint(h, ds, ss, edge));
	}
	
	if(pg.lowIsParent[orthoDir]) {
		if(pg.coord[orthoDir]==EDGEPOS>>1) 
			pg.dirs[orthoDir+2]->setChild(orthoDir+2, madePoint);
		else pg.dirs[orthoDir+2]->setChild(orthoDir, madePoint);
	} else {
		pg.dirs[orthoDir]->setChild(orthoDir+2, madePoint);
	}


	PointGenData npg;
	npg.lowIsParent[orthoDir]=pg.lowIsParent[orthoDir];
	npg.stripDir=pg.stripDir;
	npg.statDepthDesc=pg.statDepthDesc;
	npg.dynDepthDesc=pg.dynDepthDesc-1;
	npg.coord[orthoDir]=pg.coord[orthoDir];

	if(!pg.dirs[stripDir]) {
		if(pg.coord[stripDir]!=0){
			pg.dirs[stripDir+2]->setChild(stripDir, madePoint);
			return 0;
		}
		npg.coord[stripDir]=EDGEPOS;
		npg.dirs[stripDir]=0;
		npg.dirs[stripDir+2]=madePoint;
		npg.dirs[orthoDir]=pg.dirs[orthoDir]->getDir(stripDir);
		npg.dirs[orthoDir+2]=pg.dirs[orthoDir+2]->getDir(stripDir);
		npg.oldPoint=highOld;
		npg.lowIsParent[stripDir]=true;
	
		generateStrip(npg);

		if((pg.dirs[orthoDir]->cDir(stripDir+2))==0) return 0;
		npg.coord[stripDir]=EDGEPOS>>1;
		npg.dirs[stripDir]=madePoint->getDir(stripDir);
		npg.dirs[orthoDir]=pg.dirs[orthoDir]->getDir(stripDir+2);
		npg.dirs[orthoDir+2]=pg.dirs[orthoDir+2]->getDir(stripDir+2);
		npg.oldPoint=lowOld;
		npg.lowIsParent[stripDir]=true;

		return generateStrip(npg);
	}

	if(pg.lowIsParent[stripDir]) {
		if(pg.coord[stripDir]==EDGEPOS>>1)
			pg.dirs[pg.stripDir+2]->setChild(stripDir+2, madePoint);
		else pg.dirs[pg.stripDir+2]->setChild(stripDir, madePoint);
	} else {
		pg.dirs[pg.stripDir]->setChild(stripDir+2, madePoint);
	}


	if(pg.dirs[orthoDir]->cDir(stripDir)) {
		npg.coord[stripDir]=pg.coord[stripDir] + (1<<pg.dynDepthDesc);
		npg.dirs[stripDir]=pg.dirs[stripDir];
		npg.dirs[stripDir+2]=madePoint;
		npg.dirs[orthoDir]=pg.dirs[orthoDir]->getDir(stripDir);
		npg.dirs[orthoDir+2]=pg.dirs[orthoDir+2]->getDir(stripDir);		
		npg.oldPoint=highOld;
		npg.lowIsParent[stripDir]=true;
		

		generateStrip(npg);
	}

	if(!pg.dirs[orthoDir]->cDir(stripDir+2)) return 0;
	npg.coord[stripDir]=pg.coord[stripDir]-(1 << pg.dynDepthDesc);
	npg.dirs[stripDir]=madePoint;
	npg.dirs[stripDir+2]=pg.dirs[pg.stripDir+2];
	npg.dirs[orthoDir]=pg.dirs[orthoDir]->getDir(stripDir+2);
	npg.dirs[orthoDir+2]=pg.dirs[orthoDir+2]->getDir(stripDir+2);
	npg.oldPoint=lowOld;
	npg.lowIsParent[stripDir]=false;

	return generateStrip(npg);
}



//
// END OF FILE, REST IS JUNK
//
//int Landscape::LandPatch::generateCol(unsigned long col) {
//	int height=getHeight(col);
//	if(depth==-1) return 1;
//	bool leftIsParent = (0 < (col&(1<<(height+1)))
//	int leftNr=col-(1<<height);
//	int rightNr=col+(1<<height);
//	const LandPoint* leftStart=0;
//	const LandPoint* rightStart=0;
//	const LandPoint* leftOld=0;
//	const LandPoint* rightOld=0;
//	
//	if(leftNr==0){
//		leftStart=base;
//		if(oldPatch) leftOld=oldPatch->cGetBase();
//	}
//	if(rightNr==Landscape::EDGEPOS){
//		rightStart=base->getRight();
//		if(oldPatch) rightOld=oldPatch->cGetBase()->cRight();
//	}
//
//	LandPoint* searchPoint;
//	int depthDescriptor=Landscape::EDGEPOS>>1;
//	int searchCol=depthDecriptor;
//	if(!searchPoint=base->getDown()) {
//		generateCol(Landscape::EDGEPOS >> 1);
//		if(!base->cDown()) return -2;
//	}
//	const LandPoint* searchOld=NULL;
//	if(oldPatch) oldPatch->cGetBase()->cLeft();
//
//
//	while(searchCol!=col) {
//		if(searchCol==leftNr) {
//			leftPoint=searchPoint;
//			leftOld=searchOld;
//		}
//		if(searchCol==rightNr) {
//			rightPoint=searchPoint;
//			rightOld=searchOld;
//		}
//		depthDescriptor>>=1;
//		if(col<searchCol) {
//			seachCol-=depthDescriptor;
//			if(searchPoint->getLeft()) {
//				generateCol(searchCol);
//				if(!searchPoint->getLeft()) return -2;
//			}
//			searchPoint=searchPoint->getLeft();
//			if(searchOld) searchOld=searchOld->cLeft();
//		} else if (col>searchCol) {
//			seachCol+=depthDescriptor;
//			if(searchPoint->getRight()) {
//				generateCol(searchCol);
//				if(!searchPoint->getRight()) return -2;
//			}
//			searchPoint=searchPoint->getRight();
//			if(searchOld) searchOld=searchOld->cRight();
//		}
//	}
//	PointGenData pointGen(leftOld, rightOld, NULL, NULL);
//	pointGen.collumn=searchCol;
//	pointGen.row=0;
//	pointGen.up=NULL;
//	pointGen.down=NULL;
//	pointGen.left=leftStart;
//	pointGen.right=rightStart;
//	pointGen.leftIsParent=leftIsParent;
//	pointGen.upIsParent=false;
//	pointGen.rowgen=false;
//
//	generatePoint(pointGen);
//}
