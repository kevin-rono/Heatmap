#ifndef __LOCATION_H__
#define __LOCATION_H__

typedef struct _location
{
  double lat;
  double lon;
} location;

/**
 * Returns the distance between the two locations on the Earth's surface.
 *
 * @param l1 a location
 * @param l2 a location
 * @return the distance between those points
 */
double location_distance(const location *l1, const location *l2);
double location_distance_spherical(const location *l1, const location *l2);
double location_distance_oblate(const location *l1, const location *l2);

#endif
