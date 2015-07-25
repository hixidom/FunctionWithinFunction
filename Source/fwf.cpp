//g++ -o fwf.exe fwf.cpp glut32.lib -lopengl32 -lglu32 -static-libgcc -static-libstdc++

#include "GUItools01.h"
#include<time.h>
#include<fstream>
#include<new>
using namespace std;

#define TWOPI 6.28318530718

GLfloat ctemp[3];
GLint winw=1000,winh=600;
float rscale,tscale,res,itmax,tstart=0,tend,fov,rotx=0,roty=0,rotz=0,locx=0,locy=0,locz=0,xaxis[3],dp[3],tempaxis[3],linewidth,cdensity,zsdensity,stretch;
int dispnormals;
bool renderselect,panning=false,rotating=false;

pagetype gui1data;
pagetype *curpage=&gui1data;
dropdowntype ddmenu;
buttontype ShowNormals;

bool inrange(float val,float min,float max)
{
	if(val<max&&val>=min)return true;
	else return false;
}

void foov(float t,float *result)
{
	result[0]=cos(t);
	result[1]=sin(t);
	result[2]=stretch*t;
}
void dfoov(float t,float *result)
{
	result[0]=-sin(t);
	result[1]=cos(t);
	result[2]=stretch;
}

void ctorgb(GLfloat value)//converts value to RGB color
{
	value=3.0*fmod((float)value,(float)1.0);
	
	while(value>=3.0)
		value-=3.0;
	while(value<0)
		value+=3.0;		
		
	if(value<=0.5||value>2.5)ctemp[0]=1;
	else if(inrange(value,.5,1))ctemp[0]=2*(1.0-value);
	else if(inrange(value,2,2.5))ctemp[0]=2*(value-2);
	else ctemp[0]=0;
	
	if(inrange(value,.5,1.5))ctemp[1]=1;
	else if(inrange(value,0,.5))ctemp[1]=2.0*value;
	else if(inrange(value,1.5,2))ctemp[1]=2.0*(2.0-value);
	else ctemp[1]=0;

	if(inrange(value,1.5,2.5))ctemp[2]=1;
	else if(inrange(value,2.5,3))ctemp[2]=2.0*(3.0-value);
	else if(inrange(value,1,1.5))ctemp[2]=2.0*(value-1.0);
	else ctemp[2]=0;
}
void zshade(GLfloat z)
{
    int i;
	for(i=0;i<3;i++)ctemp[i]*=exp(-.2*z*zsdensity/stretch);
}

void normalize(float *vect)
{
	float mag=sqrt(pow(vect[0],2)+pow(vect[1],2)+pow(vect[2],2));
	if(mag!=0)
	{
		vect[0]=vect[0]/mag;
		vect[1]=vect[1]/mag;
		vect[2]=vect[2]/mag;
	}
}

void getnormals(float a,float b,float c,float x,float y,float z,float r,float s,float t,float *normal,float *xaxis)
{
	normal[0]=r-a;
	normal[1]=s-b;
	normal[2]=t-c;
	normalize(normal);
	
	xaxis[0]=(r+a)/2.0;
	xaxis[1]=(s+b)/2.0;
	xaxis[2]=(t+c)/2.0;
	
	xaxis[0]-=x;
	xaxis[1]-=y;
	xaxis[2]-=z;
	normalize(xaxis);
}

float dotprod(float *vector1,float *vector2)
{
	return (vector1[0]*vector2[0])+(vector1[1]*vector2[1])+(vector1[2]*vector2[2]);
}
void crossprod(float *vector1,float *vector2,float *product)
{
	product[0]=(vector1[1]*vector2[2])-(vector1[2]*vector2[1]);
	product[1]=(vector1[2]*vector2[0])-(vector1[0]*vector2[2]);
	product[2]=(vector1[0]*vector2[1])-(vector1[1]*vector2[0]);
}
void vectadd(float *vector1,float *vector2,float *result)
{
	result[0]=vector1[0]+vector2[0];
	result[1]=vector1[1]+vector2[1];
	result[2]=vector1[2]+vector2[2];
}
void vectmult(float *vector,float factor,float *result)
{
	result[0]=vector[0]*factor;
	result[1]=vector[1]*factor;
	result[2]=vector[2]*factor;
}
void recursionnormals(float t,float r,float *p,float *yaxis,float *zaxis,int it)
{
	crossprod(yaxis,zaxis,xaxis);
	if(it<itmax)
	{
		dfoov(t,dp);//zaxis[0]=dp[0]*xaxis[0]+dp[1]*yaxis[0]+dp[2]*zaxis[0]; zaxis[1]=dp[0]*xaxis[1]+dp[1]*yaxis[1]+dp[2]*zaxis[1]; zaxis[2]=dp[0]*xaxis[2]+dp[1]*yaxis[2]+dp[2]*zaxis[2]; normalize(zaxis);
		foov(t,dp);
		p[0]+=(r*dp[0]*xaxis[0])+(r*dp[1]*yaxis[0]);
		p[1]+=(r*dp[0]*xaxis[1])+(r*dp[1]*yaxis[1]);
		p[2]+=(r*dp[0]*xaxis[2])+(r*dp[1]*yaxis[2]);
		tempaxis[0]=dp[0]*xaxis[0]+dp[1]*yaxis[0]; tempaxis[1]=dp[0]*xaxis[1]+dp[1]*yaxis[1]; tempaxis[2]=dp[0]*xaxis[2]+dp[1]*yaxis[2]; normalize(tempaxis);
		dfoov(t,dp);
		zaxis[0]=dp[0]*xaxis[0]+dp[1]*yaxis[0]+dp[2]*zaxis[0]; zaxis[1]=dp[0]*xaxis[1]+dp[1]*yaxis[1]+dp[2]*zaxis[1]; zaxis[2]=dp[0]*xaxis[2]+dp[1]*yaxis[2]+dp[2]*zaxis[2]; normalize(zaxis);
		yaxis[0]=tempaxis[0]; yaxis[1]=tempaxis[1]; yaxis[2]=tempaxis[2];
		recursionnormals(t*tscale,r/rscale,p,yaxis,zaxis,it+1);
	}
	else
	{
		glBegin(GL_LINES);
			glColor3f(1,0,0);
			glVertex3fv(p);
			glVertex3f(p[0]+4.0*r*xaxis[0],p[1]+4.0*r*xaxis[1],p[2]+4.0*r*xaxis[2]);
		
			glColor3f(0,1,0);
			glVertex3fv(p);
			glVertex3f(p[0]+4.0*r*yaxis[0],p[1]+4.0*r*yaxis[1],p[2]+4.0*r*yaxis[2]);
		
			glColor3f(0,0,1);
			glVertex3fv(p);
			glVertex3f(p[0]+4.0*r*zaxis[0],p[1]+4.0*r*zaxis[1],p[2]+4.0*r*zaxis[2]);
		glEnd();
	}
}
void recursion(float t,float r,float *p,float *yaxis,float *zaxis,int it)
{
	crossprod(yaxis,zaxis,xaxis);
	foov(t,dp);
	p[0]+=(r*dp[0]*xaxis[0])+(r*dp[1]*yaxis[0]);
	p[1]+=(r*dp[0]*xaxis[1])+(r*dp[1]*yaxis[1]);
	p[2]+=(r*dp[0]*xaxis[2])+(r*dp[1]*yaxis[2]);
	tempaxis[0]=dp[0]*xaxis[0]+dp[1]*yaxis[0]; tempaxis[1]=dp[0]*xaxis[1]+dp[1]*yaxis[1]; tempaxis[2]=dp[0]*xaxis[2]+dp[1]*yaxis[2]; normalize(tempaxis);
	dfoov(t,dp);
	zaxis[0]=dp[0]*xaxis[0]+dp[1]*yaxis[0]+dp[2]*zaxis[0]; zaxis[1]=dp[0]*xaxis[1]+dp[1]*yaxis[1]+dp[2]*zaxis[1]; zaxis[2]=dp[0]*xaxis[2]+dp[1]*yaxis[2]+dp[2]*zaxis[2]; normalize(zaxis);
	yaxis[0]=tempaxis[0]; yaxis[1]=tempaxis[1]; yaxis[2]=tempaxis[2];
	if(it<itmax)recursion(t*tscale,r/rscale,p,yaxis,zaxis,it+1);
}

void render(short num)
{
	switch(num)
	{
		case 1:
			float t;
			glNewList(1,GL_COMPILE);
				//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glViewport(250,0,winw-250,winh);
		
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluOrtho2D(-((float)(winw-250)/(float)winh)-locx,((float)(winw-250)/(float)winh)-locx,-1+locy,1+locy);
				gluPerspective(fov,(winw-250)/winh,.001,10000.0);
		
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
		
				glTranslatef(0,0,-20);
				if (rotx>359)rotx=0;
				if (roty>359)roty=0;
				if (rotz>359)rotz=0;
				glRotatef(roty/2.0,1.0,0.0,0.0);
				glRotatef(rotx/2.0,0.0,1.0,0.0);
				glRotatef(rotz/2.0,0.0,0.0,1.0);
				
				float tempaxis[3],xaxis[3],yaxis[3],zaxis[3],p[3];
				
				if(curpage->buttons[0].enabled)
				{
					for(t=tend;t>tstart;t-=1.0/pow(2.0,res))
					{
						yaxis[0]=0; yaxis[1]=1; yaxis[2]=0;//yaxis points from foo central axis to foo
						zaxis[0]=0; zaxis[1]=0; zaxis[2]=1;//zaxis is the central axis of foo
						foov(t,p); p[0]=0; p[1]=0;
						recursionnormals(t,1.0,p,yaxis,zaxis,0);
					}
				}
				glLineWidth(linewidth);
				glBegin(GL_LINE_STRIP);
					for(t=tend;t>tstart;t-=1.0/pow(2.0,res))
					{
						if(cdensity>0)ctorgb(t*cdensity/10.0);
						else
						{
							ctemp[0]=1;ctemp[1]=1;ctemp[2]=1;
						}
						yaxis[0]=0; yaxis[1]=1; yaxis[2]=0;//yaxis points from foo central axis to foo
						zaxis[0]=0; zaxis[1]=0; zaxis[2]=1;//zaxis is the central axis of foo
						foov(t,p); p[0]=0; p[1]=0;
						recursion(t,1.0,p,yaxis,zaxis,0);
						zshade(p[2]);
						glColor3fv(ctemp);
						glVertex3fv(p);
					}
				glEnd();
				glLineWidth(1);
				
				glPopMatrix();
			glEndList();
			break;
		case 2:
			glNewList(2,GL_COMPILE);
				glColor3f(.7,.7,.7);
				glBegin(GL_QUADS);
					glVertex2i(0,0);
					glVertex2i(250,0);
					glVertex2i(250,winh);
					glVertex2i(0,winh);
				glEnd();
				curpage->disp();
			glEndList();
			break;
	}
}
void dispitmax()
{
	//cout<<itmax<<"\n";
}
void init(void)
{
	glClearColor(0,0,0,1);
	glShadeModel(GL_SMOOTH);
	
	glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
	glClear(GL_COLOR_BUFFER_BIT);
	
	tstart=renderselect=panning=rotating=0;
	fov=10;
	dispnormals=0;
	
	gui1data.setcolor(.6,.6,.6);
	gui1data.newbut(72,204,221,241,"Show Normals","Toggles display of normals");
	gui1data.buttons.back().setfunc(empty);
	
	gui1data.newbox(127,44,20,70,20,.1,0,100,1,.2,"Axial stretch","",&stretch);
	gui1data.newbox(127,74,20,70,20,1,0.001000,1000.000000,1,5.00000,"Spatial scaling","",&rscale);
	gui1data.boxes.back().setfunc(empty);
	gui1data.newbox(127,101,20,70,20,1,0.001000,1000.000000,1,15.000000,"Time scaling","",&tscale);
	gui1data.boxes.back().setfunc(empty);
	gui1data.newbox(127,128,20,70,20,2,0.001000,1000.000000,1,10,"End time","",&tend);
	gui1data.boxes.back().setfunc(empty);
	gui1data.newbox(127,155,20,33,20,1.000000,0.000000,6.000000,0,1.000000,"Recursions","",&itmax);
	gui1data.boxes.back().setfunc(dispitmax);
	gui1data.newbox(127,182,20,33,20,1.000000,4.000000,16.000000,0,5.000000,"Resolution","",&res);
	gui1data.boxes.back().setfunc(empty);
	gui1data.newbox(127,255,20,33,20,1.000000,1,20,0,1,"Line Width","",&linewidth);
	gui1data.boxes.back().setfunc(empty);
	gui1data.newbox(127,282,20,70,20,.1,0,1000,1,1,"Color Density","",&cdensity);
	gui1data.boxes.back().setfunc(empty);
	gui1data.newbox(127,309,20,70,20,.05,0,100,1,.5,"Z Shade Dens.","",&zsdensity);
	gui1data.boxes.back().setfunc(empty);
	gui1data.setfunc(empty);
	
	render(1);
	render(2);
}
void resize (int w, int h)
{
	winw=w;
	winh=h;
	gluOrtho2D(0,winw,winh,0);
	glViewport(0,0,winw,winh);
	render(2);
}

void mouseaction(int button,int state,int x,int y)
{
	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
	{
		if(inrange(x,0,250)&&inrange(y,0,winh))
		{
			curpage->click(x,y);
			render(2);
			render(1);
		}
		//left mouse rotate
		else if(inrange(x,251,winw)&&inrange(y,0,winh)&&panning==0)
		{
			glMatrixMode(GL_PROJECTION);
			rotating=1;
			mouseloc[0]=x;
			mouseloc[1]=y;
		}
	}
	if(button==GLUT_LEFT_BUTTON&&state==GLUT_UP&&rotating==1)
		rotating=0;
	//right mouse pan
	else if(button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN&&inrange(x,251,winw)&&inrange(y,0,winh)&&rotating==0)
	{
		panning=1;
		mouseloc[0]=x;
		mouseloc[1]=y;
	}
	else if(button==GLUT_RIGHT_BUTTON && state==GLUT_UP&&panning==1)
		panning=0;
	//scroll wheel zoom
	else if(button==3&&state==GLUT_DOWN)
	{
		if(fov>4)
			fov-=2;
		render(1);
	}
	else if(button==4&&state==GLUT_DOWN)
	{
		if(fov<178)
			fov+=2;
		render(1);
	}
}

void mousemotion(int x,int y)
{
	if(panning==1)
	{
		locx+=2.0*(float)(x-mouseloc[0])/(float)(winw-250);
		locy+=2.0*(float)(y-mouseloc[1])/(float)(winh);
		render(1);
		mouseloc[0]=x;
		mouseloc[1]=y;
	}
	else if(rotating==1)
	{
		rotx+=x-mouseloc[0];
		roty+=y-mouseloc[1];
		render(1);
		mouseloc[0]=x;
		mouseloc[1]=y;
	}
	mouseloc[0]=x;
	mouseloc[1]=y;
}

void controls(unsigned char key,int x,int y)
{
	curpage->controls(key,x,y);
	render(2);
	render(1);
	
	switch(key){
        case '=':
    	{
    		if(fov>2.1)
    			fov-=2;
    		render(1);
    		break;
    	}
    	case '-':
    	{
    		if(fov<178)
    			fov+=2;
    		render(1);
    		break;
    	}
    }
}

void specialcontrols(int key,int x,int y)
{
	curpage->specialcontrols(key,x,y);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,winw,winh);
	gluOrtho2D(0,winw,winh,0);
	
	glCallList(2);
	glCallList(1);
	glutSwapBuffers();
}

int main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(winw,winh);
	glutInitWindowPosition(100,10);
	glutCreateWindow("fwf.cpp");
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(resize);
	glutMouseFunc(mouseaction);
	glutMotionFunc(mousemotion);
	glutPassiveMotionFunc(mousemotion);
	glutKeyboardFunc(controls);
	glutSpecialFunc(specialcontrols);
	init();
	glutMainLoop();
	return 1;
}
