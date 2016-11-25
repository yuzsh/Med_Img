#pragma warning(disable:4996)

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	char infile[256], outfile[256];
	int mszx, mszy, bit_allocated;
	short *outputimg;
	char tmp, *byte0, *byte1;
	int i, j;
	FILE *fp;
	int flag1, flag2, flag3, flag4;
	unsigned char buf[4], gTag1, gTag2, eTag1, eTag2;
	long dLen;

	if (argc != 3)
	{
		printf("Usage: %s (input dicom file) (output file)\n", argv[0]);
		exit(0);
	}

	strcpy(infile, argv[1]);
	strcpy(outfile, argv[2]);

	printf("INPUT DICOM FILE = %s \n", infile);
	printf("OUTPUT RAW IMAGE FILE = %s \n", outfile);

	if ((fp = fopen(infile, "rb")) == NULL)
	{
		printf("OPEN FAILED %s\n", infile);
		exit(0);
	}

	flag1 = flag2 = flag3 = flag4 = 0;

	while (flag1 == 0 || flag2 == 0 || flag3 == 0 || flag4 == 0)
	{
		fread(buf, sizeof(unsigned char), 1, fp);
		gTag1 = *(unsigned char*)buf;

		// group number 0x0028
		if (gTag1 == 0x28)
		{
			fread(buf, sizeof(unsigned char), 1, fp);
			gTag2 = *(unsigned char*)buf;
			if (gTag2 == 0x00)
			{
				fread(buf, sizeof(unsigned char), 1, fp);
				eTag1 = *(unsigned char*)buf;

				// element number 0x0010 -> image rows
				if (eTag1 == 0x10)
				{
					fread(buf, sizeof(unsigned char), 1, fp);
					eTag2 = *(unsigned char*)buf;
					if (eTag2 == 0x00)
					{
						fread(buf, sizeof(unsigned char), 4, fp);
						dLen = *(long*)buf;
						fread(buf, sizeof(unsigned char), dLen, fp);
						mszy = *(int*)buf;
						flag1 = 1;
					}
				}

				// element number 0x0011 -> image cols
				else if (eTag1 == 0x11)
				{
					fread(buf, sizeof(unsigned char), 1, fp);
					eTag2 = *(unsigned char*)buf;
					if (eTag2 == 0x00)
					{
						fread(buf, sizeof(unsigned char), 4, fp);
						dLen = *(long*)buf;
						fread(buf, sizeof(unsigned char), dLen, fp);
						mszx = *(int*)buf;
						flag2 = 1;
					}
				}

				// element number 0x0100 -> bit number of allocating pixel value
				else if (eTag1 == 0x00)
				{
					fread(buf, sizeof(unsigned char), 1, fp);
					eTag2 = *(unsigned char*)buf;
					if (eTag2 == 0x01)
					{
						fread(buf, sizeof(unsigned char), 4, fp);
						dLen = *(long*)buf;
						fread(buf, sizeof(unsigned char), dLen, fp);
						bit_allocated = *(int*)buf;
						flag3 = 1;
					}
				}
			}
		}

		// group number 0x7FE0
		else if (gTag1 == 0xE0)
		{
			fread(buf, sizeof(unsigned char), 1, fp);
			gTag2 = *(unsigned char*)buf;
			if (gTag2 == 0x7F)
			{
				fread(buf, sizeof(char), 1, fp);
				eTag1 = *(unsigned char*)buf;

				// element number 0x0010 -> pixel data
				if (eTag1 == 0x10)
				{
					fread(buf, sizeof(unsigned char), 1, fp);
					eTag2 = *(unsigned char*)buf;
					if (eTag2 == 0x00)
					{
						fread(buf, sizeof(unsigned char), 4, fp);
						dLen = *(long*)buf;
						outputimg = (short*)malloc(dLen);
						fread(outputimg, 1, dLen, fp);
						flag4 = 1;
					}
				}
			}
		}
	}

	fclose(fp);

	// output header info
	printf("Rows           = %d \n", mszy);
	printf("Columns        = %d \n", mszx);
	printf("Bit allocated  = %d \n", bit_allocated);

	if ((fp = fopen(outfile, "wb")) == NULL)
	{
		printf("OPEN FAILED %s \n", outfile);
		exit(0);
	}

	fwrite(outputimg, sizeof(short), mszx*mszy, fp);
	fclose(fp);

	free(outputimg);

	return 0;
}