#pragma warning(disable:4996)

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	char infile[256], outfile[256];
	int mszx, mszy, outmszx, outmszy;
	short *inputimg, *outputimg;
	int i, j, intX, intY;
	float ratiox, ratioy, floatX, floatY, deltaX, deltaY;
	FILE *fp;
	int flag;

	if (argc != 8)
	{
		printf("Usage: %s (input file) (output file) (size X) (size Y) (X-ratio) (Y-ratio) (flag - 0 : nearest neighbor, 1 : linear interpolation)\n", argv[0]);
		exit(0);
	}

	strcpy(infile, argv[1]);
	strcpy(outfile, argv[2]);
	mszx = atoi(argv[3]);
	mszy = atoi(argv[4]);
	ratiox = atof(argv[5]);
	ratioy = atof(argv[6]);
	flag = atoi(argv[7]);

	outmszx = (int)(ratiox*(float)mszx + 0.5);
	outmszy = (int)(ratioy*(float)mszy + 0.5);

	printf("INPUT IMAGE FILE = %s \n", infile);
	printf("OUTPUT IMAGE FILE = %s \n", outfile);
	printf("SIZE X = %d \n", mszx);
	printf("SIZE Y = %d \n", mszy);
	printf("OUTPUT SIZE X = %d\n", outmszx);
	printf("OUTPUT SIZE Y = %d\n", outmszy);
	if (flag == 0) printf("interpolation : nearest neighbor \n");
	else if (flag == 1) printf("interpolation : linear interpolation \n");

	inputimg = (short*)malloc(mszx*mszy*sizeof(short));
	outputimg = (short*)malloc(outmszx*outmszy*sizeof(short));

	if ((fp = fopen(infile, "rb")) == NULL)
	{
		printf("OPEN FAILED %s\n", infile);
		exit(0);
	}

	fread(inputimg, sizeof(short), mszx*mszy, fp);
	fclose(fp);

	// Image Processing

	if (flag == 0)
	{
		for (j = 0; j < outmszy; j++)
		{
			floatY = (float)j / ratioy;
			intY = (int)(floatY + 0.5);
			for (i = 0; i < outmszx; i++)
			{
				floatX = (float)i / ratiox;
				intX = (int)(floatX + 0.5);
				if (intX >= 0 && intX < mszx && intY >= 0 && intY < mszy)
				{
					outputimg[j * outmszx + i] = inputimg[intY * mszx + intY];
				}
				else
				{
					outputimg[j * outmszx + i] = 0;
				}
			}
		}
	}

	else if (flag == 1)
	{
		for (j = 0; j < outmszy; j++)
		{
			floatY = (float)j / ratioy;
			intY = (int)(floatY + 0.5);
			deltaY = floatY - (float)intY;
			for (i = 0; i < outmszx; i++)
			{
				floatX = (float)i / ratiox;
				intX = (int)(floatX + 0.5);
				deltaX = floatX - (float)intX;
				if (floatX >= 0.0 && intX - 1 < mszx && floatY >= 0.0 && intY - 1 < mszy)
				{
					outputimg[j * outmszx + i] = (short)(
						(float)inputimg[intY * mszx + intX] * (1.0 - deltaX) * (1.0 - deltaY)
						+ (float)inputimg[intY * mszx + intX + 1] * deltaX * (1 - deltaY)
						+ (float)inputimg[(intY + 1) * mszx + intX] * (1.0 - deltaX) * deltaY
						+ (float)inputimg[(intY + 1) * mszx + intX + 1] * deltaX * deltaY
						+ 0.5
						);
				}
				else
					outputimg[j * outmszx + i] = 0;
			}
		}
	}

	if ((fp = fopen(outfile, "wb")) == NULL)
	{
		printf("OPEN FAILED %s\n", outfile);
		exit(0);
	}

	fwrite(outputimg, sizeof(short), outmszx*outmszy, fp);
	fclose(fp);

	free(inputimg);
	free(outputimg);

	return 0;
}