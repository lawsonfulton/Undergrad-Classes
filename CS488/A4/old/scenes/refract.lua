red = gr.material({1.0, 0.0, 0.0}, {0.2, 0.2, 0.2}, 20)
blue = gr.material({0.1, 0.2, 0.8}, {0.6, 0.6, 0.6}, 20)
green = gr.material({0.2, 0.8, 0.1}, {0, 0, 0}, 20)
white = gr.material({0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, 500)
black = gr.material({0.0, 0.0, 0.0}, {0.8, 0.8,0.8}, 3)

scene_root = gr.node('root')

-- s1 = gr.nh_sphere('s1', {1, 0, 0}, 1)
-- scene_root:add_child(s1)
-- s1:set_material(green)

-- s2 = gr.nh_sphere('s2', {-1, 0, 0}, 1)
-- scene_root:add_child(s2)
-- s2:set_material(blue)

-- b1 = gr.cube('b1')
-- scene_root:add_child(b1)
-- b1:set_material(white)
-- b1:translate(1,0,1)

-- b1 = gr.nh_box('b1', {0,0,-3}, 1)
-- scene_root:add_child(b1)
-- b1:set_material(red)
--glass = gr.transparent_material({0.0, 0.0, 0.0}, {0.2, 0.2, 0.2}, 100, 1.5,0.95)
glass = gr.transparent_material({0.0, 0.0, 0.0}, {0, 0, 0}, 100, 1.5,0.95)

-- s3 = gr.sphere('s3')
-- scene_root:add_child(s3)
-- s3:set_material(glass)
-- s3:translate(0, 1.51, 0)
-- s3:scale(0.5,0.5,0.5)

b1 = gr.cube('b1')
scene_root:add_child(b1)
b1:set_material(glass)
b1:translate(-0.5,0.01,-2.5)



cubes = gr.node('cubes')
b1 = gr.cube('b1')
cubes:add_child(b1)
b1:set_material(blue)
b1:translate(-0.5,0,-0.5)

b1 = gr.cube('b1')
cubes:add_child(b1)
b1:set_material(red)
b1:translate(-0.5,0,0.5)

b1 = gr.cube('b1')
cubes:add_child(b1)
b1:set_material(white)
b1:translate(-0.5,0,1.5)

c1 = gr.node("xx")
c1:add_child(cubes)
scene_root:add_child(c1)
c1:translate(0,0,0)

c1 = gr.node("x1")
c1:add_child(cubes)
scene_root:add_child(c1)
c1:translate(1.5,0,0)
c1 = gr.node("x1")
c1:add_child(cubes)
scene_root:add_child(c1)
c1:translate(-1.5,0,0)

--TODO check winding direction
-- the floor

plane = gr.mesh('plane', {
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1,  0, 1 }, 
		   { -1, 0, 1  }
		}, {
		   {3, 2, 1, 0}
		})
scene_root:add_child(plane)
plane:set_material(green)
plane:scale(30, 30, 30)


white_light = gr.light({3,7,-7}, {0.5, 0.5, 0.5}, {1, 0, 0})
white_light2 = gr.light({-10,20,-7}, {0.5, 0.5, 0.5}, {1, 0, 0})


gr.render(scene_root, 'refract.png', 1000, 1000,
	  {1, 1.2, -5}, {-1, -0, 5}, {0, 1, 0}, 50,
	  {0.1, 0.1, 0.1}, {white_light, white_light2})