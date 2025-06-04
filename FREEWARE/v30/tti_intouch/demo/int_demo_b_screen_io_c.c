main ()
{
int        i;

for (i = 1; i <= 1000; i++)
    {
    printf ("%3d ", i);
    if ((i % 20) == 0)
       printf("\n");
    }
}
