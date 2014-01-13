/*********************************************************************************
* MONASH UNIVERSITY, Clayton's School of IT
* Student Declaration for CSE3313 Submission 2007
*   
* This source code is the original work of Andrei Gec, ID: 19869908
* My email address is agec1@student.monash.edu.au
* I, Andrei Gec declare that this submission is my own work and has not been copied from
* any other source without attribution. I have read the School and Faculty
* rules on plagiarism and I acknowledge that severe penalties
* exist for any copying of code without attribution, including a mark of 0
* for this assessment.
*********************************************************************************/

#include <glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


void initwindow();
void update_func();
void changesize(int w, int h );



//classes

//normv is a stripped down version of vector3
class normv
{

public:
	float x;
	float y;
	float z;
	normv();

	normv operator*(float value)
	{
		normv out;
		out.x=x*value;
		out.y=y*value;
		out.z=z*value;
		return out;
	}
};

//x,y,z vertex/vector class, has operator overloading
class vector3
{
public:

	vector3();
	float x;
	float y;
	float z;
	normv normalvect;

	vector3 operator-(vector3 vector)
	{
		vector3 out;
		out.x=x-vector.x;
		out.y=y-vector.y;
		out.z=z-vector.z;
		return out;
	}

	vector3 operator+(vector3 vector)
	{
		vector3 out;
		out.x=x+vector.x;
		out.y=y+vector.y;
		out.z=z+vector.z;
		return out;
	}

	vector3 operator/(float value)
	{
		vector3 out;
		out.x=x/value;
		out.y=y/value;
		out.z=z/value;
		return out;
	}
};

//console class, push text onto message area
class CON
{
public:
	static const int maxlines=5;
	void pushtext(char * str);
	char textarray[maxlines][100];
	void init();
	void drawtext();

};
CON CONSOLE;

//FPS class
class FPZ
{
public:
	FPZ();
	int frame;
	int time;
	int timebase;
	int fps;
	float elapsedtime;	
};

//global class to hold global variables and functions that the program uses
class GLO
{
public:
	FPZ FPS;
	int dontrefresh;
	int dontmovemouse;
	int firsttime;
	int fullscreen;
	//int count;
	//int countmax;
	//60 fps display tick
	static const int displaytick=61;
	//60 move and collision check
	static const int movetick=61;
	bool rotatefunct(float * x, float * y,float *z,float anglexz,float angley,float speed);
	GLO();
	int SCRwidth;
	int SCRheight;
	void changezoom(float value);
	float zoom;
};
GLO GLOBAL;

//display list initialisation
class DL
{
public:
	int land;	
	int car;
	void init();

	void drawplayer();
	void drawland();
	void drawstations();
	void drawparliament();
	void drawsea();
	void drawmain();
};
DL DISPLAYLISTS;

//terrain class to create the random terrain
//roughness is set to 200 which is fairly smooth
//middle peak is set to 500-1000 which means the map will look like an island
class TER
{
public:
	vector3 ** TERRAIN;
	vector3 ** PLANENRM;
	void createterrain(int level,int maxsize);
	void getrandpoint(int x,int y,float randamount);
	void changeterrain(int width,int height);
	void initTERRAIN();
	void initrecurse(int wmin,int wmax,int hmin,int hmax,int randamount,int randminus);

	vector3 getnormalavg(int y,int x);
	void calcnormals();
	void drawTERRAIN();
	int getwidth();
	int getheight();
	float getypos(float xpos,float zpos);
	void setsize(int level,int maxsize);
	int getLEVEL();
	int getMAXSIZE();
	TER();
	~TER();
	void dinit();
	bool iswireframe;
	bool shownormals;
private:

	int TERRAINWIDTH;
	int TERRAINHEIGHT;
	int LEVEL;
	int MAXSIZE;
	int twmin,twmid,twmax;
	int thmin,thmid,thmax;
};

//player class to hold all relevent player information
class PLA
{
public:
	PLA();
	bool key_state[256];
	int keydown;
	vector3 position;
	vector3 normalavg;
	float xsize,zsize;
	float velocity;
	static const int maxvelocity=25;
	float angle;
	//points is per game, score is overall
	int points;
	float score;
	void addvel(float value);

	void addangle(float value);
};
PLA PLAYER;

//light class
class LIG
{
public:
	int xpos;
	int ypos;
	int zpos;
	bool isdiffuse;
	bool isambient;
	LIG * nextptr;
};

//light class to add lights to program
class LIGHEAD
{
public:
	LIG * headptr;
	int count;
	LIG *addlight(int xpos,int ypos,int zpos);
	bool isenabled;
	LIGHEAD();
	~LIGHEAD();
};

//map class to hold information about stations, the land, the sea etc
class MA
{
public:
	TER LAND;	
	LIGHEAD LIGHTS;
	normv SEA[4]; 
	static const int numofstations=4;
	normv stations[numofstations];
	//bool if that number station is decommissioned.
	bool stationdone[numofstations];
	//num of stations dec.
	int stationsleft;
	//whether or not a station is present in range
	bool nearstation;
	//the number of the nearest station if in range
	int nearstationnum;

	void initsea();
	void changesea(float value);
};
MA MAP;

//cross product
vector3 CROSS(vector3 vec1,vector3 vec2)
{
	vector3 out;
	out.x=((vec1.y*vec2.z)-(vec1.z*vec2.y));
	out.y=((vec1.z*vec2.x)-(vec1.x*vec2.z));
	out.z=((vec1.x*vec2.y)-(vec1.y*vec2.x));

	return out;
}

//magnitude function
float MAGNITUDE(vector3 vector)
{
	float out=sqrt((vector.x*vector.x)+(vector.y*vector.y)+(vector.z*vector.z));
	return out;
}

//normalize the vector
vector3 NORMALIZE(vector3 vector)
{
	float magnitude = MAGNITUDE(vector);
	if (magnitude==0)
		return vector;

	vector.x/=magnitude;
	vector.y/=magnitude;
	vector.z/=magnitude;

	return vector;
}

//get the normal from 3 polygonal vectors
vector3 NORMAL(vector3 vec1,vector3 vec2,vector3 vec3)
{
	vector3 myvec1=vec3-vec1;
	vector3 myvec2=vec2-vec1;
	//cross to get normal angle
	vector3 vectornormal=CROSS(myvec1,myvec2);
	//normalize if length >1 to 1
	vectornormal=NORMALIZE(vectornormal);

	return vectornormal;
}

//get the dot product from 2 vectors
/*
float DOT(vector3 vec1,vector3 vec2)
{
return ((vec1.x+vec2.x)+(vec1.y+vec2.y)+(vec1.z+vec2.z));
}
*/

vector3::vector3()
{
	x=0;
	y=0;
	z=0;
}

normv::normv()
{
	x=0;
	y=0;
	z=0;
}

//writing functions
void glutStrokeString(char *sstring)
{
	int length=(signed)strlen(sstring);
	int a;
	for (a=0;a<length;a++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN,sstring[a]);
}

void glutStrokeInteger(float temp)
{
int value=(int)temp;
	char sstring[20];sprintf(sstring,"%d", value);glutStrokeString(sstring);
}


//draw the consoles text on the screen
void CON::drawtext()
{
	for (int a=0;a<maxlines;a++)
	{
		//glColor3f(1,1,1);
		glPushMatrix();
		if (textarray[a][0]=='+')
			glColor3f(0,1,0);
		else if (textarray[a][0]=='-')
			glColor3f(1,0,0);
		else if (textarray[a][0]=='*')
			glColor3f(0,0.8,1);

		else
			glColor3f(1,1,1);

		if (strlen(textarray[a])>15)
			glScalef(.1,.1,.1);	
		else
			glScalef(.15,.15,.15);
		glutStrokeString(textarray[a]);
		glPopMatrix();
		glTranslatef(0,-20,0);
	}
}


//push text into the console
void CON::pushtext(char * str)
{
	int a=0;
	for (a=1;a<maxlines;a++)
	{strcpy(textarray[maxlines-a],textarray[maxlines-(a+1)]);}
	strcpy(textarray[0],str);return;
}

//set messages to blank
void CON::init()
{
	for (int a=0;a<maxlines;a++)
		strcpy(textarray[a],"");
}

void TER::dinit()
{
	if (TERRAIN!=NULL)
	{
		for (int a=0;a<TERRAINHEIGHT;a++)
		{
			delete[] TERRAIN[a];
			delete[] PLANENRM[a];
		}
		delete[] TERRAIN;
		delete[] PLANENRM;
		TERRAIN=NULL;
		PLANENRM=NULL;
	}
}

TER::~TER()
{
	dinit();

}
TER::TER()
{
	TERRAINWIDTH=0;
	TERRAINHEIGHT=0;
	LEVEL=4;
	MAXSIZE=10000;
	twmin=0;
	twmid=0;
	twmax=0;
	thmin=0;
	thmid=0;
	thmax=0;
	iswireframe=false;
	shownormals=false;
}


int TER::getLEVEL()
{
	return LEVEL;
}

int TER::getMAXSIZE()
{
	return MAXSIZE;
}
void TER::setsize(int level,int maxsize)
{
	LEVEL=level;
	MAXSIZE=maxsize;
}

//get a random value for terrain height
float getrand(float randy)
{	
	if (randy<=0)
		return 1;
	//random is between -randy/2 to randy
	//being less than -randy to randy means an island will form
	float randz=rand()%((int)randy);
	float randz2=rand()%((int)randy);
	float out=randz-(randz2/2);
	return out;
}

//set the terrain point to the average + a random value
void TER::getrandpoint(int y,int x,float randamount)
{
	float avgtotal2=0;
	float avgcount2=0;
	avgtotal2+=TERRAIN[thmin][twmin].y;avgcount2++;
	avgtotal2+=TERRAIN[thmin][twmax].y;avgcount2++;
	avgtotal2+=TERRAIN[thmax][twmin].y;avgcount2++;
	avgtotal2+=TERRAIN[thmax][twmax].y;avgcount2++;

	float avg=(avgtotal2/avgcount2);
	TERRAIN[y][x].y=avg+getrand(randamount);
}

//recursive function to create fractal terrain for game
void TER::initrecurse(int wmin,int wmax,int hmin,int hmax,int randamount,int randminus)
{
	int hmid=(hmax+hmin)/2;
	int wmid=(wmax+wmin)/2;

	if ((hmid<1||wmid<1)||(wmin==wmid||wmax==wmid))
		return;	

	if (((wmax-wmin)%2==0)&&((hmax-hmin)%2==0))
	{
		twmin=wmin;
		twmax=wmax;
		thmin=hmin;
		thmax=hmax;

		//do mid point
		if (TERRAIN[hmid][wmid].y==-1)
			getrandpoint(hmid,wmid,randamount);


		//do diamond step
		if (TERRAIN[hmax][wmid].y==-1)
			getrandpoint(hmax,wmid,randamount);

		if (TERRAIN[hmin][wmid].y==-1)
			getrandpoint(hmin,wmid,randamount);

		if (TERRAIN[hmid][wmax].y==-1)
			getrandpoint(hmid,wmax,randamount);

		if (TERRAIN[hmid][wmin].y==-1)
			getrandpoint(hmid,wmin,randamount);
	}
	else
	{
		printf("\nterrain gen error");
		return;
	}

	randamount-=randminus;
	initrecurse(wmin,wmid,hmin,hmid,randamount,randminus);
	initrecurse(wmid,wmax,hmin,hmid,randamount,randminus);
	initrecurse(wmin,wmid,hmid,hmax,randamount,randminus);
	initrecurse(wmid,wmax,hmid,hmax,randamount,randminus);
	return; 

}

//get the normal average
vector3 TER::getnormalavg(int y,int x)
{
	vector3 avgvalue;
	avgvalue.x=0;
	avgvalue.y=0;
	avgvalue.z=0;
	int avgcount=0;


	if (y-1>=0&&x-1>=0)
	{
		avgvalue = avgvalue+NORMAL(TERRAIN[y-1][x],TERRAIN[y][x],TERRAIN[y-1][x-1]);avgcount++;
		avgvalue = avgvalue+NORMAL(TERRAIN[y-1][x-1],TERRAIN[y][x],TERRAIN[y][x-1]);avgcount++;
	}


	if (y-1>=0&&x+1<=getwidth()-1)
	{
		avgvalue = avgvalue+NORMAL(TERRAIN[y-1][x+1],TERRAIN[y][x],TERRAIN[y-1][x]);avgcount++;
		avgvalue = avgvalue+NORMAL(TERRAIN[y][x+1],TERRAIN[y][x],TERRAIN[y-1][x+1]);avgcount++;
	}

	if (y+1<=getheight()-1&&x+1<=getwidth()-1)
	{
		avgvalue = avgvalue+NORMAL(TERRAIN[y+1][x+1],TERRAIN[y][x],TERRAIN[y][x+1]);avgcount++;
		avgvalue = avgvalue+NORMAL(TERRAIN[y+1][x],TERRAIN[y][x],TERRAIN[y+1][x+1]);avgcount++;
	}

	if (y+1<=getheight()-1&&x-1>=0)
	{

		avgvalue = avgvalue+NORMAL(TERRAIN[y+1][x-1],TERRAIN[y][x],TERRAIN[y+1][x]);avgcount++;
		avgvalue = avgvalue+NORMAL(TERRAIN[y][x-1],TERRAIN[y][x],TERRAIN[y+1][x-1]);avgcount++;
	}

	vector3 out=(avgvalue/avgcount);
	return out;
}

//for each terrain point calculate the vertex and plane normals
void TER::calcnormals()
{
	int h=getheight()-1;
	int w=getwidth()-1;

	for (int a=0;a<h;a++)
	{
		for (int b=0;b<w;b++)
		{	
			vector3 out=getnormalavg(a,b);

			TERRAIN[a][b].normalvect.x=out.x;
			TERRAIN[a][b].normalvect.y=out.y;
			TERRAIN[a][b].normalvect.z=out.z;
		}
	}

	for (int a=0;a<h;a++)
	{
		for (int b=0;b<w;b++)
		{
			PLANENRM[a][b].normalvect.x=(TERRAIN[a][b].normalvect.x+TERRAIN[a][b+1].normalvect.x+TERRAIN[a+1][b+1].normalvect.x+TERRAIN[a+1][b].normalvect.x)/4;
			PLANENRM[a][b].normalvect.y=(TERRAIN[a][b].normalvect.y+TERRAIN[a][b+1].normalvect.y+TERRAIN[a+1][b+1].normalvect.y+TERRAIN[a+1][b].normalvect.y)/4;
			PLANENRM[a][b].normalvect.z=(TERRAIN[a][b].normalvect.z+TERRAIN[a][b+1].normalvect.z+TERRAIN[a+1][b+1].normalvect.z+TERRAIN[a+1][b].normalvect.z)/4;
		}
	}

}

void TER::initTERRAIN()
{
	float startx=0;
	//	float starty=0;
	float startz=0;
	int TH=TERRAINHEIGHT;
	int TW=TERRAINWIDTH;
	int width=MAXSIZE/TW;
	int height=MAXSIZE/TH;

	for (int a=0;a<TH;a++)
	{
		for (int b=0;b<TW;b++)
		{			
			TERRAIN[a][b].x=startx;
			TERRAIN[a][b].y=-1;
			TERRAIN[a][b].z=startz;
			startx+=width;
		}
		startx=0;
		startz+=height;
	}


	//clamp edges.y to 0
	for (int a=0;a<TH;a++)
	{
		for (int b=0;b<TW;b++)
		{			
			if (a==0||a==TH-1||b==0||b==TW-1)
				TERRAIN[a][b].y=0;
		}

	}
	//seed centre.y to 500
	int hmid=(TERRAINHEIGHT-1)/2;
	int wmid=(TERRAINWIDTH-1)/2;
	TERRAIN[hmid][wmid].y=1000;

	//rands = roughness
	int rands=200;
	int randminus=rands/LEVEL;

	srand(time(NULL));

	initrecurse(0,TERRAINWIDTH-1,0,TERRAINHEIGHT-1,rands,randminus);

	//calculate the normals
	calcnormals();

}

void GLO::changezoom(float value)
{
	if ((zoom+value)>=180||(zoom+value)<=0)
		return;
	else 
		zoom+=value;

}


PLA::PLA()
{
	for (int a=0;a<256;a++)
		key_state[a]=false;
	position.z=-140;
	velocity=0;
	angle=0;
	xsize=0;
	zsize=0;
	points=0;
	score=0;
}


void PLA::addangle(float value)
{
	angle+=value;
	if (angle>360)angle-=360;
	else if (angle<0)angle=360+angle;
}


void lookat(float zoom,float mindist,float eyex,float eyey,float eyez,float tgtx,float tgty,float tgtz)
{
	gluPerspective(zoom,GLOBAL.SCRwidth/GLOBAL.SCRheight,mindist,10000);
	gluLookAt(eyex,eyey,eyez,tgtx,tgty,tgtz,0,1,0);
}


LIGHEAD::~LIGHEAD()
{
	LIG * curr=headptr;
	LIG * next=NULL;
	while (curr!=NULL)
	{
		next=curr->nextptr;
		delete curr;
		curr=next;
	}

}
LIGHEAD::LIGHEAD()
{
	isenabled=false;
}

LIG *LIGHEAD::addlight(int xpos, int ypos, int zpos)
{
	if (headptr==NULL)
	{
		headptr=new LIG;
		headptr->xpos=xpos;
		headptr->ypos=ypos;
		headptr->zpos=zpos;
		headptr->nextptr=0;
		return headptr;
	}
	return NULL;
}


void MA::initsea()
{
	float mx=MAP.LAND.getMAXSIZE();
	SEA[0].x=0;
	SEA[1].x=mx;
	SEA[2].x=mx;
	SEA[3].x=0;

	SEA[0].y=0;
	SEA[1].y=0;
	SEA[2].y=0;
	SEA[3].y=0;

	SEA[0].z=0;
	SEA[1].z=0;
	SEA[2].z=mx;
	SEA[3].z=mx;
}

void MA::changesea(float heightvalue)
{
	for (int a=0;a<4;a++)
		SEA[a].y+=heightvalue;
}

//project a point out with a speed and angle
bool GLO::rotatefunct(float * x, float * y,float * z,float anglexz,float angley,float speed)
{	
	anglexz*=3.14159/180;
	angley*=3.14159/360;
	float sizex=speed*(-sin(anglexz));
	float sizey=speed*(-cos(angley));
	float sizez=speed*(-cos(anglexz));
	if (x!=NULL)
		*x+=sizex;
	if (y!=NULL)
		*y+=sizey;
	if (z!=NULL)
		*z+=sizez;
	return true;
}


GLO::GLO()
{
	dontrefresh=0;
	dontmovemouse=0;
	firsttime=1;
	fullscreen=0;
	SCRwidth=800;
	SCRheight=600;
	zoom=70;
}

FPZ::FPZ()
{
	frame=0;
	time=0;
	timebase=0;
	fps=0;
}


int TER::getwidth()
{
	return TERRAINWIDTH;
}

int TER::getheight()
{
	return TERRAINHEIGHT;
}


void heightcolour(float value)
{
	int max=1000;
	if (value>0)
		glColor3f(0,value/max,0);	
	if (value>=max)
		glColor3f(1,1,1);
	else if (value<=0)
		glColor3f(.5,.27,.25);	


}

//draw terrain and normals if normals are set to get seen
void TER::drawTERRAIN()
{
	int TW=getwidth();
	int TH=getheight();
	int a=0;int b=0;
	glColor4f(1,1,1,1);

	for (a=0;a<TH-1;a++)
	{
		if (iswireframe==true)
			glBegin(GL_LINE_STRIP);
		else
			glBegin(GL_TRIANGLE_STRIP);


		heightcolour(TERRAIN[a][0].y);
		glNormal3f(TERRAIN[a][0].normalvect.x,TERRAIN[a][0].normalvect.y,TERRAIN[a][0].normalvect.z);
		glVertex3f(TERRAIN[a][0].x,TERRAIN[a][0].y,TERRAIN[a][0].z);

		heightcolour(TERRAIN[a+1][0].y);
		glNormal3f(TERRAIN[a+1][0].normalvect.x,TERRAIN[a+1][0].normalvect.y,TERRAIN[a+1][0].normalvect.z);
		glVertex3f(TERRAIN[a+1][0].x,TERRAIN[a+1][0].y,TERRAIN[a+1][0].z);


		int wcount=0;
		for (b=0;b<TW-1;b++)
		{
			wcount++;
			int a2=a;
			int b2=b;

			wcount%=2;
			if (wcount==0)
			{a2=a+1;b-=1;}
			else
			{b2=b+1;}

			heightcolour(TERRAIN[a2][b2].y);
			glNormal3f(TERRAIN[a2][b2].normalvect.x,TERRAIN[a2][b2].normalvect.y,TERRAIN[a2][b2].normalvect.z);
			glVertex3f(TERRAIN[a2][b2].x,TERRAIN[a2][b2].y,TERRAIN[a2][b2].z);
		}

		heightcolour(TERRAIN[a+1][b].y);
		glNormal3f(TERRAIN[a+1][b].normalvect.x,TERRAIN[a+1][b].normalvect.y,TERRAIN[a+1][b].normalvect.z);
		glVertex3f(TERRAIN[a+1][b].x,TERRAIN[a+1][b].y,TERRAIN[a+1][b].z);
		glEnd();
	}


	if (MAP.LAND.shownormals==true)
	{
		for (int a=0;a<TH-1;a++)
		{	
			for (int b=0;b<TW-1;b++)
			{
				normv out = TERRAIN[a][b].normalvect;
				out=out*100;
				glBegin(GL_LINES);
				glColor3f(1,0,0);
				glVertex3f(TERRAIN[a][b].x,TERRAIN[a][b].y,TERRAIN[a][b].z);
				glColor3f(1,1,0);
				glVertex3f(TERRAIN[a][b].x+out.x,TERRAIN[a][b].y+out.y,TERRAIN[a][b].z+out.z);
				glEnd();

				//plane normals

				out.x=PLANENRM[a][b].normalvect.x;
				out.y=PLANENRM[a][b].normalvect.y;
				out.z=PLANENRM[a][b].normalvect.z;

				float dist;
				dist=(TERRAIN[0][1].x-TERRAIN[0][0].x)/2;


				out=out*100;
				glBegin(GL_LINES);
				glColor3f(0,0,1);
				glVertex3f(TERRAIN[a][b].x+dist,TERRAIN[a][b].y,TERRAIN[a][b].z+dist);
				glColor3f(0,1,1);
				glVertex3f(TERRAIN[a][b].x+out.x+dist,TERRAIN[a][b].y+out.y,TERRAIN[a][b].z+out.z+dist);
				glEnd();

			}
		}
	}
}

void TER::changeterrain(int width,int height)
{
	dinit();

	TERRAINWIDTH=width;
	TERRAINHEIGHT=height;

	TERRAIN = new vector3*[TERRAINHEIGHT];
	PLANENRM= new vector3*[TERRAINHEIGHT];
	for (int a=0;a<TERRAINHEIGHT;a++)
	{
		TERRAIN[a] = new vector3[TERRAINWIDTH];
		PLANENRM[a]=new vector3[TERRAINWIDTH];
	}
}


void TER::createterrain(int level,int maxsize)
{
	if (level<=0)return;
	int vertexes=(int)(1+powf(2,level));
	setsize(level,maxsize);
	changeterrain(vertexes,vertexes);
	initTERRAIN();
	DISPLAYLISTS.init();

}


void PLA::addvel(float value)
{
	if ((velocity+value)<(maxvelocity+1)&&(velocity+value)>-(maxvelocity+1))
		velocity+=value;
}

//get the height and yaw/pitch/roll of player
float TER::getypos(float xpos,float zpos)
{
	if (xpos<0||zpos<0)
		return 0;
	int h=getheight();
	int w=getwidth();

	//use 3 point average method
	//get the average of two points on x axis, and use that
	//to get the average of two points on the y axis.

	float xarray=fabs(xpos-TERRAIN[0][0].x)/(MAXSIZE/w);
	float zarray=fabs(zpos-TERRAIN[0][0].z)/(MAXSIZE/h);

	int nearx=(int)floor(xarray);
	int farx=(int)ceil(xarray);

	int nearz=(int)floor(zarray);
	int farz=(int)ceil(zarray);

	if (farz>=h||farx>=w||nearx>=w||nearz>=h)
		return 0;

	float xarr=xarray-(int)xarray;
	float out1=((xarr)*TERRAIN[farx][nearz].y);
	float out2=((1-xarr)*TERRAIN[nearx][nearz].y);
	float outone=out1+out2;

	out1=((xarr)*TERRAIN[farx][farz].y);
	out2=((1-xarr)*TERRAIN[nearx][farz].y);
	float outtwo=out1+out2;

	float zarr=zarray-(int)zarray;
	out1=((zarr)*outtwo);
	out2=((1-zarr)*outone);
	//final result for y value
	float out=out1+out2;

	//normal
	//use the same method as above, but for x,y and z of the normal

	vector3** current=TERRAIN;
	vector3 p1;
	p1.x=((xarr)*current[farx][nearz].normalvect.x);
	p1.y=((xarr)*current[farx][nearz].normalvect.y);
	p1.z=((xarr)*current[farx][nearz].normalvect.z);

	vector3 p2;
	p2.x=((1-xarr)*current[nearx][nearz].normalvect.x);
	p2.y=((1-xarr)*current[nearx][nearz].normalvect.y);
	p2.z=((1-xarr)*current[nearx][nearz].normalvect.z);

	vector3 p3;
	p3.x=p1.x+p2.x;
	p3.y=p1.y+p2.y;
	p3.z=p1.z+p2.z;

	vector3 p4;
	p4.x=((xarr)*current[farx][farz].normalvect.x);
	p4.y=((xarr)*current[farx][farz].normalvect.y);
	p4.z=((xarr)*current[farx][farz].normalvect.z);

	vector3 p5;
	p5.x=((1-xarr)*current[nearx][farz].normalvect.x);
	p5.y=((1-xarr)*current[nearx][farz].normalvect.y);
	p5.z=((1-xarr)*current[nearx][farz].normalvect.z);

	vector3 p6;
	p6.x=p4.x+p5.x;
	p6.y=p4.y+p5.y;
	p6.z=p4.z+p5.z;


	vector3 p7;
	p7.x=((zarr)*p5.x);
	p7.y=((zarr)*p5.y);
	p7.z=((zarr)*p5.z);

	vector3 p8;

	p8.x=((1-zarr)*p3.x);
	p8.y=((1-zarr)*p3.y);
	p8.z=((1-zarr)*p3.z);

	vector3 pout;
	pout.x=p7.x+p8.x;
	pout.y=p7.y+p8.y;
	pout.z=p7.z+p8.z;


	PLAYER.normalavg=pout;
	return out;
}


void DL::init()
{
	glGenLists(2);
	land=1;
	car=2;
	glLoadIdentity();
	glNewList(land,GL_COMPILE);
	MAP.LAND.drawTERRAIN();
	glEndList();

	glLoadIdentity();
	int size=100;
	PLAYER.xsize=size;
	PLAYER.zsize=size;

	glNewList(car,GL_COMPILE);

	glBegin(GL_QUADS);
	//front
	glColor3f	(1		,0		,0);
	glVertex3f	(0		,0		,0);
	glVertex3f	(size	,0		,0);
	glVertex3f	(size	,size	,0);
	glVertex3f	(0		,size	,0);
	glColor3f	(1		,1		,1);

	//back
	glVertex3f(0,0,size);
	glVertex3f(size,0,size);
	glVertex3f(size,size,size);
	glVertex3f(0,size,size);

	//top
	glColor3f(0,0,1);
	glVertex3f(0,size,0);
	glVertex3f(size,size,0);
	glVertex3f(size,size,size);
	glVertex3f(0,size,size);


	glEnd();
	//glutSolidSphere(10,10,20);
	glEndList();

}

void DL::drawplayer()
{
	glColor3f(1,1,1);

	float px=PLAYER.position.x;
	float py=PLAYER.position.y;
	float pz=PLAYER.position.z;
	glTranslatef(px,py,pz);
	glRotatef(-PLAYER.angle,0,-1,0);
	glRotatef(PLAYER.normalavg.z*90,0,0,-1);
	glRotatef(PLAYER.normalavg.x*90,-1,0,0);
	glColor3f(0,1,0);
	glCallList(DISPLAYLISTS.car);
	glRotatef(-PLAYER.normalavg.x*90,-1,0,0);
	glRotatef(-PLAYER.normalavg.z*90,0,0,-1);
	glRotatef(PLAYER.angle,0,-1,0);
	glTranslatef(-px,-py,-pz);
}

void DL::drawland()
{
	glCallList(DISPLAYLISTS.land);
}

void DL::drawsea()
{
	glColor3f(0,0,1);
	glBegin(GL_QUADS);
	for (int a=3;a>-1;a--)
		glVertex3f(MAP.SEA[a].x,MAP.SEA[a].y,MAP.SEA[a].z);
	glEnd();	
}


void DL::drawstations()
{
	glPushMatrix();

	for (int a=0;a<MAP.numofstations;a++)
	{
		if (MAP.stationdone[a]==false)
			glColor3f(1,1,0);
		else
			glColor3f(1,0,0);

		glPushMatrix();
		glTranslatef(MAP.stations[a].x,MAP.stations[a].y,MAP.stations[a].z);
		glRotatef(90,-1,0,0);
		glutSolidCone(200,200,20,10);
		glRotatef(-90,-1,0,0);
		glPopMatrix();	

	}
	glPopMatrix();
}

void DL::drawparliament()
{
	glPushMatrix();
	float x=MAP.LAND.TERRAIN[MAP.LAND.getheight()/2][MAP.LAND.getwidth()/2].x;
	float y=MAP.LAND.TERRAIN[MAP.LAND.getheight()/2][MAP.LAND.getwidth()/2].y;
	float z=MAP.LAND.TERRAIN[MAP.LAND.getheight()/2][MAP.LAND.getwidth()/2].z;
	glTranslatef(x,y,z);
	glColor3f(0,1,.5);
	glutSolidCube(200);
	glPopMatrix();
}


void DL::drawmain()
{
	glColor3f(1,1,1);
	glColorMaterial(GL_FRONT_AND_BACK,GL_SHININESS);
	glEnable(GL_LIGHT0);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glDisable(GL_CULL_FACE);
	drawplayer();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glEnable(GL_CULL_FACE);

	drawstations();
	drawparliament();
	drawland();
	drawsea();

	glDisable(GL_LIGHT0);
}


//display timer, update fps and draw screen
void displaytimer(int value)
{value;
GLOBAL.FPS.frame++;
GLOBAL.FPS.time= glutGet(GLUT_ELAPSED_TIME);
float temp=GLOBAL.FPS.time-GLOBAL.FPS.timebase;
if (temp>1000)
{
	GLOBAL.FPS.fps=GLOBAL.FPS.frame*1000/(GLOBAL.FPS.time-GLOBAL.FPS.timebase);
	GLOBAL.FPS.timebase=GLOBAL.FPS.time;
	GLOBAL.FPS.frame=0;
	GLOBAL.FPS.elapsedtime+=1;
}

if (GLOBAL.dontrefresh==0)
{glutPostRedisplay();}
else if (GLOBAL.dontrefresh==1)
GLOBAL.dontrefresh=0;
glutTimerFunc(1000/GLOBAL.displaytick, displaytimer, 0);
}

//move timer, update player position and change sea height
void movetimer(int value)
{value;

update_func();
float val=.08*(float)MAP.stationsleft;
MAP.changesea(val);
glutTimerFunc(1000/GLOBAL.movetick, movetimer, 0);
}


void mouse(int button, int action,int x,int y)
{y;x;
printf("\n u clicked b=%d,a=%d",button,action);
//button:0=left 1=middle 2=right
//action: 0=press 1:release	
}

//light wrapper
void lights(bool active)
{
	GLfloat mat_diffuse[]={1,1,1,1};
	GLfloat mat_specular[]={1,1,1,1};
	GLfloat mat_ambient[]={.7,.5,.5};
	GLfloat mat_shininess[]={50};
	//GLfloat light_direction[]={-1,0,0};
	LIG *hax=MAP.LIGHTS.headptr;
	int count=0;
	while (hax!=NULL)
	{
		GLfloat mat_lightposition[]={hax->xpos,hax->ypos,hax->zpos,0};
		
		glClearColor(0.0,0.0,0.0,1);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
		glEnable ( GL_COLOR_MATERIAL ) ;	

		//starts at 0x4000, +1 for each light
		int nez=16384;
		nez+=count;

		char newz[10];
		sprintf(newz,"%x",nez);

		glLightfv(nez,GL_AMBIENT,mat_ambient);
		glLightfv(nez,GL_DIFFUSE,mat_diffuse);
		glLightfv(nez,GL_SPECULAR,mat_specular);
		glLightfv(nez,GL_SHININESS,mat_shininess);
		glLightfv(nez,GL_POSITION,mat_lightposition);

		hax=hax->nextptr;

	}
	if (active==true)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
}

//draw the terraid grid, lights first
void drawworld()
{
	lights(MAP.LIGHTS.isenabled);
	glEnable(GL_BLEND);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PERSPECTIVE_CORRECTION_HINT);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel (GL_SMOOTH);
	//glDepthFunc(GL_LEQUAL);
	glClear(GL_DEPTH_TEST);
	DISPLAYLISTS.drawmain();

}

//init power stations 
void initstations()
{
	int ms=MAP.LAND.getMAXSIZE();
	int randvalue=MAP.LAND.getMAXSIZE()/20;
	MAP.stations[0].x=ms/4+(rand()%randvalue);
	MAP.stations[0].z=ms/4+(rand()%randvalue);
	MAP.stations[0].y=MAP.LAND.getypos(MAP.stations[0].x,MAP.stations[0].z);

	MAP.stations[1].x=ms-(ms/4)-(rand()%randvalue);
	MAP.stations[1].z=ms/4+(rand()%randvalue);
	MAP.stations[1].y=MAP.LAND.getypos(MAP.stations[1].x,MAP.stations[1].z);

	MAP.stations[2].x=ms/4+(rand()%randvalue);
	MAP.stations[2].z=ms-(ms/4)-(rand()%randvalue);
	MAP.stations[2].y=MAP.LAND.getypos(MAP.stations[2].x,MAP.stations[2].z);


	MAP.stations[3].x=ms-(ms/4)-(rand()%randvalue);
	MAP.stations[3].z=ms-(ms/4)-(rand()%randvalue);
	MAP.stations[3].y=MAP.LAND.getypos(MAP.stations[3].x,MAP.stations[3].z);

	for (int a=0;a<MAP.numofstations;a++)
		MAP.stationdone[a]=false;
	MAP.stationsleft=MAP.numofstations;
	MAP.nearstation=false;
}

//init the game, camera, sea, time, stations
void initgame()
{
	srand(time(NULL));
	PLAYER.position.x=2000+rand()%(MAP.LAND.getMAXSIZE()-4000);
	PLAYER.position.y=0;
	PLAYER.position.z=2000+rand()%(MAP.LAND.getMAXSIZE()-4000);
	PLAYER.velocity=0;
	PLAYER.angle=0;
	MAP.initsea();
	GLOBAL.FPS.elapsedtime=0;
	MAP.LAND.createterrain(MAP.LAND.getLEVEL(),MAP.LAND.getMAXSIZE());
	initstations();
	PLAYER.points=0;
}


void decommission()
{
	if (MAP.stationdone[MAP.nearstationnum]==true)
		return;
	MAP.stationdone[MAP.nearstationnum]=true;
	PLAYER.points+=1;
	PLAYER.score+=1;
	MAP.stationsleft-=1;
	char temp[50];
	strcpy(temp,"Power Plant ");
	char temp2[20];
	sprintf(temp2,"%d",MAP.nearstationnum+1);
	strcat(temp,temp2);
	strcat(temp," Decommissioned");
	CONSOLE.pushtext(temp);
	MAP.nearstation=false;

	if (MAP.stationsleft==0)
	{
		CONSOLE.pushtext("Well done, you have won, restarting");
		initgame();
	}
}


void display()
{
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT); 
	glClearColor(0,0,0,1);
	glClear (GL_COLOR_BUFFER_BIT);	

	glEnable(GL_CULL_FACE);

	//glEnable(GL_NORMALIZE);

	int w=GLOBAL.SCRwidth;
	int h=GLOBAL.SCRheight;
	//make sure that lights are done after camera positioning, fixed point light not relative to camera

	for (int a=0;a<4;a++)
	{		
		glLoadIdentity();

		if (a==0)
		{
			//parliament cam
			glViewport (w/2, h/2, w/2,h/2);	
			glMatrixMode(GL_MODELVIEW);
			glClearColor(0.0,0.0,0.0,0.0f);
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity( );
			glMatrixMode( GL_MODELVIEW );
			lookat(GLOBAL.zoom,100,MAP.LAND.getMAXSIZE()/2,1500,MAP.LAND.getMAXSIZE()/2,PLAYER.position.x,PLAYER.position.y,PLAYER.position.z);
			drawworld();
		}
		else if (a==1)
		{
			//third person
			glViewport (0, 0, GLOBAL.SCRwidth/2,GLOBAL.SCRheight/2);

			glMatrixMode(GL_MODELVIEW);
			glClearColor(0.0,0.0,0.0,0.0f);
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity( );
			glMatrixMode( GL_MODELVIEW );
			float vx=PLAYER.position.x;
			float vy=PLAYER.position.y;
			float vz=PLAYER.position.z;
			GLOBAL.rotatefunct(&vx,&vy,&vz,PLAYER.angle,1,-800);

			lookat(70,100,vx,vy,vz,PLAYER.position.x,PLAYER.position.y,PLAYER.position.z);

			drawworld();
		}

		else if (a==2)
		{
			//top down
			float x=MAP.LAND.TERRAIN[MAP.LAND.getheight()/2][MAP.LAND.getwidth()/2].x;
			float y=MAP.LAND.TERRAIN[MAP.LAND.getheight()/2][MAP.LAND.getwidth()/2].y;
			float z=MAP.LAND.TERRAIN[MAP.LAND.getheight()/2][MAP.LAND.getwidth()/2].z;

			glViewport (0,h/2, w/2,h/2);
			glMatrixMode(GL_MODELVIEW);
			glClearColor(0.0,0.0,0.0,0.0f);
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity( );
			glMatrixMode( GL_MODELVIEW );

			lookat(80,1000,x-1,y+5000,z,x,y,z);
			drawworld();
		}

		else if (a==3)
		{
			glDisable(GL_LIGHTING);
			//text view
			glViewport(w/2,0,w/2,h/2);
			glMatrixMode (GL_PROJECTION);
			glLoadIdentity ();gluOrtho2D(0,w/2,0,h/2);
			glClear (GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			glTranslatef(10,280,0);
			glScalef(.11,.11,.11);
			glColor3f(0,1,0);
			glutStrokeString("CSE3313 Assignment 2 - Andrei Gec 19869908");


			glLoadIdentity ();gluOrtho2D(0,w/2,0,h/2);

			glTranslatef(10,250,0);
			glScalef(.11,.11,.11);
			glColor3f(1,1,0);

			glutStrokeString("Time Left=");glutStrokeInteger(60-GLOBAL.FPS.elapsedtime);

			glutStrokeString("  Points=");glutStrokeInteger(PLAYER.points);

			glutStrokeString("  Overall Score=");glutStrokeInteger(PLAYER.score);

			glLoadIdentity ();gluOrtho2D(0,w/2,0,h/2);

			glTranslatef(10,230,0);
			glScalef(.11,.11,.11);
			glColor3f(1,1,0);

			glutStrokeString("Stations Decommissioned=");glutStrokeInteger(MAP.numofstations-MAP.stationsleft);
			glutStrokeString("/");glutStrokeInteger(MAP.numofstations);

			if (MAP.nearstation==true&&MAP.stationdone[MAP.nearstationnum]==false)
			{

				glLoadIdentity ();gluOrtho2D(0,w/2,0,h/2);

				glTranslatef(10,210,0);
				glScalef(.11,.11,.11);
				glColor3f(1,1,1);

				if (PLAYER.velocity!=0)
					glutStrokeString("You must stop completely to decommission!");
				else
					glutStrokeString("Press 'd' To decommission");

			}
			glLoadIdentity ();gluOrtho2D(0,w/2,0,h/2);

			glTranslatef(10,170,0);
			glScalef(.11,.11,.11);
			glColor3f(1,1,0);

			glutStrokeString("Player.x=");glutStrokeInteger(PLAYER.position.x);

			glutStrokeString(" Player.y=");glutStrokeInteger(PLAYER.position.y);

			glutStrokeString(" Player.z=");glutStrokeInteger(PLAYER.position.z);

			glLoadIdentity ();gluOrtho2D(0,w/2,0,h/2);

			glTranslatef(10,150,0);
			glScalef(.11,.11,.11);
			glColor3f(1,1,0);

			glutStrokeString("Player.velocity=");glutStrokeInteger(PLAYER.velocity);
			glutStrokeString("/");glutStrokeInteger(PLAYER.maxvelocity);
			glutStrokeString(" Player.fovy=");glutStrokeInteger(GLOBAL.zoom);
			glutStrokeString(" Map Detail=");glutStrokeInteger(MAP.LAND.getLEVEL());

			glLoadIdentity ();gluOrtho2D(0,w/2,0,h/2);

			glTranslatef(10,110,0);
			glPushMatrix();
			glScalef(.2,.2,.2);
			glColor3f(1,1,0);
			glutStrokeString("Messages:      FPS: ");glutStrokeInteger(GLOBAL.FPS.fps);
			glPopMatrix();
			glTranslatef(0,-25,0);

			CONSOLE.drawtext();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
		}
	}
	glutSwapBuffers();
}


void initvar()
{
	initgame();
}



void changesize(int w, int h )
{
	glutReshapeWindow(GLOBAL.SCRwidth,GLOBAL.SCRheight);
	glViewport( 0, 0, w, h );
	glutPostRedisplay ();
}

void special(int key,int x,int y)	
{
	y;x;key;
	if ((key==GLUT_KEY_LEFT||key==GLUT_KEY_RIGHT)&&PLAYER.velocity==0)
		CONSOLE.pushtext("You must be moving to turn");

	PLAYER.key_state[key] = true;
	PLAYER.keydown++;
	glutPostRedisplay ();
}


void normal(unsigned char key, int x, int y)
{x;y;
if (key!=13)
{
	PLAYER.key_state[key] = true;
	PLAYER.keydown++;
}

switch(key)
{
case 'q':	{exit(1);break;}
case 13:	
	{
		int mod=glutGetModifiers();
		if (mod == (GLUT_ACTIVE_ALT))
		{
			if ((GLOBAL.fullscreen)==0)
			{printf("*Fullscreen Enabled!*");GLOBAL.fullscreen=1;}
			else
			{printf("*Fullscreen Disabled!*");GLOBAL.fullscreen=0;}
			initwindow();
			return;
		}
	}

case 'd':	
	{
		if (MAP.nearstation==true&&PLAYER.velocity==0)
			decommission();
		else if (PLAYER.velocity!=0&&MAP.nearstation==true)
			CONSOLE.pushtext("You must be stationary to decommission");
		else if (MAP.nearstation==false)
			CONSOLE.pushtext("Unable to decommission here");
		break;
	}

case '1':	
	{
		char temp[20];
		sprintf(temp,"%f",MAP.SEA[0].y);
		CONSOLE.pushtext(temp);
		return;

	}

case '2':	
	{
		printf("\nfps=%d",GLOBAL.FPS.fps);
		glutPostRedisplay();
		return;
	}

case '=':
	{
		MAP.LAND.createterrain(MAP.LAND.getLEVEL()+1,MAP.LAND.getMAXSIZE());
		glutPostRedisplay();
		return;
	}

case '-':
	{
		MAP.LAND.createterrain(MAP.LAND.getLEVEL()-1,MAP.LAND.getMAXSIZE());
		glutPostRedisplay();
		return;
	}

case '7':
	{
		MAP.LIGHTS.isenabled=!MAP.LIGHTS.isenabled;
		break;
	}

case '8':
	{
		MAP.LAND.shownormals=!MAP.LAND.shownormals;
		DISPLAYLISTS.init();
		break;
	}

case '9':
	{
		MAP.LAND.iswireframe=!MAP.LAND.iswireframe;
		DISPLAYLISTS.init();
		break;
	}

case '0':
	{
		initstations();
		//MAP.LAND.createterrain(MAP.LAND.getLEVEL(),MAP.LAND.getMAXSIZE());
		break;
	}

case '4':
	{
		glutPostRedisplay();
		return;
	}

case 'R':
	{
		initgame();
		break;
	}
};
}

void key_up_func(unsigned char key, int x, int y)
{x;y;
PLAYER.key_state[key]=false;
PLAYER.keydown--;
}

void spec_up_func(int key,int x, int y)
{x;y;
PLAYER.key_state[key]=false;
PLAYER.keydown--;
}

bool checkPositionVsStations()
{
	float px=PLAYER.position.x;
	float pz=PLAYER.position.z;
	float one;
	float two;

	one=MAP.stations[0].x;
	two=MAP.stations[0].z;

	int distaway=400;

	if ((px>=(one-distaway))&&(px<=(one+distaway))&&\
		(pz>=(two-distaway))&&(pz<=(two+distaway)))
	{
		MAP.nearstationnum=0;
		return true;
	}

	one=MAP.stations[1].x;
	two=MAP.stations[1].z;

	if ((px>=(one-distaway))&&(px<=(one+distaway))&&\
		(pz>=(two-distaway))&&(pz<=(two+distaway)))
	{
		MAP.nearstationnum=1;
		return true;
	}

	one=MAP.stations[2].x;
	two=MAP.stations[2].z;

	if ((px>=(one-distaway))&&(px<=(one+distaway))&&\
		(pz>=(two-distaway))&&(pz<=(two+distaway)))
	{
		MAP.nearstationnum=2;
		return true;
	}

	one=MAP.stations[3].x;
	two=MAP.stations[3].z;

	if ((px>=(one-distaway))&&(px<=(one+distaway))&&\
		(pz>=(two-distaway))&&(pz<=(two+distaway)))
	{
		MAP.nearstationnum=3;
		return true;
	}
	return false;
}


void update_func()
{
	if ((60-GLOBAL.FPS.elapsedtime)<=0)
	{
		CONSOLE.pushtext("Time over, you have lost. restarting");
		initgame();
	}
	if ((PLAYER.position.y+(PLAYER.zsize/2))<=MAP.SEA[0].y)
	{
		CONSOLE.pushtext("Your car has sunk; You have lost. restarting");
		initgame();
	}

	bool *key_state=PLAYER.key_state;
	if (PLAYER.keydown==1)
	{
		//printf(".");
		if (key_state[GLUT_KEY_LEFT]==true&&PLAYER.velocity!=0)
		{
			if (PLAYER.velocity>0)		
				PLAYER.addangle(5);
			else
				PLAYER.addangle(-5);
		}
		else if (key_state[GLUT_KEY_RIGHT]==true&&PLAYER.velocity!=0)
		{
			if (PLAYER.velocity>0)
				PLAYER.addangle(-5);
			else
				PLAYER.addangle(5);
		}
		else if (key_state[GLUT_KEY_UP]==true||key_state['a']==true)
			PLAYER.addvel(1);
		else if (key_state[GLUT_KEY_DOWN]==true||key_state['s']==true)
			PLAYER.addvel(-1);
		else if (key_state['z']==true)
			GLOBAL.changezoom(-.5);
		else if (key_state['Z']==true)
			GLOBAL.changezoom(.5);

	}
	if (PLAYER.velocity!=0)
		GLOBAL.rotatefunct(&PLAYER.position.x,NULL,&PLAYER.position.z,PLAYER.angle,0,PLAYER.velocity);
	PLAYER.position.y=MAP.LAND.getypos(PLAYER.position.x+(PLAYER.xsize/2),PLAYER.position.z+(PLAYER.xsize/2));
	//printf("\nx=%f,y=%f,z=%f",PLAYER.position.x,PLAYER.position.y,PLAYER.position.z);

	if (MAP.LAND.TERRAIN!=NULL)
		MAP.nearstation=checkPositionVsStations();

}

void initwindow()
{
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(GLOBAL.SCRwidth,GLOBAL.SCRheight);

	if (GLOBAL.fullscreen==1)
	{
		glutDestroyWindow(1);

		glutGameModeString("800x600:16@60");

		glutEnterGameMode();
	}
	else
	{

		if (GLOBAL.firsttime==0)
			glutLeaveGameMode();
		glutInitWindowPosition(10,10); 
		glutCreateWindow("CSE3313 Assignment 2 - Howard's World - Andrei Gec"); 
		glutReshapeWindow(GLOBAL.SCRwidth,GLOBAL.SCRheight);

	}

	glClearColor(0.0,0.0,0.0,0.0f);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glMatrixMode( GL_MODELVIEW );

	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutKeyboardFunc(normal);
	glutKeyboardUpFunc(key_up_func);
	glutSpecialUpFunc(spec_up_func);
	glutMouseFunc(mouse);
	//glutMotionFunc(mousemove);
	glutReshapeFunc(changesize);
	//glutSetCursor( GLUT_CURSOR_NONE);
	glutIgnoreKeyRepeat(true);
	DISPLAYLISTS.init();


	if (GLOBAL.firsttime==1)
	{
		displaytimer(1);movetimer(1);	
		GLOBAL.firsttime=0;}


	glutPostRedisplay();
}

int main(int argc,char* argv[])
{
	if (argc==3)
	{
		if (strcmp(argv[1],"-d")==0)
		{
		int value=atoi(argv[2]);
		if (value<0||value>10)
			printf("\n<0 or >10, invalid");
		else
			MAP.LAND.setsize(value,10000);
		}
			

		printf("\na=%s",argv[1]);
	}

	glutInit(&argc,argv); 
	//glDisable(GL_CULL_FACE);
	GLOBAL.firsttime=1;
	MAP.LIGHTS.addlight(MAP.LAND.getMAXSIZE()/2,1000,MAP.LAND.getMAXSIZE()/2);

	initwindow();
	initvar();
	glutMainLoop();
}
