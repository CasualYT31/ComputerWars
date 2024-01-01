/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

template<typename... Ts>
void awe::dialogue_sequence::updateMainText(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setMainText((*_langDic)(
		_boxes[_currentBoxID].mainText, values...)
	);
}

template<typename... Ts>
void awe::dialogue_sequence::updateNameText(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setNameText((*_langDic)(
		_boxes[_currentBoxID].nameText, values...)
	);
}

template<typename... Ts>
void awe::dialogue_sequence::updateOption1Text(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setOptions((*_langDic)(
		_boxes[_currentBoxID].options[0], values...),
		_currentBox->getOption2Text(), _currentBox->getOption3Text()
	);
}

template<typename... Ts>
void awe::dialogue_sequence::updateOption2Text(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setOptions(
		_currentBox->getOption1Text(),
		(*_langDic)(_boxes[_currentBoxID].options[1], values...),
		_currentBox->getOption3Text()
	);
}

template<typename... Ts>
void awe::dialogue_sequence::updateOption3Text(Ts... values) noexcept {
	if (_langDic && _currentBox) _currentBox->setOptions(
		_currentBox->getOption1Text(), _currentBox->getOption2Text(),
		(*_langDic)(_boxes[_currentBoxID].options[2], values...)
	);
}
