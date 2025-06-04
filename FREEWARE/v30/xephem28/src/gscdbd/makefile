CLDFLAGS =
CFLAGS = $(CLDFLAGS) -O
LDFLAGS = $(CLDFLAGS)
LIBS = -lm

GSCD_OBJS = gscdbd.o cacheio.o gscio.o support.o version.o

gscdbd: $(GSCD_OBJS)
	cc $(LDFLAGS) -o $@ $(GSCD_OBJS) $(LIBS)
