#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ItemClass : public ObjectBaseClass {
			bool													m_Have;
			GraphHandle												m_ItemGraph;
		public:
			auto& GetItemGraph() const noexcept { return m_ItemGraph; }
			
			void SetCharaPtr(bool value) noexcept { m_Have = value; }
			const auto GetHaveChara() const noexcept { return m_Have; }
		public:
			ItemClass(void) noexcept { this->m_objType = ObjType::Item; }
			~ItemClass(void) noexcept {}
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_move.vec.clear();
				this->m_Have = false;
				this->m_ItemGraph = GraphHandle::Load(this->m_FilePath + "pic.png");
			}
			void Execute(void) noexcept override {
				if (!this->m_Have) {
					this->m_IsActive = true;
					auto HitResult = this->m_MapCol->CollCheck_Line(this->m_move.pos + VECTOR_ref::up() * (-1.f - 3.f), this->m_move.pos + VECTOR_ref::up() * 15.f);
					if (HitResult.HitFlag == TRUE) {
						auto yPos = this->m_move.pos.y();
						Easing(&yPos, HitResult.HitPosition.y, 0.8f, EasingType::OutExpo);
						this->m_move.pos.y(yPos);
						this->m_move.vec.y(0.f);
					}
					else {
						this->m_move.vec.yadd(M_GR / (FPS * FPS));
					}
					this->m_move.pos += this->m_move.vec;

					UpdateMove();
				}
				else {
					this->m_IsActive = false;
				}
			}
		};
	};
};
