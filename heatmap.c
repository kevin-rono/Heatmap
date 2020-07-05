#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "track.h"
#include "trackpoint.h"
#include "location.h"

#define INITIAL_CAPACITY 30


int main(int argc, char **argv)
{
    /* argc should be = 5 for correct execution */
    if ( argc != 5 ) 
    {
        return 1;
    }

    // set values
    double cell_width = atof(argv[1]);
    double cell_height = atof(argv[2]);

    char *heatmap_characters = argv[3];

    double lat, lon;
    long time;

    int range = atoi(argv[4]);

    // make track
    track *my_trk = track_create();

    // to use for getchar
    int ch;                                                                                               

    while((ch = getchar()) != EOF)
    {
        if (ch == '\n')
        {
            track_start_segment(my_trk);
        }
        else
        {
            ungetc(ch, stdin);
            scanf("%lf %lf %ld", &lat, &lon, &time);
        }
        //create trkpt
        trackpoint *my_trkpt = trackpoint_create(lat, lon, time); 

        track_add_point(my_trk, my_trkpt);
        
        trackpoint_destroy(my_trkpt);   
    }

    // create heatmap
    int **map;

    int rows, cols;

    track_heatmap(my_trk, cell_width, cell_height, &map, &rows, &cols);

    int index;
    int max_index = strlen(heatmap_characters)-1;

    // for each row
    for (int i=0; i<rows; i++)
    {
        // for each col
        for (int j=0; j<cols; j++)
        {
            // find the index of the num of trkpts in each cell in the array of heatmap characters
            index = (int) floor(map[i][j]/range);

            // match the cell values to the characters
            map[i][j] = heatmap_characters[index];
            if (index > max_index)
            {
              putchar(heatmap_characters[max_index]);
            }
            else
            {
              printf("%c", map[i][j]);
            }
        }
        printf("\n");
    }

    track_destroy(my_trk);

    for (int i = 0; i < rows; i++) 
    {
      free(map[i]);
    }
    free(map);
}
