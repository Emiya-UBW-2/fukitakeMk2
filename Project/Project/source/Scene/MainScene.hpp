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
			static const int item_num = 9;
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
			//UI関連
			UIClass UI_class;
			float HPBuf{ 0.f };
			float MPBuf{ 0.f };
			float scoreBuf{ 0.f };
			float AddItemBuf{ 1.f };
			float SubItemBuf{ 1.f };
			int PrevPointSel{ 0 };
			//
			float m_CamShake{ 0.f };
			VECTOR_ref m_CamShake1;
			VECTOR_ref m_CamShake2;
			//
			float m_Rader = 1.f;
			float m_Rader_r = 1.f;
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
				for (int i = 0; i < item_num / 3; i++) {
					this->Obj.AddObject(ObjType::Item, "data/Item/BluePotion/", DX_LOADMODEL_PHYSICS_LOADCALC);
				}
				for (int i = 0; i < item_num / 3; i++) {
					this->Obj.AddObject(ObjType::Item, "data/Item/YellowPotion/", DX_LOADMODEL_PHYSICS_LOADCALC);
				}
				for (int i = 0; i < item_num / 3; i++) {
					this->Obj.AddObject(ObjType::Item, "data/Item/RedPotion/", DX_LOADMODEL_PHYSICS_LOADCALC);
				}
				//guide
				//this->Obj.AddObject(ObjType::Circle, "data/model/Circle/", DX_LOADMODEL_PHYSICS_LOADCALC);
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
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->Obj.GetObj(ObjType::Human, i));
					c->SetHoukiPtr((std::shared_ptr<HoukiClass>&)(*this->Obj.GetObj(ObjType::Houki, i)));
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), VECTOR_ref::vget(0.f, 0.f, -52.5f + (float)(i - 1)*20.f));
				}
				{
					auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->Obj.GetObj(ObjType::Human, 0));//自分
					this->HPBuf = Chara->GetHP();
					this->MPBuf = Chara->GetMP();
					this->scoreBuf = Chara->GetScore();
				}
				//アイテム
				{
					for (int i = 0; i < item_num; i++) {
						auto& c = *this->Obj.GetObj(ObjType::Item, i);
						c->SetMove(MATRIX_ref::RotY(deg2rad(0)), VECTOR_ref::vget(20.f * (i / (item_num / 3)), 0.f, 20.f + 20.f * (i% (item_num / 3))));
					}
				}
				//ガイドサークル
				{
					//auto& c = *this->Obj.GetObj(ObjType::Circle, 0);
					//c->SetMove(MATRIX_ref::RotY(deg2rad(-90)), VECTOR_ref::vget(-10.f, 0, -20));
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
				auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->Obj.GetObj(ObjType::Human, 0));//自分
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
					bool w_key = (CheckHitKeyWithCheck(KEY_INPUT_W) != 0);
					bool s_key = (CheckHitKeyWithCheck(KEY_INPUT_S) != 0);
					bool a_key = (CheckHitKeyWithCheck(KEY_INPUT_A) != 0);
					bool d_key = (CheckHitKeyWithCheck(KEY_INPUT_D) != 0);
					bool Lshift_key = (CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0);
					bool q_key = (CheckHitKeyWithCheck(KEY_INPUT_Q) != 0);
					bool e_key = (CheckHitKeyWithCheck(KEY_INPUT_E) != 0);
					bool space_key = (CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
					bool r_key = (CheckHitKeyWithCheck(KEY_INPUT_R) != 0);
					bool f_key = (CheckHitKeyWithCheck(KEY_INPUT_F) != 0);
					bool right_key = (CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
					bool left_key = (CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
					bool useItem_key = (CheckHitKeyWithCheck(KEY_INPUT_J) != 0);
					bool up_key = (CheckHitKeyWithCheck(KEY_INPUT_UP) != 0);
					bool down_key = (CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0);
					auto wheel = GetMouseWheelRotVol();
					up_key |= (wheel > 0);
					down_key |= (wheel < 0);
					bool useMagic_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
					bool look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
					bool eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0;

					if (GetJoypadNum() > 0) {
						DINPUT_JOYSTATE input;
						int padID = DX_INPUT_PAD1;
						GetJoypadInputState(padID);
						switch (GetJoypadType(padID)) {
						case DX_PADTYPE_OTHER:
						case DX_PADTYPE_DUAL_SHOCK_4:
						case DX_PADTYPE_DUAL_SENSE:
						case DX_PADTYPE_SWITCH_JOY_CON_L:
						case DX_PADTYPE_SWITCH_JOY_CON_R:
						case DX_PADTYPE_SWITCH_PRO_CTRL:
						case DX_PADTYPE_SWITCH_HORI_PAD:
							GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
							{
								pp_x = std::clamp(-(float)(-input.Rz) / 100.f*1.f, -9.f, 9.f) * cam_per;
								pp_y = std::clamp((float)(input.Z) / 100.f*1.f, -9.f, 9.f) * cam_per;

								float deg = rad2deg(atan2f((float)input.X, -(float)input.Y));
								if (!(input.X == 0 && input.Y == 0)) {
									w_key = (-50.f <= deg && deg <= 50.f);
									a_key = (-140.f <= deg && deg <= -40.f);
									s_key = (130.f <= deg || deg <= -130.f);
									d_key = (40.f <= deg && deg <= 140.f);
								}
								else {
									w_key = false;
									a_key = false;
									s_key = false;
									d_key = false;
								}
								//走り(未定)
								Lshift_key = (input.Buttons[10] != 0);
								space_key = (input.Buttons[10] != 0);
								//視点切り替え
								look_key = (input.Buttons[11] != 0);
								//eyechange_key = (input.Buttons[11]!=0);
								//ヨー
								q_key = (input.Buttons[6] != 0);
								e_key = (input.Buttons[7] != 0);
								//加減速
								f_key = (input.Buttons[4] != 0);
								r_key = (input.Buttons[5] != 0);
								//十字
								deg = (float)(input.POV[0]) / 100.f;
								up_key = (310.f <= deg || deg <= 50.f);
								left_key = (220.f <= deg && deg <= 320.f);
								down_key = (130.f <= deg && deg <= 230.f);
								right_key = (40.f <= deg && deg <= 140.f);
								//ボタン
								useItem_key = (input.Buttons[3] != 0);//□
								//_key = (input.Buttons[0] != 0);//△
								useMagic_key = (input.Buttons[1] != 0);//〇
								//_key = (input.Buttons[2] != 0);//×
							}
							break;
						case DX_PADTYPE_XBOX_360:
						case DX_PADTYPE_XBOX_ONE:
							break;
						default:
							break;
						}
					}

					FPSActive.GetInput(eyechange_key);

					Easing(&TPS_Per, (!FPSActive.on() && look_key) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

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
						auto& c = (std::shared_ptr<CharacterClass>&)(*this->Obj.GetObj(ObjType::Human, i));
						//拾い判定
						for (int j = 0; j < item_num; j++) {
							if (this->Obj.GetObj(ObjType::Item, j) == nullptr) { break; }
							auto& item = (std::shared_ptr<ItemClass>&)(*this->Obj.GetObj(ObjType::Item, j));
							if (!item->GetHaveChara() && !item->GetUsed()) {
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
								w_key && isready,
								s_key && isready,
								a_key && isready,
								d_key && isready,
								Lshift_key && isready,
								q_key && isready,
								e_key && isready,
								space_key && isready,
								r_key && isready,
								f_key && isready,
								right_key && isready,
								left_key && isready,
								useItem_key && isready,
								up_key && isready,
								down_key && isready,
								useMagic_key && isready
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
				//アイテム削除
				for (int i = 0; i < item_num; i++) {
					if (this->Obj.GetObj(ObjType::Item, i) == nullptr) { break; }
					auto& item = (std::shared_ptr<ItemClass>&)(*this->Obj.GetObj(ObjType::Item, i));
					if (item->GetUsed()) {
						this->Obj.DelObj(ObjType::Item, i);
						i = 0;
					}
				}
				//col

				//視点
				{
					if (Chara->GetSendCamShake()) {
						m_CamShake = 1.f;
					}
					Easing(&m_CamShake1, VECTOR_ref::vget(GetRandf(m_CamShake), GetRandf(m_CamShake), GetRandf(m_CamShake)), 0.8f, EasingType::OutExpo);
					Easing(&m_CamShake2, m_CamShake1, 0.8f, EasingType::OutExpo);
					m_CamShake = std::max(m_CamShake - 1.f / FPS, 0.f);

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

						camera_main.campos = (CamPos + CamVec * Lerp(Lerp(-20.f, -30.f - (Chara->GetFlightSpeed()/25.f), Chara->GetFlightPer()), -50.f, TPS_Per)) + m_CamShake2*5.f;
						camera_main.camvec = CamPos + CamVec * 100.f;

						camera_main.camup = Chara->GetEyeVecMat().yvec() + m_CamShake2 * 0.25f;
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
					//アイテム
					{
						int i = 0;
						int ID = -1;
						int ID1 = -1;
						int ID2 = -1;
						//
						i = 1;
						ID = Chara->GetItemSel();
						if (Chara->GetItemNum() <= ID) { ID = -1; }
						ID1 = ID;

						UI_class.SetItemGraph(i, (ID != -1) ? (&(Chara->GetItem(ID)->GetItemGraph())) : nullptr);
						UI_class.SetIntParam(9 + i, (ID != -1) ? (int)(Chara->GetItemCnt(ID)) : 0);
						//
						i = 0;
						ID = (Chara->GetItemSel() - 1);
						if (ID <= -1) { ID = (int)(Chara->GetItemNum()) - 1; }
						ID2 = ID;

						if (Chara->GetItemNum() <= ID) { ID = -1; }
						if (ID1 == ID) { ID = -1; }

						UI_class.SetItemGraph(i, (ID != -1) ? (&(Chara->GetItem(ID)->GetItemGraph())) : nullptr);
						UI_class.SetIntParam(9 + i, (ID != -1) ? (int)(Chara->GetItemCnt(ID)) : 0);
						//
						i = 2;
						ID = (Chara->GetItemSel() + 1);
						if (ID >= Chara->GetItemNum()) { ID = 0; }

						if (Chara->GetItemNum() <= ID) { ID = -1; }
						if (ID1 == ID) { ID = -1; }
						//if (ID2 == ID) { ID = -1; }
						UI_class.SetItemGraph(i, (ID != -1) ? (&(Chara->GetItem(ID)->GetItemGraph())) : nullptr);
						UI_class.SetIntParam(9 + i, (ID != -1) ? (int)(Chara->GetItemCnt(ID)) : 0);

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
					//魔法
					{
						UI_class.SetIntParam(12, Chara->GetMagicSel());
						UI_class.SetIntParam(13, Chara->GetMagicNum());
					}
					//
					{
						UI_class.SetStrParam(0, Chara->GetName());
					}
					//
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
				for (int i = 1; i < chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->Obj.GetObj(ObjType::Human, i));
					auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						c->SetCameraPosition(campos);
						c->SetCameraSize(
							std::max(
								20.f / ((pos - GetCameraPosition()).size() / 2.f),
								0.2f
							)
						);
					}
				}
			}
			void Main_Draw2(void) noexcept override {
				this->Obj.DrawDepthObject();
			}
			void LAST_Draw(void) noexcept override {
			}
			//UI表示
			void UI_Draw(void) noexcept  override {
				auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->Obj.GetObj(ObjType::Human, 0));//自分

				auto* DrawParts = DXDraw::Instance();
				auto Red = GetColor(255, 0, 0);
				auto Green = GetColor(43, 163, 91);
				auto White = GetColor(212, 255, 239);
				//キャラ
				for (int i = 1; i < chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->Obj.GetObj(ObjType::Human, i));
					auto campos = c->GetCameraPosition();
					if (0.f < campos.z() && campos.z() < 1.f) {
						int xp, yp, xs, ys;
						xp = (int)(campos.x());
						yp = (int)(campos.y());
						xs = (int)(100.f*c->GetCameraSize());
						ys = (int)(100.f*c->GetCameraSize());
						int siz = y_r(std::max((int)(20.f*c->GetCameraSize()), 10));
						int p = 0;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);
						p = 1;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, Red, FALSE);
						p = 2;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);

						UI_class.GetFont().Get(siz).Get_handle().DrawStringFormat_MID(xp, yp - ys - siz, Red, White, "%s", c->GetName().c_str());

						UI_class.GetFont().Get(siz).Get_handle().DrawStringFormat(xp + xs, yp + ys, Red, White, "%dm", (int)((c->GetMatrix().pos() - Chara->GetMatrix().pos()).size() / 12.5f));
					}
				}
				//UI
				UI_class.Draw();
				//レーダー
				{
					Easing(&m_Rader_r, m_Rader, 0.8f, EasingType::OutExpo);

					int xp1, yp1;
					int xs1, ys1, xs2, ys2;

					xs1 = y_r((int)(256.f * 0.5f));
					ys1 = y_r((int)(256.f * 0.8f));
					xs2 = y_r((int)(256.f * 0.5f));
					ys2 = y_r((int)(256.f * 0.2f));
					xp1 = DrawParts->disp_x - y_r(80) - xs1;
					yp1 = DrawParts->disp_y - y_r(300) - ys1;

					DrawLine((int)(xp1 - xs1), (int)(yp1), (int)(xp1 + xs2), (int)(yp1), Green, 3);
					DrawLine((int)(xp1), (int)(yp1 - ys1), (int)(xp1), (int)(yp1 + ys2), Green, 3);
					DrawLine((int)(xp1 - xs1), (int)(yp1), (int)(xp1 + xs2), (int)(yp1), White);
					DrawLine((int)(xp1), (int)(yp1 - ys1), (int)(xp1), (int)(yp1 + ys2), White);
					DrawBox((int)(xp1 - xs1), (int)(yp1 - ys1), (int)(xp1 + xs2), (int)(yp1 + ys2), Green, FALSE);
					DrawBox((int)(xp1 - xs1) + 1, (int)(yp1 - ys1) + 1, (int)(xp1 + xs2) - 1, (int)(yp1 + ys2) - 1, White, FALSE);
					DrawBox((int)(xp1 - xs1) + 2, (int)(yp1 - ys1) + 2, (int)(xp1 + xs2) - 2, (int)(yp1 + ys2) - 2, Green, FALSE);

					xs1 = y_r((int)(256.f * 0.5f*m_Rader_r));
					ys1 = y_r((int)(256.f * 0.8f*m_Rader_r));
					xs2 = y_r((int)(256.f * 0.5f*m_Rader_r));
					ys2 = y_r((int)(256.f * 0.2f*m_Rader_r));
					DrawBox((int)(xp1 - xs1), (int)(yp1 - ys1), (int)(xp1 + xs2), (int)(yp1 + ys2), Green, FALSE);
					DrawBox((int)(xp1 - xs1) + 1, (int)(yp1 - ys1) + 1, (int)(xp1 + xs2) - 1, (int)(yp1 + ys2) - 1, White, FALSE);
					DrawBox((int)(xp1 - xs1) + 2, (int)(yp1 - ys1) + 2, (int)(xp1 + xs2) - 2, (int)(yp1 + ys2) - 2, Green, FALSE);

					auto BaseBos = Chara->GetMatrix().pos();

					xs1 = y_r((int)(256.f * 0.5f));
					ys1 = y_r((int)(256.f * 0.8f));
					xs2 = y_r((int)(256.f * 0.5f));
					ys2 = y_r((int)(256.f * 0.2f));
					auto base = Chara->GetCharaDir().zvec()*-1.f;
					base.y(0.f);
					base = base.Norm();
					auto vec = VECTOR_ref::front();
					auto rad = std::atan2f(base.cross(vec).y(), base.dot(vec));
					{
						m_Rader = 1.f;
						bool isout = true;
						auto tmpRader = 1.f;
						int div = 4;
						for (int i = 1; i < chara_num; i++) {
							auto& c = (std::shared_ptr<CharacterClass>&)(*this->Obj.GetObj(ObjType::Human, i));
							tmpRader = 1.f;
							for (int j = 0; j < div; j++) {
								auto pos = MATRIX_ref::Vtrans(c->GetMatrix().pos() - BaseBos, MATRIX_ref::RotY(rad))*((1.f / 12.5f) * tmpRader);
								if (((-xs1 < pos.x() && pos.x() < xs2) && (-ys1 < -pos.z() && -pos.z() < ys2))) {
									if (m_Rader >= tmpRader) {
										m_Rader = tmpRader;
									}
									isout = false;
									break;
								}
								tmpRader -= 1.f / div;
							}
						}
						if (isout) {
							m_Rader = tmpRader + 1.f / div;
						}
					}

					for (int i = 1; i < chara_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*this->Obj.GetObj(ObjType::Human, i));
						auto pos = MATRIX_ref::Vtrans(c->GetMatrix().pos() - BaseBos, MATRIX_ref::RotY(rad))*((1.f / 12.5f) * m_Rader_r);
						if ((-xs1 < pos.x() && pos.x() < xs2) && (-ys1 < -pos.z() && -pos.z() < ys2)) {
							int xp, yp;
							xp = xp1 + (int)(pos.x());
							yp = yp1 + (int)(-pos.z());
							DrawCircle(xp, yp, 5, Red, TRUE);
						}
					}
				}
				//
			}
		};
	};
};
