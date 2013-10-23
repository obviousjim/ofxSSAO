varying vec3 norm;
void main()
{
    norm = gl_NormalMatrix * gl_Normal;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
    gl_Position = ftransform();
}