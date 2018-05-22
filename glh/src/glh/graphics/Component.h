#pragma once

namespace glh {
	namespace Graphics {

		struct Translation
		{
			float x = 0;
			float y = 0;
			float z = 0;
		};

		struct Rotation
		{
			float x = 0;
			float y = 0;
			float z = 0;
		};

		class Component {
		public:
			Component();
			void GetSize();

		private:
			Translation translate;
			Rotation rotate;

		};
	}
}