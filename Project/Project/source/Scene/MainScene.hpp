#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			//リソース関連
			ObjectManager Obj;				//モデル
			BackGroundClass BackGround;		//BG
			SoundHandle Env;
			//
			static const int chara_num = 3;
			static const int item_num = 6;
			static const int cart_num = 2;
			//関連
			int tgtSel = 0;
			float tgtTimer = 0.f;
			//ルール
			float m_ReadyTime = 0.f;
			bool m_StartSwitch{ false };
			bool m_RemoveSwitch{ false };
			//操作関連
			float EyeRunPer = 0.f;
			switchs FPSActive;
			switchs MouseActive;
			switchs RunKey;
			//UI関連
			UIClass UI_class;
			//
			float HPBuf{ 0.f };
			float MPBuf{ 0.f };
			float scoreBuf{ 0.f };
			float AddItemBuf{ 1.f };
			float SubItemBuf{ 1.f };
			int PrevPointSel{ 0 };
			//銃関連
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;

			float TPS_Xrad = 0;
			float TPS_Yrad = 0;
			float TPS_XradR = 0;
			float TPS_YradR = 0;
			float TPS_Per = 1.f;
		public:
			using TEMPSCENE::TEMPSCENE;
			void Set(void) noexcept override {
				SoundPool::Create();
				Set_EnvLight(
					VECTOR_ref::vget(12.5f*-300.f, 12.5f*-10.f, 12.5f*-300.f),
					VECTOR_ref::vget(12.5f*300.f, 12.5f*50.f, 12.5f*300.f),
					VECTOR_ref::vget(-0.25f, -0.5f, 0.0f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				//Load
				//BG
				this->BackGround.Load();
				//
				this->Obj.Init(&this->BackGround.GetGroundCol());
				for (int i = 0; i < chara_num; i++) {
					this->Obj.AddObject(ObjType::Human, "data/Charactor/Marisa/", DX_LOADMODEL_PHYSICS_REALTIME);
					this->Obj.AddObject(ObjType::Houki, "data/Houki/Houki01/", DX_LOADMODEL_PHYSICS_LOADCALC);
				}
				for (int i = 0; i < item_num/2; i++) {
					this->Obj.AddObject(ObjType::Item, "data/Item/BluePotion/", DX_LOADMODEL_PHYSICS_LOADCALC);
				}
				for (int i = 0; i < item_num/2; i++) {
					this->Obj.AddObject(ObjType::Item, "data/Item/YellowPotion/", DX_LOADMODEL_PHYSICS_LOADCALC);
				}
				//guide
				this->Obj.AddObject(ObjType::Circle, "data/model/Circle/", DX_LOADMODEL_PHYSICS_LOADCALC);
				//ロード
				SetCreate3DSoundFlag(FALSE);
				Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				Env.vol(64);
				//UI
				UI_class.Set();
				//
				TEMPSCENE::Set();
				//Set
				//オブジェ
				//人
				for (int i = 0; i < chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
					c->SetHoukiPtr((std::shared_ptr<HoukiClass>&)(this->Obj.GetObj(ObjType::Houki, i)));
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), VECTOR_ref::vget(0.f, 0.f, -52.5f + (float)(i - 1)*20.f));
				}
				{
					auto& Chara = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, 0));//自分
					this->HPBuf = Chara->GetHP();
					this->MPBuf = Chara->GetMP();
					this->scoreBuf = Chara->GetScore();
				}
				//アイテム
				{
					for (int i = 0; i < item_num; i++) {
						auto& c = this->Obj.GetObj(ObjType::Item, i);
						c->SetMove(MATRIX_ref::RotY(deg2rad(0)), VECTOR_ref::vget(20 * (i / (item_num / 2)), 0.f, 20 + 20 * (i% (item_num / 2))));
					}
				}
				//ガイドサークル
				{
					auto& c = this->Obj.GetObj(ObjType::Circle, 0);
					c->SetMove(MATRIX_ref::RotY(deg2rad(-90)), VECTOR_ref::vget(-10.f, 0, -20));
				}
				//UI
				tgtSel = -1;
				tgtTimer = 0.f;
				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				Set_zoom_lens(3.5f);
				//サウンド
				auto SE = SoundPool::Instance();
				SE->Add((int)SoundEnum::Shot_Gun, 3, "data/Sound/SE/gun/shot.wav");
				SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
				for (int i = 0; i < 4; i++) {
					SE->Add((int)SoundEnum::Cocking0 + i, 3, "data/Sound/SE/gun/slide/bolt/" + std::to_string(i) + ".wav");
				}
				SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
				SE->Add((int)SoundEnum::SlideFoot, 9, "data/Sound/SE/move/sliding.wav");
				SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
				SE->Add((int)SoundEnum::Heart, 9, "data/Sound/SE/move/heart.wav");
				SE->Add((int)SoundEnum::GateOpen, 1, "data/Sound/SE/GateOpen.wav");

				SE->Get((int)SoundEnum::Shot_Gun).SetVol_Local(128);
				SE->Get((int)SoundEnum::Trigger).SetVol_Local(128);
				for (int i = 0; i < 4; i++) {
					SE->Get((int)SoundEnum::Cocking0 + i).SetVol_Local(128);
				}
				SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
				SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
				SE->Get((int)SoundEnum::GateOpen).SetVol_Local(128);
				//
				this->m_ReadyTime = 15.f;
				this->m_StartSwitch = false;
				this->m_RemoveSwitch = false;
				//入力
				FPSActive.Init(false);
				MouseActive.Init(true);
			}
			//
			bool Update(void) noexcept override {
				auto& Chara = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, 0));//自分
				//FirstDoing
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					Env.play(DX_PLAYTYPE_LOOP, TRUE);
					this->m_ReadyTime = 1.f;
					PrevPointSel = Chara->GetItemSel();
				}
				//
				{
					auto prev = this->m_ReadyTime;
					this->m_ReadyTime -= 1.f / FPS;
					auto timep = 0.f;

					timep = 0.f;
					this->m_StartSwitch = (prev >= timep && this->m_ReadyTime < timep);
					timep = -10.f;
					this->m_RemoveSwitch = (prev >= timep && this->m_ReadyTime < timep);
					//printfDx("%f \n", this->m_ReadyTime);
				}
				//Input,AI
				{
					MouseActive.GetInput(CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0);
					FPSActive.GetInput(CheckHitKeyWithCheck(KEY_INPUT_V) != 0);
					RunKey.GetInput(CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0);
					auto MidPress = (GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0;
					int mx = DXDraw::Instance()->disp_x / 2, my = DXDraw::Instance()->disp_y / 2;
					if (MouseActive.on()) {
						if (MouseActive.trigger()) {
							SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
						}
						GetMousePoint(&mx, &my);
						SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
						SetMouseDispFlag(FALSE);
					}
					else {
						SetMouseDispFlag(TRUE);
					}

					float cam_per = ((camera_main.fov / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					float pp_x = std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*1.f, -9.f, 9.f) * cam_per;
					float pp_y = std::clamp((float)(mx - DXDraw::Instance()->disp_x / 2)*1.f, -9.f, 9.f) * cam_per;

					Easing(&TPS_Per, (!FPSActive.on() && MidPress) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

					TPS_Xrad += pp_x;
					TPS_Yrad += pp_y;
					TPS_Xrad = std::clamp(TPS_Xrad, deg2rad(-80), deg2rad(80));
					if (TPS_Yrad >= deg2rad(180)) { TPS_Yrad -= deg2rad(360); }
					if (TPS_Yrad <= deg2rad(-180)) { TPS_Yrad += deg2rad(360); }

					TPS_Xrad *= TPS_Per;
					TPS_Yrad *= TPS_Per;

					Easing(&TPS_XradR, TPS_Xrad, 0.5f, EasingType::OutExpo);

					TPS_YradR += (sin(TPS_Yrad)*cos(TPS_YradR) - cos(TPS_Yrad) * sin(TPS_YradR))*20.f / FPS;

					Chara->SetEyeVec((camera_main.camvec - camera_main.campos).Norm());

					InputControl Input;
					bool isready = this->m_ReadyTime < 0.f;
					for (int i = 0; i < chara_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
						//拾い判定
						for (int j = 0; j < item_num; j++) {
							auto& item = (std::shared_ptr<ItemClass>&)(this->Obj.GetObj(ObjType::Item, j));
							if (!item->GetHaveChara()) {
								auto p = (item->GetMatrix().pos() - c->GetMatrix().pos()).size();
								if ((item->GetMatrix().pos() - c->GetMatrix().pos()).size() < 12.5f*1.f) {
									c->AddItem(item);
								}
							}
						}
						//操作
						if (i == 0
							//&& false
							) {
							Input.SetInput(
								pp_x*(1.f - TPS_Per),
								pp_y*(1.f - TPS_Per),
								(CheckHitKeyWithCheck(KEY_INPUT_W) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_S) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_A) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_D) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_Q) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_E) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_R) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_F) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_M) != 0) && isready,
								(CheckHitKeyWithCheck(KEY_INPUT_N) != 0) && isready
							);
							Chara->SetInput(Input, isready);
							continue;
						}
						Input.SetInput(
							0.f,
							0.f,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready
						);
						c->SetInput(Input, isready);
					}
				}
				//Execute
				this->Obj.ExecuteObject();
				//col

				//視点
				{

					if (FPSActive.on()) {
						camera_main.campos = Chara->GetEyePosition();
						camera_main.camvec = camera_main.campos + Chara->GetEyeVecMat().zvec() * -1.f;
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();
					}
					else {
						MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot();
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * 14.f;

						VECTOR_ref CamVec;

						CamVec = MATRIX_ref::Vtrans(Chara->GetEyeVecMat().zvec() * -1.f, MATRIX_ref::RotAxis(Chara->GetMatrix().xvec(), TPS_XradR) * MATRIX_ref::RotAxis(Chara->GetMatrix().yvec(), TPS_YradR));

						CamVec = Lerp(Chara->GetEyeVecMat().zvec() * -1.f, CamVec, TPS_Per);

						CamPos +=
							Lerp(
								Lerp((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), EyeRunPer),
								Lerp(UpperMat.yvec()*4.f, UpperMat.yvec()*8.f, TPS_Per),
								Chara->GetFlightPer()
							);

						camera_main.campos = CamPos + CamVec * Lerp(Lerp(-20.f, -30.f - (Chara->GetFlightSpeed()/25.f), Chara->GetFlightPer()), -50.f, TPS_Per);
						camera_main.camvec = CamPos + CamVec * 100.f;

						camera_main.camup = Chara->GetEyeVecMat().yvec();
					}
					Easing(&EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f, EasingType::OutExpo);

					if (Chara->GetIsRun()) {
						Easing(&camera_main.fov, deg2rad(90), 0.9f, EasingType::OutExpo);
						Easing(&camera_main.near_, 3.f, 0.9f, EasingType::OutExpo);
						Easing(&camera_main.far_, 12.5f * 150.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&camera_main.fov, deg2rad(75), 0.9f, EasingType::OutExpo);
						Easing(&camera_main.near_, 10.f, 0.9f, EasingType::OutExpo);
						Easing(&camera_main.far_, 12.5f * 300.f, 0.9f, EasingType::OutExpo);
					}
				}

				this->BackGround.Execute();
				//UIパラメーター
				{
					int minute = (int)((-this->m_ReadyTime) / 60.f);
					UI_class.SetIntParam(0, minute);
					UI_class.SetfloatParam(0, (-this->m_ReadyTime) - (float)minute*60.f);

					UI_class.SetIntParam(1, (int)this->scoreBuf);
					this->scoreBuf += std::clamp((Chara->GetScore() - this->scoreBuf)*100.f, -5.f, 5.f) / FPS;

					UI_class.SetIntParam(2, Chara->GetFlightMode() ? 1 : 0);
					UI_class.SetfloatParam(1, Chara->GetFlightSpeed());
					UI_class.SetfloatParam(2, Chara->GetMatrix().pos().y() / 12.5f);

					UI_class.SetIntParam(3, (int)Chara->GetHP());
					UI_class.SetIntParam(4, (int)Chara->GetHPMax());
					UI_class.SetIntParam(5, (int)(this->HPBuf + 0.5f));
					this->HPBuf += std::clamp((Chara->GetHP() - this->HPBuf)*100.f, -5.f, 5.f) / FPS;

					UI_class.SetIntParam(6, (int)Chara->GetMP());
					UI_class.SetIntParam(7, (int)Chara->GetMPMax());
					UI_class.SetIntParam(8, (int)(this->MPBuf + 0.5f));
					this->MPBuf += std::clamp((Chara->GetMP() - this->MPBuf)*100.f, -5.f, 5.f) / FPS;

					{
						int i = 0;
						int ID = -1;
						//
						i = 0;
						ID = (Chara->GetItemSel() - 1);
						if (ID <= -1) { ID = Chara->GetItemNum() - 1; }
						if (Chara->GetItemNum() <= ID) { ID = -1; }
						UI_class.SetItemGraph(i, (ID != -1) ? (&(Chara->GetItem(ID)->GetItemGraph())) : nullptr);
						//
						i = 1;
						ID = Chara->GetItemSel();
						if (Chara->GetItemNum() <= ID) { ID = -1; }
						UI_class.SetItemGraph(i, (ID != -1) ? (&(Chara->GetItem(ID)->GetItemGraph())) : nullptr);
						//
						i = 2;
						ID = (Chara->GetItemSel() + 1);
						if (ID >= Chara->GetItemNum()) { ID = 0; }
						if (Chara->GetItemNum() <= ID) { ID = -1; }
						UI_class.SetItemGraph(i, (ID != -1) ? (&(Chara->GetItem(ID)->GetItemGraph())) : nullptr);

						if (PrevPointSel > Chara->GetItemSel()) {
							if (Chara->GetItemSel() == 0 && PrevPointSel == Chara->GetItemNum() - 1) {
								AddItemBuf = 0.f;
							}
							else {
								SubItemBuf = 0.f;
							}
						}
						else if (PrevPointSel < Chara->GetItemSel()) {
							if (Chara->GetItemSel() == Chara->GetItemNum() - 1 && PrevPointSel == 0) {
								SubItemBuf = 0.f;
							}
							else {
								AddItemBuf = 0.f;
							}
						}
						PrevPointSel = Chara->GetItemSel();

						UI_class.SetfloatParam(3, AddItemBuf);
						UI_class.SetfloatParam(4, SubItemBuf);
						Easing(&AddItemBuf, 1.f, 0.9f, EasingType::OutExpo);
						Easing(&SubItemBuf, 1.f, 0.9f, EasingType::OutExpo);
					}
				}
				TEMPSCENE::Update();
				Effect_UseControl::Update_Effect();
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();
				this->Obj.DisposeObject();
			}
			//
			void Depth_Draw(void) noexcept override {
				this->BackGround.Draw();
				//this->Obj.DrawDepthObject();
			}
			void BG_Draw(void) noexcept override {
				this->BackGround.BG_Draw();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				this->BackGround.Shadow_Draw_NearFar();
				//this->Obj.DrawObject_Shadow();
			}
			void Shadow_Draw(void) noexcept override {
				this->BackGround.Shadow_Draw();
				this->Obj.DrawObject_Shadow();
			}

			void Main_Draw(void) noexcept override {
				this->BackGround.Draw();
				this->Obj.DrawObject();
				//this->Obj.DrawDepthObject();
				//シェーダー描画用パラメーターセット
				{
					Set_is_Blackout(false);
					Set_is_lens(false);
				}
			}
			void Main_Draw2(void) noexcept override {
				this->Obj.DrawDepthObject();
			}
			void LAST_Draw(void) noexcept override {
			}
			//UI表示
			void UI_Draw(void) noexcept  override {
				UI_class.Draw();
			}
		};
	};
};
