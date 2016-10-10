/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/oculus/WebVROculus.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/IndexBuffer.hpp"

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"
#endif

using namespace minko;
using namespace minko::file;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;
using namespace minko::oculus;

WebVROculus::WebVROculus(int viewportWidth, int viewportHeight, float zNear, float zFar) :
    _zNear(zNear),
    _zFar(zFar),
    _initialized(false)
{
    std::string eval = "";

    // Retrieve VRDisplay device and store it into window.vrDisplay
    eval += "function vrDeviceCallback(vrDisplays) {                                                    \n";
    eval += "    for (var i = 0; i < vrDisplays.length; ++i) {                                          \n";
    eval += "        var vrDisplay = vrDisplays[i];                                                     \n";
    // eval += "        vrDisplay.depthNear = " + std::to_string(_zNear) + "                               \n";
    // eval += "        vrDisplay.depthFar = " + std::to_string(_zFar) +  "                                \n";
    eval += "        window.vrDisplay = vrDisplay;                                                      \n";
    eval += "        if (typeof(VRFrameData) != 'undefined')                                            \n";
    eval += "            window.vrFrameData = new VRFrameData();                                        \n";
    eval += "        break;                                                                             \n";
    eval += "    }                                                                                      \n";
    eval += "}                                                                                          \n";
    eval += "                                                                                           \n";
    eval += "                                                                                           \n";
    eval += "if (navigator.getVRDisplays !== undefined) {                                               \n";
    eval += "    navigator.getVRDisplays().then(vrDeviceCallback);                                      \n";
    eval += "}                                                                                          \n";

    // VRDisplay callbacks
    eval += "function onVRRequestPresent() {                                                            \n";
    eval += "   console.log('onVRRequestPresent');                                                      \n";
    eval += "   var renderCanvas = document.getElementById('canvas');                                   \n";
    eval += "   window.vrDisplay.requestPresent([{ source: renderCanvas }]).then(                       \n";
    eval += "        function () {                                                                      \n";
    eval += "           console.log('Success: requestPresent succeed.')                                 \n";
    eval += "        },                                                                                 \n";
    eval += "        function () {                                                                      \n";
    eval += "           console.log('Error: requestPresent failed.');                                   \n";
    eval += "        }                                                                                  \n";
    eval += "   );                                                                                      \n";
    eval += " };                                                                                        \n";
    eval += "                                                                                           \n";

    eval += "function onVRExitPresent () {                                                              \n";
    eval += "   console.log('onVRExitPresent');                                                         \n";
    eval += "    if (!window.vrDisplay || !window.vrDisplay.isPresenting)                               \n";
    eval += "        return;                                                                            \n";
    eval += "                                                                                           \n";
    eval += "    window.vrDisplay.exitPresent().then(                                                   \n";
    eval += "        function () {                                                                      \n";
    eval += "            console.log('Success: exitPresent succeed.')                                   \n";
    eval += "        },                                                                                 \n";
    eval += "        function () {                                                                      \n";
    eval += "            console.log('Error: exitPresent failed.');                                     \n";
    eval += "        }                                                                                  \n";
    eval += "    );                                                                                     \n";
    eval += "}                                                                                          \n";

    eval += "function onVRPresentChange () {                                                            \n";
    eval += "    onResize();                                                                            \n";
    eval += "}                                                                                          \n";

    eval += "function onResize () {                                                                     \n";
    eval += "   console.log('onResize');                                                                \n";
    eval += "   var renderCanvas = document.getElementById('canvas');                                   \n";
    eval += "   if (window.vrDisplay && window.vrDisplay.isPresenting) {                                \n";
    eval += "       var leftEye = window.vrDisplay.getEyeParameters('left');                            \n";
    eval += "       var rightEye = window.vrDisplay.getEyeParameters('right');                          \n";
    eval += "       console.log('Left eye', leftEye);                                                   \n";
    eval += "       console.log('Right eye', rightEye);                                                 \n";
    eval += "       renderCanvas.width = Math.max(leftEye.renderWidth, rightEye.renderWidth) * 2;       \n";
    eval += "       renderCanvas.height = Math.max(leftEye.renderHeight, rightEye.renderHeight);        \n";
    // eval += "       renderCanvas.width = " + std::to_string(viewportWidth) + ";                         \n";
    // eval += "       renderCanvas.height = " + std::to_string(viewportHeight) + ";                       \n";
    eval += "   } else {                                                                                \n";
    eval += "       renderCanvas.width = renderCanvas.offsetWidth * window.devicePixelRatio;            \n";
    eval += "       renderCanvas.height = renderCanvas.offsetHeight * window.devicePixelRatio;          \n";
    eval += "   }                                                                                       \n";
    eval += "}                                                                                          \n";

    eval += "window.addEventListener('vrdisplayactivate', onVRRequestPresent, false);                   \n";
    eval += "window.addEventListener('vrdisplaydeactivate', onVRExitPresent, false);                    \n";
    eval += "window.addEventListener('vrdisplaypresentchange', onVRPresentChange, false);               \n";
    eval += "window.addEventListener('resize', onResize, false);                                        \n";

    // Put the rendering into the VRDisplay pressing a keyboard key (keycode 70 = "F")
    eval += "window.addEventListener('keydown', function(e) {                                           \n";
    eval += "   if (e.keyCode == 70) {                                                                  \n";
    eval += "       if (!window.vrDisplay)                                                              \n";
    eval += "           return;                                                                         \n";
    eval += "                                                                                           \n";
    eval += "       if (!window.vrDisplay.isPresenting)                                                 \n";
    eval += "           onVRRequestPresent();                                                           \n";
    eval += "       else                                                                                \n";
    eval += "           onVRExitPresent();                                                              \n";
    eval += "   }                                                                                       \n";
    eval += "}, false);                                                                                 \n";

    emscripten_run_script(eval.c_str());
}

void
WebVROculus::initializeVRDevice(std::shared_ptr<component::Renderer> leftRenderer, std::shared_ptr<component::Renderer> rightRenderer, void* window)
{
    _leftRenderer = leftRenderer;
    _rightRenderer = rightRenderer;

    _renderingEndSlot = rightRenderer->renderingEnd()->connect([&](std::shared_ptr<minko::component::Renderer> rightRenderer)
    {
        std::string eval = "";
        eval += "if (!!window.vrDisplay && window.vrDisplay.isPresenting) {                 \n";
        eval += "   console.log('submitFrame');                                             \n";
        eval += "   window.vrDisplay.submitFrame();                                         \n";
        eval += "}                                                                          \n";

        emscripten_run_script_int(eval.c_str());
    });
}

void
WebVROculus::targetAdded()
{
}


void
WebVROculus::targetRemoved()
{
}

void
WebVROculus::initialize(std::shared_ptr<component::SceneManager> sceneManager)
{
}

void
WebVROculus::enable(bool value)
{
}

void
WebVROculus::updateViewport(int viewportWidth, int viewportHeight)
{
    std::cout << "Viewport width: " << viewportWidth << std::endl;
    std::cout << "Viewport height: " << viewportHeight << std::endl;

    if (!_leftRenderer || !_rightRenderer)
    {
        std::cout << "Left or right renderer is null." << std::endl;
        return;
    }

    // TODO: Get the viewport values from the VRDisplay
    // viewportWidth = 2664;
    // viewportHeight = 1586;
    std::string eval = "";
    eval += "var hasVRDisplay = !!window.vrDisplay;                                 \n";
    eval += "if (hasVRDisplay) {                                                    \n";
    eval += "   var pose = hasVRDisplay ? window.vrDisplay.getPose() : null;        \n";
    eval += "   var leftEye = window.vrDisplay.getEyeParameters('left');            \n";
    eval += "   var rightEye = window.vrDisplay.getEyeParameters('right');          \n";
    eval += "   if (hasVRDisplay && !!leftEye && !!rightEye) {                      \n";
    eval += "       leftEye.renderWidth + ' ' + leftEye.renderHeight + ' ' +        \n";
    eval += "       rightEye.renderWidth + ' ' + rightEye.renderHeight              \n";
    eval += "   }                                                                   \n";
    eval += "}                                                                      \n";

    auto viewportString = std::string(emscripten_run_script_string(eval.c_str()));

    if (viewportString != "undefined")
    {
        std::array<uint, 4> viewport;
        std::stringstream ssViewport(viewportString);

        for (auto i = 0; i < 4; i++)
            ssViewport >> viewport[i];

        viewportWidth = std::max(viewport[0], viewport[2]) * 2.f;
        viewportHeight = std::max(viewport[1], viewport[3]);

        std::cout << "New viewport value (from VRDisplay): " << viewportWidth << "x" << viewportHeight << std::endl;
    }
    else
    {
        std::cout << "VRDisplay is not ready yet!" << std::endl;
    }

    // Update left and right eyes renderer
    _leftRenderer->viewport(math::vec4(0, 0, viewportWidth / 2.f, viewportHeight));
    _rightRenderer->viewport(math::vec4(viewportWidth / 2.f, 0, viewportWidth / 2.f, viewportHeight));

    std::cout << "Left renderer viewport: " << std::to_string(math::vec4(0, 0, viewportWidth / 2.f, viewportHeight)) << std::endl;
    std::cout << "Right renderer viewport: " << std::to_string(math::vec4(viewportWidth / 2.f, 0, viewportWidth / 2.f, viewportHeight)) << std::endl;

    // // Don't forget to update the aspect ratio too
    // auto aspectRatio = (viewportWidth / 2.f) / viewportHeight;

    // std::cout << "Aspect ratio: " << aspectRatio << std::endl;

    // std::cout << "Left renderer: " << _leftRenderer << std::endl;
    // std::cout << "Left renderer target: " << _leftRenderer->target() << std::endl;

    // if (_leftRenderer->target() != nullptr)
    // {
    //     std::cout << "Update aspect ratio of the left renderer target" << std::endl;
    //     _leftRenderer->target()->component<PerspectiveCamera>()->aspectRatio(aspectRatio);
    // }
    // if (_rightRenderer->target() != nullptr)
    // {
    //     std::cout << "Update aspect ratio of the right renderer target" << std::endl;
    //     _rightRenderer->target()->component<PerspectiveCamera>()->aspectRatio(aspectRatio);
    // }
}

bool
WebVROculus::detected()
{
    auto eval = std::string("if (navigator.getVRDisplays != undefined) (1); else (0);");
    bool result = emscripten_run_script_int(eval.c_str()) != 0;

    return result;
}

float
WebVROculus::getLeftEyeFov()
{
    // FIXME: Should depend on the VRDisplay
    // Oculus Rift CV1
    // leftDegrees:43.97737166932644 (0.76754993200111731877)
    // rightDegrees:35.5747704995392 (0.6208968758567666724)
    // fov = atan(leftDegrees + rightDegrees) = 0.94
    return 0.94f;
}

float
WebVROculus::getRightEyeFov()
{
    // FIXME: Should depend on the VRDisplay
    // Oculus Rift CV1
    // leftDegrees:35.5747704995392 (0.6208968758567666724)
    // rightDegrees:43.97737166932644 (0.76754993200111731877)
    // fov = atan(leftDegrees + rightDegrees) = 0.94
    return 0.94f;
}

void
WebVROculus::updateCameraOrientation(scene::Node::Ptr target, std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera)
{
    std::string eval = "";

    // Get VRDisplay orientation
    eval += "var hasVRDisplay = !!window.vrDisplay;                                 \n";
    eval += "if (hasVRDisplay) {                                                    \n";
    eval += "   var pose = hasVRDisplay ? window.vrDisplay.getPose() : null;        \n";
    eval += "   if (!!pose && !!pose.orientation) {                                 \n";
    eval += "       pose.orientation.join(' ');                                     \n";
    eval += "   }                                                                   \n";
    eval += "}                                                                      \n";

    auto orientationString = std::string(emscripten_run_script_string(eval.c_str()));

    if (orientationString != "undefined")
    {
        std::array<float, 4> orientation;
        std::stringstream ssOrientation(orientationString);

        for (auto i = 0; i < 4; i++)
            ssOrientation >> orientation[i];

        auto quaternion = math::quat(orientation[3], orientation[0], orientation[1], orientation[2]);

        auto matrix = glm::mat4_cast(quaternion);
        target->component<Transform>()->matrix(matrix);
    }

    // Get projection matrixes
    eval = "";
    eval += "if (!!window.vrDisplay && !!window.vrFrameData) {                                                                      \n";
    eval += "   window.vrDisplay.getFrameData(window.vrFrameData);                                                                  \n";
    eval += "   if (!!window.vrFrameData.leftProjectionMatrix && !!window.vrFrameData.rightProjectionMatrix)                        \n";
    eval += "       window.vrFrameData.leftProjectionMatrix.join(' ') + ' ' + window.vrFrameData.rightProjectionMatrix.join(' ');   \n";
    eval += "}                                                                                                                      \n";

    auto projectionMatrixesString = std::string(emscripten_run_script_string(eval.c_str()));

    if (projectionMatrixesString != "undefined")
    {
        std::array<float, 16> leftProjectionMatrix;
        std::array<float, 16> rightProjectionMatrix;
        std::stringstream ssProjectionMatrixes(projectionMatrixesString);

        for (auto i = 0; i < 16; i++)
            ssProjectionMatrixes >> leftProjectionMatrix[i];

        for (auto i = 0; i < 16; i++)
            ssProjectionMatrixes >> rightProjectionMatrix[i];

        auto leftMatrix = glm::make_mat4(leftProjectionMatrix.data());
        auto rightMatrix = glm::make_mat4(rightProjectionMatrix.data());

        std::cout << "Update camera projection matrixes" << std::endl;
        _leftRenderer->target()->component<PerspectiveCamera>()->projectionMatrix(leftMatrix);
        _rightRenderer->target()->component<PerspectiveCamera>()->projectionMatrix(rightMatrix);
    }

    // Get position tracking
    // eval = "if (window.vrDisplaySensor.getState().position != null) { window.vrDisplaySensor.getState().position.x + ' ' + window.vrDisplaySensor.getState().position.y + ' ' + window.vrDisplaySensor.getState().position.z; }\n";
    // s = std::string(emscripten_run_script_string(eval.c_str()));

    // if (s != "undefined")
    // {
    //     std::array<float, 3> position;
    //     std::stringstream ssPosition(s);

    //     ssPosition >> position[0];
    //     ssPosition >> position[1];
    //     ssPosition >> position[2];

    //     target->component<Transform>()->matrix(math::translate(math::vec3(position[0], position[1], position[2])) * target->component<Transform>()->matrix());
    // }
}
