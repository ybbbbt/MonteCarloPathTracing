-- model file
model_file = './obj/modern_tables.obj'
-- directory to save result
result_directory = './result_table/'
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
center_offset = {-50, 20, 50}
-- camera eye offset, eye = center + eye_offset
eye_offset = {10, 30, 55}
-- camera up direction
up = {0, 1, 0}
-- max iteration depth for ray
max_depth = 5
-- scene ambient
scene_ambient = {0.2, 0.2, 0.2}
-- external light illumination
external_light_num = 0
