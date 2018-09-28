//Name: Lawson Fulton
//UserID: ljfulton
//Student#: 20381453

#include "scene.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
    selected = false;
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(Viewer *viewer, QMatrix4x4 transform, bool picking) const
{
    for (auto child = m_children.begin(); child != m_children.end(); ++child) {
        (*child)->walk_gl(viewer, transform * m_trans, picking);
    }
}

void SceneNode::rotate(char axis, double angle)
{
  //std::cerr << "Stub: Rotate " << m_name << " around " << axis << " by " << angle << std::endl;
  switch(axis) {
    case 'x':
        m_trans.rotate(angle, QVector3D(1,0,0));
        break;
    case 'y':
        m_trans.rotate(angle, QVector3D(0,1,0));
        break;
    case 'z':
        m_trans.rotate(angle, QVector3D(0,0,1));
        break;
    default:
        break;
  }
}

void SceneNode::scale(const Vector3D& amount)
{
    m_trans.scale(amount);
}

void SceneNode::translate(const Vector3D& amount)
{
    QMatrix4x4 trans;
    trans.translate(amount);

    if(is_joint()) {
        m_trans = trans * m_trans; //
    }
    else {
        m_trans.translate(amount);    
    }
    
}

bool SceneNode::is_joint() const
{
  return false;
}

bool SceneNode::is_geometry() const
{
  return false;
}
bool GeometryNode::is_geometry() const
{
  return true;
}


JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

void JointNode::walk_gl(Viewer *viewer, QMatrix4x4 transform, bool picking) const
{
    // QMatrix4x4 newMat = m_trans * matrixStack.top();
    // matrixStack.push(newMat);

    for (auto child = m_children.begin(); child != m_children.end(); ++child) {
        (*child)->walk_gl(viewer, transform * m_trans, picking);
    }
    
    // matrixStack.pop();
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;

  rotate('x', init);
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;

  rotate('y', init);
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{

}

void GeometryNode::walk_gl(Viewer *viewer, QMatrix4x4 transform, bool picking) const
{

    if(selected) {
        viewer->set_colour(QColor(255,255,0),QColor(0,0,0),QColor(100,100,100),0);
    } else {
        viewer->set_material(m_material);    
    }
    
    viewer->draw_sphere(transform * m_trans);

    for (auto child = m_children.begin(); child != m_children.end(); ++child) {
        (*child)->walk_gl(viewer, transform * m_trans, picking);
    }
}
 
