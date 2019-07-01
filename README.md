Quite a few years ago, I started working on a hobby project software rasterizer, which is still up on GitHub. I hit a point and lost interest in the project, leaving it abandoned.

I came back around to the subject and decided to rewrite from scratch with a greater emphasis on performance, coupled with a few more years of experience as a developer. This is still just a hobby and a very early WIP, but I am hoping to learn from it, and maybe get it to the point where it could be semi-useful... or at least as useful as a hobby software rasterizer can be in this say and age.

The primary magic happens in void SoftwareRenderer::draw_triangle_texture(Triangle3uv &t) with most other interesting stuff happening around it in the same class. Clipping is not clipping UV coordinates correctly, but is otherwise working. There is still the occasional seam between edges and I'm sure that there are more than a few pixels that get drawn more than once per edge, but it's getting there (and it's a damn site faster than my old project!)

The triangle draw function uses a scanline algorithm, so it's fairly oldschool... no  SIMD instructions.
