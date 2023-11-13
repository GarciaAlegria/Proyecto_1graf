#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include "fragment.h"

glm::vec3 L = glm::vec3(0.0f, 0.0f, -1.0f);

std::pair<float, float> barycentricCoordinates(const glm::ivec2& P, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) {
  glm::vec3 bary = glm::cross(
    glm::vec3(C.x - A.x, B.x - A.x, A.x - P.x),
    glm::vec3(C.y - A.y, B.y - A.y, A.y - P.y)
  );

  if (abs(bary.z) < 1) {
    return std::make_pair(-1, -1);
  }

  return std::make_pair(
      bary.y / bary.z,
      bary.x / bary.z
    );  
}

std::vector<Fragment> triangle(const Vertex& a, const Vertex& b, const Vertex& c, size_t width, size_t height) {
    std::vector<Fragment> fragments;
    glm::vec3 A = a.position;
    glm::vec3 B = b.position;
    glm::vec3 C = c.position;

    float minX = std::min(std::min(A.x, B.x), C.x);
    float minY = std::min(std::min(A.y, B.y), C.y);
    float maxX = std::max(std::max(A.x, B.x), C.x);
    float maxY = std::max(std::max(A.y, B.y), C.y);

    // Iterate over each point in the bounding box
    for (int y = static_cast<int>(std::ceil (minY)); y <= static_cast<int>(std::floor (maxY)); ++y) {
      for (int x = static_cast<int>(std::ceil (minX)); x <= static_cast<int>(std::floor (maxX)); ++x) {
        if (x < 0 || y < 0 || y > width || x > height)
          continue;

        glm::ivec2 P(x, y);
        auto barycentric = barycentricCoordinates(P, A, B, C);
        float w = 1 - barycentric.first - barycentric.second;
        float v = barycentric.first;
        float u = barycentric.second;
        float epsilon = 1e-10;

        if (w < -epsilon || v < -epsilon || u < -epsilon)
          continue;

        double z = A.z * w + B.z * v + C.z * u;

        glm::vec3 normal = glm::normalize(
                a.normal * w + b.normal * v + c.normal * u
        );

        float intensity = glm::dot(normal, L);

        if (intensity < 0)
            continue;

        glm::vec3 original = a.original * w + b.original * v + c.original * u;

        fragments.push_back(
          Fragment {
            static_cast<uint16_t>(P,x),
            static_cast<uint16_t>(P,y),
            z,
            Color(122, 122, 122),
            intensity, original}
        );
      }
    }
  
  return fragments;
}