#version 420

uniform mat4 MVP;

// vertices coming into the shader. Hence, instead of "attribute", we use "in" to indicate the direction.
in vec3 vCol;     
in vec3 vPos;        

// color is passed to next stage (fragment shader) in pipeline, so we use "out" instead of "varying".
out vec3 color;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    color = vCol;
}
