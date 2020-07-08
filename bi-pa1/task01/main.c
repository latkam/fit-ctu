#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>

typedef struct
{
	double a;
	double b;
	double c;
}TTRIANGLE;

int getTriangle ( TTRIANGLE * triangle, int order )
{
	char input_type[4];
	input_type[3] = '\0';
	
	printf ( "Trojuhelnik #%d:\n", order );
	
	if ( scanf ( "%3s", input_type ) != 1 
	  || ( strncmp ( input_type, "SSS", 3 )
	    && strncmp ( input_type, "SUS", 3 )
	    && strncmp ( input_type, "USU", 3 ) ) )
	{
		printf ( "Nespravny vstup.\n" );
		return 0;
	}
	
	if ( ! strncmp ( input_type, "SSS", 3 ) )
	{
	    if ( scanf ( "%lf %lf %lf", & triangle->a, & triangle->b, & triangle->c ) != 3
	          || triangle->a <= 0
           	  || triangle->b <= 0
        	  || triangle->c <= 0 )
	        {
		        printf ( "Nespravny vstup.\n" );
        		return 0;
        	}
	}
	else if ( ! strncmp ( input_type, "SUS", 3 ) )
	{
	    double gamma;
	    if ( scanf ( "%lf %lf %lf", & triangle->a, & gamma, & triangle->b ) != 3 
	      || triangle->a <= 0
	      || triangle->b <= 0
	      || gamma <= 0
	      || gamma >= 180 )
	    {
	        printf ( "Nespravny vstup.\n" );
        	return 0;
	    }
	    gamma = gamma * M_PI / 180;
	    triangle->c = sqrt ( pow(triangle->a, 2) +
	                         pow(triangle->b, 2) -
	                         ( 2*triangle->a*triangle->b*cos(gamma) ) );
	}
	else
	{
	    double alpha, beta, gamma;
	    if ( scanf ( "%lf %lf %lf", & alpha, & triangle->c, & beta ) != 3
          || triangle->c <= 0
	      || alpha <= 0
	      || alpha >= 180
	      || beta <= 0
	      || beta >= 180 )
	    {
	        printf ( "Nespravny vstup.\n" );
        	return 0;
	    }
	    gamma = 180 - alpha - beta;
	    if ( gamma == 0 )
	    {
	        printf ( "Vstup netvori trojuhelnik.\n" );
	        return 0;
	    }
	    alpha = alpha * M_PI / 180;
	    beta = beta * M_PI / 180;
	    gamma = gamma * M_PI / 180;
	    triangle->a = ( triangle->c * sin(alpha) ) / sin(gamma);
	    triangle->b = ( triangle->c * sin(beta)  ) / sin(gamma);
	} 
	return 1;
} 
//=====================================================================================================
TTRIANGLE sort ( TTRIANGLE unsorted )
{
	TTRIANGLE sorted;
	sorted.a = ( unsorted.a < unsorted.b ) ? unsorted.a : unsorted.b;
	sorted.a = ( unsorted.c <   sorted.a ) ? unsorted.c :   sorted.a;
	sorted.c = ( unsorted.a > unsorted.b ) ? unsorted.a : unsorted.b;
	sorted.c = ( unsorted.c >   sorted.c ) ? unsorted.c :   sorted.c;
	sorted.b = unsorted.a + unsorted.b + unsorted.c - sorted.a - sorted.c;
	return sorted;
}
//=====================================================================================================
int main ( void )
{
	TTRIANGLE arr[2];
	for ( int i = 0; i < 2; i++ )
	{
		if ( ! getTriangle ( & arr[i], i+1 ) )
			return 1;
		arr[i] = sort ( arr[i] );
		if ( arr[i].a + arr[i].b - arr[i].c <= 100 * DBL_EPSILON * ( arr[i].a + arr[i].b + arr[i].c ) )
		{
			printf ( "Vstup netvori trojuhelnik.\n" );
			return 1;
		}
	}
	if ( ( fabs ( arr[0].a - arr[1].a ) <= 100 * DBL_EPSILON * arr[0].a )
	  && ( fabs ( arr[0].b - arr[1].b ) <= 100 * DBL_EPSILON * arr[0].b )
	  && ( fabs ( arr[0].c - arr[1].c ) <= 100 * DBL_EPSILON * arr[0].c ) )
		printf ( "Trojuhelniky jsou shodne.\n" );
	else if ( ( fabs ( (double) arr[0].a / arr[1].a - (double) arr[0].b / arr[1].b ) ) < 100 * DBL_EPSILON * arr[0].a * arr[0].a
		   && ( fabs ( (double) arr[0].b / arr[1].b - (double) arr[0].c / arr[1].c ) ) < 100 * DBL_EPSILON * arr[0].b * arr[0].b )
		printf ( "Trojuhelniky nejsou shodne, ale jsou podobne.\n" );
	else
		printf ( "Trojuhelniky nejsou shodne ani podobne.\n" );
	return 0;
}
