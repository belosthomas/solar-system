// Copyright (C) BELOS Thomas. All Rights Reserved.

#ifndef SOLAR_SYSTEM_PROGRAM_H
#define SOLAR_SYSTEM_PROGRAM_H

#define VERTEX_ID 0
#define NORMAL_ID 1
#define TEXTCOORD_ID 2

#include <memory>
#include <string>
#include <map>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/ext.hpp>

#include "Shader"
#include "Model"
#include "View"
#include "LightView"
#include "Projection"

/**
 * A namespace regrouping all the GLTools OpenGL wrapper.
 */
namespace GLTools {

    /**
     * Permits to create OpenGL program from shaders, and to post any kind of values on it !
     */
    class Program {

    public:
        /**
         * Create one vertex shader, one fragment shader, and a fragment from these two
         * @param vertexPath The path on the vertex source code
         * @param fragmentPath The path on the fragment source code
         */
        Program(const std::string &vertexPath, const std::string &fragmentPath);

        /**
         * Create a program from one vertex shader, and one fragment shader
         * @param pVertex The vertex shader
         * @param pFragment The fragment shader
         */
        Program(std::shared_ptr<Shader> pVertex, std::shared_ptr<Shader> pFragment);

        /**
         * Create a program from a list of shader
         * @param shaders the list of shaders
         */
        Program(std::vector<std::shared_ptr<GLTools::Shader>> shaders);

        /**
         * The destructor is virtual.
         */
        virtual ~Program();

        /**
         * Use this shader.
         */
        void use() const;

        /**
         * Post a uniform integer to the shader
         * @param name Name of the uniform
         * @param number Value to pass
         */
        void post(const std::string &name, int number);

        /**
         * Post a uniform boolean to the shader
         * @param name Name of the uniform
         * @param number Value to pass
         */
        void post(const std::string &name, bool value);


        /**
         * Post a uniform float to the shader
         * @param name Name of the uniform
         * @param number Value to pass
         */
        void post(const std::string &name, float number);

        /**
         * Post a uniform mat4 to the shader
         * @param name Name of the uniform
         * @param mat Value to pass
         */
        void post(const std::string &name, const glm::mat4 &mat);

        /**
         * Post multiple uniform mat4 to the shader
         * @param name Name of the uniform
         * @param mat Value to pass
         */
        void post(const std::string &name, const std::vector<glm::mat4> &mat);

        /**
         * Post a uniform vec4 to the shader
         * @param name Name of the uniform
         * @param mat Value to pass
         */
        void post(const std::string &name, const glm::vec4 &mat);

        /**
         * Post a uniform mat4 to the shader
         * @param name Name of the uniform
         * @param mat Value to pass
         */
        void post(const std::string &name, const glm::mat3 &mat);

        /**
         * Post a uniform vec4 to the shader
         * @param name Name of the uniform
         * @param mat Value to pass
         */
        void post(const std::string &name, const glm::vec3 &mat);

        /**
         * Post a uniform texture to the shader
         * @param name The Name of the uniform
         * @param texture The texture unit id
         */
        void postTexture(const std::string &name, GLint texture);

        /**
         * Post the information of the camera.
         * @tparam vecType Either 2D or 3D
         * @param camera The camera to post
         */
        inline void post(const Model2D &model) {
            post("u", model);
        }

        inline void post(const std::string &name, const Model2D &model) {
            post(name + "ModelMatrix", model.getMatrix());
            post(name + "NormalMatrix", glm::transpose(glm::inverse(model.getMatrix())));
            post(name + "MVMatrix", model.getMatrix());
            post(name + "MVPMatrix", model.getMatrix());
        }

        inline void post(const Projection &projection, const Model3D &model, const View &view) {
            post("u", projection, model, view);
        }

        inline void post(const std::string &name, const Projection &projection, const Model3D &model, const View &view) {
            glm::mat4 modelView = view.getMatrix() * model.getMatrix();
            post(name + "ModelMatrix", model.getMatrix());
            post(name + "ViewMatrix", view.getMatrix());
            post(name + "ProjectionMatrix", projection.getMatrix());
            post(name + "NormalMatrix", glm::transpose(glm::inverse(modelView)));
            post(name + "MVMatrix", modelView);
            post(name + "MVPMatrix", projection.getMatrix() * modelView);
        }

        inline void post(const Projection &projection, const Model3D &model, const LightView &lightView,  const View &view) {
            post("u", projection, model, lightView, view);
        }

        inline void post(const std::string &name, const Projection &projection, const Model3D &model, const LightView &lightView, const View &view) {
            std::vector<glm::mat4> views = lightView.getMatrices(), modelView, modelViewProjection;
            for (int i = 0; i < views.size(); i++) {
                modelView.push_back(views[i] * model.getMatrix());
                // modelViewProjection.push_back(projection.getMatrix() * views[i] * model.getMatrix() * glm::inverse(view.getMatrix()));
                modelViewProjection.push_back(projection.getMatrix() * views[i]);
            }

            post(name + "VPMatrix", modelViewProjection);
            post(name + "ModelMatrix", model.getMatrix());
        }

    protected:
        void uniform(const std::string &name);

    private:
        void initialize();

        GLuint mId;
        std::vector<std::shared_ptr<GLTools::Shader>> mShaders;
        std::map<std::string, GLint> mUniformMap;

    };
}


#endif //SOLAR_SYSTEM_PROGRAM_H
