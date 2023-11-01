#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <math.h>
#include <stdio.h>
#include <float.h>


void downsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out); 
void upsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out); 
void bilinear (unsigned char* in, int const height, int const width, int const channel, unsigned char* out); 
double imagePSNR(unsigned char* frame1, unsigned char* frame2, unsigned int size);

int main()
{
 
	int height;
	int width;
	int channel;

    	float PSNR_up,PSNR_bilinear;

	char command;

 	printf("Take a picture? (y/n)\n");
 	scanf("%c", &command);

 	if(command == 'n')
 		exit(1);


 	printf("Cheeze !\r\n");
	system("raspistill -w 960 -h 720 -t 10 -o src.bmp");

 	unsigned char* imgIn = stbi_load("src.bmp", &width, &height, &channel, 3);
	unsigned char* imgOut_down0 = (unsigned char*) malloc (sizeof(unsigned char)*3*320*240);
	unsigned char* imgOut_up0 = (unsigned char*) malloc (sizeof(unsigned char)*3*960*720);
	unsigned char* imgOut_up1 = (unsigned char*) malloc (sizeof(unsigned char)*3*960*720);
 	
	downsampling(imgIn, height, width, channel, imgOut_down0);
	upsampling(imgOut_down0, height, width, channel, imgOut_up0);
	bilinear(imgOut_down0, height, width, channel, imgOut_up1);
	PSNR_up = imagePSNR(imgIn,imgOut_up0, 3*960*720);
	PSNR_bilinear = imagePSNR(imgIn,imgOut_up1, 3*960*720);
    	printf("%f, %f\n",PSNR_up,PSNR_bilinear);
				
	stbi_write_bmp("image_down.bmp", width/3, height/3, channel, imgOut_down0);
	stbi_write_bmp("image_up.bmp", width, height, channel, imgOut_up0);
	stbi_write_bmp("image_bi.bmp", width, height, channel, imgOut_up1);
	
	stbi_image_free(imgIn);
	free(imgOut_down0);
	free(imgOut_up0);
	free(imgOut_up1);
 

	return 0;
}

void downsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) { //0,0 position
	int x, y, c;

	for(y=0; y<height; y=y+3)
		for(x=0; x<width; x=x+3)
			for(c=0; c<channel; c++)
                out[(y/3)*320*3 + x/3*3 + c]=in[y*960*3 + x*3 + c];
}

void upsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) { //same value
    int x,y,c;
	
    for(y=0; y<height; y=y+3)
		for(x=0; x<width; x=x+3)
			for(c=0; c<channel; c++)
            {
            out[(y)*960*3 + (x)*3 + c] = in[(y/3)*320*3 + x/3*3 + c];
            out[(y)*960*3 + (x+1)*3 + c] = in[(y/3)*320*3 + x/3*3 + c];
            out[(y)*960*3 + (x+2)*3 + c] = in[(y/3)*320*3 + x/3*3 + c];
            out[(y+1)*960*3 + (x)*3 + c] = in[(y/3)*320*3 + x/3*3 + c];
            out[(y+1)*960*3 + (x+1)*3 + c] = in[(y/3)*320*3 + x/3*3 + c];
            out[(y+1)*960*3 + (x+2)*3 + c] = in[(y/3)*320*3 + x/3*3 + c];
            out[(y+2)*960*3 + (x)*3 + c] = in[(y/3)*320*3 + x/3*3 + c];
            out[(y+2)*960*3 + (x+1)*3 + c] = in[(y/3)*320*3 + x/3*3 + c];
            out[(y+2)*960*3 + (x+2)*3 + c] = in[(y/3)*320*3 + x/3*3 + c];
            }
}

void bilinear (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) { //bilinear
    int x,y,c;

    unsigned char A, B, C, D;
    unsigned char A00, A01, A02,
    			  A10, A11, A12,
    			  A20, A21, A22,
    			  B10, B20;
	
    for(y=0; y<height; y=y+3)
		for(x=0; x<width; x=x+3)
			for(c=0; c<channel; c++)
            {

            	A = in[(y/3)*320*3 	 + (x/3)*3   + c];

            	if(x+3 == width){
            		B = 0;
            		D = 0;
            	}
            	else{
            		B = in[(y/3)*320*3 	 + (x/3+1)*3 + c];
            		D = in[(y/3+1)*320*3 + (x/3+1)*3 + c];
            	}
            		
            	
            	if(y+3 == height){
            		C = 0;
            		D = 0;
            	}
            	else{
            		C = in[(y/3+1)*320*3 + (x/3)*3   + c];
            		D = in[(y/3+1)*320*3 + (x/3+1)*3 + c];
            	}

            	B10 = (2*B + 1*D)/3;
            	B20 = (1*B + 2*D)/3;

            	A00 = (3*A + 0*B)/3;
            	A01 = (2*A + 1*B)/3;
            	A02 = (1*A + 2*B)/3;

            	A10 = (2*A   + 1*C)/3;
            	A11 = (2*A10 + 1*B10)/3;
            	A12 = (1*A10 + 2*B10)/3;

            	A20 = (1*A   + 2*C)/3;
            	A21 = (2*A20 + 1*B20)/3;
            	A22 = (1*A20 + 2*B20)/3;

            	if(x+3 == width && y+3 == height){
					out[(y+0)*960*3 + (x+0)*3 + c] = A00;
		            out[(y+0)*960*3 + (x+1)*3 + c] = A00;
		            out[(y+0)*960*3 + (x+2)*3 + c] = A00;
		            out[(y+1)*960*3 + (x+0)*3 + c] = A00;
		            out[(y+1)*960*3 + (x+1)*3 + c] = A00;
		            out[(y+1)*960*3 + (x+2)*3 + c] = A00;
		            out[(y+2)*960*3 + (x+0)*3 + c] = A00;
		            out[(y+2)*960*3 + (x+1)*3 + c] = A00;
		            out[(y+2)*960*3 + (x+2)*3 + c] = A00;
            	}
	            else if(x+3 == width){
					out[(y+0)*960*3 + (x+0)*3 + c] = A00;
		            out[(y+0)*960*3 + (x+1)*3 + c] = A00;
		            out[(y+0)*960*3 + (x+2)*3 + c] = A00;
		            out[(y+1)*960*3 + (x+0)*3 + c] = A10;
		            out[(y+1)*960*3 + (x+1)*3 + c] = A10;
		            out[(y+1)*960*3 + (x+2)*3 + c] = A10;
		            out[(y+2)*960*3 + (x+0)*3 + c] = A20;
		            out[(y+2)*960*3 + (x+1)*3 + c] = A20;
		            out[(y+2)*960*3 + (x+2)*3 + c] = A20;
            	}
            	else if(y+3 == height){
					out[(y+0)*960*3 + (x+0)*3 + c] = A00;
		            out[(y+0)*960*3 + (x+1)*3 + c] = A01;
		            out[(y+0)*960*3 + (x+2)*3 + c] = A02;
		            out[(y+1)*960*3 + (x+0)*3 + c] = A00;
		            out[(y+1)*960*3 + (x+1)*3 + c] = A01;
		            out[(y+1)*960*3 + (x+2)*3 + c] = A02;
		            out[(y+2)*960*3 + (x+0)*3 + c] = A00;
		            out[(y+2)*960*3 + (x+1)*3 + c] = A01;
		            out[(y+2)*960*3 + (x+2)*3 + c] = A02;
            	}
            	else{
					out[(y+0)*960*3 + (x+0)*3 + c] = A00;
		            out[(y+0)*960*3 + (x+1)*3 + c] = A01;
		            out[(y+0)*960*3 + (x+2)*3 + c] = A02;
		            out[(y+1)*960*3 + (x+0)*3 + c] = A10;
		            out[(y+1)*960*3 + (x+1)*3 + c] = A11;
		            out[(y+1)*960*3 + (x+2)*3 + c] = A12;
		            out[(y+2)*960*3 + (x+0)*3 + c] = A20;
		            out[(y+2)*960*3 + (x+1)*3 + c] = A21;
		            out[(y+2)*960*3 + (x+2)*3 + c] = A22;
            	}
            }
}

//Calculates image PSNR value
double imagePSNR(unsigned char* frame1, unsigned char* frame2, unsigned int size)
{  
    double MSE=0.0;
    double MSEtemp=0.0;
    double psnr=0.0;
    unsigned int index;
 
    //Calculate MSE
    for(index=0;index<size;index++)
    {
        MSEtemp=abs(frame1[index]-frame2[index]);
        MSE+=MSEtemp*MSEtemp;
    }
    MSE/=size;
 
    //Avoid division by zero
    if(MSE==0) return 99.0;
 
    psnr=10*log10(255*255/MSE);
 
    return psnr;
}
