extgl Copyright (c) 2001-2002 Lev Povalahev (levp@gmx.net)

--------------------------------------------------
How to setup your project to use extgl
--------------------------------------------------

To use extgl place extgl.h in the same dir where gl.h resides.
In your program replace

#include <GL/gl.h>

with

#include <GL/extgl.h>

And add extgl.c to your project.

Then, after initializing OpenGL rendering context, call extgl_Initialize(). If the functions returned 0 everything is OK and you can use the available extensions and core OpenGL 1.2, 1.3 and 1.4 functions (of course only if the driver supports that).

--------------------------------------------------
Querying extensions
--------------------------------------------------

To find out if a particular extension or OpenGL 1.2/1.3/1.4 is supported you can use the structure

extgl_Extensions

For example to check if OpenGL 1.3 is supported you can use this piece of code:

if (extgl_Extensions.OpenGL13)
  puts("OpenGL 1.3 is supported");

The same for OpenGL 1.2 and 1.4 (i.e. the names are extgl_Extensions.OpenGL12 and extgl_Extensions.OpenGL14)

To check if an extension is available this can be done:

if (extgl_Extensions.ARB_vertex_program)
  puts("ARB_vertex_program is supported");

or generically

extgl_Extensions.'extension_name_without_the "GL_"'

here's a list of extensions that are supported:

GL_ARB_imaging
GL_ARB_depth_texture
GL_ARB_matrix_palette
GL_ARB_multisample
GL_ARB_multitexture
GL_ARB_point_parameters
GL_ARB_shadow
GL_ARB_shadow_ambient
GL_ARB_texture_compression
GL_ARB_texture_env_add
GL_ARB_texture_env_dot3
GL_ARB_texture_env_combine
GL_ARB_texture_env_crossbar
GL_ARB_texture_border_clamp
GL_ARB_texture_cube_map
GL_ARB_texture_mirrored_repeat
GL_ARB_transpose_matrix
GL_ARB_vertex_blend
GL_ARB_vertex_program
GL_ARB_window_pos
GL_EXT_abgr
GL_EXT_bgra
GL_EXT_blend_function_separate
GL_EXT_compiled_vertex_array
GL_EXT_cull_vertex
GL_EXT_draw_range_elements
GL_EXT_fog_coord
GL_EXT_multi_draw_arrays
GL_EXT_point_parameters
GL_EXT_secondary_color  
GL_EXT_separate_specular_color
GL_EXT_shadow_funcs
GL_EXT_stencil_two_side
GL_EXT_stencil_wrap
GL_EXT_texture_compression_s3tc
GL_EXT_texture_filter_anisotropic
GL_EXT_texture_lod_bias
GL_EXT_vertex_shader
GL_EXT_vertex_weighting
GL_ATI_element_array
GL_ATI_envmap_bumpmap
GL_ATI_fragment_shader
GL_ATI_pn_triangles
GL_ATI_texture_mirror_once
GL_ATI_vertex_array_object;
GL_ATI_vertex_streams
GL_ATIX_point_sprites
GL_ATIX_texture_env_route
GL_HP_occlusion_test
GL_NV_blend_square
GL_NV_copy_depth_to_color
GL_NV_depth_clamp
GL_NV_element_array
GL_NV_evaluators
GL_NV_fence
GL_NV_float_buffer
GL_NV_fog_distance
GL_NV_light_max_exponent
GL_NV_occlusion_query
GL_NV_packed_depth_stencil
GL_NV_point_sprite
GL_NV_primitive_restart
GL_NV_register_combiners
GL_NV_register_combiners2
GL_NV_texgen_reflection
GL_NV_texture_env_combine4
GL_NV_texture_rectangle
GL_NV_texture_shader
GL_NV_texture_shader2
GL_NV_texture_shader3
GL_NV_vertex_array_range
GL_NV_vertex_array_range2
GL_NV_vertex_program
GL_NV_vertex_program1_1
GL_NV_vertex_program2
GL_SGIS_generate_mipmap
GL_SGIX_shadow
GL_SGIX_depth_texture

--------------------------------------------------
WGL Extensions:
--------------------------------------------------

To query if a given WGL extension is supported the extgl_Extensions.wgl structure can be used:

extgl_Extensions.wgl.ARB_buffer_region, or generically

extgl_Extensions.wgl.'extension_name_without_the "WGL_"'

a list of all supported WGL extensions:

WGL_ARB_buffer_region
WGL_ARB_extensions_string
WGL_ARB_make_current_read;
WGL_ARB_multisample
WGL_ARB_pbuffer
WGL_ARB_pixel_format
WGL_ARB_render_texture 
WGL_EXT_extensions_string
WGL_EXT_swap_control
WGL_NV_render_depth_texture
WGL_NV_render_texture_rectangle

