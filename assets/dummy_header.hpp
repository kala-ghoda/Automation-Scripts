/*
 * Just a dummy file to test the automation implementation.
 */
#ifndef ASSETS_DUMMY_HEADER_H_
#define ASSETS_DUMMY_HEADER_H_
#pragma once

#include <iostream>
#include <cerrno>

namespace dummy {

class Dummy {
	public:
		Dummy(int arr[]) {
			for (auto ele : arr) {
				m_dummy.push_back(ele);
			}
		}

		int getter(int i);

		void setter(int i, int val);

	private:
		std::vector<int> m_dummy;
};

Dummy::getter(int i) {
	return m_dummy[i];
}

Dummy::setter(int i, int val) {
	m_dummy[i] = val;
}

} //namespace done

#endif // ASSETS_DUMMY_HEADER_H_
