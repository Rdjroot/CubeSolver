#version 330
in vec4 vPosition;
in vec3 aColor;
out vec4 ourColor;
uniform mat4 matrix;
uniform mat4 model;

void main() {
    ourColor = vec4(aColor, 1.0f);
    gl_Position = model * matrix * vPosition;
}
