/* errcheck.c */


/*
 * Call this function in your rendering loop to check for GL errors
 * during development.  Remove from release code.
 */


#include <GL/gl.h>
#include <GL/glu.h>
#incldue <stdio.h>



GLboolean CheckError( const char *message )
{
   GLenum error = glGetError();
   if (error) {
      char *err = (char *) gluErrorString( error );
      fprintf( stderr, "GL Error: %s at %s\n", err, message );
   }
}
