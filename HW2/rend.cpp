#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

short	ctoi(float color);
int GzNewRender(GzRender **render, GzDisplay *display)
{
	/*
	- malloc a renderer struct
	- span interpolator needs pointer to display for pixel writes
	*/
	*render = new GzRender();
	if (render == NULL)
		return GZ_FAILURE;
	if (display == NULL)
		return GZ_FAILURE;
	(*render)->display = display;
	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
	/*
	-free all renderer resources
	*/
	if (render == NULL)
		return GZ_FAILURE;
	free(render);
	render = NULL;
	delete render;
	return GZ_SUCCESS;
}


int GzBeginRender(GzRender	*render)
{
	/*
	- set up for start of each frame - init frame buffer
	*/
	if (render == NULL)
		return GZ_FAILURE;
	GzInitDisplay(render->display);
	if (render->display == NULL)
		return GZ_FAILURE;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList,
	GzPointer *valueList) /* void** valuelist */
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

			render->flatcolor[RED] = color[i][RED];
			render->flatcolor[GREEN] = color[i][GREEN];
			render->flatcolor[BLUE] = color[i][BLUE];
		}
	}
	return GZ_SUCCESS;
}


int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList)
	/* numParts - how many names and values */
{
	/*
	- pass in a triangle description with tokens and values corresponding to
	GZ_NULL_TOKEN:		do nothing - no values
	GZ_POSITION:		3 vert positions
	- Invoke the scan converter and return an error code
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
			//get triangle cord
			GzCoord* cord = (GzCoord*)valueList[i];
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
								cord[k][Z] = cord[k+1][Z];
								cord[k+1][X] = tempx;
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

		}



	}
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
	return(short)((int)(color * ((1 << 12) - 1)));
}

