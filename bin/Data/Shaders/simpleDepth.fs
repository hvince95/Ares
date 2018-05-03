#version 330 core

void main()
{             
	// since we have no colour information we can
	// just have an empty fragment shader

	// The empty fragment shader does no processing 
	// whatsoever and at the end of its run the depth 
	// buffer is updated. We could explicitly set the 
	// depth by uncommenting its one line, but this 
	// is effectively what happens behind the scene 
	// anyways.

    // gl_FragDepth = gl_FragCoord.z;
}  