-- A simple test scene featuring some spherical cows grazing
-- around Stonehenge.  "Assume that cows are spheres..."

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

-- ##############################################
-- the arch
-- ##############################################

inst = gr.node('inst')

arc = gr.node('arc')
inst:add_child(arc)
arc:translate(0, 0, -10)

p1 = gr.nh_box('p1', {0, 0, 0}, 1)
arc:add_child(p1)
p1:set_material(stone)
p1:translate(-2.4, 0, -0.4)
p1:scale(0.8, 4, 0.8)

p2 = gr.nh_box('p2', {0, 0, 0}, 1)
arc:add_child(p2)
p2:set_material(stone)
p2:translate(1.6, 0, -0.4)
p2:scale(0.8, 4, 0.8)

s = gr.nh_sphere('s', {0, 0, 0}, 1)
arc:add_child(s)
s:set_material(stone)
s:translate(0, 4, 0)
s:scale(4, 0.6, 0.6)

-- #############################################
-- Let's assume that cows are spheres
-- #############################################

cow = gr.node('the_cow')

for _, spec in pairs({
			{'body', {0, 0, 0}, 1.0},
			{'head', {.9, .3, 0}, 0.6},
			{'tail', {-.94, .34, 0}, 0.2},
			{'lfleg', {.7, -.7, -.7}, 0.3},
			{'lrleg', {-.7, -.7, -.7}, 0.3},
			{'rfleg', {.7, -.7, .7}, 0.3},
			{'rrleg', {-.7, -.7, .7}, 0.3}
		     }) do
   part = gr.nh_sphere(unpack(spec))
   part:set_material(hide)
   cow:add_child(part)
end

-- ##############################################
-- the scene
-- ##############################################

scene = gr.node('scene')
scene:rotate('X', 23)

-- the floor

plane = gr.mesh('plane', {
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1,  0, 1 }, 
		   { -1, 0, 1  }
		}, {
		   {3, 2, 1, 0}
		})
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

-- Construct a central altar in the shape of a buckyball.  The
-- buckyball at the centre of the real Stonehenge was destroyed
-- in the great fire of 733 AD.

require('buckyball')

scene:add_child(buckyball)
buckyball:set_material(stone)
buckyball:scale(1.5, 1.5, 1.5)

-- Use the instanced cow model to place some actual cows in the scene.
-- For convenience, do this in a loop.

cow_number = 1

for _, pt in pairs({
		      {{1,1.3,14}, 20},
		      {{5,1.3,-11}, 180},
		      {{-5.5,1.3,-3}, -60}}) do
   cow_instance = gr.node('cow' .. tostring(cow_number))
   scene:add_child(cow_instance)
   cow_instance:add_child(cow)
   cow_instance:translate(unpack(pt[1]))
   cow_instance:rotate('Y', pt[2])
   cow_instance:scale(1.4, 1.4, 1.4)
   
   cow_number = cow_number + 1
end

-- Place a ring of arches.

for i = 1, 6 do
   an_arc = gr.node('arc' .. tostring(i))
   an_arc:rotate('Y', (i-1) * 60)
   scene:add_child(an_arc)
   an_arc:add_child(arc)
end

gr.render(scene,
	  'myscene.png', 1000, 1000,
	  {-7, 3, 1.5}, {0.5, -0.2, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})


-- red = gr.material({1.0, 0.0, 0.0}, {0.9, 0.9, 0.9}, 20)
-- blue = gr.material({0.1, 0.2, 0.8}, {1, 1, 1}, 20)
-- green = gr.material({0.2, 0.8, 0.1}, {1, 1, 1}, 20)
-- white = gr.material({0.8, 0.8, 0.8}, {0.8, 0.8, 0.8}, 20)
-- black = gr.material({0.0, 0.0, 0.0}, {0.8, 0.8,0.8}, 3)

-- scene_root = gr.node('root')

-- -- s1 = gr.nh_sphere('s1', {1, 0, 0}, 1)
-- -- scene_root:add_child(s1)
-- -- s1:set_material(green)

-- -- s2 = gr.nh_sphere('s2', {-1, 0, 0}, 1)
-- -- scene_root:add_child(s2)
-- -- s2:set_material(blue)

-- -- b1 = gr.cube('b1')
-- -- scene_root:add_child(b1)
-- -- b1:set_material(red)

-- b1 = gr.nh_box('b1', {0, 0, 0}, 1)
-- scene_root:add_child(b1)
-- b1:set_material(blue)

-- -- s3 = gr.nh_sphere('s3', {0, 0, 0}, 1)
-- -- scene_root:add_child(s3)
-- -- s3:scale(1,2,1)
-- -- s3:translate(0,1,0)
-- -- s3:set_material(red)

-- --TODO check winding direction
-- -- plane = gr.mesh('plane', {
-- -- 	{1, -1, 0},
-- -- 	{1, 1, 0},
-- -- 	{-1, 1, 0},
-- -- 	{-1, -1, 0},
-- --      }, {
-- -- 	{3,2,1,0}
-- --      })
-- -- scene_root:add_child(plane)
-- -- plane:set_material(green)
-- --plane:scale(30, 30, 30)



-- white_light = gr.light({0,1,-10}, {1.0, 1.0, 1.0}, {1, 0, 0})


-- gr.render(scene_root, 'myscene.png', 400, 400,
-- 	  {0, 0, -5}, {0, 0, 5}, {0, 1, 0}, 50,
-- 	  {0.1, 0.1, 0.1}, {white_light})
