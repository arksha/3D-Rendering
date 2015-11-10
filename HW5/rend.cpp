/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include    <assert.h>

#define Pi 3.1415926
short	ctoi(float color);
bool rastlee(GzRender* render, GzCoord* cord, GzCoord* norms,GzTextureIndex* perspTextcord);
bool posVectorToScreenSpace(const GzMatrix mat, const GzCoord vector, GzCoord result);
void matrixmult(GzMatrix top, GzMatrix add, GzMatrix result);//multiply matrix
int NormVector(GzCoord vector);//normalize vector
float dotvector(GzCoord vect, GzCoord vect1);//dot product of two vector;
bool crossvector(const GzCoord vec1, const GzCoord vec2, GzCoord crossProduct);//cross product of two vector
int ShadingEquation(GzRender *render, GzColor color, GzCoord norm);

float interpolate(float A, float B, float C, float D, float x, float y);//interpolate with plane and pixel

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
	 
	//initial ks, ka, kd, spec
	GzColor defKs = DEFAULT_SPECULAR, defKa = DEFAULT_AMBIENT, defKd = DEFAULT_DIFFUSE;
	(*render)->Ks[RED] = defKs[RED];
	(*render)->Ks[GREEN] = defKs[GREEN];
	(*render)->Ks[BLUE] = defKs[BLUE];

	(*render)->Ka[RED] = defKa[RED];
	(*render)->Ka[GREEN] = defKa[GREEN];
	(*render)->Ka[BLUE] = defKa[BLUE];

	(*render)->Kd[RED] = defKd[RED];
	(*render)->Kd[GREEN] = defKd[GREEN];
	(*render)->Kd[BLUE] = defKd[BLUE];

	(*render)->spec = DEFAULT_SPEC;
	(*render)->interp_mode = GZ_RGB_COLOR;
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
		matrixmult(render->Ximage[render->matlevel], matrix, render->Ximage[render->matlevel+1]);
	}
	render->matlevel++;

	if (render->matlevel == 0 || render->matlevel == 1){
		GzMatrix identityMtx;
		identityMtx[0][0] = 1;
		identityMtx[0][1] = 0;
		identityMtx[0][2] = 0;
		identityMtx[0][3] = 0;

		identityMtx[1][0] = 0;
		identityMtx[1][1] = 1;
		identityMtx[1][2] = 0;
		identityMtx[1][3] = 0;

		identityMtx[2][0] = 0;
		identityMtx[2][1] = 0;
		identityMtx[2][2] = 1;
		identityMtx[2][3] = 0;

		identityMtx[3][0] = 0;
		identityMtx[3][1] = 0;
		identityMtx[3][2] = 0;
		identityMtx[3][3] = 1;
		memcpy(render->Xnorm[render->matlevel], identityMtx, sizeof(GzMatrix));
	}
	else{

		GzMatrix R;
		float K = 1 / sqrt(matrix[0][0] * matrix[0][0] + matrix[1][0] * matrix[1][0] + matrix[2][0] * matrix[2][0]);

		R[0][0] = matrix[0][0] * K;
		R[0][1] = matrix[0][1] * K;
		R[0][2] = matrix[0][2] * K;
		R[0][3] = 0;

		R[1][0] = matrix[1][0] * K;
		R[1][1] = matrix[1][1] * K;
		R[1][2] = matrix[1][2] * K;
		R[1][3] = 0;

		R[2][0] = matrix[2][0] * K;
		R[2][1] = matrix[2][1] * K;
		R[2][2] = matrix[2][2] * K;
		R[2][3] = 0;

		R[3][0] = 0;
		R[3][1] = 0;
		R[3][2] = 0;
		R[3][3] = 1;

		GzMatrix Xformprod;
		matrixmult(render->Xnorm[render->matlevel - 1], R, Xformprod);
		memcpy(render->Xnorm[render->matlevel], Xformprod, sizeof(GzMatrix));
	}


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
		switch (nameList[i]){
		case GZ_RGB_COLOR:
			GzColor* color;
				color = (GzColor *)valueList[i];
			render->flatcolor[RED] = color[i][RED];
			render->flatcolor[GREEN] = color[i][GREEN];
			render->flatcolor[BLUE] = color[i][BLUE];
			break;
		case GZ_INTERPOLATE:
			int * interpmodept;
				interpmodept = (int *)valueList[i];
			render->interp_mode = *interpmodept;
			break;
		case GZ_DIRECTIONAL_LIGHT:
			GzLight* dirlight;
			dirlight = (GzLight *)valueList[i];
			if (render->numlights < MAX_LIGHTS){
				render->lights[render->numlights] = *dirlight;
				NormVector(render->lights[render->numlights].direction);
				render->numlights += 1;
			}
			break;
		case GZ_AMBIENT_LIGHT:
			GzLight* amblight;
			amblight = (GzLight *)valueList[i];
			render->ambientlight = *amblight;
			break;
		case GZ_AMBIENT_COEFFICIENT:
			float* ka;
			ka = (float*)valueList[i];
			render->Ka[RED] = ka[RED];
			render->Ka[GREEN] = ka[GREEN];
			render->Ka[BLUE] = ka[BLUE];
			break;
		case GZ_DIFFUSE_COEFFICIENT:
			float* kd;
			kd = (float*)valueList[i];
			render->Kd[RED] = kd[RED];
			render->Kd[GREEN] = kd[GREEN];
			render->Kd[BLUE] = kd[BLUE];
			break;
		case GZ_SPECULAR_COEFFICIENT:
			float* ks;
			ks = (float*)valueList[i];
			render->Ks[RED] = ks[RED];
			render->Ks[GREEN] = ks[GREEN];
			render->Ks[BLUE] = ks[BLUE];
			break;
		case GZ_DISTRIBUTION_COEFFICIENT:
			float* specpower;
			specpower = (float*)valueList[i];
			render->spec = *specpower;
			break;
		case GZ_TEXTURE_MAP:
			GzTexture texturefun = (GzTexture)valueList[i];
			render->tex_fun = texturefun;
			break;
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

	GzCoord* cord;
	GzCoord* Norms;
	GzTextureIndex* Textcord;//uv
	for (int i = 0; i < numParts; i++){
		switch (nameList[i]){
		case GZ_NULL_TOKEN:
			break;
		case GZ_POSITION:
			cord = (GzCoord*)valueList[i];
			break;
		case GZ_NORMAL:
			Norms = (GzCoord *)valueList[i];
			break;
		case GZ_TEXTURE_INDEX:
			Textcord = (GzTextureIndex *)valueList[i];//uv 
			break;
		}
	}

	bool behind = false;//behind Visual point
	GzCoord* xformcord = new GzCoord[3];
	GzCoord* xformNorms = new GzCoord[3];
	//get triangle cord

	// Xform positions of verts using matrix on top of stack
	GzMatrix tosmatrx;
	memcpy(tosmatrx, render->Ximage[render->matlevel], sizeof(GzMatrix));
	GzMatrix tosNmatrx;
	memcpy(tosNmatrx, render->Xnorm[render->matlevel], sizeof(GzMatrix));
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

	//
	NormVector(Norms[0]);
	NormVector(Norms[1]);
	NormVector(Norms[2]);

	for (int j = 0; j < 3; j++) {
		xformNorms[j][X] = tosNmatrx[0][0] * Norms[j][X] + tosNmatrx[0][1] * Norms[j][Y] + tosNmatrx[0][2] * Norms[j][Z];
		xformNorms[j][Y] = tosNmatrx[1][0] * Norms[j][X] + tosNmatrx[1][1] * Norms[j][Y] + tosNmatrx[1][2] * Norms[j][Z];
		xformNorms[j][Z] = tosNmatrx[2][0] * Norms[j][X] + tosNmatrx[2][1] * Norms[j][Y] + tosNmatrx[2][2] * Norms[j][Z];
	}
	NormVector(xformNorms[0]);
	NormVector(xformNorms[1]);
	NormVector(xformNorms[2]);


	if (behind == true) {
		return GZ_SUCCESS;
	}
	if (rastlee(render, xformcord, xformNorms, Textcord)){
		return GZ_SUCCESS;
	}


	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
	return(short)((int)(color * ((1 << 12) - 1)));
}

bool rastlee(GzRender* render, GzCoord* cord, GzCoord * norms, GzTextureIndex * Textcord){

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

				float tempNx = norms[k][X];
				float tempNy = norms[k][Y];
				float tempNz = norms[k][Z];
				norms[k][X] = norms[k + 1][X];
				norms[k][Y] = norms[k + 1][Y];
				norms[k][Z] = norms[k + 1][Z];
				norms[k + 1][X] = tempNx;
				norms[k + 1][Y] = tempNy;
				norms[k + 1][Z] = tempNz;

				float tempU = Textcord[k][U];
				float tempV = Textcord[k][V];
				Textcord[k][U] = Textcord[k + 1][U];
				Textcord[k][V] = Textcord[k + 1][V];
				Textcord[k + 1][U] = tempU;
				Textcord[k + 1][V] = tempV;

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

			float tempNx = norms[0][X];
			float tempNy = norms[0][Y];
			float tempNz = norms[0][Z];
			norms[0][X] = norms[1][X];
			norms[0][Y] = norms[1][Y];
			norms[0][Z] = norms[1][Z];
			norms[1][X] = tempNx;
			norms[1][Y] = tempNy;
			norms[1][Z] = tempNz;

			
			float tempU = Textcord[0][U];
			float tempV = Textcord[0][V];
			Textcord[0][U] = Textcord[1][U];
			Textcord[0][V] = Textcord[1][V];
			Textcord[1][U] = tempU;
			Textcord[1][V] = tempV;
		
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

			float tempNx = norms[2][X];
			float tempNy = norms[2][Y];
			float tempNz = norms[2][Z];
			norms[2][X] = norms[1][X];
			norms[2][Y] = norms[1][Y];
			norms[2][Z] = norms[1][Z];
			norms[1][X] = tempNx;
			norms[1][Y] = tempNy;
			norms[1][Z] = tempNz;

			float tempU = Textcord[2][U];
			float tempV = Textcord[2][V];
			Textcord[2][U] = Textcord[1][U];
			Textcord[2][V] = Textcord[1][V];
			Textcord[1][U] = tempU;
			Textcord[1][V] = tempV;
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

	
	
	//Gouraud shading
	GzColor goudinter;
	GzColor color0, color1, color2;
	/*
	ShadingEquation(render, color0, norms[0]);
	ShadingEquation(render, color1, norms[1]);
	ShadingEquation(render, color2, norms[2]);*/
	
	if (render->interp_mode == GZ_COLOR) {
		if (render->tex_fun == NULL) {
			ShadingEquation(render, color0, norms[0]);
			ShadingEquation(render, color1, norms[1]);
			ShadingEquation(render, color2, norms[2]);
		}
		else {//use texture function
			render->Ka[RED] =1;
			render->Ka[GREEN] =1;
			render->Ka[BLUE] = 1;

			render->Kd[RED] = 1;
			render->Kd[GREEN] =1;
			render->Kd[BLUE] = 1;

			render->Ks[RED] = 1;
			render->Ks[GREEN] =1;
			render->Ks[BLUE] = 1;
			ShadingEquation(render, color0, norms[0]);
			ShadingEquation(render, color1, norms[1]);
			ShadingEquation(render, color2, norms[2]);
		}
	}
	GzColor gA, gB, gC, gD;
	
	
	gA[RED] = vec01[Y] * (color2[RED] - color1[RED]) - (color1[RED] - color0[RED]) * vec12[Y];
	gB[RED] = (color1[RED] - color0[RED]) * vec12[X] - vec01[X] * (color2[RED] - color1[RED]);
	gC[RED] = vec01[X] * vec12[Y] - vec01[Y] * vec12[X];
	gD[RED] = -(gA[RED]*cord[0][X] + gB[RED]*cord[0][Y] + gC[RED]*color0[RED]);
	
	gA[GREEN] = vec01[Y] * (color2[GREEN] - color1[GREEN]) - (color1[GREEN] - color0[GREEN]) * vec12[Y];
	gB[GREEN] = (color1[GREEN] - color0[GREEN]) * vec12[X] - vec01[X] * (color2[GREEN] - color1[GREEN]);
	gC[GREEN] = vec01[X] * vec12[Y] - vec01[Y] * vec12[X];
	gD[GREEN] = -(gA[GREEN] * cord[0][X] + gB[GREEN] * cord[0][Y] + gC[GREEN] * color0[GREEN]);

	gA[BLUE] = vec01[Y] * (color2[BLUE] - color1[BLUE]) - (color1[BLUE] - color0[BLUE]) * vec12[Y];
	gB[BLUE] = (color1[BLUE] - color0[BLUE]) * vec12[X] - vec01[X] * (color2[BLUE] - color1[BLUE]);
	gC[BLUE] = vec01[X] * vec12[Y] - vec01[Y] * vec12[X];
	gD[BLUE] = -(gA[BLUE] * cord[0][X] + gB[BLUE] * cord[0][Y] + gC[BLUE] * color0[BLUE]);

	//prepare for interp UV texture
	float Vzp;// Vz' 
	GzTextureIndex*  perspTextcord = new GzTextureIndex[3];//warp uv-> UV from image space to perspective space for each vertex
	for (int k = 0; k < 3; k++){
	
		Vzp = cord[k][Z] / (INT_MAX - cord[k][Z]);
		perspTextcord[k][U] = Textcord[k][U] / (Vzp + 1);
		perspTextcord[k][V] = Textcord[k][V] / (Vzp + 1);
	}

	

	//if in bbox draw pixel
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

			if ((e01 == 0 || e12 == 0 || e20 == 0)||((e01 > 0) && (e12 > 0) && (e20 > 0)) ||
				((e01 < 0) && (e12 < 0) && (e20 < 0))){

				interpz = (-(A*i) - (B*j) - D) / C;
				GzIntensity r, g, b, a;
				GzDepth z;
				
				GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);

				if (interpz < z){
					GzCoord pers01, pers12;
					pers01[U] = perspTextcord[1][U] - perspTextcord[0][U];
					pers01[V] = perspTextcord[1][V] - perspTextcord[0][V];

					pers12[U] = perspTextcord[2][U] - perspTextcord[1][U];
					pers12[V] = perspTextcord[2][V] - perspTextcord[1][V];


					//	GzCoord intepcrossprod;
					//crossvector(pers01, pers12, intepcrossprod);

					GzTextureIndex tA, tB, tC, tD, textureinter, newuv;
					tA[U] = vec01[Y] * (pers12[U]) - (pers01[U]) * vec12[Y];
					tB[U] = (pers01[U]) * vec12[X] - vec01[X] * (pers12[U]);
					tC[U] = vec01[X] * vec12[Y] - vec01[Y] * vec12[X];
					tD[U] = -(tA[U] * cord[0][X] + tB[U] * cord[0][Y] + tC[U] * perspTextcord[0][U]);

					tA[V] = vec01[Y] * (pers12[V]) - (pers01[V]) * vec12[Y];
					tB[V] = (pers01[V]) * vec12[X] - vec01[X] * (pers12[V]);
					tC[V] = vec01[X] * vec12[Y] - vec01[Y] * vec12[X];
					tD[V] = -(tA[V] * cord[0][X] + tB[V] * cord[0][Y] + tC[V] * perspTextcord[0][V]);
					// interpolateUV to pixel
					/*textureinter[U] = -(tA[U] * i + tB[U] * j + tD[U]) / tC[U];
					textureinter[V] = -(tA[V] * i + tB[V] * j + tD[V]) / tC[V];*/
					textureinter[U] = interpolate(tA[U], tB[U], tC[U], tD[U], i, j);
					textureinter[V] = interpolate(tA[V], tB[V], tC[V], tD[V], i, j);
					//unwarping
					float Vzp = interpz / (INT_MAX - interpz);
					newuv[U] = textureinter[U] * (Vzp + 1);
					newuv[V] = textureinter[V] * (Vzp + 1);

					//texture color lookup with bilinear interpolation
					GzColor texColor;
					if (render->tex_fun != NULL) {
						render->tex_fun(newuv[U], newuv[V], texColor);
					}
					switch (render->interp_mode){
					case GZ_FLAT:
						r = (GzIntensity)ctoi((float)render->flatcolor[0]);
						g = (GzIntensity)ctoi((float)render->flatcolor[1]);
						b = (GzIntensity)ctoi((float)render->flatcolor[2]);
						z = interpz;
						break;
					case GZ_COLOR://ground shading
						
						
						render->Ka[RED] = texColor[RED];
						render->Ka[GREEN] = texColor[GREEN];
						render->Ka[BLUE] = texColor[BLUE];
						render->Kd[RED] = texColor[RED];
						render->Kd[GREEN] = texColor[GREEN];
						render->Kd[BLUE] = texColor[BLUE];
						render->Ks[RED] = texColor[RED];
						render->Ks[GREEN] = texColor[GREEN];
						render->Ks[BLUE] = texColor[BLUE];
					
						
						goudinter[RED] = -(gA[RED] * i + gB[RED] * j + gD[RED]) / gC[RED];
						goudinter[GREEN] = -(gA[GREEN] * i + gB[GREEN] * j + gD[GREEN]) / gC[GREEN];
						goudinter[BLUE] = -(gA[BLUE] * i + gB[BLUE] * j + gD[BLUE]) / gC[BLUE];

						//texture color
						if (render->tex_fun != NULL) {
							goudinter[RED] *= texColor[RED] ;
							goudinter[GREEN] *= texColor[GREEN];
							goudinter[BLUE] *= texColor[BLUE];
						}

						if (goudinter[RED] > 1.0) goudinter[RED] = 1.0;
						if (goudinter[GREEN] > 1.0) goudinter[GREEN] = 1.0;
						if (goudinter[BLUE] > 1.0) goudinter[BLUE] = 1.0;
						r = (GzIntensity)ctoi(goudinter[RED]);
						g = (GzIntensity)ctoi(goudinter[GREEN]);
						b = (GzIntensity)ctoi(goudinter[BLUE]);
						z = interpz;
						break;

					case GZ_NORMALS:
						//interpolizing norms;
						GzColor pA, pB, pC, pD;
						pA[RED] = vec01[Y] * (norms[2][RED] - norms[1][RED]) - (norms[1][RED] - norms[0][RED]) * vec12[Y];
						pB[RED] = (norms[1][RED] - norms[0][RED]) * vec12[X] - vec01[X] * (norms[2][RED] - norms[1][RED]);
						pC[RED] = vec01[X] * vec12[Y] - vec01[Y] * vec12[X];
						pD[RED] = -(pA[RED] * cord [0][X] + pB[RED] * cord[0][Y] + pC[RED] * norms[0][RED]);

						pA[GREEN] = vec01[Y] * (norms[2][GREEN] - norms[1][GREEN]) - (norms[1][GREEN] - norms[0][GREEN]) * vec12[Y];
						pB[GREEN] = (norms[1][GREEN] - norms[0][GREEN]) * vec12[X] - vec01[X] * (norms[2][GREEN] - norms[1][GREEN]);
						pC[GREEN] = vec01[X] * vec12[Y] - vec01[Y] * vec12[X];
						pD[GREEN] = -(pA[GREEN] * cord[0][X] + pB[GREEN] * cord[0][Y] + pC[GREEN] * norms[0][GREEN]);

						pA[BLUE] = vec01[Y] * (norms[2][BLUE] - norms[1][BLUE]) - (norms[1][BLUE] - norms[0][BLUE]) * vec12[Y];
						pB[BLUE] = (norms[1][BLUE] - norms[0][BLUE]) * vec12[X] - vec01[X] * (norms[2][BLUE] - norms[1][BLUE]);
						pC[BLUE] = vec01[X] * vec12[Y] - vec01[Y] * vec12[X];
						pD[BLUE] = -(pA[BLUE] * cord[0][X] + pB[BLUE] * cord[0][Y] + pC[BLUE] * norms[0][BLUE]);

						GzColor phonginter;

						phonginter[0] = -(pA[0] * i + pB[0] * j + pD[0]) / pC[0];
						phonginter[1] = -(pA[1] * i + pB[1] * j + pD[1]) / pC[1];
						phonginter[2] = -(pA[2] * i + pB[2] * j + pD[2]) / pC[2];
						NormVector(phonginter);
				
						//Then shading calculation with texture color used as Kd and Ka
						if (render->tex_fun != NULL) {
							render->Ka[RED] = render->Kd[RED] = texColor[RED];
							render->Ka[GREEN] = render->Kd[GREEN] = texColor[GREEN];
							render->Ka[BLUE] = render->Kd[BLUE] = texColor[BLUE];
						}
						GzColor colorpix;
						ShadingEquation(render, colorpix, phonginter);
						r = (GzIntensity)ctoi(colorpix[RED]);
						g = (GzIntensity)ctoi(colorpix[GREEN]);
						b = (GzIntensity)ctoi(colorpix[BLUE]);
						z = interpz;
						
						break;
					}
					GzPutDisplay(render->display, i, j, r, g, b, a, z);
				}
			}
			
			
		}

	}
	return true;
}

void matrixmult(GzMatrix top, GzMatrix add, GzMatrix result){
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = 0;

		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				result[i][j] += top[i][k] * add[k][j];
			}

		}
	}

}

int NormVector(GzCoord vector) {
	if (vector == NULL) {
		return GZ_FAILURE;
	}

	float norm = sqrt((vector[X] * vector[X]) + (vector[Y] * vector[Y]) + (vector[Z] * vector[Z]));

	vector[X] = vector[X] / norm;
	vector[Y] = vector[Y] / norm;
	vector[Z] = vector[Z] / norm;

	return GZ_SUCCESS;
}
float dotvector(GzCoord vect, GzCoord vect1){
	float vectprod;
	return vectprod = vect[X] * vect1[X] + vect[Y] * vect1[Y] + vect[Z] * vect1[Z];
}

bool crossvector(const GzCoord vec1, const GzCoord vec2, GzCoord crossProduct)
{
	crossProduct[X] = vec1[Y] * vec2[Z] - vec1[Z] * vec2[Y];
	crossProduct[Y] = vec1[Z] * vec2[X] - vec1[X] * vec2[Z];
	crossProduct[Z] = vec1[X] * vec2[Y] - vec1[Y] * vec2[X];
	return true;
}

int ShadingEquation(GzRender *render, GzColor color, GzCoord norm){
	NormVector(norm);//normalize vector
	GzCoord  E = { 0.0, 0.0, -1.0 };
	GzCoord* R = new GzCoord[render->numlights];
	NormVector(E);
	//N*L
	float NdotE;
	int* testN = new int[render->numlights];
	float* NdotL = new float[render->numlights];
	float* negNdotL = new float[render->numlights];
	float* RdotE = new float[render->numlights];
			
	
	GzCoord negN = { -norm[X], -norm[Y], -norm[Z] };

	NdotE = dotvector(norm, E);
	//compute R 
	for (int i = 0; i < render->numlights; i++){
		NdotL[i] = dotvector(norm, render->lights[i].direction);
		negNdotL[i] = dotvector(negN, render->lights[i].direction);
		if (NdotL[i] >= 0 && NdotE >= 0){
			//compute lighting model
			//R = 2(NdotL)N-L
			R[i][X] = 2 * NdotL[i]*norm[X] - render->lights[i].direction[X];
			R[i][Y] = 2 * NdotL[i]*norm[Y] - render->lights[i].direction[Y];
			R[i][Z] = 2 * NdotL[i]*norm[Z] - render->lights[i].direction[Z];
			NormVector(R[i]);
			testN[i] = 0;
		}
		else if (NdotL[i] < 0 && NdotE < 0){
			//flip N and compute backside
			R[i][X] = 2 * negNdotL[i] * (-norm[X]) - render->lights[i].direction[X];
			R[i][Y] = 2 * negNdotL[i] * (-norm[Y]) - render->lights[i].direction[Y];
			R[i][Z] = 2 * negNdotL[i] * (-norm[Z]) - render->lights[i].direction[Z];
			NormVector(R[i]);
			testN[i] = 1;
		}
		else{
			//skip
			testN[i] = 2;
		}
	}
	//compute Spectical light part
	GzColor specSum = { 0, 0, 0 };
	for (int i = 0; i < render->numlights; i++){
		RdotE[i] = dotvector(R[i], E);
		if (testN[i] != 2){
			assert(abs(dotvector(R[i], R[i]) - 1) < 0.00001);
			if (RdotE[i] < 0)
				RdotE[i] = 0;
			if (RdotE[i] > 1)
				RdotE[i] = 1;
			specSum[RED] += render->lights[i].color[RED] * pow(RdotE[i], render->spec);
			specSum[GREEN] += render->lights[i].color[GREEN] * pow(RdotE[i], render->spec);
			specSum[BLUE] += render->lights[i].color[BLUE] * pow(RdotE[i], render->spec);
		}
	}
	GzColor specpart;
	specpart[RED] = render->Ks[RED] * specSum[RED];
	specpart[GREEN] = render->Ks[GREEN] * specSum[GREEN];
	specpart[BLUE] = render->Ks[BLUE] * specSum[BLUE];

	//compute diffusion part
	GzColor diffSum = { 0, 0, 0 };
	for (int i = 0; i < render->numlights; i++){
		if (testN[i] == 2)continue;
		if (testN[i] == 0){
			//both positive
			diffSum[RED] += render->lights[i].color[RED] * NdotL[i];
			diffSum[GREEN] += render->lights[i].color[GREEN] * NdotL[i];
			diffSum[BLUE] += render->lights[i].color[BLUE] * NdotL[i];
		}
		if (testN[i] == 1){
			//both negative
			diffSum[RED] += render->lights[i].color[RED] * negNdotL[i];
			diffSum[GREEN] += render->lights[i].color[GREEN] * negNdotL[i];
			diffSum[BLUE] += render->lights[i].color[BLUE] * negNdotL[i];
		}
		
	}
	GzColor diffpart;
	diffpart[RED] = render->Kd[RED] * diffSum[RED];
	diffpart[GREEN] = render->Kd[GREEN] * diffSum[GREEN];
	diffpart[BLUE] = render->Kd[BLUE] * diffSum[BLUE];
	
	//compute ambient part
	
	GzColor ambpart;
	
	ambpart[RED] = render->Ka[RED] * render->ambientlight.color[RED];
	ambpart[GREEN] = render->Ka[GREEN] * render->ambientlight.color[GREEN];
	ambpart[BLUE] = render->Ka[BLUE] * render->ambientlight.color[BLUE];


	//calculate c
	color[RED] = specpart[RED] + diffpart[RED] + ambpart[RED];
	color[GREEN] = specpart[GREEN] + diffpart[GREEN] + ambpart[GREEN];
	color[BLUE] = specpart[BLUE] + diffpart[BLUE] + ambpart[BLUE];

	delete[] testN;
	delete[] NdotL;
	delete[] negNdotL;
	delete[] RdotE;
	delete[] R;

	return GZ_SUCCESS;

}

float interpolate(float A, float B, float C, float D,float x,float y){
	return  -(A * x + B * y + D) / C;
}
