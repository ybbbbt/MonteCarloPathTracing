-- model file
model_file = './obj/scene02.obj'
-- directory to save result
result_directory = './result_scene02/'
-- save result per frame
save_per_frame = 50
-- set true to enable view
view = true
-- set true to save image
save_image = true
-- result width
width = 600
-- result height
height = 400
-- result fov
fov = 50
-- model center offset, center = center + center_offset
center_offset = {2, 2, 8 }
-- camera eye offset, eye = center + eye_offset
eye_offset = {0, 2, 5 }
-- camera up direction
up = {0, 1, 0 }
-- max iteration depth for ray
max_depth = 5
-- scene ambient
scene_ambient = {0.2, 0.2, 0.2}
-- external light illumination
external_light_num = 2
-- external light 0: origin
el_origin_0 = {-45, 0, -45 }
-- external light 0: x direction expand
el_dx_0 = {60, 0, 0 }
-- external light 0: y direction expand
el_dy_0 = {0, 0, 60 }
-- external light 0: emission
el_emission_0 = {1.5, 1.5, 1.5}
-- external light 1: origin
el_origin_1 = {15, 10, 15 }
-- external light 1: x direction expand
el_dx_1 = {-60, 0, 0 }
-- external light 1: y direction expand
el_dy_1 = {0, 0, -60 }
-- external light 1: emission
el_emission_1 = {1.5, 1.5, 1.5}
