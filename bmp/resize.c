/**
 * resize.c
 *
 * Robert Hill
 * CS50, Pset5
 *
 * Program that resizes a bit map image.
 */

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4 || atoi(argv[1]) < 1 || atoi(argv[1]) > 100)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
    
    // remember resize factor
    int n = atoi(argv[1]);
       
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    
    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // remember infile height and width
    LONG inwidth = bi.biWidth;
    LONG inheight = bi.biHeight;  
    
    // resize the height and width in BITMAPINFOHEADER by factor provided by 
    // user
    bi.biWidth *= n;
    bi.biHeight *= n; 
    
    // calculate padding of infile and resized outfile
    int inpad =  (4 - (inwidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int outpad = (4 - (bi.biWidth * sizeof(RGBTRIPLE) % 4)) % 4;
    
    // resize biSizeImage field for outfile's BITMAPINFOHEADER
    bi.biSizeImage = ((bi.biWidth * sizeof(RGBTRIPLE) + outpad) * 
        abs(bi.biHeight));
      
    // resize bfSize field for outfile's BITMAPFILEHEADER
    bf.bfSize = bf.bfOffBits + bi.biSizeImage;
        
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
            
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // set buffer for scanline
    RGBTRIPLE *buffer = malloc(sizeof(RGBTRIPLE) * bi.biWidth);
    
    // iterate over infile's scanlines
    for (int i = 0, height = abs(inheight); i < height; i++)
    {
        int count = 0;
        
        // iterate over pixels in scanline
        for (int j = 0; j < inwidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            // resize and buffer
            for (int k = 0; k < n; k++)
            {
                buffer[count] = triple;
                count++;
            }
         }

        // skip over padding, if any
        fseek(inptr, inpad, SEEK_CUR);
        
        // write outfile's scanlines
        for (int l = 0; l < n; l++)
        {
            fwrite((buffer), sizeof(RGBTRIPLE), bi.biWidth, outptr);
            
            // add padding, if any
            for (int m  = 0; m < outpad; m++)
            {
                fputc(0x00, outptr);
            }
        }
    }
    
    // free malloc
    free(buffer);
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);
    
    // that's all folks
    return 0;
}
