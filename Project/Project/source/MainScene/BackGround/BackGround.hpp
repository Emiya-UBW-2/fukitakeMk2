
#pragma once

#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
			MV1 Sky;
			MV1 Ground;
			MV1 GroundCol;
			MV1 GroundPos;

			std::vector<MV1> Tower;
			VECTOR_ref minTower, maxTower;
			MV1 Build;
			VECTOR_ref minBuild, maxBuild;
		public:
			void Load(void) noexcept {
				MV1::Load("data/model/sky/model.mv1", &Sky);
				MV1::Load("data/model/ground/model.mv1", &Ground);
				MV1::Load("data/model/ground/col.mv1", &GroundCol);
				MV1::Load("data/model/ground/pos.mv1", &GroundPos);

				int TowerFrame = -1;
				for (int i = 0; i < GroundPos.frame_num(); i++) {
					if (GroundPos.frame_name(i) == "Tower") {
						TowerFrame = i;
					}
				}

				MV1::Load("data/model/ground/Build.mv1", &Build);
				minBuild = Build.mesh_minpos(0);
				maxBuild = Build.mesh_maxpos(0);
				for (int i = 1; i < Build.mesh_num(); i++) {
					if (minBuild.x() > Build.mesh_minpos(i).x()) { minBuild.x(Build.mesh_minpos(i).x()); }
					if (minBuild.y() > Build.mesh_minpos(i).y()) { minBuild.y(Build.mesh_minpos(i).y()); }
					if (minBuild.z() > Build.mesh_minpos(i).z()) { minBuild.z(Build.mesh_minpos(i).z()); }

					if (maxBuild.x() < Build.mesh_maxpos(i).x()) { maxBuild.x(Build.mesh_maxpos(i).x()); }
					if (maxBuild.y() < Build.mesh_maxpos(i).y()) { maxBuild.y(Build.mesh_maxpos(i).y()); }
					if (maxBuild.z() < Build.mesh_maxpos(i).z()) { maxBuild.z(Build.mesh_maxpos(i).z()); }
				}


				for (int i = 0; i < GroundPos.frame_child_num(TowerFrame); i++) {
					Tower.resize(Tower.size() + 1);
					if (Tower.size() == 1) {
						MV1::Load("data/model/ground/Tower.mv1", &Tower.back());
					}
					else {
						Tower.back() = Tower[0].Duplicate();
					}
				}
				minTower = Tower[0].mesh_minpos(0);
				maxTower = Tower[0].mesh_maxpos(0);

				GroundCol.SetupCollInfo(64, 16, 64);
				{
					auto Base = GroundPos.frame(TowerFrame);
					for (int i = 0; i < GroundPos.frame_child_num(TowerFrame); i++) {
						auto Pos = GroundPos.frame((int)GroundPos.frame_child(TowerFrame, i));
						auto buf = Pos - Base;
						Tower[i].SetMatrix(
							MATRIX_ref::RotY(std::atan2f(buf.x(), buf.z()))*
							MATRIX_ref::Mtrans(Pos));
					}
				}
			}
			//DrawCall => 100
			void BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				Sky.DrawModel();
				SetUseLighting(TRUE);
			}
			//‹¤’Ê
			void DrawCommon(void) noexcept {
				Ground.DrawModel();
				for (auto& m : Tower) {
					if (CheckCameraViewClip_Box(
						(m.GetMatrix().pos() + minTower).get(),
						(m.GetMatrix().pos() + maxTower).get()) == FALSE
						) {
						m.DrawModel();
					}
				}
				if (CheckCameraViewClip_Box(
					(minBuild).get(),
					(maxBuild).get()) == FALSE
					) {
					Build.DrawModel();
				}
			}
			void Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
			}
			void Shadow_Draw(void) noexcept {
				DrawCommon();
			}
			void Draw(void) noexcept {
				DrawCommon();
			}

			const auto& GetGroundCol(void) noexcept { return GroundCol; }
		};
	};
};