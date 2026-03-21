/*
 * Quadrant Sphere - a 3D ray marched sphere at the origin.
 * Each quadrant of the sphere is a different color.
 * The location uniform positions the camera, always looking at the origin.
 */

out vec4 color_out;

uniform float time;
uniform vec2 resolution;
uniform vec3 location;

float sphereSDF(vec3 p, float r) {
    return length(p) - r;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = (fragCoord.xy - 0.5 * resolution) / resolution.y;

    vec3 ro = location;

    // Build a look-at camera pointing from location toward the origin
    vec3 forward = normalize(-ro);
    vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
    vec3 up = cross(right, forward);
    vec3 rd = normalize(forward + uv.x * right + uv.y * up);

    // Scale sphere radius with camera distance so it has a fixed apparent size
    float radius = length(ro) * 0.15;

    // Ray march
    float t = 0.0;
    for (int i = 0; i < 64; i++) {
        vec3 p = ro + rd * t;
        float d = sphereSDF(p, radius);
        if (d < 0.001) break;
        t += d;
        if (t > 20.0) break;
    }

    vec3 col = vec3(0.0);

    if (t < 20.0) {
        vec3 hit = ro + rd * t;
        vec3 n = normalize(hit);

        // Color by octant based on sign of x, y, and z
        vec3 s = step(0.0, hit);
        int octant = int(s.x) + int(s.y) * 2 + int(s.z) * 4;
        if (octant == 0) col = vec3(1.0, 0.2, 0.2); // red
        if (octant == 1) col = vec3(0.2, 1.0, 0.2); // green
        if (octant == 2) col = vec3(0.2, 0.2, 1.0); // blue
        if (octant == 3) col = vec3(1.0, 1.0, 0.2); // yellow
        if (octant == 4) col = vec3(1.0, 0.2, 1.0); // magenta
        if (octant == 5) col = vec3(0.2, 1.0, 1.0); // cyan
        if (octant == 6) col = vec3(1.0, 0.6, 0.2); // orange
        if (octant == 7) col = vec3(0.6, 0.2, 1.0); // purple

        // Simple diffuse lighting from above-right
        float diff = max(dot(n, normalize(vec3(1.0, 1.0, 1.0))), 0.0);
        col *= 0.3 + 0.7 * diff;
    }

    fragColor = vec4(col, 1.0);
}

void main(void) {
    mainImage(color_out, gl_FragCoord.xy);
}
