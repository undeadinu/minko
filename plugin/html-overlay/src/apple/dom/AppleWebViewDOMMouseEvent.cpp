/*
 Copyright (c) 2013 Aerys

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

#include "minko/Common.hpp"
#include "apple/dom/AppleWebViewDOMMouseEvent.hpp"
#include "apple/dom/AppleWebViewDOMEngine.hpp"

using namespace minko;
using namespace minko::dom;
using namespace apple;
using namespace apple::dom;

int
AppleWebViewDOMMouseEvent::clientX()
{
    std::string js = "(" + _jsAccessor + ".clientX)";
    int result = atoi(_engine->eval(js).c_str());

    return result;
}

int
AppleWebViewDOMMouseEvent::clientY()
{
    std::string js = "(" + _jsAccessor + ".clientY)";
    int result = atoi(_engine->eval(js).c_str());

    return result;
}

int
AppleWebViewDOMMouseEvent::pageX()
{
    std::string js = "(" + _jsAccessor + ".pageX)";
    int result = atoi(_engine->eval(js).c_str());

    return result;
}

int
AppleWebViewDOMMouseEvent::pageY()
{
    std::string js = "(" + _jsAccessor + ".pageY)";
    int result = atoi(_engine->eval(js).c_str());

    return result;
}

int
AppleWebViewDOMMouseEvent::screenX()
{
    std::string js = "(" + _jsAccessor + ".screenX)";
    int result = atoi(_engine->eval(js).c_str());

    return result;
}

int
AppleWebViewDOMMouseEvent::screenY()
{
    std::string js = "(" + _jsAccessor + ".screenY)";
    int result = atoi(_engine->eval(js).c_str());

    return result;
}

int
AppleWebViewDOMMouseEvent::button()
{
    std::string js = "(" + _jsAccessor + ".button)";
    int result = atoi(_engine->eval(js).c_str());
    
    return result;
}
