/*   CS580 HW1 display functions to be completed   */

#include   "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */
	*framebuffer = (char*) malloc(3 * width * height);
	if (!(*framebuffer)){
		return GZ_FAILURE;
	}
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/

	if (!display || xRes > MAXXRES || yRes > MAXYRES || xRes <= 0 || yRes <= 0)
		return GZ_FAILURE;
	*display = new GzDisplay();
	(*display)->fbuf = (GzPixel*) malloc(xRes * yRes * sizeof(GzPixel));
	(*display)->xres = xRes;
	(*display)->yres = yRes;
	if (!((*display)->fbuf) || !((*display)->xres) || !((*display)->yres)) {
		return GZ_FAILURE;
	}
	return GZ_SUCCESS;
	
}


int GzFreeDisplay(GzDisplay	*display)
{
/* HW1.3 clean up, free memory */
	free(display->fbuf);
	display->fbuf = NULL;
	free(display);
	display = NULL;
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* HW1.4 pass back values for a display */
	*xRes = display->xres;
	*yRes = display->yres;
	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* HW1.5 set everything to some default values - start a new frame */
	if (!display)
		return GZ_FAILURE;
	for (int i = 0; i < (display->xres * display->yres); i++){
		display->fbuf[i].red = 0;
		display->fbuf[i].green = 0;
		display->fbuf[i].blue= 0;
		display->fbuf[i].alpha = 1;
		display->fbuf[i].z = 0;
	}
	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.6 write pixel values into the display */
	// boundery checking 
	if (i < 0 || j < 0 || i >= display->xres || j >= display->yres)
		return GZ_SUCCESS;
	//clamping code
	if (r < 0)
		r = 0;
	else if (r > 4095)
		r = 4095;
	if (g < 0) 
		g = 0;
	else if (g > 4095) 
		g = 4095;
	if (b < 0) 
		b = 0;
	else if (b > 4095) 	
		b = 4095;
	int x = ARRAY(i, j);
	display->fbuf[x].red = r;
	display->fbuf[x].green = g;
	display->fbuf[x].blue = b;
	display->fbuf[x].alpha = a;
	display->fbuf[x].z = z;
	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.7 pass back a pixel value to the display */
	int x = ARRAY(i, j);
	*r = display->fbuf[x].red;
	*g = display->fbuf[x].green;
	*b = display->fbuf[x].blue;
	*a = display->fbuf[x].alpha;
	*z = display->fbuf[x].z;

	if (i < 0 || j < 0 || i >= display->xres || j >= display->yres)
		return GZ_FAILURE;
	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\r" */
	FILE* fptr = outfile;
	fprintf(fptr, "P6 %d %d 255\n", display->xres, display->yres); 

	unsigned char r, g, b;
		for (int i = 0; i < (display->xres * display->yres); i++)
		{
			r = display->fbuf[i].red >> 4;
			g = display->fbuf[i].green >> 4;
			b = display->fbuf[i].blue >> 4;

			fwrite(&r, 1, 1, fptr);
			fwrite(&g, 1, 1, fptr);
			fwrite(&b, 1, 1, fptr);		
	}
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	if (framebuffer == NULL || display == NULL)
		return GZ_FAILURE;
	unsigned char r,g,b;
	int j=0;
	for (int i = 0; i < (display->xres * display->yres); i++){
		r = display->fbuf[i].red>>4;
		g = display->fbuf[i].green >> 4;
		b = display->fbuf[i].blue >> 4;
		
		framebuffer[j] = b;
		j++;
		framebuffer[j] = g;
		j++;
		framebuffer[j] = r;
		j++;
	}


	return GZ_SUCCESS;
}