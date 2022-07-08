#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			GraphHandle HeartGraph;
			GraphHandle ScoreBoard;
			FontPool UI;

			int intParam[3]{ 0 };
			float floatParam[4]{ 0 };
		public:
			void Set(void) noexcept {
				HeartGraph = GraphHandle::Load("data/UI/Heart.png");
				ScoreBoard = GraphHandle::Load("data/UI/Score.png");
			}
			void Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				//心拍数
				{
					int siz = y_r(32);
					int xP = siz, yP = DrawParts->disp_y - siz - y_r(15);
					int xo, yo;
					HeartGraph.GetSize(&xo, &yo);
					HeartGraph.DrawRotaGraph(xP, yP, (float)(siz) / xo * floatParam[0], 0.f, true);
					UI.Get(y_r(24)).Get_handle().DrawStringFormat(xP + siz / 2, yP + siz / 2 - y_r(24), GetColor(255, 0, 0), "%03d", intParam[0]);
				}
				//スタミナ
				{
					float Xsize = (float)(y_r(200));
					int siz = y_r(12);
					int xP = siz, yP = DrawParts->disp_y - siz - y_r(12);
					DrawBox(xP, yP, xP + (int)Xsize, yP + siz, GetColor(0, 0, 0), TRUE);
					DrawBox(xP, yP, xP + (int)(Xsize * floatParam[1]), yP + siz, (floatParam[1] > 0.3f) ? GetColor(0, 255, 0) : GetColor(255, 200, 0), TRUE);
					DrawBox(xP, yP, xP + (int)Xsize, yP + siz, GetColor(128, 128, 128), FALSE);
				}
				//向き
				{
					float Xsize = (float)(y_r(125));
					int siz = y_r(12);
					int xP = siz + (int)Xsize, yP = DrawParts->disp_y - siz - y_r(64);
					int xpt = xP, ypt = yP;
					for (int i = 0; i < 5; i++) {
						float max = Xsize * (i + 1) / 5;
						float rad = floatParam[2] * (i + 1) / 5;
						DrawLine(
							xpt, ypt,
							xP + (int)(max*sin(rad)), yP + (int)(max*-cos(rad)),
							GetColor(255, 0, 0), 5 - i);
						xpt = xP + (int)(max*sin(rad));
						ypt = yP + (int)(max*-cos(rad));
					}
					{
						float max = Xsize;
						float rad = floatParam[2];
						xpt = xP + (int)(max*sin(rad));
						ypt = yP + (int)(max*-cos(rad));

						float yap = Xsize / 5;
						DrawLine(
							xpt - (int)(yap*sin(rad * 2.f - deg2rad(15))), ypt - (int)(yap*-cos(rad * 2.f - deg2rad(15))),
							xpt, ypt,
							GetColor(255, 0, 0), 2);
						DrawLine(
							xpt - (int)(yap*sin(rad * 2.f + deg2rad(15))), ypt - (int)(yap*-cos(rad * 2.f + deg2rad(15))),
							xpt, ypt,
							GetColor(255, 0, 0), 2);
					}

					UI.Get(y_r(24)).Get_handle().DrawStringFormat_RIGHT(xP - siz / 2, yP - y_r(14), GetColor((int)(192.f - 64.f*floatParam[2] * 2.f), 0, 0), "Q");
					UI.Get(y_r(24)).Get_handle().DrawStringFormat(xP + siz / 2, yP - y_r(14), GetColor((int)(192.f + 64.f*floatParam[2] * 2.f), 0, 0), "E");
				}
				//弾数
				{
					int xP = DrawParts->disp_x - y_r(24), yP = DrawParts->disp_y - y_r(32);

					UI.Get(y_r(32)).Get_handle().DrawStringFormat_RIGHT(xP - y_r(48), yP - y_r(18), GetColor(255, 255, 255), "%02d", intParam[1]);
					UI.Get(y_r(24)).Get_handle().DrawStringFormat_RIGHT(xP - y_r(30), yP - y_r(6), GetColor(192, 192, 192), "/");
					UI.Get(y_r(18)).Get_handle().DrawStringFormat_RIGHT(xP, yP, GetColor(192, 192, 192), "%02d", intParam[2]);
				}
				//スコア
				{
					int xP = y_r(150), yP = DrawParts->disp_y - y_r(64);
					UI.Get(y_r(32)).Get_handle().DrawStringFormat_RIGHT(xP + y_r(54), yP, GetColor(255, 255, 255), "%02d", (int)floatParam[3]);
					UI.Get(y_r(18)).Get_handle().DrawStringFormat(xP + y_r(54), yP + y_r(14), GetColor(255, 255, 255), ".%02d", (int)((floatParam[3] - (float)((int)floatParam[3]))*100.f));
				}
			}

			void SetIntParam(int ID, int value) { intParam[ID] = value; }
			void SetfloatParam(int ID, float value) { floatParam[ID] = value; }

			auto& GetScoreBoard(void) { return ScoreBoard; }
		};

		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			//リソース関連
			ObjectManager Obj;				//モデル
			BackGroundClass BackGround;		//BG
			SoundHandle Env;
			//
			static const int chara_num = 3;
			static const int tgt_num = 2;
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
			float scoreBuf{ 0.f };
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
				for (int i = 0; i < chara_num; i++) {
					this->Obj.AddObject(ObjType::Human);
					this->Obj.LoadObj("data/Charactor/Charactor01/");
				}
				//guide
				this->Obj.AddObject(ObjType::Circle);
				this->Obj.LoadObj("data/model/Circle/");
				//init
				this->Obj.InitObject(&this->BackGround.GetGroundCol());
				//ロード
				SetCreate3DSoundFlag(FALSE);
				Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				Env.vol(64);
				//UI
				UI_class.Set();
				this->scoreBuf = 0.f;
				//
				TEMPSCENE::Set();
				//Set
				//人
				for (int i = 0; i < chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), false, VECTOR_ref::vget(0.f, 0.f, -52.5f + (float)(i - 1)*20.f));
				}
				//ガイドサークル
				{
					auto& c = this->Obj.GetObj(ObjType::Circle, 0);
					c->SetMove(MATRIX_ref::RotY(deg2rad(-90)), BackGround.ShotPos[0] + VECTOR_ref::vget(-10.f, 0, -20));
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
					this->m_ReadyTime = 10.f;
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
					printfDx("%f \n", this->m_ReadyTime);
				}
				//Input,AI
				{
					MouseActive.GetInput(CheckHitKey_M(KEY_INPUT_TAB) != 0);
					FPSActive.GetInput(CheckHitKey_M(KEY_INPUT_V) != 0);
					RunKey.GetInput(CheckHitKey_M(KEY_INPUT_LSHIFT) != 0);
					auto MidPress = (GetMouseInput_M() & MOUSE_INPUT_MIDDLE) != 0;
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
					
					easing_set(&TPS_Per, (!FPSActive.on() && MidPress) ? 1.f : 0.f, 0.9f);

					TPS_Xrad += pp_x;
					TPS_Yrad += pp_y;
					TPS_Xrad = std::clamp(TPS_Xrad, deg2rad(-40), deg2rad(40));
					if (TPS_Yrad >= deg2rad(180)) { TPS_Yrad -= deg2rad(360); }
					if (TPS_Yrad <= deg2rad(-180)) { TPS_Yrad += deg2rad(360); }

					TPS_Xrad *= TPS_Per;
					TPS_Yrad *= TPS_Per;

					easing_set(&TPS_XradR, TPS_Xrad, 0.5f);

					TPS_YradR += (sin(TPS_Yrad)*cos(TPS_YradR) - cos(TPS_Yrad) * sin(TPS_YradR))*20.f / FPS;

					Chara->SetEyeVec((camera_main.camvec - camera_main.campos).Norm());

					for (int i = 0; i < chara_num; i++) {
						if (i == 0
							//&& false
							) {
							Chara->SetInput(
								pp_x*(1.f - TPS_Per),
								pp_y*(1.f - TPS_Per),
								CheckHitKey_M(KEY_INPUT_W) != 0,
								CheckHitKey_M(KEY_INPUT_S) != 0,
								CheckHitKey_M(KEY_INPUT_A) != 0,
								CheckHitKey_M(KEY_INPUT_D) != 0,
								RunKey.press(),
								CheckHitKey_M(KEY_INPUT_SPACE) != 0,
								this->m_ReadyTime < 0.f
							);
							continue;
						}


						auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
						c->SetInput(
							0.f,
							0.f,
							false,
							false,
							false,
							false,
							false,
							false,
							true
						);
					}
				}
				//Execute
				this->Obj.ExecuteObject();
				//col
				/*
				for (int i = 0; i < chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
					auto& Gun = c->GetGunPtr();
					if (Gun != nullptr) {
						for (int j = 0; j < tgt_num; j++) {
							auto s = (i*tgt_num + j);
							auto& t = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, s));
							if (Gun->CheckBullet(t->GetCol())) {
								//エフェクト
								Effect_UseControl::Set_Effect(Effect::ef_fire, Gun->GetHitPos(), Gun->GetHitVec(), 1.f);
								//ヒット演算
								if (i == 0) {
									if (tgtSel != -1 && tgtSel != s) {
										auto& tOLD = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, tgtSel));
										tOLD->ResetHit();
									}
									tgtSel = s;
									tgtTimer = 5.f;
								}
								c->AddScore(t->SetHitPos(Gun->GetHitPos()));
							}
						}
						if (Gun->CheckBullet(&this->BackGround.GetGroundCol())) {
							//エフェクト
							Effect_UseControl::Set_Effect(Effect::ef_fire, Gun->GetHitPos(), Gun->GetHitVec(), 1.f);
						}
					}
				}
				tgtTimer = std::max(tgtTimer - 1.f / FPS, 0.f);
				for (int j = 0; j < gun_num; j++) {
					auto& Gun = (std::shared_ptr<GunClass>&)(this->Obj.GetObj(ObjType::Gun, j));
					if (Gun->GetIsShot()) {
						//エフェクト
						auto mat = Gun->GetMuzzleMatrix();
						Effect_UseControl::Set_Effect(Effect::ef_fire2, mat.pos(), mat.GetRot().zvec()*-1.f, 1.f);
					}
				}
				//*/
				//視点
				{

					if (FPSActive.on()) {
						camera_main.campos = Chara->GetEyePosition();
						camera_main.camvec = camera_main.campos + Chara->GetEyeVector();
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();
					}
					else {
						MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot()*MATRIX_ref::RotY(TPS_YradR);
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * 14.f;

						VECTOR_ref CamVec = MATRIX_ref::Vtrans(Chara->GetEyeVector(), MATRIX_ref::RotY(TPS_YradR));

						CamVec = MATRIX_ref::Vtrans(CamVec, MATRIX_ref::RotAxis(UpperMat.xvec(), TPS_XradR));
						CamVec = Leap(Chara->GetEyeVector(), CamVec, TPS_Per);

						CamPos += Leap((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), EyeRunPer);
						camera_main.campos = (CamPos + CamVec * -20.f);
						camera_main.camvec = CamPos + CamVec * 100.f;
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();
					}
					easing_set(&EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f);

					if (Chara->GetIsRun()) {
						easing_set(&camera_main.fov, deg2rad(90), 0.9f);
						easing_set(&camera_main.near_, 3.f, 0.9f);
						easing_set(&camera_main.far_, 12.5f * 150.f, 0.9f);
					}
					else {
						easing_set(&camera_main.fov, deg2rad(75), 0.9f);
						easing_set(&camera_main.near_, 10.f, 0.9f);
						easing_set(&camera_main.far_, 12.5f * 300.f, 0.9f);
					}
				}
				//UIパラメーター
				{
					UI_class.SetIntParam(0, (int)(Chara->GetHeartRate()));
					UI_class.SetfloatParam(0, 1.f + sin(Chara->GetHeartRateRad()*4.f)*0.1f);
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
					auto& Chara = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, 0));
					//
					Set_is_Blackout(true);
					Set_Per_Blackout((1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
					//
					Set_is_lens(false);
				}
				for (auto& t : BackGround.TurnOff) {
					DrawCone3D(t.get(), (t + VECTOR_ref::up()*12.5f*10.f).get(), 12.5f*10.f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
				}
				for (auto& t : BackGround.TurnOn) {
					DrawCone3D(t.get(), (t + VECTOR_ref::up()*12.5f*10.f).get(), 12.5f*10.f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
				}
			}
			void Main_Draw2(void) noexcept override {
				this->Obj.DrawDepthObject();
			}
			void LAST_Draw(void) noexcept override {
			}
			//UI表示
			void UI_Draw(void) noexcept  override {
				auto* DrawParts = DXDraw::Instance();
				UI_class.Draw();
			}
		};
	};
};