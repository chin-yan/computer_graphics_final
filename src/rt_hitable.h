#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>

#include "rt_ray.h"  // 為了 Ray 類

namespace rt {

// 前向聲明
class Material;

struct HitRecord {
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    Material* material;  // 使用前向聲明的 Material
};

class Hitable {
public:
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
};

} // namespace rt
