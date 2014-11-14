/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */
 
#include <stdio.h>
#include <stdint.h>
 
typedef uint8_t BYTE;

int main(void)
{
    // open input file and if null, return 1
    FILE* infile = fopen("card.raw", "r");
    if (infile == NULL)
    {
        printf("File could not be opened"); 
        return 1;
    }
    
    // declare outfile and set to NULL
    FILE* outfile = NULL;
    
    // keep track of images found
    int image_count = 0; 
          
    // loop until eof
    while (!feof(infile))
    {
        char image_name[8];
        
        // create a buffer size of FAT block
        BYTE buffer[512];
        
        // test for JPEG signature
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff &&
            (buffer[3] == 0xe0 || buffer[3] == 0xe1))
        {
            // if JPEG signature found, close previous outfile
            if (outfile != NULL)
            {
                fclose(outfile);
            }
            
            // name new JPEG file
            sprintf(image_name, "%03d.jpg", image_count);
            
            // open outfile for new JPEG
            outfile = fopen(image_name, "w");
            
            // increment counter
            image_count++;
            
            // write buffer to new outfile
            fwrite(buffer, sizeof(buffer), 1, outfile);
        }
        else
        {
            // write buffer to current file
            if (image_count > 0)
            {
               fwrite(buffer, sizeof(buffer), 1, outfile);
            }
        }
        
        // read into buffer from infile 512 bytes
        fread(buffer, sizeof(buffer), 1, infile);
      }
      
      // close last file      
      if (image_count > 0)
      {
            fclose(outfile);
      }
      
      // close infile  
      fclose(infile);
       
      // all's well 
      return 0;
       
}
