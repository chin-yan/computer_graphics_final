#pragma once

#include "rt_ray.h"
#include "rt_hitable.h"

namespace rt {

// 声明一个用于生成随机点的函数
glm::vec3 random_in_unit_sphere();

class Material {
public:
    virtual bool scatter(const Ray& ray_in, const HitRecord& rec,
                         glm::vec3& attenuation, Ray& scattered) const = 0;
};

// 朗伯特（漫反射）材质
class Lambertian : public Material {
public:
    Lambertian(const glm::vec3& a) : albedo(a) {}
    
    virtual bool scatter(const Ray& ray_in, const HitRecord& rec,
                         glm::vec3& attenuation, Ray& scattered) const {
        glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = Ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }
    
    glm::vec3 albedo;
};

// 金属（反射）材质
class Metal : public Material {
public:
    Metal(const glm::vec3& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}
    
    virtual bool scatter(const Ray& ray_in, const HitRecord& rec,
                         glm::vec3& attenuation, Ray& scattered) const {
        glm::vec3 reflected = glm::reflect(glm::normalize(ray_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return (glm::dot(scattered.direction(), rec.normal) > 0);
    }
    
    glm::vec3 albedo;
    float fuzz;
};

// 反射函数
inline glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
    return v - 2 * glm::dot(v, n) * n;
}
}  // namespace rt
