#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "track.h"

typedef struct segment
{
    int count;
    int capacity;
    double length;
    trackpoint **trkpt;
} segment;

struct track
{
    segment *segments;
    int count;
    int capacity;
};

/**
 * Resized the trkpt array by doubling its size.  There is no effect if
 * there is a memory allocation error.
 *
 * @param trk a pointer to a valid track
 */
void track_trkpt_embiggen(track *trk, int i);
/**
 * Resized the segment array by doubling its size.  There is no effect if
 * there is a memory allocation error.
 *
 * @param trk a pointer to a valid track
 */
void track_seg_embiggen(track *trk);
/**
 * Creates a track with one empty segment.
 *
 * @return a pointer to the new track, or NULL if there was an allocation error
 */
track *track_create()
{
    track *trk = malloc(sizeof(track));
    if (trk != NULL)
    {
        trk->segments = malloc(10 * sizeof(segment));
        if (trk->segments != NULL)
        {
            trk->segments[0].count = 0;
            trk->segments[0].capacity = 10;
            trk->segments[0].length = 0;
            trk->segments[0].trkpt = calloc(trk->segments[0].capacity, sizeof(trackpoint*));
        }
        else
        {
            return NULL;
        }
        trk->count = 1;
        trk->capacity = 10; 
        
        return trk;
    }
    else 
    {
        return NULL;
    }
}

/**
 * Destroys the given track, releasing all memory held by it.
 *
 * @param trk a pointer to a valid track
 */
void track_destroy(track *trk)
{
    for (int i=0; i<trk->count; i++)
    {
        for (int j=0; j<trk->segments[i].count; j++)
        {
            free(trk->segments[i].trkpt[j]);
        }
        free(trk->segments[i].trkpt);
    }
    free(trk->segments);
    free(trk);
}

/**
 * Returns the number of segments in the given track.
 *
 * @param trk a pointer to a valid track
 */
int track_count_segments(const track *trk)
{
    return trk->count;
}

/**
 * Returns the number of trackpoints in the given segment of the given
 * track.  The segment is specified by a 0-based index.  The return
 * value is 0 if the segment index is invalid.
 *
 * @param trk a pointer to a valid track
 * @param i a nonnegative integer less than the number of segments in trk
 * @return the number of trackpoints in the corresponding segment
 */
int track_count_points(const track *trk, int i)
{
    if (i >= 0 && i < trk->count)
    {
        return trk->segments[i].count;
    }
    else 
    {
        return 0;
    }
}

/**
 * Returns a copy of the given point in this track.  The segment is
 * specified as a 0-based index, and the point within the segment is
 * specified as a 0-based index into the corresponding segment.  The
 * return value is NULL if either index is invalid or if there is a memory
 * allocation error.  It is the caller's responsibility to destroy the
 * returned trackpoint.
 *
 * @param trk a pointer to a valid track
 * @param i a nonnegative integer less than the number of segments in trk
 * @param j a nonnegative integer less than the number of points in segment i
 * of track trk
 */
trackpoint *track_get_point(const track *trk, int i, int j)
{
    if (i >= 0 || i < trk->count || j >= 0 || j < trk->segments[i].count)
    {
        return trackpoint_copy(trk->segments[i].trkpt[j]);
    }
    else
    {
        return NULL;
    }

}

/**
 * Returns an array containing the length of each segment in this track.
 * The length of a segment is the sum of the distances between each point
 * in it and the next point.  The length of a segment with fewer than two
 * points is zero.  If there is a memory allocation error then the returned
 * pointer is NULL.  It is the caller's responsibility to free the returned
 * array.
 *
 * @param trk a pointer to a valid track
 */
double *track_get_lengths(const track *trk)
{
    double *segments_len = malloc(sizeof(double)*trk->count);

    if (segments_len != NULL)
    {   
        // for each segment
        for (int i=0; i<trk->count; i++)
        {
            segments_len[i] = trk->segments[i].length;
        }
        return segments_len;
    }
    else 
    {
        return NULL;
    }
}


/**
 * Adds a copy of the given point to the last segment in this track.
 * The point is not added and there is no change to the track if there
 * is a last point in the track (the last point in the current segment
 * or the last point on the previous segment if the current segment
 * is empty) and the timestamp on the new point is
 * not strictly after the timestamp on the last point.  There is no
 * effect if there is a memory allocation error.  The return value
 * indicates whether the point was added.  This function must execute
 * in amortized O(1) time (so a sequence of n consecutive operations must
 * work in worst-case O(n) time).
 *
 * @param trk a pointer to a valid track
 * @param pt a trackpoint with a timestamp strictly after the last trackpoint
 * in the last segment in this track (if there is such a point)
 * @return true if and only if the point was added
 */
bool track_add_point(track *trk, const trackpoint *pt)
{
    // to keep track of segment length
    double pt_distance = 0;
    location loc1;
    location loc2;

    //if first seg is empty
    if (trk->count == 1 && trk->segments[0].count == 0) 
    {
        //add the point at the beginning of the first segment
        trk->segments[0].trkpt[0] = trackpoint_copy(pt);

        //increment first segment count
        trk->segments[0].count++;

        return true;
    }
    // if current segment is empty
    else if (trk->segments[(trk->count)-1].count == 0)
    {
        // if trk segments > 2
        if (trk->count > 2)
        {
            // if pt time is greater than last trkpt
            if (trackpoint_time(pt) > trackpoint_time(trk->segments[(trk->count)-2].trkpt[trk->segments[(trk->count)-2].count-1]))
            {
                //add point to the beginning of the empty segment
                trk->segments[(trk->count)-1].trkpt[0] = trackpoint_copy(pt);

                //increment curr segment count
                trk->segments[(trk->count)-1].count++;

                return true;
            }
            else 
            {
                return false;
            }
        }
        // two segments and the second one is empty
        else
        {
            if (trackpoint_time(pt) > trackpoint_time(trk->segments[0].trkpt[trk->segments[0].count-1]))
            {
                trk->segments[1].trkpt[0] = trackpoint_copy(pt);   
                trk->segments[(trk->count)-1].count ++;
                return true;
            }
            else
            {
                return false;
            }
        }     
        
    }
    //if current segment is not empty
    else 
    //if (trk->segments[trk->count-1].count != 0) 
    {
        // if pt time is greater than last trkpt
        if (trackpoint_time(pt) > trackpoint_time(trk->segments[(trk->count)-1].trkpt[trk->segments[(trk->count)-1].count-1]))
        {
            // resize if necessary
            if (trk->segments[(trk->count)-1].count == trk->segments[(trk->count)-1].capacity)
            {
                track_trkpt_embiggen(trk, (trk->count)-1);
            }

            //add point to the next index of the curr segment
            trk->segments[(trk->count)-1].trkpt[trk->segments[(trk->count)-1].count] = trackpoint_copy(pt);

            //increment curr segment count
            trk->segments[(trk->count)-1].count++;

            // update segment length
            for (int i =0; i<trk->segments[(trk->count)-1].count; i++)
            {
                if (i+1 == trk->segments[(trk->count)-1].count)
                {
                    pt_distance = 0;
                }
                else
                {
                    loc1 = trackpoint_location(trk->segments[(trk->count)-1].trkpt[i]);
                    loc2 = trackpoint_location(trk->segments[(trk->count)-1].trkpt[i+1]);

                    pt_distance += location_distance(&loc1, &loc2); 
                }            
            }
            trk->segments[(trk->count)-1].length = pt_distance;

            return true;
        }
        else
        {
            return false;
        }
    }
    
}

void track_trkpt_embiggen(track *trk, int i)
{
    trackpoint **bigger_trkpt = realloc(trk->segments[i].trkpt, sizeof(trackpoint*) * trk->segments[i].capacity * 2);
    if (bigger_trkpt != NULL)
    {
        trk->segments[i].trkpt = bigger_trkpt;
        trk->segments[i].capacity *= 2;
    }
}

/**
 * Starts a new segment in the given track.  There is no effect on the track
 * if the current segment is empty or if there is a memory allocation error.
 *
 * @param trk a pointer to a valid track
 */
void track_start_segment(track *trk)
{
    // resize if necessary
    if (trk->count == trk->capacity)
    {
        track_seg_embiggen(trk);
    }

    // add if there is room
    if (trk->count < trk->capacity)
    {
        // if curr segment is empty
        if (trk->segments[trk->count-1].count == 0)
        {
            // no effect
            return;
        }
        // if curr segment is not empty
        else
        {
            trk->segments[trk->count].count = 0;
            trk->segments[trk->count].capacity = 10;
            trk->segments[trk->count].length = 0;
            trk->segments[trk->count].trkpt = calloc(trk->segments[0].capacity, sizeof(trackpoint*));
            trk->count++;
        }
    }
    
}

void track_seg_embiggen(track *trk)
{
    segment *bigger_segment = realloc(trk->segments, sizeof(segment) * trk->capacity * 2);
    if (bigger_segment != NULL)
    {
        trk->segments = bigger_segment;
        trk->capacity *= 2;
    }
}

/**
 * Merges the given range of segments in this track into one.  The segments
 * to merge are specified as the 0-based index of the first segment to
 * merge and one more than the index of the last segment to merge.
 * The resulting segment replaces the first merged one and later segments
 * are moved up to replace the other merged segments.  If the range is
 * invalid then there is no effect.
 *
 * @param trk a pointer to a valid track
 * @param start an integer greater than or equal to 0 and strictly less than
 * the number if segments in trk
 * @param end an integer greater than or equal to start and less than or
 * equal to the number of segments in trk
 */
void track_merge_segments(track *trk, int start, int end)
{
    location loc3;
    location loc4;
    double interval_distance = 0;
    int new_capacity = 0;

    // if range is invalid return
    if (start < 0 || start > trk->count || end < start || end > trk->count)
    {
        return;
    }
    // merge including one segment
    else if (start == end-1)
    {
        return;
    }
    //if range is valid
    else
    {
        // for each segment to add
        for (int i=start+1; i<end; i++)
        {
            {
                // check if not enough space
                new_capacity = trk->segments[start].count + trk->segments[i].count;

                if (trk->segments[start].capacity < new_capacity)
                {
                     // malloc space for new trkpt* array at the last index of the start trkpt array
                        trk->segments[start].trkpt = realloc(trk->segments[start].trkpt, sizeof(trackpoint*) * 2);
                        if (trk->segments[start].trkpt == NULL)
                        {
                            return;
                        }
                }

                // for each of the new trkpt ptrs
                for (int j=0; j<trk->segments[i].count; j++)
                {
                    // copy them into the start segment
                    trk->segments[start].trkpt[trk->segments[start].count+j] = trk->segments[i].trkpt[j];
                }

                //update distance

                // find the distance at each interval when merging
                loc3 = trackpoint_location(trk->segments[start].trkpt[trk->segments[start].count-1]);
                loc4 = trackpoint_location(trk->segments[start].trkpt[trk->segments[start].count]);

                interval_distance += location_distance(&loc3, &loc4);
                trk->segments[start].length += interval_distance;

                // add the length of each added segment
                trk->segments[start].length += trk->segments[i].length;

                //update count of start segment
                trk->segments[start].count += trk->segments[i].count;
                
                //update capacity
                trk->segments[start].capacity += trk->segments[i].capacity;
                
                // free the prev trkptn array
                free(trk->segments[i].trkpt);
            }
        }
        // move the remaining segments up
        for (int k=0; end+k<trk->count; k++)
        {
            trk->segments[start+1+k].trkpt = trk->segments[end+k].trkpt;

            // update count and capacity
            trk->segments[start+1+k].count = trk->segments[end+k].count;
            trk->segments[start+1+k].capacity = trk->segments[end+k].capacity;

            // make the curr point to null
            trk->segments[end+k].trkpt = NULL;
        }

        // updated track count
        trk->count -= (end - (start+1));
    }
}

/**
 * Creates a heapmap of the given track.  The heatmap will be a
 * rectangular 2-D array with each row separately allocated.  The last
 * three paramters are (simulated) reference parameters used to return
 * the heatmap and its dimensions.  Each element in the heatmap
 * represents an area bounded by two circles of latitude and two
 * meridians of longitude.  
 * 
 * The circle of latitude bounding the top of
 * the top row is the northernmost (highest) latitude of any
 * trackpoint in the given track.  The meridian bounding the left of
 * the first column is the western edge of the smallest spherical
 * wedge bounded by two meridians the contains all the points in the
 * track (the "western edge" for a nontrivial wedge being the one
 * that, when you move east from it along the equator, you stay in the
 * wedge).  When there are multple such wedges, choose the one with
 * the lowest normalized (adjusted to the range -180 (inclusive) to
 * 180 (exclusive)) longitude.  The distance (in degrees) between the
 * bounds of adjacent rows and columns is given by the last two
 * parameters.  
 * 
 * The heat map will have just enough rows and just
 * enough columns so that all points in the track fall into some cell.
 * The value in each entry in the heatmap is the number of trackpoints
 * located in the corresponding cell.  If a trackpoint is on the
 * border of two or more cells then it is counted in the bottommost
 * and rightmost cell it is on the border of, but do not add a row or
 * column just to place points on the south and east borders into
 * them and instead place the points on those borders by breaking ties
 * only between cells that already exist.
 * 
 * If there are no trackpoints in the track then the function
 * creates a 1x1 heatmap with the single element having a value of 0.
 * 
 * If the cell size is invalid or if there is a memory allocation
 * error then the map is set to NULL and the rows and columns
 * parameters are unchanged.  It is the caller's responsibility to
 * free each row in the returned array and the array itself.
 *
 * @param trk a pointer to a valid trackpoint
 * @param cell_width a positive double less than or equal to 360.0
 * @param cell_height a positive double less than or equal to 180.0
 * @param map a pointer to a pointer to a 2-D array of ints
 * @param rows a pointer to an int
 * @param cols a pointer to an int
 */
void track_heatmap(const track *trk, double cell_width, double cell_height,
		    int ***map, int *rows, int *cols)
{
    if (trk!= NULL && cell_width >= 0 && cell_height >= 0 && map != NULL)
    {
        // if there are no trackpoints in trk
        if (trk->segments[0].count == 0)
        {
            *map = malloc(sizeof(int*));
            if (*map != NULL)
            {
                **map = calloc(1, sizeof(int));
                *rows = *cols = 1;
            }   
        }
        // if trk has trkpts
        else
        {
            double north_bound = -90;
            double south_bound = 90;
            double west_bound;
            double min_distance = 400;

            int total_trkpts = 0;

            // find total number of trkpt
            for (int i=0; i<trk->count; i++)
            {
                total_trkpts += trk->segments[i].count;
            }

            // find locations
            location *locations = malloc(total_trkpts * sizeof(location));
            int k = 0;
            for (int i = 0; i < trk->count; i++)
            {
                for (int j = 0; j<trk->segments[i].count; j++)
                {
                    locations[k] = trackpoint_location(trk->segments[i].trkpt[j]);
                    k++;
                }
            }

            for (int i=0; i<total_trkpts; i++)
            {
                // keep track of the farthest dist
                double farthest_east = 0;

                for (int j=0; j<total_trkpts; j++)
                {
                    // distance with each trkpt
                    double eastern_distance = 0;

                    if (i != j)
                    {
                        // if i is east of j
                        if (locations[j].lon >= locations[i].lon)
                        {
                            eastern_distance = locations[j].lon - locations[i].lon;
                        }
                        // if i is west of j
                        else
                        {
                            eastern_distance = locations[i].lon - locations[j].lon + 360;   
                        }

                        // update farthest distance
                        if (eastern_distance > farthest_east)
                        {
                            farthest_east = eastern_distance;
                        }

                    }
                    
                }
                // find north bound -> greatest lat
                if (locations[i].lat > north_bound)
                {
                    north_bound = locations[i].lat;
                }
                // find south bound -> smallest lat
                if (locations[i].lat < south_bound)
                {
                    south_bound = locations[i].lat;
                }

                // update min distance
                if (farthest_east < min_distance || (farthest_east == min_distance && locations[i].lon < west_bound))
                {
                    min_distance = farthest_east;
                    west_bound = locations[i].lon;
                }

            }

            // get rows and columns
            int row_num = (int) ceil((north_bound - south_bound) / cell_height);
            int col_num = (int) ceil(min_distance / cell_width);

            // make heatmap
            int** map_temp = malloc(sizeof(int*) * row_num);
            if (map_temp != NULL)
            {
                // for each row create cols
                for (int l=0; l<row_num; l++)
                {
                    map_temp[l] = calloc(col_num , sizeof(int)); 
                    if (map_temp[l] == NULL)
                    {
                        return;
                    }
                }
            }
        
            *rows = row_num;
            *cols = col_num;

            // for each trkpt
            for (int i=0; i<total_trkpts; i++)
            {
                // insert trkpts
                int col_index, row_index;

                row_index = (int) floor((north_bound - locations[i].lat) / cell_height);
                if (row_index == row_num)
                {
                    row_index --;
                }

                if (locations[i].lon >= west_bound)
                {
                    col_index = (int) floor((locations[i].lon - west_bound) / cell_width);
                    if (col_index == col_num)
                    {
                        col_index --;
                    }
                }
                else
                {
                    col_index = (int) floor((west_bound - locations[i].lon + 360) / cell_width);
                    if (col_index == col_num)
                    {
                        col_index --;
                    }
                }
                map_temp[row_index][col_index] ++;
                
            }
            *map = map_temp;
            free(locations);
        }
    }
}


