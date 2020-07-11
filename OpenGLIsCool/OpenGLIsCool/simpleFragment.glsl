#version 420

// "varying" -> "in". Note the variable names match in both .glsl files.
in vec3 color;

void main()
{
    gl_FragColor = vec4(color, 1.0);
}
