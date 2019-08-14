/*
 * This file is part of Quick Charts.
 * Copyright 2019 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// This requires "sdf_core.frag" which is included through SDFShader.

// The maximum number of segments we can support for a single pie.
// This is based on OpenGL's MAX_FRAGMENT_UNIFORM_COMPONENTS.
// MAX_FRAGMENT_UNIFORM_COMPONENTS is required to be at least 1024.
// Assuming a segment of size 1, each segment needs
// 2 (size of a vec2) * 2 (number of points) + 4 (size of vec4) + 1 (segment size)
// components. We also need to leave some room for the other uniforms.
#define MAX_SEGMENTS 100

uniform float opacity;
uniform float innerRadius;
uniform float outerRadius;
uniform vec4 backgroundColor;

uniform vec2 triangles[MAX_SEGMENTS * 2];
uniform vec4 colors[MAX_SEGMENTS];
uniform int segments[MAX_SEGMENTS];
uniform int segmentCount;

in vec2 uv;

out vec4 out_color;

const vec2 origin = vec2(0.0, 0.0);
const float lineSmooth = 0.001;

void main()
{
    vec2 point = uv * (1.0 + lineSmooth * 2.0);

    float thickness = (outerRadius - innerRadius) / 2.0;
    float donut = sdf_annular(sdf_circle(point, innerRadius + thickness), thickness);

    vec4 color = vec4(0.0);
    float totalSegments = sdf_null;
    int index = 0;

    for (int i = 0; i < segmentCount && i < MAX_SEGMENTS; ++i) {
        float segment = sdf_null;
        for(int j = 0; j < segments[i] && j < MAX_SEGMENTS; j++) {
            segment = sdf_union(segment, sdf_round(sdf_triangle(point, origin, triangles[index++], triangles[index++]), lineSmooth));
        }
        totalSegments = sdf_union(totalSegments, segment);
        color = sdf_render(sdf_intersect(donut, segment), color, colors[i], lineSmooth);
    }

    // Finally, render an end segment with the background color.
    float segment = sdf_subtract(sdf_round(donut, lineSmooth), totalSegments);
    color = sdf_render(segment, color, backgroundColor, lineSmooth);

    out_color = color * opacity;
}
