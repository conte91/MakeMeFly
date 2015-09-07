#include <stdint.h>
#include <math.h>
#include <stdio.h>

#define E (2.71828182845905)

float Q_rsqrt( float number )
{
	int32_t i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( int32_t * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}
float sqr( float x ) { return x * x; }
// meaning of 'precision': the returned answer should be base^x, where
//                         x is in [power-precision/2,power+precision/2]
float mypow( float base, float power, float precision )
{   
   if ( power < 0 ) return 1 / mypow( base, -power, precision );
   if ( power >= 10 ) return sqr( mypow( base, power/2, precision/2 ) );
   if ( power >= 1 ) return base * mypow( base, power-1, precision );
   if ( precision >= 1 ) return 1/Q_rsqrt( base );
   return 1/Q_rsqrt( mypow( base, power*2, precision*2 ) );
}

float power( float base, float p ) { return mypow( base, p, .00001 );}

int main(){
  float maxErr=0;
  for(float i=0; i<30; i+=0.1){
    float r=power(E,i);
    float err=fabs((pow(E,i)-r)/pow(E,i));
    if(err>maxErr) maxErr=err;
    printf("%f %f %f %f\n", i, pow(E,i), r, err*100);
  }
  printf("Maxerr: %f\n", maxErr*100);
  return 0;
}
