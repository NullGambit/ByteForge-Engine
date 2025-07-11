module forge.math.transform;

public import forge.math.vector;
// public import forge.math.quaternion;
// public import forge.math.matrix;

// struct Transform 
// {

//     @property
//     position()
//     {
//         return m_position;
//     }

//     @property
//     position(Vec3 position)
//     {
//         m_isDirty = true;
//         m_position = position;
//     }

//     @property
//     scale()
//     {
//         return m_scale;
//     }

//     @property
//     scale(Vec3 scale)
//     {
//         m_isDirty = true;
//         m_scale = scale;
//     }

//     @property
//     rotation()
//     {
//         return m_rotation;
//     }

//     @property
//     rotation(Quat rotation)
//     {
//         m_isDirty = true;
//         m_rotation = rotation;
//     }

//     @property 
//     isDirty()
//     {
//         return m_isDirty;
//     }

// private:
//     Vec3 m_position;
//     Vec3 m_scale;
//     Quat m_rotation;
//     Mat4 m_model;
//     bool m_isDirty;
// }