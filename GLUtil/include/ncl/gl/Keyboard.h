#pragma once
#include <initializer_list>
#include <vector>
#include <algorithm>

namespace ncl {
	namespace gl {

		struct Key {
			enum Status { PRESSED, RELEASED };
			std::vector<int> values;
			mutable bool isLowerCase = false;
			mutable Status status = RELEASED;

			Key() {}

			Key(std::initializer_list<int> list) {
				values = std::vector<int>(list.begin(), list.end());
			}

			bool pressed() const{
				return status == PRESSED;
			}

			void release() const{
				status = RELEASED;
			}

			void press() const{
				status = PRESSED;
			}

			int value() const {
				if (values.size() == 1) return values[0];
				if (!isLowerCase) {
					return values[0];
				}
				return values[1];
			}
		};

		class Keyboard {
		public:
			const Key SPACE_BAR = { { ' ' } };
			const Key TAB = { {258} };
			const Key BACK_SPACE = { { 259 } };
			const Key Enter = { {260} };
			const Key DEL = { {261} };
			const Key RIGHT = { {262} };
			const Key LEFT = { {263} };
			const Key DOWN = { {264} };
			const Key UP = { {265} };
			const Key A = { { 'A', 'a' } };
			const Key B = { { 'B', 'b' } };
			const Key C = { { 'C', 'c' } };
			const Key D = { { 'D', 'd' } };
			const Key E = { { 'E', 'e' } };
			const Key F = { { 'F', 'f' } };
			const Key G = { { 'G', 'g' } };
			const Key H = { { 'H', 'h' } };
			const Key I = { { 'I', 'i' } };
			const Key J = { { 'J', 'j' } };
			const Key K = { { 'K', 'k' } };
			const Key L = { { 'L', 'l' } };
			const Key M = { { 'M', 'm' } };
			const Key N = { { 'N', 'n' } };
			const Key O = { { 'O', 'o' } };
			const Key P = { { 'P', 'p' } };
			const Key Q = { { 'Q', 'q' } };
			const Key R = { { 'R', 'r' } };
			const Key S = { { 'S', 's' } };
			const Key T = { { 'T', 't' } };
			const Key U = { { 'U', 'u' } };
			const Key V = { { 'V', 'v' } };
			const Key W = { { 'W', 'w' } };
			const Key X = { { 'X', 'x' } };
			const Key Y = { { 'Y', 'y' } };
			const Key Z = { { 'Z', 'z' } };



			const Key NUM_0 = { { '0' } };
			const Key NUM_1 = { { '1' } };
			const Key NUM_2 = { { '2' } };
			const Key NUM_3 = { { '3' } };
			const Key NUM_4 = { { '4' } };
			const Key NUM_5 = { { '5' } };
			const Key NUM_6 = { { '6' } };
			const Key NUM_7 = { { '7' } };
			const Key NUM_8 = { { '8' } };
			const Key NUM_9 = { { '9' } };

			Keyboard() {
				
			}

			void clearModifiers() {
				shift = ctrl = alt = false;
			}

			void activatModifier(int mod) {
				switch (mod) {
				case 1: shift = true; break;
				case 2: ctrl = true; break;
				case 4: alt = true; break;
				}
			}

			bool shiftHeld() {
				return shift;
			}

			bool controlHeld() {
				return ctrl;
			}

			bool altHeld() {
				return alt;
			}

			static const Key* begin();
			static const Key* end();
			static const Key* get(int value);
			static void init();
			static void dispose();

			static Keyboard& get() {
				return *instance;
			}

		private:
			static Keyboard* instance;
			bool shift, ctrl, alt;
		};


		Keyboard* Keyboard::instance;

		const Key* Keyboard::begin() {
			return &instance->SPACE_BAR;
		}
		
		const Key* Keyboard::end() {
			const Key* ptr = &instance->NUM_9;
			return ++ptr;
		}

		void Keyboard::init() {
			instance = new Keyboard;
		}

		void Keyboard::dispose() {
			delete instance;
		}

		
		 const Key* Keyboard::get(const int value) {
			 const Key* ptr = begin();
			 const Key* end = Keyboard::end();

			 while (ptr != end) {
				 const std::vector<int>& values = ptr->values;
				 if (std::any_of(values.begin(), values.end(), [&](int v) { return v == value; })) {
					 return ptr;
				 }
				 ptr++;
			 }
			 return nullptr;
		}

	}
}