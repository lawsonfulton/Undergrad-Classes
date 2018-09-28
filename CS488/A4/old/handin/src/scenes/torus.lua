
red = gr.material({1.0, 0.0, 0.0}, {0.9, 0.9, 0.9}, 20)
blue = gr.material({0.1, 0.2, 0.8}, {0.7, 0.7, 0.7}, 20)
green = gr.material({0.2, 0.8, 0.1}, {1, 1, 1}, 20)
white = gr.material({0.8, 0.8, 0.8}, {0.8, 0.8, 0.8}, 20)
black = gr.material({0.0, 0.0, 0.0}, {0, 0,0}, 3)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)


scene = gr.node('root')

-- s1 = gr.nh_sphere('s1', {1, 0, 0}, 1)
-- scene:add_child(s1)
-- s1:set_material(green)

-- s2 = gr.nh_sphere('s2', {-1, 0, 0}, 1)
-- scene:add_child(s2)
-- s2:set_material(blue)

-- b1 = gr.cube('b1')
-- scene:add_child(b1)
-- b1:set_material(red)

-- b1 = gr.nh_box('b1', {0, 0, 0}, 1)
-- scene:add_child(b1)
-- b1:set_material(blue)

b1 = gr.torus('b1')
scene:add_child(b1)
b1:set_material(black)

--b1:translate(0,1.5,0)
--b1:rotate('x',90)
-- the floor

plane = gr.mesh('plane', {
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1,  0, 1 }, 
		   { -1, 0, 1  }
		}, {
		   {3, 2, 1, 0}
		})
--scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)



white_light = gr.light({5,10,-10}, {0.5, 0.5, 0.5}, {1, 0, 0})
white_light2 = gr.light({-10,10,-10}, {0.5, 0.5, 0.5}, {1, 0, 0})


gr.render(scene, 'torus.png', 400, 400,
	  {0, 5, -8}, {0, -2, 5}, {0, 1, 0}, 50,
	  {0.1, 0.1, 0.1}, {white_light, white_light2})