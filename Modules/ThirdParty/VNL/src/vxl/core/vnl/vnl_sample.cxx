// This is core/vnl/vnl_sample.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vnl_sample.h"
#include <vnl/vnl_math.h>

#include <vcl_cmath.h>
#include <vxl_config.h>

#if VXL_STDLIB_HAS_DRAND48
# include <stdlib.h> // dont_vxl_filter
#else
// rand() is not always a good random number generator,
// so use a simple congruential random number generator when drand48 not available - PVr
static unsigned long vnl_sample_seed = 12345;
#endif

# include <vcl_ctime.h>

void vnl_sample_reseed()
{
#if VXL_STDLIB_HAS_SRAND48
  srand48( vcl_time(0) );
#elif !VXL_STDLIB_HAS_DRAND48
  vnl_sample_seed = (unsigned long)vcl_time(0);
#endif
}

void vnl_sample_reseed(int seed)
{
#if VXL_STDLIB_HAS_SRAND48
  srand48( seed );
#elif !VXL_STDLIB_HAS_DRAND48
  vnl_sample_seed = seed;
#endif
}

double vnl_sample_uniform(double a, double b)
{
#if VXL_STDLIB_HAS_DRAND48
  double u = drand48(); // uniform on [0, 1)
#else
// rand() is not always a good random number generator,
// so use a simple congruential random number generator when drand48 not available - PVr
  vnl_sample_seed = (vnl_sample_seed*16807)%2147483647L;
  double u = double(vnl_sample_seed)/2147483647L; // uniform on [0, 1)
#endif
  return (1.0 - u)*a + u*b;
}

void vnl_sample_normal_2(double *x, double *y)
{
  double u     = vnl_sample_uniform(1, 0); // not (0,1): should not return 0
  double theta = vnl_sample_uniform(0, 2 * vnl_math::pi);

  double r = vcl_sqrt(-2*vcl_log(u));

  if (x) *x = r * vcl_cos(theta);
  if (y) *y = r * vcl_sin(theta);
}

double vnl_sample_normal(double mean, double sigma)
{
  double x;
  vnl_sample_normal_2(&x, 0);
  return mean + sigma * x;
}

// Implementation of Bernoulli sampling by Peter Vanroose
int vnl_sample_bernoulli(double q)
{
  // quick return if possible:
  if (q==0.0) return 0;
  if (q==1.0) return 1;
  if (q<0.0 || q>1.0) return -1;
  // q should be the probability of returning 0:
  return (vnl_sample_uniform(0.0, 1.0/q) >= 1.0) ? 1 : 0;
}

// Implementation of binomial sampling by Peter Vanroose
int vnl_sample_binomial(int n, double q)
{
  // Returns a random "k" value, between 0 and n, viz. the sum of n random
  // and independent drawings from a Bernoulli distribution with parameter q.

  if (n <= 0 || q<0.0 || q>1.0) return -1; // That is: when the input makes no sense, return nonsense "-1".
  if (q==0.0) return 0;
  if (q==1.0) return n;
  int k = 0;
  for (int i=n-1; i>=0; --i) {
    k += vnl_sample_bernoulli(q);
  }
  return k;
}
