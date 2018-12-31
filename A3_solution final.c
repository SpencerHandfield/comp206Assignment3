/* FILE: A3_solutions.c is where you will code your answers for Assignment 3.
 * 
 * Each of the functions below can be considered a start for you. They have 
 * the correct specification and are set up correctly with the header file to
 * be run by the tester programs.  
 *
 * You should leave all of the code as is, especially making sure not to change
 * any return types, function name, or argument lists, as this will break
 * the automated testing. 
 *
 * Your code should only go within the sections surrounded by
 * comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A3_solutions.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "A3_provided_functions.h"

unsigned char*
bmp_open( char* bmp_filename,        unsigned int *width, 
          unsigned int *height,      unsigned int *bits_per_pixel, 
          unsigned int *padding,     unsigned int *data_size, 
          unsigned int *data_offset                                  )
{
  unsigned char *img_data=NULL;
  // REPLACE EVERYTHING FROM HERE

  FILE *bmpfile = fopen(bmp_filename, "rb");

  if (bmp_filename == NULL)
  {
	  printf("unable to open file");
	  //return 0;
  }
  //open and read the bmp file (from lectures)
  char b, m;
  fread (&b, 1, 1, bmpfile);
  fread (&m, 1, 1, bmpfile);
  fread (data_size, 1, sizeof(unsigned int), bmpfile);
  rewind(bmpfile);
  //alloc mem
  img_data = (unsigned char*) malloc(*data_size);
  if (fread (img_data, 1, *data_size, bmpfile) != *data_size)
  {
	  printf("unable to read %d bytes", *data_size);
	  //return 0;
  }
  //width in all sizes for the picture
  *width = *(unsigned int*)(img_data+18);
  *height = *(unsigned int*)(img_data+22);
  *bits_per_pixel = *(unsigned short*)(img_data+28);
  *data_offset = *(unsigned int*)(img_data+10);

  //finalize the padding based on the size of the rows
  int sizeval = (((*width)*(*bits_per_pixel))/8);
  int pad = 4-((sizeval)%4);
  if (pad == 4)
  {
	  pad = 0;
  }
  *padding = pad;

  return img_data;

  //printf( "UNIMPLEMENTED FUNCTION: bmp_open has not yet been coded. Please complete before submitting!\n");
  // TO HERE!  
  return img_data;  
}

void 
bmp_close( unsigned char **img_data )
{
  // REPLACE EVERYTHING FROM HERE
	//empty and close
	if (*img_data != NULL)
	{
		free (*img_data);
		*img_data = NULL;
	}
  //printf( "UNIMPLEMENTED FUNCTION: bmp_close has not yet been coded. Please complete before submitting!\n");
  // TO HERE!  
}

unsigned char***  
bmp_scale( unsigned char*** pixel_array, unsigned char* header_data, unsigned int header_size,
           unsigned int* width, unsigned int* height, unsigned int num_colors,
           float scale )
{
  unsigned char*** new_pixel_array = NULL; 
  // REPLACE EVERYTHING FROM HERE
  //set new values based on the scaling
  unsigned int new_width = (unsigned int)(*width)*scale;
  unsigned int new_height = (unsigned int)(*height)*scale;
  //alloc mem
  //new array
  new_pixel_array = (unsigned char***) malloc(sizeof(unsigned char**)*(new_height));
  if(pixel_array == NULL )
  {
      printf( "Error: bmp_to_3D_array failed to allocate memory for image");
      return NULL;
  }
  //continue alloc mem
  for (int row = 0; row<new_height; row++)
  {
	  new_pixel_array[row] = (unsigned char**) malloc (sizeof(unsigned char*)*(new_width));
	  for (int column=0; column < new_width; column++)
	  {
		  new_pixel_array[row][column] = (unsigned char*) malloc(sizeof(unsigned char)*(num_colors));
	  }
  }

  //populate the now allocated 3D array
  for (int row = 0; row < new_height; row++)
  {
	  for(int column = 0; column < new_width; column++)
	  {
		  for (int color = 0; color < num_colors; color++)
		  {
			  new_pixel_array[row][column][color] = pixel_array[(int)(row/scale)][(int)(column/scale)][color];
		  }
	  }
  }

  //adjusting header
  *(unsigned int*)(header_data+18) = new_width;
  *(unsigned int*)(header_data+22) = new_height;
  *height = new_height;
  *width = new_width;

  //printf( "UNIMPLEMENTED FUNCTION: bmp_scale has not yet been coded. Please complete before submitting!\n");
  // TO HERE! 
  return new_pixel_array;
}         

int 
bmp_collage( char* background_image_filename,     char* foreground_image_filename, 
             char* output_collage_image_filename, int row_offset,                  
             int col_offset,                      float scale )
{
  // REPLACE EVERYTHING FROM HERE

	//vals
	unsigned char* bg_header_data;
	unsigned int bg_header_size;
	unsigned int bg_width;
	unsigned int bg_height;
	unsigned int bg_num_colors;
	unsigned char*** bg_pixel_array = NULL;

	unsigned char* fg_header_data;
	unsigned int fg_header_size;
	unsigned int fg_width;
	unsigned int fg_height;
	unsigned int fg_num_colors;
	unsigned char*** fg_pixel_array = NULL;

	//checks
	if (bg_pixel_array == NULL)
	{
		printf("BG image is empty");
				return -1;
	}
	if (fg_pixel_array == NULL)
		{
			printf("BG image is empty");
					return -1;
	}
	if (bg_num_colors != 4 || fg_num_colors !=4)
	{
		printf("wrong number of colors");
		return -1;
	}
	if ((fg_width)*(fg_height) > (bg_width)*(bg_height))
	{
			printf("image sizes don't match");
			return -1;
	}
	if (((fg_width + col_offset) >= (bg_height)) ||((fg_height + row_offset) >= (bg_height)))
	{
		printf("image doesn't fit");
		return 0;
	}


	//instantiate background in array
	bg_pixel_array = bmp_to_3D_array(background_image_filename, &bg_header_data, &bg_header_size, &bg_width, &bg_height, &bg_num_colors);
	//instantiate foreground in array
	fg_pixel_array = bmp_to_3D_array(foreground_image_filename, &fg_header_data, &fg_header_size, &fg_width, &fg_height, &fg_num_colors);
	//scale
	unsigned char*** fg_scaled_pixel_array = bmp_scale(fg_pixel_array, fg_header_data, fg_header_size, &fg_width, &fg_height, fg_num_colors, scale);

	//iterate through entire image to put fg on bg by replacing pixels
	for (int row=row_offset; row < fg_height; row++)
	{
		for (int column = col_offset; column < fg_width; column++)
		{
			//make sure only replaces within bounds
			if (row < (row_offset + fg_height) && column < (col_offset + fg_width))
			{
				//replace
				if (fg_scaled_pixel_array[row-row_offset][column-col_offset][3] != 0)
				{
					bg_pixel_array[row][column] = fg_scaled_pixel_array[row-row_offset][column-col_offset];
				}
			}
		}
	}
	bmp_from_3D_array(output_collage_image_filename, bg_header_data, bg_header_size, bg_pixel_array, bg_width, bg_height, bg_num_colors);


  //printf( "UNIMPLEMENTED FUNCTION: bmp_collage has not yet been coded. Please complete before submitting!\n");
  // TO HERE! 
  return 0;
}

