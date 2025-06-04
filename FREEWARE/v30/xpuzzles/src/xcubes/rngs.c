/*
Dr. Park's algorithm published in the Oct. '88 ACM 
"Random Number Generators: Good Ones Are Hard To Find"
His version available at ftp://cs.wm.edu/pub/rngs.tar
Present form by many authors.
*/

static long Seed = 1;       /* This is required to be 32 bits long */

/*
 *      Given an integer, this routine initializes the RNG seed.
 */
void SetRNG(s)
  long s;
{
  Seed = s;
}

/*
 *      Returns an integer between 0 and 2147483647, inclusive.
 */
long LongRNG()
{
  if ((Seed = Seed % 44488 * 48271 - Seed / 44488 * 3399) < 0)
    Seed += 2147483647;
  return Seed - 1;
}
