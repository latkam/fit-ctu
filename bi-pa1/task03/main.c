#ifndef __PROGTEST__
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#endif /* __PROGTEST__ */

typedef struct
{
    double x, y;
}TPOINT;

typedef struct
{
    double n_x, param_x;
    double n_y, param_y;
}TPEQ;

typedef struct
{
    TPEQ   p_eq;
    TPOINT min, max;
}TLINE;

typedef struct
{
    TPOINT points[4];
    TLINE  sides[4];
}TRECTANGLE;
//======================================================================================================================
int almostEqual ( double x, double y )
{
    if ( fabs( x - y ) <= 100 * DBL_EPSILON * x * x )
        return 1;
    return 0;
}
//======================================================================================================================
TPEQ getLineEq ( TPOINT a, TPOINT b )
{
    /** Computes parametric equation of a line represented by two points a, b:
     *  @param[in] a first point representing the line,
     *  @param[in] b second point representing the line,
     *  @return equation of the line.
     */
    TPEQ p_eq;
    p_eq.n_x     = a.x;
    p_eq.param_x = b.x - a.x;
    p_eq.n_y     = a.y;
    p_eq.param_y = b.y - a.y;
    return p_eq;
}
//======================================================================================================================
TRECTANGLE initializeRectangle ( double * rx1, double * ry1, double * rx2, double * ry2 )
{
    TRECTANGLE rec;
    rec.points[0].x = rec.points[3].x = * rx1;
    rec.points[0].y = rec.points[1].y = * ry1;
    rec.points[1].x = rec.points[2].x = * rx2;
    rec.points[2].y = rec.points[3].y = * ry2;

    rec.sides[0].p_eq = getLineEq(rec.points[0], rec.points[1] );
    rec.sides[0].min  = ( rec.points[0].x < rec.points[1].x ) ? rec.points[0] : rec.points[1];
    rec.sides[0].max  = ( rec.points[0].x > rec.points[1].x ) ? rec.points[0] : rec.points[1];

    rec.sides[1].p_eq = getLineEq(rec.points[1], rec.points[2] );
    rec.sides[1].min  = ( rec.points[1].y < rec.points[2].y ) ? rec.points[1] : rec.points[2];
    rec.sides[1].max  = ( rec.points[1].y > rec.points[2].y ) ? rec.points[1] : rec.points[2];

    rec.sides[2].p_eq = getLineEq(rec.points[2], rec.points[3] );
    rec.sides[2].min  = ( rec.points[2].x < rec.points[3].x ) ? rec.points[2] : rec.points[3];
    rec.sides[2].max  = ( rec.points[2].x > rec.points[3].x ) ? rec.points[2] : rec.points[3];

    rec.sides[3].p_eq = getLineEq(rec.points[3], rec.points[0] );
    rec.sides[3].min  = ( rec.points[0].y < rec.points[3].y ) ? rec.points[0] : rec.points[3];
    rec.sides[3].max  = ( rec.points[0].y > rec.points[3].y ) ? rec.points[0] : rec.points[3];

    return rec;
}
//======================================================================================================================
int pointInsideRectangle ( double * point_x, double * point_y, TRECTANGLE * rectangle )
{
    double min_x = fmin ( rectangle->points[0].x, rectangle->points[2].x );
    double max_x = fmax ( rectangle->points[0].x, rectangle->points[2].x );
    double min_y = fmin ( rectangle->points[0].y, rectangle->points[2].y );
    double max_y = fmax ( rectangle->points[0].y, rectangle->points[2].y );

    if ( * point_x - min_x >= -100 * DBL_EPSILON * * point_x * * point_x
      && * point_x - max_x <=  100 * DBL_EPSILON * * point_x * * point_x
      && * point_y - min_y >= -100 * DBL_EPSILON * * point_y * * point_y
      && * point_y - max_y <=  100 * DBL_EPSILON * * point_y * * point_y )
        return 1;
    return 0;
}
//======================================================================================================================
int intersectInsideLine ( TPOINT * intersection, TLINE * line )
{
    double min_x = fmin ( line->min.x, line->max.x );
    double max_x = fmax ( line->min.x, line->max.x );
    double min_y = fmin ( line->min.y, line->max.y );
    double max_y = fmax ( line->min.y, line->max.y );

    if ( intersection->x - min_x >= -100 * DBL_EPSILON * intersection->x * intersection->x
      && intersection->x - max_x <=  100 * DBL_EPSILON * intersection->x * intersection->x
      && intersection->y - min_y >= -100 * DBL_EPSILON * intersection->y * intersection->y
      && intersection->y - max_y <=  100 * DBL_EPSILON * intersection->y * intersection->y )
        return 1;
    return 0;
}
//======================================================================================================================
double pointsDistance ( TPOINT * a, TPOINT * b ) { return sqrt(pow(b->x-a->x, 2)+pow(b->y-a->y,2)); }
//======================================================================================================================
int getIntersects ( TLINE line, TLINE rec_side, TPOINT * intersection )
{
    /**Computes number of intersections of two lines:
     * @param[in] line line to be clipped,
     * @param[in] rec_side side of the rectangle,
     * @param[in,out] intersection point representing the intersection,
     * @return number of intersections. 0 - no intersection or inf intersections, 1 - one intersection. */
    double div_x, div_y;
    if ( line.p_eq.param_x == 0      // Dividing would not be correct if any of the operand is 0.
      || rec_side.p_eq.param_x == 0
      || line.p_eq.param_y == 0
      || rec_side.p_eq.param_y == 0 )
    {
        if ( ( line.p_eq.param_x == 0 && rec_side.p_eq.param_x == 0 )
          || ( line.p_eq.param_y == 0 && rec_side.p_eq.param_y == 0 ) )
        {
            div_x = div_y = 1;
        }
        else
        {
            div_x = 1; div_y = 2;
        }
    }
    else
    {
        div_x = line.p_eq.param_x / rec_side.p_eq.param_x;
        div_y = line.p_eq.param_y / rec_side.p_eq.param_y;
    }

    if ( fabs ( div_x - div_y ) <= 100 * DBL_EPSILON * div_x * div_x ) // Lines are parallel or equivalent.
        return 0;

    double n1l, t1, n1r, s1; // n1l + t1 = n1r + s1
    double n2l, t2, n2r, s2; // n2l + t2 = n2r + s2

    // Preparation of equations.
    n1l = line.p_eq.n_x;
    t1  = line.p_eq.param_x;
    n1r = rec_side.p_eq.n_x;
    s1  = rec_side.p_eq.param_x;
    n2l = line.p_eq.n_y;
    t2  = line.p_eq.param_y;
    n2r = rec_side.p_eq.n_y;
    s2  = rec_side.p_eq.param_y;

    // Move some variables from one side of equation to another. Variables n1l and n2l will no longer be needed.
    s1  *= -1;  // t1 + s1 = n1r
    n1r -= n1l; // t2 + s2 = n2r
    s2  *= -1;
    n2r -= n2l;

    // Compute parameter t. Variables s1 and s2 will no longer be needed.
    double multiply1 = s2;      // t1 + s1 = n1r / *( s2)
    double multiply2 = s1 * -1; // t2 + s2 = n2r / *(-s1)
    t1  *= multiply1;
    n1r *= multiply1;
    t2  *= multiply2;
    n2r *= multiply2;

    t1  += t2;
    n1r += n2r;
    t1  = n1r/t1;

    intersection->x = line.p_eq.n_x + (line.p_eq.param_x * t1);
    intersection->y = line.p_eq.n_y + (line.p_eq.param_y * t1);
    
    if ( intersection->x - rec_side.min.x >= -100 * DBL_EPSILON * intersection->x * intersection->x
      && intersection->x - rec_side.max.x <=  100 * DBL_EPSILON * intersection->x * intersection->x
      && intersection->y - rec_side.min.y >= -100 * DBL_EPSILON * intersection->y * intersection->y
      && intersection->y - rec_side.max.y <=  100 * DBL_EPSILON * intersection->y * intersection->y
      && intersectInsideLine(intersection, &line) )
        return 1;
    else
        return 0;
}
//======================================================================================================================
int clipLine ( double rx1, double ry1, double rx2, double ry2, double * ax, double * ay, double * bx, double * by )
{
    int intersect_cnt = 0, intersect_needed = 2;
    TPOINT intersect[2], tmp;
    TRECTANGLE rec;
    TLINE line;
    intersect[0].x = * ax;
    intersect[0].y = * ay;
    intersect[1].x = * bx;
    intersect[1].y = * by;
    
    rec = initializeRectangle( & rx1, & ry1, & rx2, & ry2 );

    line.min.x = * ax;
    line.min.y = * ay;
    line.max.x = * bx;
    line.max.y = * by;

    if ( pointInsideRectangle ( & line.min.x, & line.min.y, & rec ) )
        intersect_needed -= 1;
        
    if ( pointInsideRectangle ( & line.max.x, & line.max.y, & rec ) )
        intersect_needed -= 1;

    line.p_eq = getLineEq( line.min, line.max );

    for ( int i = 0; i < 4; i++ )
    {
        if ( getIntersects ( line, rec.sides[i], & tmp ) )
        {
            if ( tmp.x - rec.sides[i].min.x >= -100 * DBL_EPSILON * tmp.x * tmp.x
              && tmp.y - rec.sides[i].min.y >= -100 * DBL_EPSILON * tmp.y * tmp.y
              && tmp.x - rec.sides[i].max.x <=  100 * DBL_EPSILON * tmp.x * tmp.x
              && tmp.y - rec.sides[i].max.y <=  100 * DBL_EPSILON * tmp.y * tmp.y )
            {
                intersect_cnt += 1;
                 if ( intersect_cnt > 1
                   && almostEqual ( intersect[0].x, tmp.x )
                   && almostEqual ( intersect[0].y, tmp.y ) )
                     intersect_cnt -= 1;
                 else if ( intersect_cnt > 1
                        && almostEqual ( intersect[1].x, tmp.x )
                        && almostEqual ( intersect[1].y, tmp.y ) )
                     intersect_cnt -= 1;
                 else
                    intersect[intersect_cnt-1] = tmp;
            }
        }
    }

    // If found two identical intersections and needed only one - found one, in fact.
    if ( intersect_needed == 1
      && intersect_cnt    == 2
      && almostEqual ( intersect[0].x, intersect[1].x )
      && almostEqual ( intersect[0].y, intersect[1].y ) )
        intersect_cnt -= 1;

    if ( intersect_cnt == 1 )
    {
        if ( ! pointInsideRectangle ( ax, ay, & rec ) )
        {
            * ax = intersect[0].x;
            * ay = intersect[0].y;
        }
        if ( ! pointInsideRectangle ( bx, by, & rec ) )
        {
            * bx = intersect[0].x;
            * by = intersect[0].y;
        }
        return 1;

    }
    
    if ( intersect_cnt < intersect_needed )
        return 0;

    // Pair point and the closest intersection
    double distance1 = pointsDistance ( & line.min, & intersect[0] );
    double distance2 = pointsDistance ( & line.min, & intersect[1] );
    if ( distance1 - distance2 > -100 * DBL_EPSILON * distance1 * distance1 )
    {
        tmp = intersect[0];
        intersect[0] = intersect[1];
        intersect[1] = tmp;
    }

    *ax = intersect[0].x;
    *ay = intersect[0].y;
    *bx = intersect[1].x;
    *by = intersect[1].y;
    return 1;
}
//======================================================================================================================
#ifndef __PROGTEST__
//======================================================================================================================
int main ( void )
{
    
    return 0;
}
#endif /* __PROGTEST__ */

