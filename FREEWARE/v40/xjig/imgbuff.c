
#ifndef _global_h
#	include "global.h"
#endif
#ifndef _imgbuff_h
#	include "imgbuff.H"
#endif

ImageBuffer::ImageBuffer() {
	ximage=0;
	width=0;
	height=0;
#ifdef USE_MIT_SHM
int	major,minor;
Bool	pmaps;

	shm=(shared)?XShmQueryVersion(dpy,&major,&minor,&pmaps):0;
	if (shm&&verbose)	{
		printf( "--- using shared memory extension V%d.%d %s\n", major, minor,
					((pmaps)?"(shared pixmaps supported)":"") );
	}
#endif
}

ImageBuffer::~ImageBuffer() {
	FreeData();
}

void ImageBuffer::FreeData() {
	if (ximage) {
#ifdef USE_MIT_SHM
		if (shm) {
			XShmDetach(dpy,&shminfo);
			XDestroyImage(ximage);
			shmdt(shminfo.shmaddr);
			shmctl(shminfo.shmid, IPC_RMID, 0 );
		}
		else
#endif
		{
			free( ximage->data );
			ximage->data = 0L;
			XDestroyImage(ximage);
		}
		ximage=0;
	}
}

void ImageBuffer::AllocData(int w, int h, int bpp8) {
	FreeData();
	width  = w;
	height = h;

#ifdef USE_MIT_SHM
	if (shm) {
		ximage = XShmCreateImage(dpy, DefaultVisual(dpy,scr),
			DefaultDepth(dpy,scr), ZPixmap, NULL, &shminfo, width, height );
		shminfo.shmid    = shmget(IPC_PRIVATE, ximage->bytes_per_line * ximage->height, IPC_CREAT|0777);
		shminfo.shmaddr  = ximage->data = (char*)shmat(shminfo.shmid,0,0);
		shminfo.readOnly = False;
		XShmAttach(dpy,&shminfo);
	}
	else
#endif
	{
		char *data=(char*)malloc(width*height*bpp8);
		ximage = XCreateImage(dpy, DefaultVisual(dpy,scr),
			DefaultDepth(dpy,scr), ZPixmap, 0,
			data, width, height, (8*bpp8), width*bpp8 );
	}
}


XImage *ImageBuffer::Init(int w,int h,int bpp8) {
	// w+=10; h+=10;
	if (w>width||h>height) {
		FreeData();
		AllocData(w,h,bpp8);
	}
	return ximage;
}

