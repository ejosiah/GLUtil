#pragma once

#include "UICore.h"

namespace ncl {
	namespace gl {
		namespace ui {
			class Event {
			public:
				Event(const Component* source) :source{ source } {

				}

				virtual const Component* getSource() const {
					return source;
				}

			private:
				const Component* source;
			};

			class ActionEvent : public Event {
			public:
				ActionEvent(const Component* source, const char* action)
					:Event{ source }, action{ action } {

				}
			protected:
				const char* action;

			};
		}
	}
}