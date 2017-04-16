#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <string.h>

//This program is created to convert images to a ben day pop art picture.
//The program is created by Chun You Sim.

/////////////////////////////////////////////////////////////////////////////////////////////////
//Creating functions to assist calculations
/////////////////////////////////////////////////////////////////////////////////////////////////

//Create callback function for C's qsort function
//Source from Anderson,E,F.,2015. Computing for Graphics: Fundamental Algorithms
int comp(const void *x,const void *y)
{
if(*(int*)x < *(int*)y) return -1;
if(*(int*)x == *(int*)y) return 0;
else return 1;
}
//Source Ends here

//Create a function to convert pixels to grayscale
Uint32 CovertGrayscale(pixel)
{
	/*Parameters are...
	pixel:	The pixel to be converted to grayscale*/
	
	Uint8 r = pixel >> 16 & 0xFF;
	Uint8 g = pixel >> 8 & 0xFF;
	Uint8 b = pixel & 0xFF;
	Uint8 v = 0.212671f*r + 0.715160f*g + 0.072169f*b;
	pixel = (0xFF<<24) | (v << 16) | (v<<8) | v;
	return pixel;
}

//Create function to sort colours into RGB and then qsort them
void sortColourRGB(Uint32** array, Uint32 red[], Uint32 green[], Uint32 blue[], int totalsize, int startvalue)
{
	/*Parameters are...
	Uint32** array:	The array with every pixel of the picture's RGB values in it.
	Uint32 red[]:	The array to contain all the R values of array a.
	Uint32 green[]:	The array to contain all the G values of array a.
	Uint32 blue[]:	The array to contain all the B values of array a.
	int totalsize:	The total number of pixels in the picture.
	int startvalue:	The starting value in the array for the assigning to occur.*/
	
	for (int i=startvalue, z=0; i<(totalsize); i++,z++)
	{
		red[z] = array[i][0];
		green[z] = array[i][1];
		blue[z] = array[i][2];
	}
	//Quicksort the red, green and blue arrays
	qsort(red,(totalsize-startvalue),sizeof(Uint32),&comp);	
	qsort(green,(totalsize-startvalue),sizeof(Uint32),&comp);
	qsort(blue,(totalsize-startvalue),sizeof(Uint32),&comp);
}

//Create a function for manual_qsort for a 2d array
//Source code referenced from Anderson,E,F.,2015. Computing for Graphics: Fundamental Algorithms
void manual_qsort(Uint32** a, int start, int end, int column)
{
	/*Parameters are...
	Uint32** a:	The array with every pixel of the picture's RGB values in it.
	int start:	The starting value for the manual qsort.
	int end:	The ending value for the manual qsort.
	int column:	The column which indicates the longest axis to be sorted.*/
	
	int pivot, l, r;
	int tmp1,tmp2,tmp3;
	
	if(start<end)
	{
		pivot=a[start+(end-start)/2][column];
		l=start;
		r=end;
		
		while(l<r)
		{
			while((a[l][column]<pivot) && (l<=end))
			{
				l++;
			}
			while((a[r][column]>pivot) && (r>=start))
			{
				r--;
			}
		
			if (l<=r)
			{
				tmp1 = a[l][0];
				tmp2 = a[l][1];
				tmp3 = a[l][2];
				
				a[l][0] = a[r][0];
				a[l][1] = a[r][1];
				a[l][2] = a[r][2];

				a[r][0] = tmp1;
				a[r][1] = tmp2;
				a[r][2] = tmp3;
				
				l++;
				r--;
			}
		}
		
		manual_qsort(a,start,r,column);
		manual_qsort(a,l,end,column);
	}
}
//Source code reference ends here

//Create a function to find out the longest axis for the median cut algorithm to cut
int longest_axisRGB(Uint32 red[], Uint32 green[], Uint32 blue[], int totalsize, int startvalue)
{
	/*Parameters are...
	Uint32 red[]:	The array that contains all the R values of the picture.
	Uint32 green[]:	The array that contains all the G values of the picture.
	Uint32 blue[]:	The array that contains all the B values of the picture.
	int totalsize:	The total number of pixels in the picture.
	int startvalue:	The starting value for the assigning to occur.*/
	
	int longestR, longestG, longestB, longestColumn;
	
	longestR = red[totalsize-1] - red[startvalue];
	longestB = blue[totalsize-1] - blue[startvalue];
	longestG = green[totalsize-1] - green[startvalue];
	
	if (longestR>longestB)
	{
		if (longestR> longestG)
		{
		//LongestR is the longest
		longestColumn = 0;
		}
		else
		{
		//LongestG is the longest
		longestColumn = 1;
		}
	}
	
	else
	{
		if (longestB>longestG)
		{
		//LongestB is the longest
		longestColumn = 2;
		}
		else
		{
		//LongestG is the longest. It is also the longest if columns are equal.
		longestColumn = 1;
		}
	}

	return longestColumn;

}

//Creating a function for the Median Cut Algorithm
void MedianCutAlgorithm(Uint32** colour, Uint32 colour_palette[][3], int startvalue, int totalsize, int MaxElementCount, int *counterNum_ptr)
{
	/*Parameters are...
	Uint32** colour:	The array with every pixel of the picture's RGB values in it.
	Uint32 colour_palette[][3]:	The 2D array which will store the reduced colour palette.
	int startvalue:	The starting index for where the Median Cut Algorithm is to work on.
	int totalsize:	The last index +1 for where the Median Cut Algorithm is to work on.
	int MaxElementCount:	The minimum element count between start and end for the function to not continue to cut the array but get the colour_palette instead.
	int *counterNum_ptr:	The pointer to an integer which helps to cycle through the assignment of the colour_palette array*/
	
	int start, end, longestColumn;
	start = startvalue;
	end = totalsize;
	
	int average_R, average_G, average_B;
	average_R = average_G = average_B = 0;
	if (((end)-start) <= MaxElementCount)	//If end-start is below or equals to the MaxElementCount, get the colour palette.
	{
		for (int i = start; i<end; i++)
		{
		average_R += colour[i][0];
		average_G += colour[i][1];
		average_B += colour[i][2];
		}
		
		average_R /= ((end)-start);
		average_G /= ((end)-start);
		average_B /= ((end)-start);
		
		*counterNum_ptr += 1;
		printf("colour palette of index %d is generated\n",*counterNum_ptr);
		colour_palette[*counterNum_ptr][0] = average_R;
		colour_palette[*counterNum_ptr][1] = average_G;
		colour_palette[*counterNum_ptr][2] = average_B;
	} 
	
	else
	{
	//Introduce RGB arrays to store the colours. All of the array sizes should be w*h
	
	Uint32 *red, *green, *blue;
	
	red = malloc((totalsize)*sizeof(Uint32));
	
	if(red == NULL)
	{
		printf("Insufficient memory\n");
		exit(1);
	}
	
	green = malloc((totalsize)*sizeof(Uint32));
	
	if(green == NULL)
	{
		printf("Insufficient memory\n");
		exit(1);
	}
	
	blue = malloc((totalsize)*sizeof(Uint32));
	
	if(blue == NULL)
	{
		printf("Insufficient memory\n");
		exit(1);
	}
	
	//sort pixel array into individual colours
	//Do a quick sort of the RGB arrays
	sortColourRGB(colour, red, green, blue, end, start);

	//Get the longest axis of the RGB
	longestColumn = longest_axisRGB(red, green, blue, end, start);
	
	//Sort the colour array according to longest channel
	manual_qsort(colour, start, (end-1), longestColumn);

	//Free RGB
	free(red);
	free(green);
	free(blue);
	
	//Divide it into half and do a recursive function
	MedianCutAlgorithm(colour, colour_palette, start, (((end-start)/2)+start), MaxElementCount, counterNum_ptr);
	MedianCutAlgorithm(colour, colour_palette, (((end-start)/2)+start), end, MaxElementCount, counterNum_ptr);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Creating Working Functions
/////////////////////////////////////////////////////////////////////////////////////////////////
void ColourQuantization(SDL_Surface* QuantizedSurface, int w,int h, Uint32 * Quantized_Pixels, int colour_palette_no)
{
	/*Parameters are...
	SDL_Surface* QuantizedSurface:	The SDL Surface which contains the image to be quantized.
	int w:	The width of the image.
	int h:	The height of the image.
	Uint32 * Quantized_Pixels:	The pixels to be quantized.
	int colour_palette_no:	The number of colours that will result after the colour quantization.*/
	
	Uint32 **colour; //Creates 2D array pointers
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Get the RGB values of the image into an array
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Malloc 2D colour array
	colour = (Uint32 **) malloc(w*h*sizeof(Uint32*));
	for (int i = 0; i < (w*h); i++ )
	{
		colour[i] = (Uint32*) malloc(3*sizeof(Uint32));
	}
	
	if(colour == NULL)
	{
		free(colour);
		printf("Insufficient memory\n");
		exit(1);
	}
	
	for (int i = 0; i < (w*h); i++ )
	{
		if (colour[i] == NULL)
		{
			free(colour[i]);
			printf("Insufficient memory\n");
			exit(1);
		}
	}
	
	//Assigning the colour 2D array with the RGB coordinates of the picture
	
	int counting = 0;	// Variable to go through the colour array
	
	for (int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
		Uint8 r1,g1,b1;
		
		SDL_GetRGB(Quantized_Pixels[y*w + x], QuantizedSurface->format, &r1,&g1,&b1);
		colour[counting][0] = r1;
		colour[counting][1] = g1;
		colour[counting][2] = b1;
		counting++;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Introducing variables. The totalsize should depends on how many pixels that exist in the image.
	int totalsize = w*h;
	int longestColumn;
	//Create a counter to pass the average RGB values into the colour_palette array
	int counterNum = -1;
	int *counterNum_ptr = &counterNum;
	//Colour_palette_no refers to the maximum number of colours in the colour palette that should result.
	//colour_palette_no. It should be a power of 2
	
	//Counting how many elements should be in the final cut for the median cut algorithm to stop
	int MaxElementCount = totalsize/colour_palette_no;
	if (totalsize%colour_palette_no>0)
	{
		MaxElementCount = MaxElementCount+1;
	}
	
	//Create the array for the reduced colour palette
	Uint32 colour_palette[colour_palette_no][3];
	
	//Getting the reduced colour_palette using Median Cut Function
	MedianCutAlgorithm(colour, colour_palette, 0, totalsize, MaxElementCount, counterNum_ptr);
	
	//Assigning reduced colour_palette to image
	counting = 0;
	
	for (int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
		Uint8 r1,g1,b1;
		SDL_GetRGB(Quantized_Pixels[y*w + x], QuantizedSurface->format, &r1,&g1,&b1);
		
		int compare1, compare2, Euclidean_Distance, closest_colour_R, closest_colour_G, closest_colour_B;
		
			for(int z=0; z<(colour_palette_no); z++)	//Finding the closest colour_palette to the pixel
			{
			int a,b,c;
			
			a = (colour_palette[z][0] - r1)*(colour_palette[z][0] - r1);
			b = (colour_palette[z][1] - g1)*(colour_palette[z][1] - g1);
			c = (colour_palette[z][2] - b1)*(colour_palette[z][2] - b1);
			
			Euclidean_Distance = sqrt((a+b+c));

			if (z == 0)	//If it is the first loop
			{
				compare1 = Euclidean_Distance;
				closest_colour_R = colour_palette[z][0];
				closest_colour_G = colour_palette[z][1];
				closest_colour_B = colour_palette[z][2];
			}
			
			else
			{
				compare2 = Euclidean_Distance;
			
				if (compare2<compare1)
				{
				compare1 = compare2;
				closest_colour_R = colour_palette[z][0];
				closest_colour_G = colour_palette[z][1];
				closest_colour_B = colour_palette[z][2];
				}
			}
			}
		//Set colour of the pixel to the closest colour_palette
		Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,closest_colour_R,closest_colour_G,closest_colour_B);
		Quantized_Pixels[y*w+x] = pixel;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	for (int i=0; i<h*w; i++)
		{
		free(colour[i]);
		colour[i]=NULL;
		}
	free(colour);
	colour = NULL;
	counterNum_ptr = NULL;
}

void EdgeDetection(int h, int w, SDL_Surface* BluredSurface, Uint32 *pixels, SDL_Surface* GreaterBluredSurface, Uint32 *GBS_Pixels)
{
	/*Parameters are...
	int h:	The height of the image.
	int w:	The width of the image.
	SDL_Surface* BluredSurface:	The surface which contains the image for light convolution blurring as well as for the results of the edge detection to be mapped on.
	Uint32 *pixels:	The pixels to undergo light convolution blurring as well as for the results of the edge detection to be mapped on.
	SDL_Surface* GreaterBluredSurface:	The surface which contains the image for heavy convolution blurring.
	Uint32 *GBS_Pixels: The pixels to undergo heavy convolution blurring. */
	
	Uint32 *BlurValueArray, *HeavyBlurValueArray; //Creates array pointers.
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Light Convolution Blurring
	
	BlurValueArray = malloc(((h-1)*(w-1))*sizeof(Uint32));
	
	if(BlurValueArray == NULL)
	{
		printf("Insufficient memory\n");
		exit(1);
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Heavy Convolution Blurring
	HeavyBlurValueArray = malloc(((h-2)*(w-2))*sizeof(Uint32));
	
	if(HeavyBlurValueArray == NULL)
	{
		printf("Insufficient memory\n");
		exit(1);
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Light Convolution Blurring
	int counter = 0;
	for (int y=1; y<(h-1); y++)
	{
		for(int x=1; x<(w-1); x++)
		{
			//Filter starts here.
			Uint8 r1,r2,r3,r4,r5,r6,r7,r8,r9;
			Uint8 g1;
			Uint8 b1;
			
			SDL_GetRGB(pixels[(y-1)*w + (x-1)], BluredSurface->format, &r1,&g1,&b1);
			r1= r1*1/16;

			SDL_GetRGB(pixels[(y-1)*w + (x)], BluredSurface->format, &r2,&g1,&b1);
			r2= r2*2/16;
			
			SDL_GetRGB(pixels[(y-1)*w + (x+1)], BluredSurface->format, &r3,&g1,&b1);
			r3= r3*1/16;
			
			SDL_GetRGB(pixels[(y)*w + (x-1)], BluredSurface->format, &r4,&g1,&b1);
			r4= r4*2/16;
			
			SDL_GetRGB(pixels[(y)*w + (x)], BluredSurface->format, &r5,&g1,&b1);
			r5= r5*4/16;
			
			SDL_GetRGB(pixels[(y)*w + (x+1)], BluredSurface->format, &r6,&g1,&b1);
			r6= r6*2/16;
			
			SDL_GetRGB(pixels[(y+1)*w + (x-1)], BluredSurface->format, &r7,&g1,&b1);
			r7= r7*1/16;
			
			SDL_GetRGB(pixels[(y+1)*w + (x)], BluredSurface->format, &r8,&g1,&b1);
			r8= r8*2/16;
			
			SDL_GetRGB(pixels[(y+1)*w + (x+1)], BluredSurface->format, &r9,&g1,&b1);
			r9= r9*1/16;
			
			Uint32 BlurValue = r1+r2+r3+r4+r5+r6+r7+r8+r9;
			BlurValueArray[counter] = BlurValue;
			counter++;
		}
	}
	
	//Assign blured values
	counter = 0;
	for (int y=1; y<(h-1); y++)
	{
		for(int x=1; x<(w-1); x++)
		{
		Uint32 pixel = SDL_MapRGB(BluredSurface->format,BlurValueArray[counter],BlurValueArray[counter],BlurValueArray[counter]);
		pixels[y*w+x] = pixel;
		counter++;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Heavy Convolution Blurring
	counter = 0;
	
	for (int y=2; y<(h-2); y++)
	{
		for(int x=2; x<(w-2); x++)
		{
			//Filter starts here.
			Uint8 r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25;
			Uint8 g1;
			Uint8 b1;
			
			SDL_GetRGB(GBS_Pixels[(y-2)*w + (x-2)], GreaterBluredSurface->format, &r1,&g1,&b1);
			r1= r1*1/265;
			
			SDL_GetRGB(GBS_Pixels[(y-2)*w + (x-1)], GreaterBluredSurface->format, &r2,&g1,&b1);
			r2= r2*4/265;
			
			SDL_GetRGB(GBS_Pixels[(y-2)*w + (x)], GreaterBluredSurface->format, &r3,&g1,&b1);
			r3= r3*7/265;
			
			SDL_GetRGB(GBS_Pixels[(y-2)*w + (x+1)], GreaterBluredSurface->format, &r4,&g1,&b1);
			r4= r4*4/265;
			
			SDL_GetRGB(GBS_Pixels[(y-2)*w + (x+2)], GreaterBluredSurface->format, &r5,&g1,&b1);
			r5= r5*1/265;
			
			SDL_GetRGB(GBS_Pixels[(y-1)*w + (x-2)], GreaterBluredSurface->format, &r6,&g1,&b1);
			r6= r6*4/265;
			
			SDL_GetRGB(GBS_Pixels[(y-1)*w + (x-1)], GreaterBluredSurface->format, &r7,&g1,&b1);
			r7= r7*16/265;

			SDL_GetRGB(GBS_Pixels[(y-1)*w + (x)], GreaterBluredSurface->format, &r8,&g1,&b1);
			r8= r8*26/265;
			
			SDL_GetRGB(GBS_Pixels[(y-1)*w + (x+1)], GreaterBluredSurface->format, &r9,&g1,&b1);
			r9= r9*16/265;
			
			SDL_GetRGB(GBS_Pixels[(y-1)*w + (x+2)], GreaterBluredSurface->format, &r10,&g1,&b1);
			r10= r10*4/265;
			
			SDL_GetRGB(GBS_Pixels[(y)*w + (x-2)], GreaterBluredSurface->format, &r11,&g1,&b1);
			r11= r11*7/265;
			
			SDL_GetRGB(GBS_Pixels[(y)*w + (x-1)], GreaterBluredSurface->format, &r12,&g1,&b1);
			r12= r12*26/265;
			
			SDL_GetRGB(GBS_Pixels[(y)*w + (x)], GreaterBluredSurface->format, &r13,&g1,&b1);
			r13= r13*41/265;
			
			SDL_GetRGB(GBS_Pixels[(y)*w + (x+1)], GreaterBluredSurface->format, &r14,&g1,&b1);
			r14= r14*26/265;
			
			SDL_GetRGB(GBS_Pixels[(y)*w + (x+2)], GreaterBluredSurface->format, &r15,&g1,&b1);
			r15= r15*7/265;
			
			SDL_GetRGB(GBS_Pixels[(y+1)*w + (x-2)], GreaterBluredSurface->format, &r16,&g1,&b1);
			r16= r16*4/265;
			
			SDL_GetRGB(GBS_Pixels[(y+1)*w + (x-1)], GreaterBluredSurface->format, &r17,&g1,&b1);
			r17= r17*16/265;
			
			SDL_GetRGB(GBS_Pixels[(y+1)*w + (x)], GreaterBluredSurface->format, &r18,&g1,&b1);
			r18= r18*26/265;
			
			SDL_GetRGB(GBS_Pixels[(y+1)*w + (x+1)], GreaterBluredSurface->format, &r19,&g1,&b1);
			r19= r19*16/265;
			
			SDL_GetRGB(GBS_Pixels[(y+1)*w + (x+2)], GreaterBluredSurface->format, &r20,&g1,&b1);
			r20= r20*4/265;
			
			SDL_GetRGB(GBS_Pixels[(y+2)*w + (x-2)], GreaterBluredSurface->format, &r21,&g1,&b1);
			r21= r21*1/265;
			
			SDL_GetRGB(GBS_Pixels[(y+2)*w + (x-1)], GreaterBluredSurface->format, &r22,&g1,&b1);
			r22= r22*4/265;
			
			SDL_GetRGB(GBS_Pixels[(y+2)*w + (x)], GreaterBluredSurface->format, &r23,&g1,&b1);
			r23= r23*7/265;
			
			SDL_GetRGB(GBS_Pixels[(y+2)*w + (x+1)], GreaterBluredSurface->format, &r24,&g1,&b1);
			r24= r24*4/265;
			
			SDL_GetRGB(GBS_Pixels[(y+2)*w + (x+2)], GreaterBluredSurface->format, &r25,&g1,&b1);
			r25= r25*1/265;
			
			Uint32 HeavyBlurValue = r1+r2+r3+r4+r5+r6+r7+r8+r9+r10+r11+r12+r13+r14+r15+r16+r17+r18+r19+r20+r21+r22+r23+r24+r25;
			
			HeavyBlurValueArray[counter] = HeavyBlurValue;
			counter++;
		}
	}
	
	counter=0;
	for (int y=2; y<(h-2); y++)
	{
		for(int x=2; x<(w-2); x++)
		{
		Uint32 pixel = SDL_MapRGB(GreaterBluredSurface->format,HeavyBlurValueArray[counter],HeavyBlurValueArray[counter],HeavyBlurValueArray[counter]);
		GBS_Pixels[y*w+x] = pixel;
		counter++;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Subtract weaker gaussian from stronger gaussian to get edge detection
	for (int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
		pixels[y*w+x] = GBS_Pixels[y*w+x]-pixels[y*w+x];
		}
	}
	
	printf("Image is still working. Message 1/5\n");
	
	free(HeavyBlurValueArray);
	free(BlurValueArray);
}

void BenDay(int h, int w, SDL_Surface *QuantizedSurface, Uint32 * Quantized_Pixels, SDL_Surface *BenDaySurface, Uint32 * BenDay_Pixels)
{
	/*Parameters are...
	int h:	The height of the image.
	int w:	The width of the image.
	SDL_Surface *QuantizedSurface:	The surface which contains the image that is being edited on
	Uint32 * Quantized_Pixels:	The pixels of the image to be edited on
	SDL_Surface *BenDaySurface:	The surface which contains the image of the Ben Day Dots template
	Uint32 * BenDay_Pixels:	The pixels of the Ben Day Dots template. */
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Convert colours close to red/blue/yellow/black and white to respective colours
	
	for (int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
		Uint8 r1,g1,b1;
		SDL_GetRGB(Quantized_Pixels[y*w + x], QuantizedSurface->format, &r1,&g1,&b1);
		
		
		if (r1>150 && g1<50 && b1<50)	//Red
		{
		Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,200,0,0);
		Quantized_Pixels[y*w+x] = pixel;
		}
		
		if (r1<125 && g1<125 && b1>150)	//Blue
		{
		Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,0,40,170);
		Quantized_Pixels[y*w+x] = pixel;
		}
	
		if (r1>220 && g1>170 && b1<130)	//Yellow
		{
		Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,250,220,100);
		Quantized_Pixels[y*w+x] = pixel;
		}
		
		if (r1<100 && g1<100 && b1<100)	//Black
		{
		Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,0,0,0);
		Quantized_Pixels[y*w+x] = pixel;
		}
		
		if (r1>200 && g1>200 && b1>200)	//White
		{
		Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,255,255,255);
		Quantized_Pixels[y*w+x] = pixel;
		}
		}
	}
	
	printf("Image is still working. Message 2/5\n");
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Convert non red/blue/yellow/black and white colours to ben day templates
	for (int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
		Uint8 r1,g1,b1,r2,g2,b2;
		SDL_GetRGB(Quantized_Pixels[y*w + x], QuantizedSurface->format, &r1,&g1,&b1);
		SDL_GetRGB(BenDay_Pixels[y*w + x], BenDaySurface->format, &r2,&g2,&b2);
		
		if ((r1==200 && g1==0 && b1==0) || (r1==0 && g1==40 && b1==170) || (r1==250 && g1==220 && b1==100) || (r1==0 && g1==0 && b1==0) || (r1==255 && g1==255 && b1==255))
			{
			continue;
			} 
		
		else
			{
			//if darker than a certain colour
			if ((r1 + b1 + g1)<200)
				{
				if (r2!=255 && g2!=255 && b2!=255)	//if the ben day pixel is not white, change the pixel of the quantized image to white
					{
					Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,255,255,255);
					Quantized_Pixels[y*w+x] = pixel;
					}
				}
			
			else
				{
				if (r2==255 && g2==255 && b2==255)	//if the ben day pixel is white, change the pixel of the quantized image to white
					{
					Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,255,255,255);
					Quantized_Pixels[y*w+x] = pixel;
					}
				
				}
			}
		}
	
	}
	
	printf("Image is still working. Message 3/5\n");
}

void ThickenEdges(int h, int w, SDL_Surface *BluredSurface, Uint32 *pixels)
{
	/*Parameters are...
	int h:	The height of the image.
	int w:	The width of the image.
	SDL_Surface *BluredSurface:	The surface which contains the edge detection.
	Uint32 *pixels:	The pixels which contains the edge detection. */
	
	int *ThickenLinesArray = malloc(((h-1)*(w-1))*sizeof(int));
	
	if(ThickenLinesArray == NULL)
	{
		printf("Insufficient memory\n");
		exit(1);
	}
	
	int counter = 0;
	
	for (int y=1; y<h-1; y++)
	{
		for(int x=1; x<w-1; x++)
		{
		Uint8 r1,g1,b1;
		SDL_GetRGB(pixels[(y)*w + (x)], BluredSurface->format, &r1,&g1,&b1);
		if (r1==0 && g1==0 && b1==0)
			{
			int existed_element = 0;
			for(int z=0; z<counter; z++)
				{
				if (((y*w)+x) == ThickenLinesArray[z])
				existed_element = 1;
				break;
				}
			if (existed_element == 1)
				{
				continue;
				}
			

			ThickenLinesArray[counter] = ((y-1)*w)+(x-1);
			counter++;
			ThickenLinesArray[counter] = ((y-1)*w)+(x);
			counter++;
			ThickenLinesArray[counter] = ((y-1)*w)+(x+1);
			counter++;

			ThickenLinesArray[counter] = (y*w)+(x-1);
			counter++;
			ThickenLinesArray[counter] = (y*w)+x;
			counter++;
			ThickenLinesArray[counter] = (y*w)+(x+1);
			counter++;

			ThickenLinesArray[counter] = ((y+1)*w)+(x-1);
			counter++;
			ThickenLinesArray[counter] = ((y+1)*w)+(x);
			counter++;
			ThickenLinesArray[counter] = ((y+1)*w)+(x+1);
			counter++;
			
			}
		}
	}
	
	for (int x=0; x<counter; x++)
	{
		Uint32 pixel = SDL_MapRGB(BluredSurface->format,0,0,0);
		int number = ThickenLinesArray[x];
		pixels[number] = pixel;
	}
	
	printf("Image is still working. Message 4/5\n");
	free(ThickenLinesArray);
}

void CombineReplace(int h, int w, SDL_Surface *BluredSurface, Uint32 *pixels, SDL_Surface *QuantizedSurface, Uint32 *Quantized_Pixels)
{
	/*Parameters are...
	int h:	The height of the image.
	int w:	The width of the image.
	SDL_Surface *BluredSurface:	The surface which contains the edge detection.
	Uint32 *pixels:	The pixels which contains the edge detection. 
	SDL_Surface *QuantizedSurface:	The surface which contains the colour quantized and benday implemented image.
	Uint32 *Quantized_Pixels:	The pixels which are colour quantized and/or have the benday implemented. */
	
	//Method 1
	//Combining Edge detection and colour quantized image (Replacing)
	for (int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
		Uint8 r1,g1,b1;
		SDL_GetRGB(pixels[(y)*w + (x)], BluredSurface->format, &r1,&g1,&b1);
		if (r1>0 || g1>0 || b1>0)
			{
			continue;
			}
		else
			{
			Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,r1,g1,b1);
			Quantized_Pixels[y*w+x] = pixel;
			}
		
		}
	}
	printf("Image is still working. Message 5/5\n");
}

void CombineMultiply(int h, int w, SDL_Surface *BluredSurface, Uint32 *pixels, SDL_Surface *QuantizedSurface, Uint32 *Quantized_Pixels)
{
	/*Parameters are...
	int h:	The height of the image.
	int w:	The width of the image.
	SDL_Surface *BluredSurface:	The surface which contains the edge detection.
	Uint32 *pixels:	The pixels which contains the edge detection. 
	SDL_Surface *QuantizedSurface:	The surface which contains the colour quantized and benday implemented image.
	Uint32 *Quantized_Pixels:	The pixels which are colour quantized and/or have the benday implemented. */
	
	//Method2
	//Combining Edge detection and colour quantized image (Multiply)
	for (int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
		Uint8 r1,g1,b1,r2,g2,b2;
		float r3,g3,b3;
		SDL_GetRGB(pixels[(y)*w + (x)], BluredSurface->format, &r1,&g1,&b1);
		SDL_GetRGB(Quantized_Pixels[(y)*w + (x)], QuantizedSurface->format, &r2,&g2,&b2);
		
		r3 = (float)r1/255;
		g3 = (float)g1/255;
		b3 = (float)b1/255;
		
		r3 = (float)r2*r3;
		g3 = (float)g2*g3;
		b3 = (float)b2*b3;

		//Make sure division is equal to float value.
		//Multiply resulting value with r2,g2,b2
		
		Uint32 pixel = SDL_MapRGB(QuantizedSurface->format,(Uint8)r3,(Uint8)g3,(Uint8)b3);
		Quantized_Pixels[y*w+x] = pixel;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Initialising SDL Window, Renderer, Texture, Surfaces
/////////////////////////////////////////////////////////////////////////////////////////////////

int main (int argc, char*argv[])	//Command Line arguments
{
int ProgramReload = argc - 2;	//This variable is to check if there are more then one image loaded by the user. If yes, enables option to display next image
int Current_image = 2;	//This is the current image that is being displayed
do
{
	SDL_Window *window = NULL;	//Create the pointer WINDOW and make sure it has enough memory space
	SDL_Renderer *renderer = NULL;	//Create the pointer RENDERER and make sure it has enough memory space
	SDL_Surface *BluredSurface = NULL;	//Create the pointer BLUREDSURFACE and make sure it has enough memory space
	SDL_Surface *GreaterBluredSurface = NULL;	//Create the pointer GREATERBLUREDSURFACE and make sure it has enough memory space
	SDL_Surface *QuantizedSurface = NULL;	//Create the pointer QUANTIZEDSURFACE and make sure it has enough memory space
	SDL_Surface *BenDaySurface = NULL;	//Create the pointer BENDAYSURFACE and make sure it has enough memory space
	SDL_Surface *BenDayImage = NULL;	//Create the pointer BENDAYIMAGE and make sure it has enough memory space
	SDL_Surface *OriginalSurface = NULL;	//Create the pointer ORIGINALSURFACE and make sure it has enough memory space
	SDL_Surface *DisplayedImage = NULL;	//Create the pointer DISPLAYEDIMAGE and make sure it has enough memory space
	SDL_Texture *texture = NULL;	//Create the pointer TEXTURE and make sure it has enough memory space
	
	int w, h;	//Creates integer variables, width and height which will be used to set the size of the window
	
	//Check for command line
	//If there is less than 3 command line arguments, which means no arguments for pictures and/or ben day template, print error
	if (argc<3)
		{
		printf("ERROR\n");
		fprintf(stderr, "Usage should be: %s <ben_day_dot template> <image_file> ...\n\n", argv[0]);
		return (1);
		}

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)	//Initialise everything in SDL. If it is less than 0, there is an error
	{
		printf("Error in initialisation: %s\n",SDL_GetError());	//Prints out the error
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Creating SDL_Window, Rednerer, Texture, Surfaces
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Assign the window pointer above with created window.
	window = SDL_CreateWindow("BenDayDots",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w,h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL)	//If window fails to create, it will return a NULL
	{
		printf("Error in initialising window: %s\n",SDL_GetError());
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Assign the renderer pointer above with created renderer.
	renderer = SDL_CreateRenderer(window, -1, 0);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Assign the Surfaces with the image
	BluredSurface = IMG_Load(argv[Current_image]);
	GreaterBluredSurface = IMG_Load(argv[Current_image]);
	QuantizedSurface = IMG_Load(argv[Current_image]);
	BenDaySurface = IMG_Load(argv[Current_image]);	//Just to get the size to be the same format
	DisplayedImage = IMG_Load(argv[Current_image]);
	OriginalSurface = IMG_Load(argv[Current_image]);
	
	/*If the BluredSurface is not an image file, or the file directory is wrong, print an error. Since all the surface are the same image,
	checking one of them is enough.*/
	if (!BluredSurface) 
	{
            fprintf(stderr, "Couldn't load %s: %s\n", argv[Current_image], SDL_GetError());
            return 1;
    }
        
    //Assign the BenDayImage with the image to be loaded as seen in argv[1]
    BenDayImage = IMG_Load(argv[1]);
        
    //If the BenDayImage is not an image file, or the file directory is wrong, print an error
	if (!BenDayImage) 
	{
            fprintf(stderr, "Couldn't load %s: %s\n", argv[1], SDL_GetError());
            return 1;
    }
        
    /////////////////////////////////////////////////////////////////////////////////////////////////
    
    //Create a texture
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, BluredSurface -> w, BluredSurface ->h);
	//If the texture cannot be created, print error
	if (!texture) 
	{
            fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
            return 1;
    }
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Set window size according to size of image
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	SDL_SetWindowSize(window, w, h);
	
	//Blitscaling BenDayImage to the size of the image file
    SDL_Rect rect;
    rect.x = 0;
    rect.y =0;
    rect.w = w;
    rect.h = h;

    SDL_BlitScaled(BenDayImage, NULL, BenDaySurface, &rect);
    
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Convert existing surface to a new surface format.
	BluredSurface = SDL_ConvertSurfaceFormat(BluredSurface, SDL_PIXELFORMAT_ARGB8888,0);
	GreaterBluredSurface = SDL_ConvertSurfaceFormat(GreaterBluredSurface, SDL_PIXELFORMAT_ARGB8888,0);
	QuantizedSurface = SDL_ConvertSurfaceFormat(QuantizedSurface, SDL_PIXELFORMAT_ARGB8888,0);
	BenDaySurface = SDL_ConvertSurfaceFormat(BenDaySurface, SDL_PIXELFORMAT_ARGB8888,0);
	OriginalSurface = SDL_ConvertSurfaceFormat(OriginalSurface, SDL_PIXELFORMAT_ARGB8888,0);
	DisplayedImage = SDL_ConvertSurfaceFormat(DisplayedImage, SDL_PIXELFORMAT_ARGB8888,0);
	
	//Change the surface pixels from (void*)pixels to (Uint32*)pixels
	Uint32 * pixels = (Uint32 *) BluredSurface -> pixels;
	Uint32 * GBS_Pixels = (Uint32 *) GreaterBluredSurface -> pixels;
	Uint32 * Quantized_Pixels = (Uint32 *) QuantizedSurface -> pixels;
	Uint32 * BenDay_Pixels = (Uint32 *) BenDaySurface -> pixels;
	Uint32 * Original_Pixels = (Uint32 *) OriginalSurface-> pixels;
	Uint32 * Displayed_Pixels = (Uint32 *) DisplayedImage -> pixels;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Here is where all the main code is runned*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	int isRunning = 1;	//Creates a boolean for the window loop
	SDL_Event ev;	// Creates the SDL Event
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Reducing colour palette of the image (Median Cut Colour Quantization)
	/////////////////////////////////////////////////////////////////////////////////////////////////
	ColourQuantization(QuantizedSurface, w, h, Quantized_Pixels,16);	//Last argument is the colour_palette no. It should be a power of 2.
	printf("\n\n");
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Creating Edge Detection (Convolution Blurring)
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Set colours of BluredSurface and GreaterBluredSurface to that of a colour palette of 2
	ColourQuantization(BluredSurface, w, h, pixels,2);	//Last argument is the colour_palette no. It should be a power of 2.
	
	for (int y = 0; y< h ;y++)	//Change the GreaterBluredSurface pixels to be the same as the BluredSurface pixels
	{
		for(int x = 0; x< w ; x++)
		{	
			GBS_Pixels[y*w + x] = pixels[y*w + x];
		}			
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Grayscale code starts here
	//Changes BluredSurface and GreaterBluredSurface to grey.
	for (int y = 0; y< h ;y++)
	{
		for(int x = 0; x< w ; x++)
		{	
			//Grayscale function comes here
			Uint32 pixel = pixels[y*w + x];
			pixels[y*w + x] = CovertGrayscale(pixel);
			GBS_Pixels[y*w + x] = pixels[y*w + x];
		}			
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//EdgeDetection
	EdgeDetection(h, w, BluredSurface, pixels, GreaterBluredSurface, GBS_Pixels);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Replacing GBS_Pixels with Quantized_Pixels to store the results before we BenDay the image for easier viewing
	for (int y = 0; y< h ;y++)
	{
		for(int x = 0; x< w ; x++)
		{	
			GBS_Pixels[y*w + x] = Quantized_Pixels[y*w + x];
		}			
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Convert some colours to red/blue/yellow/black/white when appropriate and others to ben day dots template
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	BenDay(h, w, QuantizedSurface, Quantized_Pixels, BenDaySurface, BenDay_Pixels);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Combining edges from edge detection to the image.
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	ThickenEdges(h, w, BluredSurface, pixels);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Combining Edge detection and colour quantized image
	//Method 1
	CombineReplace(h, w, BluredSurface, pixels, QuantizedSurface, Quantized_Pixels);
	
	//Method2
	//CombineMultiply(h, w, BluredSurface, pixels, QuantizedSurface, Quantized_Pixels);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	int loadinglog = 0;
	int instructionslog = 0;

	while(isRunning)	//While True, keep doing the loop
	{	

		SDL_UpdateTexture(texture, NULL, DisplayedImage -> pixels, DisplayedImage -> w * sizeof(Uint32));
		
		char decision[2];
		char savename[200];
		
		//Put all the events into a queue. PollEvent process the queue and once it is done, it gets rid of the event
		while(SDL_PollEvent(&ev))
		{
			switch (ev.type)	//Checks the type of event
			
				case SDL_QUIT:	//If the event type is to QUIT or (ev.type == SDL_QUIT)
				{
					isRunning = 0;
					ProgramReload = 0;
					break;
				}
				
				if(ev.type == SDL_KEYUP)
					switch (ev.key.keysym.sym)
					{
						case SDLK_q:    //When user presses q, it displays the original image
							for (int y = 0; y< h ;y++)
							{
								for(int x = 0; x< w ; x++)
								{	
									Displayed_Pixels[y*w + x] = Original_Pixels[y*w + x];
								}			
							}
							break;
								
						case SDLK_w:    //When user presses w, it displays the BenDay Image
							for (int y = 0; y< h ;y++)
							{
								for(int x = 0; x< w ; x++)
								{	
									Displayed_Pixels[y*w + x] = Quantized_Pixels[y*w + x];
								}			
							}
							break;	
							
						case SDLK_e:    //When user presses e, it displays the Quantized Surface without BenDay or edge
							for (int y = 0; y< h ;y++)
							{
								for(int x = 0; x< w ; x++)
								{	
									Displayed_Pixels[y*w + x] = GBS_Pixels[y*w + x];
								}			
							}
							break;
							
						case SDLK_r:    //When user presses e, it displays the Edge Detection
							for (int y = 0; y< h ;y++)
							{
								for(int x = 0; x< w ; x++)
								{	
									Displayed_Pixels[y*w + x] = pixels[y*w + x];
								}			
							}
							break;
						
						case SDLK_s:    //When user presses s, he can save the image
							printf("/////////////////////////////////////////////////////////////////////////////////////////////////");
							printf("Please type the filename you want the save file to be.\n");
							scanf("%s",savename);
							fflush(stdin);
							if (IMG_SavePNG(DisplayedImage, savename) < 0)
							{
								fprintf(stderr,"Saving has failed: %s\n", SDL_GetError());
							}
							else
							{
								printf("Image has been saved\n");
							}
							instructionslog = 0;
							SDL_FlushEvent(SDL_KEYUP);
							break;
						
						case SDLK_d:    //When user presses d, he can choose to display the next image provided the user provides more than one image
							if (ProgramReload>1)
							{
								while(loadinglog==0)
								{
								printf("/////////////////////////////////////////////////////////////////////////////////////////////////");
								printf("\nWould you like to load the next image?\nIf yes -> press y\nIf no -> press n\n");
								scanf("%s", decision);
								fflush(stdin);
								
									if (*decision == 'y' || *decision == 'Y')
									{
										loadinglog = 1;
										isRunning = 0;
										break;
									}
									
									if (*decision == 'n' || *decision == 'N')
									{
										loadinglog = 1;
										ProgramReload = 0;
										break;
									}
									
									else
									{
										printf("PLEASE INPUT EITHER y OR n\n");
										continue;
									}
								}
								instructionslog = 0;
								SDL_FlushEvent(SDL_KEYUP);
							}
							break;
					}
		}
		
		SDL_RenderClear(renderer);
		//Take texture and draw onto render target
		SDL_RenderCopy(renderer,texture,NULL,NULL);
		//Update the screen with any rendering performed since the previous call
		SDL_RenderPresent(renderer);
		
		//Printing out instructions for the user
		if (instructionslog==0)
		{
			printf("//////////////////////////////////////////////////////////////////////////\n");
			printf("User guide\nPress q -> Original Image\nPress w -> BenDay Image\nPress e -> Quantized Colour Image\nPress r -> Edge Detection\n");
			printf("Press s -> Save Currently displayed image\nPress d ->Choose to load next image or not if you have more than one image loaded.\n");
			printf("//////////////////////////////////////////////////////////////////////////\n");
			instructionslog = 1;
		}
		//Delay 100 milliseconds before it returns
		SDL_Delay(100);

	}

	
	SDL_DestroyWindow(window);	//Destroy and free the memory space used to create the Window
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(BluredSurface);
	SDL_FreeSurface(GreaterBluredSurface);
	SDL_FreeSurface(QuantizedSurface);
	SDL_FreeSurface(BenDaySurface);
	SDL_FreeSurface(BenDayImage);
	SDL_FreeSurface(OriginalSurface);
	SDL_FreeSurface(DisplayedImage);
	
	window = NULL;
	renderer = NULL;
	BluredSurface = NULL;
	GreaterBluredSurface = NULL;
	QuantizedSurface = NULL;
	BenDaySurface = NULL;
	BenDayImage = NULL;
	texture = NULL;
	
	SDL_Quit();
	ProgramReload --;
	Current_image ++;
}while(ProgramReload>0);
	return 0;
}
