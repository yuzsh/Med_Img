#pragma warning(disable:4996)

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	char infile[256], outfile[256];
	int mszx, mszy;
	short *inputimg, *outputimg;
	char tmp, *byte0, *byte1;
	int i;
	FILE *fp;

	if (argc != 5)
	{
		printf("Usage: %s (input file) (size X) (size Y) (output file)\n", argv[0]);
		exit(0);
	}

	strcpy(infile, argv[1]);
	mszx = atoi(argv[2]);
	mszy = atoi(argv[3]);
	strcpy(outfile, argv[4]);

	printf("INPUT IMAGE FILE = %s \n", infile);
	printf("OUTPUT IMAGE FILE = %s \n", outfile);
	printf("SIZE X = %d \n", mszx);
	printf("SIZE Y = %d \n", mszy);

	inputimg = (short*)malloc(mszx*mszy*sizeof(short));
	outputimg = (short*)malloc(mszx*mszy*sizeof(short));
	if (inputimg == NULL || outputimg == NULL)
	{
		printf("COULD NOT BE ALLOCATED\n");
		exit(0);
	}

	if ((fp = fopen(infile, "rb")) == NULL)
	{
		printf("OPEN FAILED %s\n", infile);
		exit(0);
	}

	fread(inputimg, sizeof(short), mszx*mszy, fp);

	fclose(fp);

	// byte swap
	for (i = 0; i < mszx*mszy; i++)
	{
		byte0 = (char*)&inputimg[i];
		byte1 = (char*)&inputimg[i] + 1;

		tmp = *byte0;
		*byte0 = *byte1;
		*byte1 = tmp;
		outputimg[i] = inputimg[i];

		//outputimg[i] = 4095-outputimg[i];
	}

	if ((fp = fopen(outfile, "wb")) == NULL)
	{
		printf("OPEN FAILED %s\n", outfile);
		exit(0);
	}

	fwrite(outputimg, sizeof(short), mszx*mszy, fp);

	fclose(fp);

	free(inputimg);
	free(outputimg);

	return 0;
}