//
// Created by thomas on 11/26/18.
//

#include "Square.h"

GLGeometry::Square::Square(unsigned int code) : Drawable(code) {

    std::vector<glm::vec3> vertexs;
    vertexs.emplace_back(glm::vec3(-1, -1, 0));
    vertexs.emplace_back(glm::vec3( 1, -1, 0));
    vertexs.emplace_back(glm::vec3( 1,  1, 0));
    vertexs.emplace_back(glm::vec3(-1,  1, 0));

    std::vector<GLuint> indices;
    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);

    indices.emplace_back(0);
    indices.emplace_back(3);
    indices.emplace_back(2);


    mVertexArrayObject.add(VERTEX_ID, std::make_shared<GLTools::ArrayBuffer>(vertexs));
    mVertexArrayObject.add(NORMAL_ID, std::make_shared<GLTools::ArrayBuffer>(vertexs));
    mVertexArrayObject.add(TEXTCOORD_ID, std::make_shared<GLTools::ArrayBuffer>(vertexs));
    mVertexArrayObject.set(std::make_shared<GLTools::ElementArrayBuffer>(indices));
}

void GLGeometry::Square::render(GLTools::Camera<glm::vec2> &camera, std::shared_ptr<GLTools::Program> program, GLTools::RenderStep renderStep) const {
    program->post(camera);
    program->use();

    mVertexArrayObject.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
