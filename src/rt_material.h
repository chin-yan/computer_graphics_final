#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "rt_ray.h"
#include "rt_hitable.h"

#include <stdlib.h> // 為了使用 drand48()

namespace rt {

// 輔助函數 - 在單位球體內生成隨機點
inline glm::vec3 random_in_unit_sphere() {
    glm::vec3 p;
    do {
        p = 2.0f * glm::vec3(drand48(), drand48(), drand48()) - glm::vec3(1.0f);
    } while (glm::dot(p, p) >= 1.0f);
    return p;
}

// 反射函數
inline glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
    return v - 2.0f * glm::dot(v, n) * n;
}

// 折射函數
inline bool refract(const glm::vec3& v, const glm::vec3& n, float ni_over_nt, glm::vec3& refracted) {
    glm::vec3 uv = glm::normalize(v);
    float dt = glm::dot(uv, n);
    float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * glm::sqrt(discriminant);
        return true;
    }
    return false;
}

// 施利克近似（Schlick approximation）- 計算反射率
inline float schlick(float cosine, float ref_idx) {
    float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * pow((1.0f - cosine), 5);
}

// 前向聲明
class Ray;
struct HitRecord;

// 材質基類
class Metal : public Material {
public:
    Metal(const glm::vec3& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}
    
    virtual bool scatter(const Ray& r_in, const HitRecord& rec,
                        glm::vec3& attenuation, Ray& scattered) const override {
        glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return (glm::dot(scattered.direction(), rec.normal) > 0);
    }
    
    glm::vec3 albedo;
    float fuzz;
};

// 朗伯特材質（漫反射）
class Lambertian : public Material {
public:
    Lambertian(const glm::vec3& a) : albedo(a) {}
    
    virtual bool scatter(const Ray& r_in, const HitRecord& rec,
                         glm::vec3& attenuation, Ray& scattered) const override {
        glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = Ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }
    
    glm::vec3 albedo;
};

// ... 其他材質類定義 ...

} // namespace rt
