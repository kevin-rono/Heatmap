#ifndef __TRACK_H__
#define __TRACK_H__

#include <stdbool.h>

#include "trackpoint.h"

typedef struct track track;

/**
 * Creates a track with one empty segment.
 *
 * @return a pointer to the new track, or NULL if there was an allocation error
 */
track *track_create();

/**
 * Destroys the given track, releasing all memory held by it.
 *
 * @param trk a pointer to a valid track
 */
void track_destroy(track *trk);

/**
 * Returns the number of segments in the given track.
 *
 * @param trk a pointer to a valid track
 */
int track_count_segments(const track *trk);

/**
 * Returns the number of trackpoints in the given segment of the given
 * track.  The segment is specified by a 0-based index.  The return
 * value is 0 if the segment index is invalid.
 *
 * @param trk a pointer to a valid track
 * @param i a nonnegative integer less than the number of segments in trk
 * @return the number of trackpoints in the corresponding segment
 */
int track_count_points(const track *trk, int i);

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
trackpoint *track_get_point(const track *trk, int i, int j);

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
double *track_get_lengths(const track *trk);

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
bool track_add_point(track *trk, const trackpoint *pt);

/**
 * Starts a new segment in the given track.  There is no effect on the track
 * if the current segment is empty or if there is a memory allocation error.
 *
 * @param trk a pointer to a valid track
 */
void track_start_segment(track *trk);

/**
 * Merges the given range of segments in this track into one.  The segments
 * to merge are specified as the 0-based index of the first segment to
 * merge and one more than the index of the last segment to merge.
 * The resulting segment replaces the first merged one and later segments
 * are moved up to replace the other merged segments.  If the range is
 * invalid then there is no effect.
 *
 * @param trk a pointer to a valid track
 * @param start an integer greather than or equal to 0 and strictly less than
 * the number if segments in trk
 * @param end an integer greater than or equal to start and less than or
 * equal to the number of segments in trk
 */
void track_merge_segments(track *trk, int start, int end);

/**
 * Creates a heapmap of the given track.  The heatmap will be a
 * rectangular 2-D array with each row separately allocated.  The last
 * three paramters are (simulated) reference parameters used to return
 * the heatmap and its dimensions.  Each element in the heatmap
 * represents an area bounded by two circles of latitude and two
 * meridians of longitude.  The circle of latitude bounding the top of
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
 * parameters.  The heat map will have just enough rows and just
 * enough columns so that all points in the track fall into some cell.
 * The value in each entry in the heatmap is the number of trackpoints
 * located in the corresponding cell.  If a trackpoint is on the
 * border of two or more cells then it is counted in the bottommost
 * and rightmost cell it is on the border of, but do not add a row or
 * column just to place points on the south and east borders into
 * them and instead place the points on those borders by breaking ties
 * only between cells that already exist.
 * If there are no trackpoints in the track then the function
 * creates a 1x1 heatmap with the single element having a value of 0.
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
		    int ***map, int *rows, int *cols);

#endif
