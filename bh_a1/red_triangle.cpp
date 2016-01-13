// First new GL program
// Just makes a red triangle

#include "Angel.h"

const int NumPoints = 3;
GLuint buffer, buffer1,buffer2;
GLuint program;
//--------------------------------------------------------------------------

void
init( void )
{
    // Specifiy the vertices for a triangle
    vec2 vertices[3] = {
        vec2( -0.85, -0.715 ), vec2( -0.15, -0.5615 ), vec2(- 0.05, -0.255 )
    };
	
	vec2 vertices1[4] = {
        vec2( 0.25, -0.135 ), vec2( 0.25, 0.135 ), vec2( 0.755, -0.135 ),vec2( 0.755, 0.135 )
    };
	
	vec2 vertices2[5] = {
         vec2( -0.85, 0.25 ), vec2( -0.15, 0.9815 ), vec2( 0.034, -0.0835 ),vec2( 0.045, 0.705 ), vec2( 0.45, 0.512 )
    };
    // Create a vertex array object
    GLuint vao[3];
    glGenVertexArrays( 3, vao );
    

    // Create and initialize a buffer object
    glBindVertexArray( vao[0] );
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

    glBindVertexArray( vao[1] );	
    glGenBuffers( 1, &buffer1 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer1 );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW );
	
    glBindVertexArray( vao[2] );
    glGenBuffers( 1, &buffer2 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer2 );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW );
    
    // Load shaders and use the resulting shader program
    program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader


    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------
 
void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );	
    glDrawArrays( GL_TRIANGLES, 0, NumPoints );    // draw the points

    
    glBindBuffer( GL_ARRAY_BUFFER, buffer1 );

    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );    // draw the points
	
    glBindBuffer( GL_ARRAY_BUFFER, buffer2 );

    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 5);    // draw the points
	
	glFlush();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
       glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA );
    glutInitWindowSize( 512, 512 );

    glutCreateWindow( "Red Triangle" );
    glewExperimental=GL_TRUE; 
    glewInit();    
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;

}
