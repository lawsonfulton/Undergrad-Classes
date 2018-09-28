-- //Name: Lawson Fulton
-- //UserID: ljfulton
-- //Student#: 20381453
--
-- CS488 -- Introduction to Computer Graphics
-- 
-- a3mark.lua
--
-- A very simple scene creating a trivial puppet.  The TAs will be
-- using this scene as part of marking your assignment.  You'll
-- probably want to make sure you get reasonable results with it!



rootnode = gr.node('root')
rotateNode = gr.node('rotateNode')

torsoNode = gr.node("torsoNode")
shouldersNode = gr.node("shouldersNode")
hipsNode = gr.node("hipsNode")
leftUpperArmNode = gr.node("leftUpperArmNode")
rightUpperArmNode = gr.node("rightUpperArmNode")
leftThighNode = gr.node("leftThighNode")
rightThighNode = gr.node("rightThighNode")
neckNode = gr.node("neckNode")
leftForearmNode = gr.node("leftForearmNode")
rightForearmNode = gr.node("rightForearmNode")
leftCalfNode = gr.node("leftCalfNode")
rightCalfNode = gr.node("rightCalfNode")
headNode = gr.node("headNode")
noseNode = gr.node("noseNode")
eyeNode = gr.node("eyeNode")
leftHandNode = gr.node("leftHandNode")
rightHandNode = gr.node("rightHandNode")
leftFootNode = gr.node("leftFootNode")
rightFootNode = gr.node("rightFootNode")

torso = gr.sphere("torso")
shoulders = gr.sphere("shoulders")
hips = gr.sphere("hips")
leftUpperArm = gr.sphere("leftUpperArm")
rightUpperArm = gr.sphere("rightUpperArm")
leftThigh = gr.sphere("leftThigh")
rightThigh = gr.sphere("rightThigh")
neck = gr.sphere("neck")
nose = gr.sphere("nose")
leftEye = gr.sphere("leftEye")
rightEye = gr.sphere("rightEye")
leftForearm = gr.sphere("leftForearm")
rightForearm = gr.sphere("rightForearm")
leftCalf = gr.sphere("leftCalf")
rightCalf = gr.sphere("rightCalf")
head = gr.sphere("head")
leftHand = gr.sphere("leftHand")
rightHand = gr.sphere("rightHand")
leftFoot = gr.sphere("leftFoot")
rightFoot = gr.sphere("rightFoot")

upperNeckJoint = gr.joint("upperNeckJoint", {0,0,360}, {0,0,360})
lowerNeckJoint = gr.joint("lowerNeckJoint", {0,0,360}, {0,0,360})
leftShoulderJoint = gr.joint("leftShoulderJoint", {0,0,360}, {0,0,360})
rightShoulderJoint = gr.joint("rightShoulderJoint", {0,0,360}, {0,0,360})
leftElbowJoint = gr.joint("leftElbowJoint", {0,0,360}, {0,0,360})
rightElbowJoint = gr.joint("rightElbowJoint", {0,0,360}, {0,0,360})
leftWristJoint = gr.joint("leftWristJoint", {0,0,360}, {0,0,360})
rightWristJoint = gr.joint("rightWristJoint", {0,0,360}, {0,0,360})
leftHipJoint = gr.joint("leftHipJoint", {0,0,360}, {0,0,360})
rightHipJoint = gr.joint("rightHipJoint", {0,0,360}, {0,0,360})
leftKneeJoint = gr.joint("leftKneeJoint", {0,0,360}, {0,0,360})
rightKneeJoint = gr.joint("rightKneeJoint", {0,0,360}, {0,0,360})
leftAnkleJoint = gr.joint("leftAnkleJoint", {0,0,360}, {0,0,360})
rightAnkleJoint = gr.joint("rightAnkleJoint", {0,0,360}, {0,0,360})


red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 20)
blue = gr.material({0.1, 0.2, 0.8}, {1, 1, 1}, 20)
green = gr.material({0.2, 0.8, 0.1}, {1, 1, 1}, 20)
white = gr.material({0.8, 0.8, 0.8}, {0.8, 0.8, 0.8}, 20)
black = gr.material({0.0, 0.0, 0.0}, {0.8, 0.8,0.8}, 3)

torso:set_material(green);
shoulders:set_material(green);
hips:set_material(green);
leftUpperArm:set_material(white);
rightUpperArm:set_material(white);
leftThigh:set_material(blue);
rightThigh:set_material(blue);
neck:set_material(white);
leftForearm:set_material(white);
rightForearm:set_material(white);
leftCalf:set_material(blue);
rightCalf:set_material(blue);
head:set_material(white);
leftHand:set_material(white);
rightHand:set_material(white);
leftFoot:set_material(black);
rightFoot:set_material(black);
nose:set_material(red);
leftEye:set_material(black);
rightEye:set_material(black);

rootnode:add_child(rotateNode)
rotateNode:add_child(torsoNode)
torsoNode:add_child(torso)
torso:scale(1,1.5,0.5)

-- torso
torsoNode:add_child(shouldersNode)
shouldersNode:translate(0,1.3,0)
shouldersNode:add_child(shoulders)
shoulders:scale(1.3,0.3,0.5)

-- neck
torsoNode:add_child(lowerNeckJoint)
lowerNeckJoint:translate(0, 1.5, 0)

lowerNeckJoint:add_child(neckNode)
neckNode:translate(0,0.1,0)
neckNode:add_child(neck)
neck:scale(0.2,0.4,0.2)

neckNode:add_child(upperNeckJoint)
upperNeckJoint:translate(0, 0.4, 0)

-- head
upperNeckJoint:add_child(headNode)
headNode:translate(0,0.4,0)
headNode:add_child(head)
head:scale(0.7,0.7,0.7)

headNode:add_child(noseNode)
noseNode:translate(0,-0.2,0.7)
noseNode:add_child(nose)
nose:scale(0.1,0.1,0.1)

eyeSize = 0.18
headNode:add_child(eyeNode)
eyeNode:add_child(leftEye)
leftEye:translate(-0.2,0,0.7)
leftEye:scale(eyeSize, eyeSize, eyeSize)

eyeNode:add_child(rightEye)
rightEye:translate(0.2,0,0.7)
rightEye:scale(eyeSize, eyeSize, eyeSize)

-- arms
shouldersNode:add_child(leftShoulderJoint)
leftShoulderJoint:translate(-1.3,0,0)
leftShoulderJoint:add_child(leftUpperArmNode)
leftUpperArmNode:translate(0,-1,0)
leftUpperArmNode:add_child(leftUpperArm)
leftUpperArm:scale(0.2,1,0.2)

shouldersNode:add_child(rightShoulderJoint)
rightShoulderJoint:translate(1.3,0,0)
rightShoulderJoint:add_child(rightUpperArmNode)
rightUpperArmNode:translate(0,-1,0)
rightUpperArmNode:add_child(rightUpperArm)
rightUpperArm:scale(0.2,1,0.2)

leftUpperArmNode:add_child(leftElbowJoint)
leftElbowJoint:translate(0,-1,0)
leftElbowJoint:add_child(leftForearmNode)
leftForearmNode:translate(0,-0.8,0)
leftForearmNode:add_child(leftForearm)
leftForearm:scale(0.15,0.8,0.15)

rightUpperArmNode:add_child(rightElbowJoint)
rightElbowJoint:translate(0,-1,0)
rightElbowJoint:add_child(rightForearmNode)
rightForearmNode:translate(0,-0.8,0)
rightForearmNode:add_child(rightForearm)
rightForearm:scale(0.15,0.8,0.15)

leftForearmNode:add_child(leftWristJoint)
leftWristJoint:translate(0,-0.8,0)
leftWristJoint:add_child(leftHandNode)
leftHandNode:translate(0,-0.1,0)
leftHandNode:add_child(leftHand)
leftHand:scale(0.1,0.25,0.2)

rightForearmNode:add_child(rightWristJoint)
rightWristJoint:translate(0,-0.8,0)
rightWristJoint:add_child(rightHandNode)
rightHandNode:translate(0,-0.1,0)
rightHandNode:add_child(rightHand)
rightHand:scale(0.1,0.25,0.2)

-- legs
torsoNode:add_child(hipsNode)
hipsNode:translate(0,-1.3,0)
hipsNode:add_child(hips)
hips:scale(1,0.4,0.5)

hipsNode:add_child(leftHipJoint)
leftHipJoint:translate(-0.7,0,0)
leftHipJoint:add_child(leftThighNode)
leftThighNode:translate(0,-1,0)
leftThighNode:add_child(leftThigh)
leftThigh:scale(0.3,1.3,0.3)

hipsNode:add_child(rightHipJoint)
rightHipJoint:translate(0.7,0,0)
rightHipJoint:add_child(rightThighNode)
rightThighNode:translate(0,-1,0)
rightThighNode:add_child(rightThigh)
rightThigh:scale(0.3,1.3,0.3)

leftThighNode:add_child(leftKneeJoint)
leftKneeJoint:translate(0,-1,0)
leftKneeJoint:add_child(leftCalfNode)
leftCalfNode:translate(0,-1,0)
leftCalfNode:add_child(leftCalf)
leftCalf:scale(0.25,1,0.25)

rightThighNode:add_child(rightKneeJoint)
rightKneeJoint:translate(0,-1,0)
rightKneeJoint:add_child(rightCalfNode)
rightCalfNode:translate(0,-1,0)
rightCalfNode:add_child(rightCalf)
rightCalf:scale(0.25,1,0.25)

leftCalfNode:add_child(leftAnkleJoint)
leftAnkleJoint:translate(0,-0.8,0)
leftAnkleJoint:add_child(leftFootNode)
leftFootNode:translate(0,-0.1,0.2)
leftFootNode:add_child(leftFoot)
leftFoot:scale(0.2,0.2,0.5)

rightCalfNode:add_child(rightAnkleJoint)
rightAnkleJoint:translate(0,-0.8,0)
rightAnkleJoint:add_child(rightFootNode)
rightFootNode:translate(0,-0.1,0.2)
rightFootNode:add_child(rightFoot)
rightFoot:scale(0.2,0.2,0.5)


--Whole body
rootnode:translate(0, 1, -11)
--rootnode:rotate('y', -20.0)

return rootnode
