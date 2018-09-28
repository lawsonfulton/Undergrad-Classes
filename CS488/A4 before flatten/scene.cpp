#include "scene.hpp"
#include "ray.hpp"

#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

std::list<GeometryNode> SceneNode::getFlattened() const {
  std::list<GeometryNode> flattened;

  for (auto nodeIt = m_children.begin(); nodeIt != m_children.end(); ++nodeIt) {
    std::list<GeometryNode> childFlattened = (*nodeIt)->getFlattened();

    //Transform them
    for(auto childIt = childFlattened.begin(); childIt != childFlattened.end(); ++childIt) {
      childIt->m_trans = childIt->m_trans * m_trans;
      flattened.push_back(*childIt);
    }

    // //Extend the list
    // flattened.reserve(flattened.size() + distance(childFlattened.begin(),childFlattened.end()));
    // flattened.insert(flattened.end(),childFlattened.begin(),childFlattened.end());
  }

  return flattened;
}

void SceneNode::rotate(char axis, double angle)
{
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
  set_transform(m_trans);
}

void SceneNode::scale(const Vector3D& amount)
{
    m_trans.scale(amount);
    set_transform(m_trans);
}

void SceneNode::translate(const Vector3D& amount)
{
    m_trans.translate(amount);    
    set_transform(m_trans);
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

bool SceneNode::is_geometry() const
{
  return false;
}
bool GeometryNode::is_geometry() const
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
 
bool GeometryNode::computeIntersection(const Ray &ray, Intersection &i) {
  double t = DBL_INF;
  Vector3D normal;
  bool intersects = m_primitive->rayIntersection(ray, t, normal);

  i.node = this;
  i.ray = &ray;
  i.t = t;
  i.normal = normal;

  return intersects;
}

const Material* GeometryNode::get_material() const {
  return m_material;
}

Material* GeometryNode::get_material() {
  return m_material;
}

std::list<GeometryNode> GeometryNode::getFlattened() const {
  return std::list<GeometryNode>(1, *this);
}



