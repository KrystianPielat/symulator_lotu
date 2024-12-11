#ifndef UTILITIES
#define UTILITIES

#include <iostream>
#include <cmath>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <freeimage/FreeImage.h>

using namespace std;



void quad(GLfloat v0[3], GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int M = 10, int N = 10);

void loadImageFile(char* name);


void quad(GLfloat v0[3], GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int M, int N)
{
	if(M<1) M=1; if(N<1) N=1;
	GLfloat ai[3], ci[3], bj[3], dj[3], p0[3], p1[3];

	GLfloat v01[] = { v1[0]-v0[0], v1[1]-v0[1], v1[2]-v0[2] };
	GLfloat v03[] = { v3[0]-v0[0], v3[1]-v0[1], v3[2]-v0[2] };
	GLfloat normal[] = { v01[1]*v03[2] - v01[2]*v03[1] ,
						 v01[2]*v03[0] - v01[0]*v03[2] ,
						 v01[0]*v03[1] - v01[1]*v03[0] };
	float n = sqrt( normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2] );
	glNormal3f( normal[0]/n, normal[1]/n, normal[2]/n );

	for(int i=0; i<M; i++){

		for(int k=0; k<3; k++){ 
			ai[k] = v0[k] + i*(v1[k]-v0[k])/M;
			ci[k] = v3[k] + i*(v2[k]-v3[k])/M;
		}

		glBegin(GL_QUAD_STRIP);
		for(int j=0; j<=N; j++){
			for(int k=0; k<3; k++){

				bj[k] = v1[k] + j*(v2[k]-v1[k])/N;
				dj[k] = v0[k] + j*(v3[k]-v0[k])/N;


				p0[k] = ai[k] + j*(ci[k]-ai[k])/N;

				p1[k] = p0[k] + (bj[k]-dj[k])/M;
			}

			glTexCoord2f(i*1.0f/M, j*1.0f/N);
			glVertex3f(p0[0],p0[1],p0[2]);

			glTexCoord2f((i+1)*1.0f/M, j*1.0f/N);
			glVertex3f(p1[0],p1[1],p1[2]);
		}
		glEnd();
	}
}


void loadImageFile(char* name)
{

	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(name,0);
	FIBITMAP* image = FreeImage_Load(format, name); 
	if(image==NULL) cerr << "Failed to load image" << name <<endl;
	FIBITMAP* image32b = FreeImage_ConvertTo32Bits(image);


	int w = FreeImage_GetWidth(image32b);
	int h = FreeImage_GetHeight(image32b);
	GLubyte* tex = FreeImage_GetBits(image32b);


	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, tex);


	FreeImage_Unload(image);
	FreeImage_Unload(image32b);
}

#endif