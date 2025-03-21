#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>

#include "rt_ray.h"

namespace rt {

// 前向声明 Material 类
class Material;

struct HitRecord {
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    Material* mat_ptr;  // 指向材质的指针
};

class Hitable {
public:
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
};

} // namespace rt
