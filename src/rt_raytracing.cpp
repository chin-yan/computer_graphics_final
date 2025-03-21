#include "rt_raytracing.h"
#include "rt_ray.h"
#include "rt_hitable.h"
#include "rt_sphere.h"
#include "rt_triangle.h"
#include "rt_box.h"
#include "rt_material.h"  // 确保包含新的材质头文件

#include "cg_utils2.h"
#include <stdlib.h>

namespace rt {

// 全局场景变量
struct Scene {
    Sphere ground;
    std::vector<Sphere> spheres;
    std::vector<Box> boxes;
    std::vector<Triangle> mesh;
    Box mesh_bbox;
    // 存储材质，以便后续清理内存
    std::vector<Material*> materials;
    
    // 析构函数清理材质
    ~Scene() {
        for (auto material : materials) {
            delete material;
        }
    }
} g_scene;

// 已经存在的 random_in_unit_sphere 函数实现
glm::vec3 random_in_unit_sphere()
{
    glm::vec3 p;
    do {
        p = 2.0f * glm::vec3(drand48(), drand48(), drand48()) - glm::vec3(1.0f);
    } while (glm::length(p) >= 1.0f);
    return p;
}

// 碰撞检测函数
bool hit_world(const Ray &r, float t_min, float t_max, HitRecord &rec)
{
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    if (g_scene.ground.hit(r, t_min, closest_so_far, temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
    }
    for (int i = 0; i < g_scene.spheres.size(); ++i) {
        if (g_scene.spheres[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    for (int i = 0; i < g_scene.boxes.size(); ++i) {
        if (g_scene.boxes[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    for (int i = 0; i < g_scene.mesh.size(); ++i) {
        if (g_scene.mesh[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

// 修改 color 函数以使用材质
glm::vec3 color(RTContext &rtx, const Ray &r, int max_bounces)
{
    if (max_bounces < 0) return glm::vec3(0.0f);

    HitRecord rec;
    if (hit_world(r, rtx.epsilon, 9999.0f, rec)) {
        rec.normal = glm::normalize(rec.normal);
        if (rtx.show_normals) { return rec.normal * 0.5f + 0.5f; }

        Ray scattered;
        glm::vec3 attenuation;
        
        // 检查是否有材质，如果没有则使用默认行为
        if (rec.mat_ptr && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(rtx, scattered, max_bounces - 1);
        } else {
            // 如果没有材质或散射失败，使用旧的漫反射行为
            glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            return 0.5f * color(rtx, Ray(rec.p, target - rec.p), max_bounces - 1);
        }
    }

    // 天空颜色
    glm::vec3 unit_direction = glm::normalize(r.direction());
    float t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - t) * rtx.ground_color + t * rtx.sky_color;
}
// 修改 setupScene 函数添加更多球体和材质
void setupScene(RTContext &rtx, const char *filename)
{
    // 设置高对比度背景 - 黑色地面与亮色天空对比
    rtx.ground_color = glm::vec3(0.0f, 0.0f, 0.0f);  // 纯黑色的地面
    rtx.sky_color = glm::vec3(0.6f, 0.7f, 1.0f);     // 明亮的天蓝色

    // 创建材质
    Material* ground_material = new Lambertian(glm::vec3(0.0f, 0.0f, 0.0f));  // 纯黑色地面材质
    g_scene.materials.push_back(ground_material);
    
    // 创建极端金属材质 - 完美反射、极亮的银色
    Material* metal_material = new Metal(glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);  // 纯白色金属，无模糊
    g_scene.materials.push_back(metal_material);

    // 设置地面 - 使用纯黑色材质
    g_scene.ground = Sphere(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, ground_material);
    
    // 清空球体列表
    g_scene.spheres.clear();

    // 加载兔子模型，使用极端金属材质
    cg::OBJMesh mesh;
    cg::objMeshLoad(mesh, filename);
    g_scene.mesh.clear();
    for (int i = 0; i < mesh.indices.size(); i += 3) {
        int i0 = mesh.indices[i + 0];
        int i1 = mesh.indices[i + 1];
        int i2 = mesh.indices[i + 2];
        glm::vec3 v0 = mesh.vertices[i0] + glm::vec3(0.0f, 0.135f, 0.0f);
        glm::vec3 v1 = mesh.vertices[i1] + glm::vec3(0.0f, 0.135f, 0.0f);
        glm::vec3 v2 = mesh.vertices[i2] + glm::vec3(0.0f, 0.135f, 0.0f);
        g_scene.mesh.push_back(Triangle(v0, v1, v2, metal_material));
    }
}
// MODIFY THIS FUNCTION!
void updateLine(RTContext &rtx, int y)
{
    int nx = rtx.width;
    int ny = rtx.height;
    float aspect = float(nx) / float(ny);
    glm::vec3 lower_left_corner(-1.0f * aspect, -1.0f, -1.0f);
    glm::vec3 horizontal(2.0f * aspect, 0.0f, 0.0f);
    glm::vec3 vertical(0.0f, 2.0f, 0.0f);
    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::mat4 world_from_view = glm::inverse(rtx.view);

    // 可以在此处声明调试标志
    static bool debug_first_pixel = true;

    // 你可以尝试通过取消注释此行来并行化此循环：
    //#pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < nx; ++x) {
        // 在这里可以正确使用 x 变量
        if (debug_first_pixel && x == nx/2 && y == ny/2) {
            std::cout << "调试中心像素..." << std::endl;
            std::cout << "场景中的球体数量: " << g_scene.spheres.size() << std::endl;
            std::cout << "场景中的三角形数量: " << g_scene.mesh.size() << std::endl;
            debug_first_pixel = false;
        }

        glm::vec3 col(0.0f);
        
        // 以下是原有代码
        if (rtx.current_frame <= 0) {
            // 这里我们让第一帧与旧图像混合，
            // 以平滑重置累积时的过渡
            glm::vec4 old = rtx.image[y * nx + x];
            rtx.image[y * nx + x] = glm::clamp(old / glm::max(1.0f, old.a), 0.0f, 1.0f);
        }
        
        // 对每个像素进行多次采样
        for (int s = 0; s < rtx.samples_per_pixel; s++) {
            // 添加随机抖动
            float random_u = float(rand()) / float(RAND_MAX); // 0-1之间的随机数
            float random_v = float(rand()) / float(RAND_MAX);
            
            // 计算带抖动的 u 和 v
            float u = (float(x) + random_u) / float(nx);
            float v = (float(y) + random_v) / float(ny);
            
            Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            r.A = glm::vec3(world_from_view * glm::vec4(r.A, 1.0f));
            r.B = glm::vec3(world_from_view * glm::vec4(r.B, 0.0f));
            
            // 累积颜色结果
            col += color(rtx, r, rtx.max_bounces);
        }
        
        // 计算平均值并更新图像
        col = glm::sqrt(col / float(rtx.samples_per_pixel)); // gamma校正
        rtx.image[y * nx + x] += glm::vec4(col, 1.0f);
    }
}

void updateImage(RTContext &rtx)
{
    if (rtx.freeze) return;                    // Skip update
    rtx.image.resize(rtx.width * rtx.height);  // Just in case...

    updateLine(rtx, rtx.current_line % rtx.height);

    if (rtx.current_frame < rtx.max_frames) {
        rtx.current_line += 1;
        if (rtx.current_line >= rtx.height) {
            rtx.current_frame += 1;
            rtx.current_line = rtx.current_line % rtx.height;
        }
    }
}

void resetImage(RTContext &rtx)
{
    rtx.image.clear();
    rtx.image.resize(rtx.width * rtx.height);
    rtx.current_frame = 0;
    rtx.current_line = 0;
    rtx.freeze = false;
}

void resetAccumulation(RTContext &rtx)
{
    rtx.current_frame = -1;
}

}  // namespace rt
