#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include <cmath>

#define M_PI  3.14159265358979323846

class Object {
public:
    // Constructor
    Object() {
        setupMesh();
    }

    // Load model (e.g., from obj file)
    void loadModel(const std::string& path) {
        // Implementation for loading model
        // Can use libraries like Assimp
    }

    // Draw object
    void draw(Shader& shader) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Draw ring
    void drawRing(Shader& shader) {
        glBindVertexArray(ringVAO);
        glDrawElements(GL_TRIANGLES, ringIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Setup ring mesh
    void setupRingMesh(float innerRadius, float outerRadius, unsigned int segments) {
        generateRing(innerRadius, outerRadius, segments);

        glGenVertexArrays(1, &ringVAO);
        glGenBuffers(1, &ringVBO);
        glGenBuffers(1, &ringEBO);

        glBindVertexArray(ringVAO);

        glBindBuffer(GL_ARRAY_BUFFER, ringVBO);
        glBufferData(GL_ARRAY_BUFFER, ringVertices.size() * sizeof(float), &ringVertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ringEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ringIndices.size() * sizeof(unsigned int), &ringIndices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

private:
    // Model data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    // Ring data
    std::vector<float> ringVertices;
    std::vector<unsigned int> ringIndices;
    unsigned int ringVAO, ringVBO, ringEBO;

    // Initialize buffer data
    void setupMesh() {
        generateSphere(1.0f, 36, 18);  // Sphere radius = 1.0f, 36 longitude segments, 18 latitude segments

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void generateSphere(float radius, unsigned int longitudeSegments, unsigned int latitudeSegments) {
        vertices.clear();
        indices.clear();

        for (unsigned int y = 0; y <= latitudeSegments; ++y) {
            for (unsigned int x = 0; x <= longitudeSegments; ++x) {
                float xSegment = (float)x / (float)longitudeSegments;
                float ySegment = (float)y / (float)latitudeSegments;
                float xPos = radius * cos(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);
                float yPos = radius * cos(ySegment * M_PI);
                float zPos = radius * sin(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);

                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);
                vertices.push_back(xPos); // Normal vector (x, y, z)
                vertices.push_back(yPos);
                vertices.push_back(zPos);
                vertices.push_back(xSegment); // Texture coordinates (u, v)
                vertices.push_back(ySegment);
            }
        }

        for (unsigned int y = 0; y < latitudeSegments; ++y) {
            for (unsigned int x = 0; x < longitudeSegments; ++x) {
                unsigned int i1 = y * (longitudeSegments + 1) + x;
                unsigned int i2 = i1 + longitudeSegments + 1;

                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i1 + 1);

                indices.push_back(i1 + 1);
                indices.push_back(i2);
                indices.push_back(i2 + 1);
            }
        }
    }

    void generateRing(float innerRadius, float outerRadius, unsigned int segments) {
        ringVertices.clear();
        ringIndices.clear();

        for (unsigned int i = 0; i <= segments; ++i) {
            float theta = i * 2.0f * M_PI / segments;
            float x = cos(theta);
            float z = sin(theta);

            ringVertices.push_back(x * innerRadius);
            ringVertices.push_back(0.0f);
            ringVertices.push_back(z * innerRadius);
            ringVertices.push_back(0.0f); // Normal vector (x, y, z)
            ringVertices.push_back(1.0f);
            ringVertices.push_back(0.0f);
            ringVertices.push_back(0.0f); // Texture coordinates (u, v)
            ringVertices.push_back(0.0f);

            ringVertices.push_back(x * outerRadius);
            ringVertices.push_back(0.0f);
            ringVertices.push_back(z * outerRadius);
            ringVertices.push_back(0.0f); // Normal vector (x, y, z)
            ringVertices.push_back(1.0f);
            ringVertices.push_back(0.0f);
            ringVertices.push_back(1.0f); // Texture coordinates (u, v)
            ringVertices.push_back(1.0f);
        }

        for (unsigned int i = 0; i < segments; ++i) {
            ringIndices.push_back(2 * i);
            ringIndices.push_back(2 * i + 1);
            ringIndices.push_back(2 * (i + 1));

            ringIndices.push_back(2 * (i + 1));
            ringIndices.push_back(2 * i + 1);
            ringIndices.push_back(2 * (i + 1) + 1);
        }
    }
};

#endif
