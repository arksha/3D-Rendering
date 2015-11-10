/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define Pi 3.1415926
short	ctoi(float color);
bool rastlee(GzRender* render, GzCoord* cord);
bool posVectorToScreenSpace(const GzMatrix mat, const GzCoord vector, GzCoord result);
int GzRotXMat(float degree, GzMatrix mat)
{
	// Create rotate matrix : rotate along x axis
	// Pass back the matrix using mat value
	if (mat == NULL)
	{
		return GZ_FAILURE;
	}
	float rad = degree * (Pi / 180.0);

	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;
	mat[1][0] = 0;
	mat[1][1] = cos(rad);
	mat[1][2] = -sin(rad);
	mat[1][3] = 0;
	mat[2][0] = 0;
	mat[2][1] = sin(rad);
	mat[2][2] = cos(rad);
	mat[2][3] = 0;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
	// Create rotate matrix : rotate along y axis
	// Pass back the matrix using mat value
	if (mat == NULL)
	{
		return GZ_FAILURE;
	}
	float rad = degree * (Pi / 180.0);

	mat[0][0] = cos(rad);
	mat[0][1] = 0;
	mat[0][2] = sin(rad);
	mat[0][3] = 0;
	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = 0;
	mat[2][0] = -sin(rad);
	mat[2][1] = 0;
	mat[2][2] = cos(rad);
	mat[2][3] = 0;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
	// Create rotate matrix : rotate along z axis
	// Pass back the matrix using mat value
	if (mat == NULL)
	{
		return GZ_FAILURE;
	}
	float rad = degree * (Pi / 180.0);

	mat[0][0] = cos(rad);
	mat[0][1] = -sin(rad);
	mat[0][2] = 0;
	mat[0][3] = 0;
	mat[1][0] = sin(rad);
	mat[1][1] = cos(rad);
	mat[1][2] = 0;
	mat[1][3] = 0;
	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = 0;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
	// Create translation matrix
	// Pass back the matrix using mat value
	if (mat == NULL)
	{
		return GZ_FAILURE;
	}
	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = translate[X];
	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = translate[Y];
	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = translate[Z];
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
	// Create scaling matrix
	// Pass back the matrix using mat value
	if (mat == NULL)
	{
		return GZ_FAILURE;
	}
	mat[0][0] = scale[X];
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;
	mat[1][0] = 0;
	mat[1][1] = scale[Y];
	mat[1][2] = 0;
	mat[1][3] = 0;
	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = scale[Z];
	mat[2][3] = 0;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzDisplay	*display)
{
	/*
	- malloc a renderer struct
	- setup Xsp and anything only done once
	- save the pointer to display
	- init default camera
	*/
	*render = new GzRender();
	if (*render == NULL)
		return GZ_FAILURE;
	if (display == NULL)
		return GZ_FAILURE;
	(*render)->display = display;
	(*render)->matlevel = -1;//set empty stack
	//set default camera
	(*render)->camera.position[X] = DEFAULT_IM_X;
	(*render)->camera.position[Y] = DEFAULT_IM_Y;
	(*render)->camera.position[Z] = DEFAULT_IM_Z;
	(*render)->camera.FOV = DEFAULT_FOV;
	(*render)->camera.lookat[X] = 0.0;
	(*render)->camera.lookat[Y] = 0.0;
	(*render)->camera.lookat[Z] = 0.0;
	(*render)->camera.worldup[X] = 0.0;
	(*render)->camera.worldup[Y] = 1.0;
	(*render)->camera.worldup[Z] = 0.0;
	//Xsp
	(*render)->Xsp[0][0] = (*render)->display->xres / 2;
	(*render)->Xsp[0][1] = 0.0;
	(*render)->Xsp[0][2] = 0.0;
	(*render)->Xsp[0][3] = (*render)->display->xres /2;

	(*render)->Xsp[1][0] = 0.0;
	(*render)->Xsp[1][1] = -((*render)->display->yres /2);
	(*render)->Xsp[1][2] = 0.0;
	(*render)->Xsp[1][3] = (*render)->display->yres / 2;

	(*render)->Xsp[2][0] = 0.0;
	(*render)->Xsp[2][1] = 0.0;
	(*render)->Xsp[2][2] = MAXINT;
	(*render)->Xsp[2][3] = 0.0;

	(*render)->Xsp[3][0] = 0.0;
	(*render)->Xsp[3][1] = 0.0;
	(*render)->Xsp[3][2] = 0.0;
	(*render)->Xsp[3][3] = 1.0;

	
	return GZ_SUCCESS;

}


int GzFreeRender(GzRender *render)
{
	/*
	-free all renderer resources
	*/
	if (render == NULL)
		return GZ_FAILURE;
	render->display = NULL;
	free(render);
	render = NULL;
	delete render;
	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
	/*
	- setup for start of each frame - init frame buffer color,alpha,z
	- compute Xiw and projection xform Xpi from camera definition
	- init Ximage - put Xsp at base of stack, push on Xpi and Xiw
	- now stack contains Xsw and app can push model Xforms when needed
	*/
	if (render == NULL)
		return GZ_FAILURE;
	GzInitDisplay(render->display);
	if (render->display == NULL)
		return GZ_FAILURE;
	//compute Xiw 
	GzCoord cl, camZaxis;
	cl[X] = render->camera.lookat[X] - render->camera.position[X];
	cl[Y] = render->camera.lookat[Y] - render->camera.position[Y];
	cl[Z] = render->camera.lookat[Z] - render->camera.position[Z];

	float normcl;
	normcl = sqrt(cl[X] * cl[X] + cl[Y] * cl[Y] + cl[Z] * cl[Z]);
	camZaxis[X] = cl[X] / normcl;
	camZaxis[Y] = cl[Y] / normcl;
	camZaxis[Z] = cl[Z] / normcl;

	GzCoord  camUp, camYaxis;
	float zdotup = ((render->camera.worldup[X]) * (camZaxis[X])
		+ (render->camera.worldup[Y]) * (camZaxis[Y])
		+ (render->camera.worldup[Z]) * (camZaxis[Z]));
	camUp[X] = render->camera.worldup[X] - zdotup * camZaxis[X];
	camUp[Y] = render->camera.worldup[Y] - zdotup * camZaxis[Y];
	camUp[Z] = render->camera.worldup[Z] - zdotup * camZaxis[Z];

	float normup;
	normup = sqrt(camUp[X] * camUp[X] + camUp[Y] * camUp[Y] + camUp[Z] * camUp[Z]);
	camYaxis[X] = camUp[X] / normup;
	camYaxis[Y] = camUp[Y] / normup;
	camYaxis[Z] = camUp[Z] / normup;

	GzCoord camXaxis;

	camXaxis[X] = camYaxis[Y] * camZaxis[Z] - camYaxis[Z] * camZaxis[Y];
	camXaxis[Y] = camYaxis[Z] * camZaxis[X] - camYaxis[X] * camZaxis[Z];
	camXaxis[Z] = camYaxis[X] * camZaxis[Y] - camYaxis[Y] * camZaxis[X];

	float xc, yc, zc;
	xc = (camXaxis[X] * render->camera.position[X]) +
		(camXaxis[Y] * render->camera.position[Y]) + (camXaxis[Z] * render->camera.position[Z]);
	yc = (camYaxis[X] * render->camera.position[X]) +
		(camYaxis[Y] * render->camera.position[Y]) + (camYaxis[Z] * render->camera.position[Z]);
	zc = (camZaxis[X] * render->camera.position[X]) +
		(camZaxis[Y] * render->camera.position[Y]) + (camZaxis[Z] * render->camera.position[Z]);

	render->camera.Xiw[0][0] = camXaxis[X];
	render->camera.Xiw[0][1] = camXaxis[Y];
	render->camera.Xiw[0][2] = camXaxis[Z];
	render->camera.Xiw[0][3] = -(xc);
	render->camera.Xiw[1][0] = camYaxis[X];
	render->camera.Xiw[1][1] = camYaxis[Y];
	render->camera.Xiw[1][2] = camYaxis[Z];
	render->camera.Xiw[1][3] = -(yc);
	render->camera.Xiw[2][0] = camZaxis[X];
	render->camera.Xiw[2][1] = camZaxis[Y];
	render->camera.Xiw[2][2] = camZaxis[Z];
	render->camera.Xiw[2][3] = -(zc);
	render->camera.Xiw[3][0] = 0;
	render->camera.Xiw[3][1] = 0;
	render->camera.Xiw[3][2] = 0;
	render->camera.Xiw[3][3] = 1;

	//Xpi
	float d;
	d = 1 / (tan((render->camera.FOV / (float)2.0) * (Pi / (float)180.0)));
	render->camera.Xpi[0][0] = 1;
	render->camera.Xpi[0][1] = 0;
	render->camera.Xpi[0][2] = 0;
	render->camera.Xpi[0][3] = 0;

	render->camera.Xpi[1][0] = 0;
	render->camera.Xpi[1][1] = 1;
	render->camera.Xpi[1][2] = 0;
	render->camera.Xpi[1][3] = 0;

	render->camera.Xpi[2][0] = 0;
	render->camera.Xpi[2][1] = 0;
	render->camera.Xpi[2][2] = 1/d;
	render->camera.Xpi[2][3] = 0;

	render->camera.Xpi[3][0] = 0;
	render->camera.Xpi[3][1] = 0;
	render->camera.Xpi[3][2] = 1/d;
	render->camera.Xpi[3][3] = 1;

	//init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
	GzPushMatrix(render, render->Xsp);
	GzPushMatrix(render, render->camera.Xpi);
	GzPushMatrix(render, render->camera.Xiw);

	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
	/*
	- overwrite renderer camera structure with new camera definition
	*/
	if (render == NULL)
	{
		return GZ_FAILURE;
	}
	if (camera == NULL)
	{
		return GZ_FAILURE; 
	}
	memcpy(&(render->camera), camera, sizeof(GzCamera));

	return GZ_SUCCESS;
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
	/*
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	if (render == NULL)
	{
		return GZ_FAILURE;
	}
	//check overflow
	if (render->matlevel == MATLEVELS) {
		return GZ_FAILURE;
	}
	GzMatrix prodmatrx;
	if (render->matlevel == -1)
	{
		memcpy(render->Ximage[render->matlevel + 1], matrix, sizeof(GzMatrix));
	}
	else{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				prodmatrx[i][j] = 0;

			}
		}
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					prodmatrx[i][j] += render->Ximage[render->matlevel][i][k] * matrix[k][j];
				}

			}
		}
		memcpy(render->Ximage[render->matlevel + 1], prodmatrx, sizeof(GzMatrix));
	}

	render->matlevel++;

	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
	/*
	- pop a matrix off the Ximage stack
	- check for stack underflow
	*/
	if (render == NULL){
		return GZ_FAILURE;
	}
	if (render->matlevel < 0){
		return GZ_FAILURE;
	}
	render->matlevel--;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList,
	GzPointer	*valueList) /* void** valuelist */
{
	/*
	- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
	- later set shaders, interpolaters, texture maps, and lights
	*/

	if (!render || !nameList || !valueList)
		return GZ_FAILURE;
	for (int i = 0; i < numAttributes; i++){
		if (nameList[i] == GZ_RGB_COLOR){
			GzColor* color = (GzColor *)valueList[i];
			if ((*color)[RED] < 0){
				(*color)[RED] = 0;
			}
			if ((*color)[GREEN] < 0){
				(*color)[GREEN] = 0;
			}
			if ((*color)[BLUE] < 0){
				(*color)[BLUE] = 0;
			}
			if ((*color)[RED] < 0){
				(*color)[RED] = 0;
			}
			if ((*color)[RED] >4095){
				(*color)[RED] = 4095;
			}
			if ((*color)[GREEN] >4095){
				(*color)[GREEN] = 4095;
			}
			if ((*color)[BLUE] >4095){
				(*color)[BLUE] = 4095;
			}
			render->flatcolor[RED] = color[i][RED];
			render->flatcolor[GREEN] = color[i][GREEN];
			render->flatcolor[BLUE] = color[i][BLUE];
		}
	}
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, GzPointer	*valueList)
/* numParts : how many names and values */
{
	/*
	- pass in a triangle description with tokens and values corresponding to
	GZ_POSITION:3 vert positions in model space
	- Xform positions of verts using matrix on top of stack
	- Clip - just discard any triangle with any vert(s) behind view plane
	- optional: test for triangles with all three verts off-screen (trivial frustum cull)
	- invoke triangle rasterizer
	*/
	//check error
	if (render == NULL){
		return GZ_FAILURE;
	}
	if (nameList == NULL){
		return GZ_FAILURE;
	}
	if (valueList == NULL){
		return GZ_FAILURE;
	}

	for (int i = 0; i < numParts; i++){
		if (nameList[i] == GZ_NULL_TOKEN){
			continue;
		}
		if (nameList[i] == GZ_POSITION){
			GzCoord* cord = (GzCoord*)valueList[i];
			GzCoord* xformcord = new GzCoord[3];
			bool behind = false;//behind Visual point

			//get triangle cord

			// Xform positions of verts using matrix on top of stack
			GzMatrix tosmatrx;
			memcpy(tosmatrx, render->Ximage[render->matlevel], sizeof(GzMatrix));
			float W;
			for (int j = 0; j < 3; j++) {
				// xform verticies

				xformcord[j][X] = tosmatrx[0][0] * cord[j][X] + tosmatrx[0][1] * cord[j][Y] +
					tosmatrx[0][2] * cord[j][Z] + tosmatrx[0][3] * (float)1.0;
				xformcord[j][Y] = tosmatrx[1][0] * cord[j][X] + tosmatrx[1][1] * cord[j][Y] +
					tosmatrx[1][2] * cord[j][Z] + tosmatrx[1][3] * (float)1.0;
				xformcord[j][Z] = tosmatrx[2][0] * cord[j][X] + tosmatrx[2][1] * cord[j][Y] +
					tosmatrx[2][2] * cord[j][Z] + tosmatrx[2][3] * (float)1.0;

				W = tosmatrx[3][0] * cord[j][X] + tosmatrx[3][1] * cord[j][Y] + tosmatrx[3][2] * cord[j][Z] + tosmatrx[3][3] * (float)1.0;
				

				//Clip - just discard any triangle with any vert(s) behind view plane
				if (xformcord[j][Z] < render->camera.position[Z]) {
					behind = true;
					break;
				}
				xformcord[j][X] /= W;
				xformcord[j][Y] /= W;
				xformcord[j][Z] /= W;
			}
			if (behind == true) {
				break;
			}
			if (rastlee(render, xformcord)){
				return true;
			}
		}



	}
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
	return(short)((int)(color * ((1 << 12) - 1)));
}

bool rastlee(GzRender* render, GzCoord* cord){

	//swap and sort Y of vertex 

	//sort Y by ascending order

	for (int j = 0; j < 2; j++){
		for (int k = 0; k < 2 - j; k++){
			if (cord[k][Y]>cord[k + 1][Y]){
				float tempx = cord[k][X];
				float tempy = cord[k][Y];
				float tempz = cord[k][Z];
				cord[k][X] = cord[k + 1][X];
				cord[k][Y] = cord[k + 1][Y];
				cord[k][Z] = cord[k + 1][Z];
				cord[k + 1][X] = tempx;
				cord[k + 1][Y] = tempy;
				cord[k + 1][Z] = tempz;
			}
		}
	}


	// determine L/R CW
	//horizontal case have two top Y
	if (cord[0][Y] == cord[1][Y]){
		if (cord[0][X] > cord[1][X]){
			float tempx = cord[0][X];
			float tempy = cord[0][Y];
			float tempz = cord[0][Z];
			cord[0][X] = cord[1][X];
			cord[0][Y] = cord[1][Y];
			cord[0][Z] = cord[1][Z];
			cord[1][X] = tempx;
			cord[1][Y] = tempy;
			cord[1][Z] = tempz;
		}

	}
	//horizontal case have two bot Y
	else if (cord[1][Y] == cord[2][Y]){
		if (cord[2][X] > cord[1][X]){
			float tempx = cord[2][X];
			float tempy = cord[2][Y];
			float tempz = cord[2][Z];
			cord[2][X] = cord[1][X];
			cord[2][Y] = cord[1][Y];
			cord[2][Z] = cord[1][Z];
			cord[1][X] = tempx;
			cord[1][Y] = tempy;
			cord[1][Z] = tempz;
		}
	}

	//bound box
	int top = (int)floor(cord[0][Y]);
	int bot = (int)ceil(cord[2][Y]);
	int left = cord[0][X];
	int right = cord[2][X];

	for (int i = 0; i < 3; i++){
		if (cord[i][X] < left)
			left = (int)floor(cord[i][X]);
		if (cord[i][X]>right)
			right = (int)ceil(cord[i][X]);
	}

	//interpolate z
	float interpz;
	GzCoord vec01, vec12;
	vec01[X] = cord[1][X] - cord[0][X];
	vec01[Y] = cord[1][Y] - cord[0][Y];
	vec01[Z] = cord[1][Z] - cord[0][Z];

	vec12[X] = cord[2][X] - cord[1][X];
	vec12[Y] = cord[2][Y] - cord[1][Y];
	vec12[Z] = cord[2][Z] - cord[1][Z];

	float A = vec01[Y] * vec12[Z] - vec01[Z] * vec12[Y];
	float B = vec01[Z] * vec12[X] - vec01[X] * vec12[Z];
	float C = vec01[X] * vec12[Y] - vec01[Y] * vec12[X];
	float D = -(A*cord[0][X] + B*cord[0][Y] + C*cord[0][Z]);

	//if in bbox
	for (int i = left; i < right; i++){
		if (i<0 || i>render->display->xres){
			continue;
		}
		for (int j = top; j < bot; j++){
			if (j<0 || j>render->display->yres){
				continue;
			}

			float a01 = cord[1][Y] - cord[0][Y];
			float a12 = cord[2][Y] - cord[1][Y];
			float a20 = cord[0][Y] - cord[2][Y];
			float b01 = -(cord[1][X] - cord[0][X]);
			float b12 = -(cord[2][X] - cord[1][X]);
			float b20 = -(cord[0][X] - cord[2][X]);
			float c01 = -b01*cord[0][Y] - a01*cord[0][X];
			float c12 = -b12*cord[1][Y] - a12*cord[1][X];
			float c20 = -b20*cord[2][Y] - a20*cord[2][X];

			float e01 = a01*(float)i + b01*(float)j + c01;
			float e12 = a12*(float)i + b12*(float)j + c12;
			float e20 = a20*(float)i + b20*(float)j + c20;

			if (e01 == 0 || e12 == 0 || e20 == 0){

				interpz = (-(A*i) - (B*j) - D) / C;
				GzIntensity r, g, b, a;
				GzDepth z;
				GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
				if (interpz < z){
					r = (GzIntensity)ctoi((float)render->flatcolor[0]);
					g = (GzIntensity)ctoi((float)render->flatcolor[1]);
					b = (GzIntensity)ctoi((float)render->flatcolor[2]);
					z = interpz;
					GzPutDisplay(render->display, i, j, r, g, b, a, z);
				}
			}
			else if (e01 > 0 && e12 > 0 && e20 > 0){
				interpz = (-(A*i) - (B*j) - D) / C;
				GzIntensity r, g, b, a;
				GzDepth z;
				GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
				if (interpz < z){
					r = (GzIntensity)ctoi((float)render->flatcolor[0]);
					g = (GzIntensity)ctoi((float)render->flatcolor[1]);
					b = (GzIntensity)ctoi((float)render->flatcolor[2]);
					z = interpz;
					GzPutDisplay(render->display, i, j, r, g, b, a, z);
				}
			}
			else if (e01 < 0 && e12 < 0 && e20 < 0) {
				interpz = (-(A*i) - (B*j) - D) / C;
				GzIntensity r, g, b, a;
				GzDepth z;
				GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
				if (interpz < z){
					r = (GzIntensity)ctoi((float)render->flatcolor[0]);
					g = (GzIntensity)ctoi((float)render->flatcolor[1]);
					b = (GzIntensity)ctoi((float)render->flatcolor[2]);
					z = interpz;
					GzPutDisplay(render->display, i, j, r, g, b, a, z);
				}
			}
		}

	}
	return true;
}
