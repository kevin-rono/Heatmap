#ifndef __TRACKPOINT_H__
#define __TRACKPOINT_H__

#include "location.h"

typedef struct trackpoint trackpoint;

/**
 * Creates a trackpoint with the given values.  If the values are
 * invalid or there was an allocation error then the return value is
 * NULL.  Otherwise, it is the caller's responsibility to eventually
 * free the trackpoint by passing it to trackpoint_destroy.
 *
 * @param lat a value between -90 and 90 (inclusive)
 * @param lon a value between -180 (inclusive) and 180 (exclusive)
 * @param time a long
 * @return a pointer to the trackpoint created, or NULL
 */
trackpoint *trackpoint_create(double lat, double lon, long time);


/**
 * Returns a copy of the given trackpoint.  It is the caller's responsibility
 * to destroy the returned trackpoint.  If there is a memory allocation error
 * then the return value is NULL;
 */
trackpoint *trackpoint_copy(const trackpoint *pt);

/**
 * Destroys the given trackpoint.
 *
 * @param pt a pointer to a valid trackpoint
 */
void trackpoint_destroy(trackpoint *pt);

/**
 * Returns the location of the given trackpoint.
 *
 * @param pt a pointer to a valid trackpoint
 * @return the location of that trackpoint
 */
location trackpoint_location(const trackpoint *pt);

/**
 * Returns the timestamp of the given trackpoint.
 *
 * @param pt a pointer to a valid trackpoint
 * @return the timestamp of that trackpoint
 */
long trackpoint_time(const trackpoint *pt);

#endif
