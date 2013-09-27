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

#include "ArrayProvider.hpp"

using namespace minko;
using namespace minko::data;

ArrayProvider::ArrayProvider(const std::string& name, uint index) :
	_name(name),
	_index(index)
{
}

void
ArrayProvider::index(unsigned int index)
{
	if (_index == index)
		return;

	_index = index;
	for (auto& propertyName : propertyNames())
		swap(
			propertyName,
			formatPropertyName(propertyName.substr(propertyName.find_first_of(']') + 2)),
			true
		);
}

std::string
ArrayProvider::formatPropertyName(const std::string& propertyName) const
{
	return _name + "[" + std::to_string(_index) + "]." + propertyName;
}