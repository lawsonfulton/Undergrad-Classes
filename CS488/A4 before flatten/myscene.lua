

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 20)
blue = gr.material({0.1, 0.2, 0.8}, {1, 1, 1}, 20)
green = gr.material({0.2, 0.8, 0.1}, {1, 1, 1}, 20)
white = gr.material({0.8, 0.8, 0.8}, {0.8, 0.8, 0.8}, 20)
black = gr.material({0.0, 0.0, 0.0}, {0.8, 0.8,0.8}, 3)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {1, 0, 0}, 1)
scene_root:add_child(s1)
s1:set_material(green)

s1 = gr.nh_sphere('s2', {-1, 0, 0}, 1)
scene_root:add_child(s1)
s1:set_material(blue)



white_light = gr.light({0,5,-10}, {1.0, 1.0, 1.0}, {1, 0, 0})


gr.render(scene_root, 'myscene.png', 800, 800,
	  {0, 0, -10}, {0, 0, 1}, {0, 1, 0}, 50,
	  {0.1, 0.1, 0.1}, {white_light})
