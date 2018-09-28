#ifndef SCENE_HPP
#define SCENE_HPP

//Name: Lawson Fulton
//UserID: ljfulton
//Student#: 20381453

#include <list>
#include <vector>
#include <stack>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "Viewer.hpp"


class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual void walk_gl(Viewer *viewer, QMatrix4x4 transform, bool picking = false) const;

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.inverted();
  }

  void apply_transform(const Matrix4x4& m)
  {
    m_trans = m * m_trans;
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  std::string getName() {
    return m_name;
  }
  std::vector<SceneNode*> getJointNodes() {
    std::vector<SceneNode*> joints;
    if(is_joint()) {
      joints.push_back(this);
    }

    for (auto child = m_children.begin(); child != m_children.end(); ++child) {
        std::vector<SceneNode*> childJoints = (*child)->getJointNodes();
        joints.reserve(joints.size() + distance(childJoints.begin(),childJoints.end()));
        joints.insert(joints.end(),childJoints.begin(),childJoints.end());
    }
      
    return joints;
  }


  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  virtual bool is_geometry() const;

  SceneNode* getNode(const std::string &name){
    if (m_name == name) {
      return this;
    } else {
      SceneNode* node = 0;
      for (auto child = m_children.begin(); child != m_children.end(); ++child) {
        node = (*child)->getNode(name);
        if(node) {
          return node;
        }
      }
      return node;
    }
  }

  void toggleSelected() {
    selected = ! selected;
    //also toggle geometry children
    if(!this->is_geometry()) {
      for (auto child = m_children.begin(); child != m_children.end(); ++child) {
        if((*child)->is_geometry()) (*child)->toggleSelected();
      }
    }
  }

  void toggleChildrenSelected() {
    for (auto child = m_children.begin(); child != m_children.end(); ++child) {
       (*child)->toggleSelected();
    }
  }

  bool isSelected() {
    return selected;
  }
  
protected:
  
  // Useful for picking
  int m_id;
  std::string m_name;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;

  bool selected;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual void walk_gl(Viewer *viewer, QMatrix4x4 transform, bool bicking = false) const;

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  
protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  virtual void walk_gl(Viewer *viewer, QMatrix4x4 transform, bool picking = false) const;

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }
  virtual bool is_geometry() const;

protected:
  Material* m_material;
  Primitive* m_primitive;
};

#endif
