
#include "OpenglNoel.h"
#include "external/imgui/imgui.h"

#include <iostream>

OpenglNoel::OpenglNoel() : GLTools::Window("Solar System"), mSphere(0, 256, 256), mCube(0), mSquare(0), mMouseSet(false), mTextureManager("res/textures") {

    mRender3DProgram = std::make_shared<GLTools::Program>("res/shaders/basic3d.vs.glsl", "res/shaders/basic3d.fs.glsl");
    mGeometryProgram = std::make_shared<GLTools::Program>("res/shaders/geometry/default.vs.glsl", "res/shaders/geometry/default.fs.glsl");

    mShadingProgram = std::make_shared<GLTools::Program>("res/shaders/shading/default.vs.glsl", "res/shaders/shading/default.fs.glsl");
    mShadingAmbienProgram = std::make_shared<GLTools::Program>("res/shaders/shading/default.vs.glsl", "res/shaders/shading/ambient.fs.glsl");
    mShadingDiffuseProgram = std::make_shared<GLTools::Program>("res/shaders/shading/default.vs.glsl", "res/shaders/shading/diffuse.fs.glsl");
    mShadingNormalProgram = std::make_shared<GLTools::Program>("res/shaders/shading/default.vs.glsl", "res/shaders/shading/normal.fs.glsl");
    mShadingShadowProgram = std::make_shared<GLTools::Program>("res/shaders/shading/default.vs.glsl", "res/shaders/shading/shadow.fs.glsl");
    mShadingSpecularProgram = std::make_shared<GLTools::Program>("res/shaders/shading/default.vs.glsl", "res/shaders/shading/specular.fs.glsl");


    // mShadowProgram = std::make_shared<GLTools::Program>("res/shaders/shadow/directionalSM.vs.glsl", "res/shaders/shadow/directionalSM.fs.glsl");

    std::shared_ptr<GLTools::Shader> pVertex = std::make_shared<GLTools::Shader>(GL_VERTEX_SHADER, "res/shaders/shadow/directionalSM.vs.glsl");
    std::shared_ptr<GLTools::Shader> pGeometry = std::make_shared<GLTools::Shader>(GL_GEOMETRY_SHADER, "res/shaders/shadow/directionalSM.gs.glsl");
    std::shared_ptr<GLTools::Shader> pFragment = std::make_shared<GLTools::Shader>(GL_FRAGMENT_SHADER, "res/shaders/shadow/directionalSM.fs.glsl");

    std::vector<std::shared_ptr<GLTools::Shader>> mShadowShaders;
    mShadowShaders.push_back(std::make_shared<GLTools::Shader>(GL_VERTEX_SHADER, "res/shaders/shadow/directionalSM.vs.glsl"));
    mShadowShaders.push_back(std::make_shared<GLTools::Shader>(GL_GEOMETRY_SHADER, "res/shaders/shadow/directionalSM.gs.glsl"));
    mShadowShaders.push_back(std::make_shared<GLTools::Shader>(GL_FRAGMENT_SHADER, "res/shaders/shadow/directionalSM.fs.glsl"));

    mShadowProgram = std::make_shared<GLTools::Program>(mShadowShaders);


    mShadowView = GLTools::LightView(glm::vec3(0, 0, 0));
    mShadowProjection = GLTools::PerspectiveProjection(90.0f);

    setDeferred(true);
}

void OpenglNoel::render(GLTools::RenderStep renderStep) {

    if (renderStep == GLTools::RENDER_SCREEN) {

        mModel.identity();

        mRender3DProgram->use();
        mRender3DProgram->post("uLightPosition", mCamera.getMatrix() * glm::vec4(mShadowView.getLightPosition(), 1.0f));
        mRender3DProgram->post("uCameraPosition", glm::vec4(mCamera.getPosition(), 1.0f));
        mRender3DProgram->post("uAmbientPower", mLightToolAmbient);
        mRender3DProgram->post("uDiffusePower", mLightToolDiffuse);
        mRender3DProgram->post("uSpecularPower", mLightToolSpecular);
        // mRender3DProgram->post(mShadowProjection, mModel, mShadowView);
        mRender3DProgram->post(mProjection, mModel, mCamera);

        mScene->render(mRender3DProgram, renderStep);
        renderLight(mRender3DProgram, renderStep);
        renderGui(renderStep);

    }

    if (renderStep == GLTools::RENDER_DEFERRED_FRAMEBUFFER) {

        mModel.identity();

        mGeometryProgram->post(mProjection, mModel, mCamera);
        mScene->render(mGeometryProgram, renderStep);
        renderLight(mGeometryProgram, renderStep);


    }

    if (renderStep == GLTools::RENDER_DEFERRED_SHADOW) {

        mModel.identity();

        mShadowProgram->post(mShadowProjection, mModel, mShadowView, mCamera);
        mShadowProgram->post("uLightPosition", glm::vec4(mShadowView.getLightPosition(), 1.0f));
        mShadowProgram->post("uFarPlane", mScene->getBoundingBoxDiagonal());

        mScene->render(mShadowProgram, renderStep);
    }

    if (renderStep == GLTools::RENDER_DEFERRED_SCREEN) {

        mModel2D.identity();

        if (mSplittedMode) {

            renderDeferred(mShadingAmbienProgram, glm::vec2(-2.0 / 3.0,-1.0 / 2.0), glm::vec2(1.0 / 3.0,1.0 / 2.0));
            renderDeferred(mShadingDiffuseProgram, glm::vec2(-0.0 / 3.0,-1.0 / 2.0), glm::vec2(1.0 / 3.0,1.0 / 2.0));
            renderDeferred(mShadingNormalProgram, glm::vec2(2.0 / 3.0,-1.0 / 2.0), glm::vec2(1.0 / 3.0,1.0 / 2.0));
            renderDeferred(mShadingShadowProgram, glm::vec2(-2.0 / 3.0,1.0 / 2.0), glm::vec2(1.0 / 3.0,1.0 / 2.0));
            renderDeferred(mShadingSpecularProgram, glm::vec2(0.0 / 3.0,1.0 / 2.0), glm::vec2(1.0 / 3.0,1.0 / 2.0));
            renderDeferred(mShadingProgram, glm::vec2(2.0 / 3.0,1.0 / 2.0), glm::vec2(1.0 / 3.0,1.0 / 2.0));

        } else {
            renderDeferred(mShadingProgram, glm::vec2(0,0), glm::vec2(1,1));

        }

        renderGui(renderStep);


    }

}

void OpenglNoel::renderLight(std::shared_ptr<GLTools::Program> program, GLTools::RenderStep renderStep) {
    mModel.pushMatrix();

    mModel.translate(mShadowView.getLightPosition());
    mModel.scale(10.0f);

    program->post("uAmbientHasTexture", GL_FALSE);
    program->post("uDiffuseHasTexture", GL_FALSE);
    program->post("uSpecularHasTexture", GL_FALSE);
    program->post("uShininessHasTexture", GL_FALSE);
    program->post("uNormalHasTexture", GL_FALSE);
    program->post("uAmbient", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    program->post("uDiffuse", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    program->post("uSpecular", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    program->post("uShininess", 1.0f);
    program->post(mProjection, mModel, mCamera);

    mSphere.render(program, renderStep);

    mModel.popMatrix();
}


void OpenglNoel::renderDeferred(std::shared_ptr<GLTools::Program> program, glm::vec2 position, glm::vec2 size) {

    program->post("uGeometry", mProjection, mModel, mCamera);
    program->post("uLightPosition", glm::vec4(mShadowView.getLightPosition(), 1.0f));
    program->post("uCameraPosition", glm::vec4(mCamera.getPosition(), 1.0f));
    // program->post("uLight", mShadowView.getMatrices());
    program->post("uLightInverseMatrix", (glm::inverse(mCamera.getMatrix())));
    program->post("uLightShadowMapBias",  mLightToolShadowBias);
    program->post("uFarPlane", mScene->getBoundingBoxDiagonal());
    program->postTexture("uGPosition", 0);
    program->postTexture("uGNormal", 1);
    program->postTexture("uGAmbient", 2);
    program->postTexture("uGDiffuse", 3);
    program->postTexture("uGlossyShininess", 4);
    program->postTexture("uGShadow", 5);
    program->post("uAmbientPower", mLightToolAmbient);
    program->post("uDiffusePower", mLightToolDiffuse);
    program->post("uSpecularPower", mLightToolSpecular);
    program->post("uShadowPower", mLightToolShadow);

    mModel2D.pushMatrix();

   // mModelView2D->translate(glm::vec2(-2.0/3.0,-0.5));
    mModel2D.translate(position);
    mModel2D.scale(size);


    program->post(mModel2D);
    //mModelView2D->translate(position);

    mSquare.render(program, GLTools::RENDER_DEFERRED_SCREEN);
    mModel2D.popMatrix();

}

void OpenglNoel::renderGui(GLTools::RenderStep renderStep) {

    newImguiFrame();


    // Create a window called "My First Tool", with a menu bar.
    ImGui::Begin("Light Tool", &mLightToolActive);

    ImGui::SliderFloat("ambient", &mLightToolAmbient, 0.0f, 1.0f);
    ImGui::SliderFloat("diffuse", &mLightToolDiffuse, 0.0f, 1.0f);
    ImGui::SliderFloat("specular", &mLightToolSpecular, 0.0f, 1.0f);
    ImGui::SliderFloat("shadow", &mLightToolShadow, 0.0f, 1.0f);
    ImGui::SliderFloat("shadow bias", &mLightToolShadowBias, 0.0f, 1.0f);

    // mShadowView.setPhi(mLightToolPhi);
    // mShadowView.setTheta(mLightToolTheta);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void OpenglNoel::keyboard(Uint32 type, bool repeat, int key) {

    switch (key) {
        case SDLK_UP:
        case SDLK_z:
            if (!mCameraLock) mCamera.moveFront(3.0f);
            break;
        case SDLK_LEFT:
        case SDLK_q:
            if (!mCameraLock) mCamera.moveLeft(3.0f);
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            if (!mCameraLock) mCamera.moveLeft(-3.0f);
            break;
        case SDLK_DOWN:
        case SDLK_s:
            if (!mCameraLock) mCamera.moveFront(-3.0f);
            break;
        case SDLK_i:
            mShadowView.moveFront(3.0f);
            break;
        case SDLK_j:
            mShadowView.moveLeft(3.0f);
            break;
        case SDLK_l:
            mShadowView.moveLeft(-3.0f);
            break;
        case SDLK_k:
            mShadowView.moveFront(-3.0f);
            break;
        case SDLK_p:
            mShadowView.moveUp(3.0f);
            break;
        case SDLK_m:
            mShadowView.moveUp(-3.0f);
            break;
        case SDLK_w:
            mCameraLock = !mCameraLock;
            break;
        case SDLK_LSHIFT:
            if (!mCameraLock) mCamera.moveUp(3.0f);
            break;
        case SDLK_SPACE:
            if (!mCameraLock) mCamera.moveUp(-3.0f);
            break;
        case SDLK_h:
            setDeferred(true);
            mSplittedMode = true;
            std::cout << "Set splitted mode to true" << std::endl;
            break;
        case SDLK_f:
            setDeferred(true);
            mSplittedMode = false;
            break;
        case SDLK_g:
            setDeferred(false);
            mSplittedMode = false;
            break;
        default:
            break;
    }
    
}

void OpenglNoel::mouseMove(glm::vec2 mousePosition, unsigned int selection) {
    glm::vec2 diff = mousePosition - mMouseStart;
    mMouseStart = mousePosition;
    
    if (mMouseSet) {
        if (!mCameraLock) mCamera.rotateLeft(diff.x * 2 * M_PI);
        if (!mCameraLock) mCamera.rotateUp(diff.y * 2 * M_PI);
    } else {
        mMouseSet = true;
    }
}

void OpenglNoel::resize(unsigned int width, unsigned int height) {
    mProjection = GLTools::PerspectiveProjection(90.0f, width, height, mScene->getBoundingBoxDiagonal() / 1000.0f, mScene->getBoundingBoxDiagonal());
    mShadowProjection = GLTools::PerspectiveProjection(90.0f, 1.0f, 1.0f, mScene->getBoundingBoxDiagonal() / 1000.0f, mScene->getBoundingBoxDiagonal());
}

bool OpenglNoel::needRenderShadow() {
    return true;
}

void OpenglNoel::useScene(std::shared_ptr<GLScene::Scene> scene) {
    mScene = scene;
    mShadowView = GLTools::LightView(glm::vec3(0, scene->getBoundingBoxDiagonal() / 5.0f, 0));
}
