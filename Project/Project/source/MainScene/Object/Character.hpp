#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterClass : public ObjectBaseClass {
		private://キャラパラメーター
			const float SpeedLimit{ 2.f };
			const float UpperTimerLimit = 3.f;
			const float HeartRateMin{ 60.f };//心拍数最小
			const float HeartRateMax{ 180.f };//心拍数最大
		private:
			VECTOR_ref m_posBuf;
			MATRIX_ref m_UpperMatrix;

			VECTOR_ref m_FlightVecBuf;
			MATRIX_ref m_FlightMatrix;


			std::array<float, 4> m_Vec{ 0,0,0,0 };
			std::array<float, (int)CharaAnimeID::AnimeIDMax> m_AnimPerBuf{ 0 };
			float m_MoveVector{ 0.f };
			float m_xrad_Add{ 0.f }, m_yrad_Add{ 0.f };
			VECTOR_ref m_rad_Buf, m_rad;
			float m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float m_RunTimer{ 0.f };
			float m_RunPer{ 0.f };
			float m_FlightPer{ 0.f };
			float m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			bool m_TurnBody{ false };
			bool m_IsRun{ false };
			bool m_Flightmode{ false };
			CharaAnimeID UpperAnimSelect, PrevUpperAnimSel;
			CharaAnimeID BottomAnimSelect;
			bool m_Ready_Start{ true };
			bool m_ReturnStand{ false };
			//
			VECTOR_ref m_FradAdd_Buf, m_FradAdd;
			//入力
			bool m_Press_GoFront{ false };
			bool m_Press_GoRear{ false };
			bool m_Press_GoLeft{ false };
			bool m_Press_GoRight{ false };
			//体力
			float m_HeartRate{ HeartRateMin };//心拍数
			float m_HeartRate_r{ HeartRateMin };//心拍数
			float m_HeartRateRad{ 0.f };//呼吸Sin渡し
			//表情
			int m_Eyeclose{ 0 };
			float m_EyeclosePer{ 0.f };
			float m_NeckPer{ 0.f };
			//サウンド
			int charaSound{ -1 };
			SoundHandle m_Breath;
			bool heartp{ false };
		public://ゲッター
			const auto GetPressFrontGround() const noexcept { return this->m_Press_GoFront && !this->m_Flightmode; }
			const auto GetPressRearGround() const noexcept { return this->m_Press_GoRear && !this->m_Flightmode; }
			const auto GetPressLeftGround() const noexcept { return this->m_Press_GoLeft && !this->m_Flightmode; }
			const auto GetPressRightGround() const noexcept { return this->m_Press_GoRight && !this->m_Flightmode; }

			const auto GetCharaDir(void) const noexcept {
				if (!this->m_Flightmode) {
					return this->m_UpperMatrix * this->m_move.mat;
				}
				else {
					return this->m_UpperMatrix * this->m_FlightMatrix;
				}
			}
			const auto GetEyeVector(void) const noexcept { return GetCharaDir().zvec() * -1.f; }
			const auto GetEyePosition(void) const noexcept { return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + this->GetEyeVector().Norm() * 0.5f; }
			const auto& GetIsRun(void) const noexcept { return this->m_IsRun; }
			const auto& GetHeartRate(void) const noexcept { return this->m_HeartRate; }
			const auto& GetHeartRateRad(void) const noexcept { return this->m_HeartRateRad; }
			const auto& GetFlightPer(void) const noexcept { return this->m_FlightPer; }
		private: //
			void SetVec(int pDir, bool Press) {
				this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
		private: //
			//以前の状態保持
			void ExecuteSavePrev(void) noexcept {
				this->PrevUpperAnimSel = this->UpperAnimSelect;
				this->m_PrevRunPer2 = this->m_RunPer2;
			}
			//アニメ
			void ExecuteAnim(void) noexcept {
				auto SE = SoundPool::Instance();
				//アニメ演算
				{
					//上半身
					{
						UpperAnimSelect = CharaAnimeID::Upper_Normal;
						if (this->m_IsRun) {
							UpperAnimSelect = CharaAnimeID::Upper_Running;
						}
						//首
						easing_set(&m_NeckPer, (true) ? 0.f : 1.f, 0.9f);
					}
					//下半身
					{
						if (GetPressFrontGround()) {
							BottomAnimSelect = (this->m_IsRun) ? CharaAnimeID::Bottom_Run : CharaAnimeID::Bottom_Walk;
						}
						else {
							BottomAnimSelect = CharaAnimeID::Bottom_Stand;
							if (GetPressLeftGround()) { BottomAnimSelect = CharaAnimeID::Bottom_LeftStep; }
							if (GetPressRightGround()) { BottomAnimSelect = CharaAnimeID::Bottom_RightStep; }
							if (GetPressRearGround()) { BottomAnimSelect = CharaAnimeID::Bottom_WalkBack; }
						}
					}
					//
					SetAnimLoop((int)CharaAnimeID::Upper_Running, 0.5f * this->m_Vec[0] * this->m_RunPer2);
					SetAnimLoop((int)CharaAnimeID::Bottom_Turn, 0.5f);
					SetAnimLoop((int)CharaAnimeID::Bottom_Run, 0.5f * this->m_Vec[0] * this->m_RunPer2);
					SetAnimLoop((int)CharaAnimeID::Bottom_Walk, 1.15f * this->m_Vec[0]);
					SetAnimLoop((int)CharaAnimeID::Bottom_LeftStep, 1.15f * this->m_Vec[1]);
					SetAnimLoop((int)CharaAnimeID::Bottom_WalkBack, 1.15f * this->m_Vec[2]);
					SetAnimLoop((int)CharaAnimeID::Bottom_RightStep, 1.15f * this->m_Vec[3]);
				}
				//アニメセレクト
				{
					//上半身
					this->m_AnimPerBuf[(int)CharaAnimeID::Upper_Flight] = 1.f;
					this->m_AnimPerBuf[(int)CharaAnimeID::Bottom_Flight] = 1.f;
					//下半身
					easing_set(&this->m_AnimPerBuf[(int)CharaAnimeID::Bottom_Turn], (this->m_TurnBody) ? abs(this->m_rad.y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f);
					//その他
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//上半身
						if (
							//i == (int)CharaAnimeID::Upper_Flight ||
							i == (int)CharaAnimeID::Upper_Normal ||
							i == (int)CharaAnimeID::Upper_Running
							)
						{
							this->m_AnimPerBuf[i] += ((i == (int)UpperAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						//下半身
						if (
							//i == (int)CharaAnimeID::Bottom_Flight ||
							i == (int)CharaAnimeID::Bottom_Stand ||
							i == (int)CharaAnimeID::Bottom_Run ||
							i == (int)CharaAnimeID::Bottom_Walk ||
							i == (int)CharaAnimeID::Bottom_LeftStep ||
							i == (int)CharaAnimeID::Bottom_RightStep ||
							i == (int)CharaAnimeID::Bottom_WalkBack
							)
						{
							this->m_AnimPerBuf[i] += ((i == (int)BottomAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i], 0.f, 1.f);
						//反映
						if (
							i == (int)CharaAnimeID::Upper_Flight ||
							i == (int)CharaAnimeID::Bottom_Flight
							) {
							this->m_obj.get_anime(i).per = this->m_AnimPerBuf[i] * this->m_FlightPer;
						}
						else {
							this->m_obj.get_anime(i).per = this->m_AnimPerBuf[i] * (1.f - this->m_FlightPer);
						}

					}
					//
				}
				//足音
				if (!this->m_Flightmode) {
					if (BottomAnimSelect != CharaAnimeID::Bottom_Stand) {
						auto Time = this->m_obj.get_anime((int)BottomAnimSelect).time;
						if (BottomAnimSelect != CharaAnimeID::Bottom_Run) {
							//L
							if ((9.f < Time && Time < 10.f)) {
								if (charaSound != 1) {
									charaSound = 1;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
								}
							}
							//R
							if ((27.f < Time &&Time < 28.f)) {
								if (charaSound != 3) {
									charaSound = 3;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
								}
							}
						}
						else {
							//L
							if (
								(18.f < Time &&Time < 19.f) ||
								(38.f < Time &&Time < 39.f)
								) {
								if (charaSound != 5) {
									charaSound = 5;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
								}
							}
							//R
							if (
								(8.f < Time &&Time < 9.f) ||
								(28.f < Time &&Time < 29.f)
								) {
								if (charaSound != 6) {
									charaSound = 6;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
								}
							}
						}
						this->m_ReturnStand = true;
					}
					else if (this->m_ReturnStand) {
						if (charaSound != 7) {
							charaSound = 7;
							SE->Get((int)SoundEnum::SlideFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f, (int)(192.f * this->m_RunPer2 / SpeedLimit));
						}
						this->m_ReturnStand = false;
					}
				}
				//
			}
			//
			void ExecuteHeartRate(void) noexcept {
				auto SE = SoundPool::Instance();
				auto addRun = (this->m_RunPer2 - this->m_PrevRunPer2);
				if (addRun > 0.f) {
					this->m_HeartRate_r += (10.f + GetRandf(10.f)) / FPS;
				}
				else if (addRun < 0.f) {
					this->m_HeartRate_r -= (5.f + GetRandf(5.f)) / FPS;
				}
				this->m_HeartRate_r += (this->m_MoveVector * this->m_RunPer2 / SpeedLimit * 1.95f) / FPS;
				this->m_HeartRate_r -= (2.f + GetRandf(4.f)) / FPS;
				this->m_HeartRate_r = std::clamp(this->m_HeartRate_r, HeartRateMin, HeartRateMax);

				if (this->m_HeartRate < this->m_HeartRate_r) {
					this->m_HeartRate += 5.f / FPS;
				}
				else if (this->m_HeartRate >= this->m_HeartRate_r) {
					this->m_HeartRate -= 5.f / FPS;
				}
				//this->m_HeartRate = this->m_HeartRate_r;
				this->m_HeartRateRad += deg2rad(this->m_HeartRate) / FPS;
				if (this->m_HeartRateRad >= DX_PI_F * 2) {
					this->m_HeartRateRad -= DX_PI_F * 2;
				}
				if (
					(deg2rad(0) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(10)) ||
					(deg2rad(120) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(130)) ||
					(deg2rad(240) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(250))
					) {
					if (!this->heartp) {
						this->heartp = true;
						SE->Get((int)SoundEnum::Heart).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper2).pos(), 12.5f * 1.f);
					}
				}
				else {
					this->heartp = false;
				}
			}
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				ValueSet(deg2rad(0.f), deg2rad(0.f), false, VECTOR_ref::vget(0.f, 0.f, 0.f));
			}
			~CharacterClass(void) noexcept {}
		public: //継承
			void Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_obj.get_anime((int)CharaAnimeID::Bottom_Stand).per=1.f;
				//
				SetCreate3DSoundFlag(TRUE);
				this->m_Breath = SoundHandle::Load("data/Sound/SE/voice/WinningTicket/breath.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Breath.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Breath.get());
			}
			void Execute(void) noexcept override {
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < this->m_obj.get_anime().size(); i++) { this->m_obj.get_anime(i).per = 0.f; }
					this->m_obj.work_anime();
				}
				ExecuteSavePrev();
				//操作//0.01ms
				{
					//移動ベクトル取得
					{
						SetVec(0, GetPressFrontGround());
						SetVec(1, GetPressLeftGround());
						SetVec(2, GetPressRearGround());
						SetVec(3, GetPressRightGround());
					}
					//
					easing_set(&this->m_RunPer, this->m_IsRun ? 1.f : 0.f, 0.975f);
					easing_set(&this->m_FlightPer, this->m_Flightmode ? 1.f:0.f, 0.95f);

					//this->m_PronePer = std::clamp(this->m_PronePer + (GetIsProne() ? 1.f : -3.f) / FPS, 0.f, 1.f);
					//this->m_yrad_Upper、this->m_yrad_Bottom、this->m_rad.z()決定
					{
						if (this->m_MoveVector <= 0.1f) {
							if (abs(this->m_rad.y() - this->m_yrad_Upper) > deg2rad(50.f)) {
								this->m_TurnBody = true;
							}
							if (abs(this->m_rad.y() - this->m_yrad_Upper) < deg2rad(0.5f)) {
								this->m_TurnBody = false;
							}
						}
						else {
							this->m_TurnBody = false;
						}
						//
						auto FrontP = ((GetPressFrontGround() && !GetPressRearGround())) ? (atan2f(this->m_Vec[1] - this->m_Vec[3], this->m_Vec[0] - this->m_Vec[2]) * this->m_Vec[0]) : 0.f;
						FrontP += (!GetPressFrontGround() && GetPressRearGround()) ? (atan2f(-(this->m_Vec[1] - this->m_Vec[3]), -(this->m_Vec[0] - this->m_Vec[2])) * this->m_Vec[2]) : 0.f;
						auto TmpRunPer = Leap(0.85f, 0.7f, this->m_RunPer);
						if (this->m_Flightmode) { TmpRunPer = 0.95f; }
						if (this->m_TurnBody || (this->m_MoveVector > 0.1f) || this->m_Flightmode) { easing_set(&this->m_yrad_Upper, this->m_rad.y(), TmpRunPer); }
						auto OLDP = this->m_yrad_Bottom;
						easing_set(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP, TmpRunPer);
						{
							auto zbuf = this->m_rad_Buf.z();
							easing_set(&zbuf, (this->m_yrad_Bottom - OLDP) * 2.f, 0.9f);
							this->m_rad_Buf.z(zbuf);
						}
					}
				}
				//上半身回転//0.06ms
				{
					this->m_UpperMatrix = MATRIX_ref::RotX(this->m_rad.x()) * MATRIX_ref::RotY(this->m_rad.y() - this->m_yrad_Bottom);
					this->m_obj.frame_Reset(this->Frames[(int)CharaFrame::Upper].first);
					SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * LeapM(this->m_UpperMatrix,MATRIX_ref::zero(),this->m_FlightPer));
				}
				//AnimUpdte//0.03ms
				ExecuteAnim();
				//SetMat指示//0.03ms
				{
					this->m_RunPer2 = Leap(0.35f, SpeedLimit, this->m_RunPer);
					if (this->m_PrevRunPer2 == 0.f) {
						this->m_PrevRunPer2 = this->m_RunPer2;
					}

					auto OLDpos = this->m_posBuf;
					VECTOR_ref vecBuf = MATRIX_ref::Vtrans(VECTOR_ref::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]), MATRIX_ref::RotY(this->m_yrad_Upper));
					this->m_MoveVector = std::clamp(vecBuf.size(), 0.f, 1.f);

					if (this->m_MoveVector > 0.1f) {
						this->m_move.vec.x((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).x());
						this->m_move.vec.z((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).z());
					}
					else {
						this->m_move.vec.x(vecBuf.x());
						this->m_move.vec.z(vecBuf.z());
					}
					{
						auto HitResult = this->m_MapCol->CollCheck_Line(this->m_posBuf + VECTOR_ref::up() * -1.f, this->m_posBuf + VECTOR_ref::up() * 15.f);
						if (HitResult.HitFlag == TRUE) {
							auto yPos = this->m_posBuf.y();
							easing_set(&yPos, HitResult.HitPosition.y, 0.8f);
							this->m_posBuf.y(yPos);
							if (this->m_move.vec.y() != 0.f) {
								this->m_Flightmode = false;
							}
							this->m_move.vec.y(0.f);
						}
						else {
							if (!this->m_Flightmode) {
								this->m_move.vec.yadd(M_GR / (FPS * FPS));
							}
							else {
								this->m_move.vec.y(0.01f);
							}
						}
					}
					this->m_posBuf += this->m_move.vec;

					if (this->m_Flightmode) {

						easing_set(&this->m_FlightVecBuf, this->GetCharaDir().zvec() * -1.f * 3.f * 60.f / FPS, 0.95f);

						this->m_posBuf += this->m_FlightVecBuf;
					}
					else {
						easing_set(&this->m_FlightVecBuf, VECTOR_ref::zero(), 0.975f);
					}

					col_wall(OLDpos, &this->m_posBuf, *this->m_MapCol);
					//this->m_move.mat = MATRIX_ref::RotZ(this->m_rad.z()) * MATRIX_ref::RotY(this->m_yrad_Bottom);
					if (!this->m_Flightmode) {
						this->m_move.mat = LeapM(MATRIX_ref::RotZ(this->m_rad.z()) * MATRIX_ref::RotY(this->m_yrad_Bottom), this->m_move.mat, this->m_FlightPer);
						this->m_FlightMatrix = this->m_move.mat;
					}
					else {
						this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), this->m_FradAdd.x());
						this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_FradAdd.y());

						easing_setM(&this->m_move.mat, this->m_FlightMatrix, 0.6f);
					}

					easing_set(&this->m_move.pos, this->m_posBuf, 0.9f);
					UpdateMove();
				}
				{
					this->m_obj.work_anime();
				}
				//顔//スコープ内0.01ms
				{
					SetShape(CharaShape::None, 0.f);
					SetShape(CharaShape::O, (0.75f + sin(this->m_HeartRateRad * 3)*0.25f)*(0.5f));
					if (this->m_EyeclosePer <= 0.05f && (GetRand(100) == 0)) {
						this->m_Eyeclose = 1;
					}
					if (this->m_EyeclosePer >= 0.95f) {
						this->m_Eyeclose = 0;
					}
					easing_set(&this->m_EyeclosePer, (float)this->m_Eyeclose, 0.5f);
					SetShape(CharaShape::EYECLOSE, this->m_EyeclosePer);
				}
				//心拍数//0.00ms
				ExecuteHeartRate();
			}
			//void Draw(void) noexcept override { ObjectBaseClass::Draw(); }
		public:
			void ValueSet(float pxRad, float pyRad, bool IsFlight, const VECTOR_ref& pPos) {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					this->m_AnimPerBuf[i] = 0.f;
				}
				this->m_HeartRate = HeartRateMin;
				this->m_RunPer = 0.f;
				this->m_MoveVector = 0.f;
				this->m_RunPer2 = 0.f;
				this->m_PrevRunPer2 = 0.f;
				this->m_HeartRateRad = 0.f;
				this->m_xrad_Add = 0.f;
				this->m_yrad_Add = 0.f;
				this->m_TurnBody = false;
				this->m_Press_GoFront = false;
				this->m_Press_GoRear = false;
				this->m_Press_GoLeft = false;
				this->m_Press_GoRight = false;
				this->m_Ready_Start = false;
				this->m_IsRun = false;
				this->heartp = false;
				//動作にかかわる操作
				this->m_rad_Buf.x(pxRad);
				this->m_rad_Buf.y(pyRad);
				this->m_Flightmode = IsFlight;
				this->m_posBuf = pPos;

				this->m_FradAdd_Buf.clear();
				this->m_FradAdd.clear();
				//上記を反映するもの
				this->m_rad = this->m_rad_Buf;
				this->m_yrad_Upper = this->m_rad.y();
				this->m_yrad_Bottom = this->m_rad.y();
				this->m_move.vec.clear();
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_rad.x()) * MATRIX_ref::RotY(this->m_rad.y() - this->m_yrad_Bottom);
				this->m_FradAdd;
				SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), this->m_posBuf);
			}
			void SetInput(
				float pAddxRad, float pAddyRad,
				bool pGoFrontPress,
				bool pGoBackPress,
				bool pGoLeftPress,
				bool pGoRightPress,
				bool pRunPress,
				bool pGoFlight,
				bool pReady
			) {
				this->m_Ready_Start = pReady;

				this->m_Flightmode |= pGoFlight && this->m_Ready_Start;
				this->m_Press_GoFront = pGoFrontPress && this->m_Ready_Start;
				this->m_Press_GoRear = pGoBackPress && this->m_Ready_Start;
				this->m_Press_GoLeft = pGoLeftPress && this->m_Ready_Start;
				this->m_Press_GoRight = pGoRightPress && this->m_Ready_Start;

				if (!this->m_IsRun && (pRunPress && pGoFrontPress&& this->m_Ready_Start)) {
					this->m_RunTimer = 1.f;
				}
				if (this->m_RunTimer > 0.f) {
					this->m_RunTimer -= 1.f / FPS;
					this->m_IsRun = true;
				}
				else {
					this->m_RunTimer = 0.f;
					this->m_IsRun = (pRunPress && pGoFrontPress&& this->m_Ready_Start);
				}
				if (GetPressRearGround() || (!GetPressFrontGround() && (GetPressLeftGround() || GetPressRightGround()))) {
					this->m_IsRun = false;
				}
				//回転
				{
					auto limchange = Leap(1.f, powf(1.f - this->m_Vec[0], 0.5f), this->m_RunPer * 0.8f);
					auto tmp2 = 0.2f * GetRandf(deg2rad(1.f));
					auto tmp3 = 0.5f;

					easing_set(&this->m_xrad_Add, tmp2 + 0.0002f * sin(this->m_HeartRateRad) * powf(this->m_HeartRate / HeartRateMin, 3.f), 0.95f);
					easing_set(&this->m_yrad_Add, tmp2 + 0.0002f * sin(this->m_HeartRateRad * 3) * powf(this->m_HeartRate / HeartRateMin, 3.f), 0.95f);

					auto prev = this->m_rad_Buf;
					this->m_rad_Buf.x(std::clamp(this->m_rad_Buf.x() + pAddxRad, -deg2rad(40.f) * limchange, deg2rad(25.f) * limchange));
					this->m_rad_Buf.yadd(pAddyRad);
					this->m_rad_Buf.xadd(this->m_xrad_Add * tmp3);
					this->m_rad_Buf.yadd(this->m_yrad_Add * tmp3);

					this->m_rad_Buf = prev +(this->m_rad_Buf - prev)*(1.f - this->m_FlightPer);

					easing_set(&this->m_rad, this->m_rad_Buf, 0.5f);
				}
				//操縦
				{
					this->m_FradAdd_Buf.x(-pAddxRad);
					this->m_FradAdd_Buf.y(pAddyRad);
					easing_set(&this->m_FradAdd, this->m_FradAdd_Buf, 0.95f);
				}
			}
			void SetEyeVec(const VECTOR_ref& camvec) noexcept {
				this->m_obj.frame_Reset(this->Frames[(int)CharaFrame::Head].first);
				auto v1 = (GetFrameWorldMat(CharaFrame::Head).GetRot() * GetCharaDir().Inverse()).zvec()*-1.f;
				auto v2 = Leap(MATRIX_ref::Vtrans(camvec.Norm(), GetCharaDir().Inverse()), v1, m_NeckPer);

				auto radlimit = deg2rad(70);
				if (v1.dot(v2) <= cos(radlimit)) {
					v2 = v1 * cos(radlimit) + v1.cross(v1.cross(v2)) * (-sin(radlimit));
				}
				SetFrameLocalMat(CharaFrame::Head, MATRIX_ref::RotVec2(v1, v2) * GetFrameLocalMat(CharaFrame::Head).GetRot());
			}
		};
	};
};
