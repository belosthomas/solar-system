#include <iostream>

#include "Framebuffer.h"

GLTools::Framebuffer::Framebuffer(int width, int height) {

    std::cout << "Creating frame buffer object of size " << width << "x" << height << "..." << std::endl;

    for (unsigned int i = 0; i < GBufferTextureCount; i++) {
        std::cout << "Creating texture " << i << std::endl;
        glGenTextures(1, &mGBufferTextures[i]);
        glBindTexture(GL_TEXTURE_2D, mGBufferTextures[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GBufferTextureFormat[i], width, height);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    std::cout << "Generating framebuffer..." << std::endl;
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);

    for (unsigned int i = 0; i < GBufferTextureCount; i++) {

        if ((GBufferTextureType)i == GDepth) {
            std::cout << "Creating Depth Attachment..." << std::endl;
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mGBufferTextures[i], 0);
        } else {
            std::cout << "Creating Color Attachment " << i << " ..." << std::endl;
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mGBufferTextures[i], 0);
        }

    }

    glDrawBuffers(GDrawBuffersSize, GDrawBuffers);

    GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    switch (framebufferStatus) {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:  throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        case GL_FRAMEBUFFER_UNSUPPORTED: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_UNSUPPORTED");
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        default:
            throw std::runtime_error("gl draw framebuffer is not complete. unknown error :(");

    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

GLTools::Framebuffer::~Framebuffer() {

    glDeleteFramebuffers(1, &mFBO);

    for (unsigned int i = 0; i < GBufferTextureCount; i++) {
        glDeleteTextures(1, &mGBufferTextures[i]);
    }

}

void GLTools::Framebuffer::use() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
    glDrawBuffers(GDrawBuffersSize, GDrawBuffers);
}

void GLTools::Framebuffer::useScreen() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GLTools::Framebuffer::bindTextures() {
    for (unsigned int i = 0; i < GDrawBuffersSize; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, mGBufferTextures[i]);
    }
}

GLTools::ShadowFramebuffer::ShadowFramebuffer() {
    glGenTextures(1, &mDirectionalSMCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDirectionalSMCubemap);
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     mDirectionalSMResolution, mDirectionalSMResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glGenFramebuffers(1, &mDirectionalSMFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mDirectionalSMFBO);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDirectionalSMCubemap, 0);

    GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    switch (framebufferStatus) {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:  throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        case GL_FRAMEBUFFER_UNSUPPORTED: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_UNSUPPORTED");
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        default:
            throw std::runtime_error("gl draw framebuffer is not complete. unknown error :(");

    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
/*
    glGenSamplers(1, &mDirectionalSMSampler);
    glSamplerParameteri(mDirectionalSMSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(mDirectionalSMSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(mDirectionalSMSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(mDirectionalSMSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(mDirectionalSMSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glSamplerParameteri(mDirectionalSMSampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
*/

}

GLTools::ShadowFramebuffer::~ShadowFramebuffer() {
    glDeleteFramebuffers(1, &mDirectionalSMFBO);
    glDeleteTextures(1, &mDirectionalSMCubemap);
}

void GLTools::ShadowFramebuffer::use() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mDirectionalSMFBO);
    glDrawBuffer(GL_NONE);
    glViewport(0, 0, mDirectionalSMResolution, mDirectionalSMResolution);
}

void GLTools::ShadowFramebuffer::bindTexture() {
    glActiveTexture(GL_TEXTURE0 + GDrawBuffersSize);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDirectionalSMCubemap);
    // glBindSampler(GDrawBuffersSize, mDirectionalSMSampler);
}

GLTools::ComputeFramebuffer::ComputeFramebuffer(int width, int height) {


    std::cout << "Creating compute frame buffer object of size " << width << "x" << height << "..." << std::endl;

    std::cout << "Creating texture" << std::endl;
    glGenTextures(1, &mGBufferTextureIn);
    glBindTexture(GL_TEXTURE_2D, mGBufferTextureIn);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &mGBufferTextureOut);
    glBindTexture(GL_TEXTURE_2D, mGBufferTextureOut);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Generating framebuffer..." << std::endl;
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGBufferTextureIn, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGBufferTextureOut, 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    switch (framebufferStatus) {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:  throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        case GL_FRAMEBUFFER_UNSUPPORTED: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_UNSUPPORTED");
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: throw std::runtime_error("gl draw framebuffer is not complete. GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        default:
            throw std::runtime_error("gl draw framebuffer is not complete. unknown error :(");

    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

GLTools::ComputeFramebuffer::~ComputeFramebuffer() {

    glDeleteFramebuffers(1, &mFBO);

    glDeleteTextures(1, &mGBufferTextureIn);
    glDeleteTextures(1, &mGBufferTextureOut);


}

void GLTools::ComputeFramebuffer::use() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);
}

void GLTools::ComputeFramebuffer::useRead() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
}

void GLTools::ComputeFramebuffer::bindTextures() {
    glBindImageTexture(0, mGBufferTextureIn, 0, GL_FALSE, 0 ,GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, mGBufferTextureOut, 0, GL_FALSE, 0 ,GL_WRITE_ONLY, GL_RGBA32F);
}

void GLTools::ComputeFramebuffer::useScreen() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
