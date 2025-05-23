#version 300 es

precision mediump float;
layout(location = 0) out vec4 out_color;

in vec3 ec_pos;

void main() {
    vec3 ec_normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    ec_normal.z *= zoom;
    ec_normal = normalize(ec_normal);

    out_color = vec4(1.0, 1.0, 1.0, 1.0);
}
