#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace rt {

struct RTContext {
    int width = 500;
    int height = 500;
    std::vector<glm::vec4> image;
    bool freeze = false;
    int current_frame = 0;
    int current_line = 0;
    int max_frames = 1000;
    int max_bounces = 3;
    float epsilon = 2e-4f;
    glm::mat4 view = glm::mat4(1.0f);
    glm::vec3 ground_color = glm::vec3(0.5f, 0.5f, 0.5f);  // 几乎黑色的地面
    glm::vec3 sky_color = glm::vec3(0.1f, 0.1f, 0.3f);        // 深蓝色的天空
    bool show_normals = true;
    // Add more settings and parameters here
    int samples_per_pixel = 16;
    bool enable_gamma_correction = true;  // 控制是否開啟Gamma校正
    float metallic_roughness = 0.0f;      // 金屬材質的粗糙度
    float material_intensity = 1.0f;      // 材質強度
};

void setupScene(RTContext &rtx, const char *mesh_filename);
void updateImage(RTContext &rtx);
void resetImage(RTContext &rtx);
void resetAccumulation(RTContext &rtx);

}  // namespace rt
