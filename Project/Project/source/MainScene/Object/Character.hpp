#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//入力
		class CharacterMoveGroundControl {
		private:
			std::array<float, 4>								m_Vec{ 0,0,0,0 };
			bool												m_Press_GoFront{ false };
			bool												m_Press_GoRear{ false };
			bool												m_Press_GoLeft{ false };
			bool												m_Press_GoRight{ false };
			switchs												m_QKey;
			switchs												m_EKey;
			bool												m_IsRun{ false };
			bool												m_IsSprint{ false };
			float												m_RunPer{ 0.f };
			float												m_RunTimer{ 0.f };
			float												m_SprintPer{ 0.f };
			VECTOR_ref											m_rad_Buf, m_rad, m_radAdd;
			int													m_TurnRate{ 0 };
			float												m_TurnRatePer{ 0.f };
		private: //内部
			void			SetVec(int pDir, bool Press) {
				this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
		public:
			const auto		GetTurnRatePer() const noexcept { return  this->m_TurnRatePer; }
			const auto		GetRad() const noexcept { return  this->m_rad; }
			const auto		GetVecFront() const noexcept { return  this->m_Vec[0] || m_IsSprint; }
			const auto		GetVecRear() const noexcept { return this->m_Vec[2]; }
			const auto		GetVecLeft() const noexcept { return this->m_Vec[1]; }
			const auto		GetVecRight() const noexcept { return this->m_Vec[3]; }
			const auto		GetPressFront() const noexcept { return this->m_Press_GoFront; }
			const auto		GetPressRear() const noexcept { return this->m_Press_GoRear; }
			const auto		GetPressLeft() const noexcept { return this->m_Press_GoLeft; }
			const auto		GetPressRight() const noexcept { return this->m_Press_GoRight; }
			const auto		GetRun() const noexcept { return this->m_IsRun; }
			const auto		GetRunPer() const noexcept { return  this->m_RunPer; }
			const auto		GetSprint() const noexcept { return this->m_IsSprint; }
			const auto		GetSprintPer() const noexcept { return  this->m_SprintPer; }
			const auto		GetVec() const noexcept { return VECTOR_ref::vget(GetVecLeft() - GetVecRight(), 0, GetVecRear() - GetVecFront()); }
			const auto		GetFrontP() const noexcept {
				auto FrontP = ((GetPressFront() && !GetPressRear())) ? (atan2f(GetVec().x(), -GetVec().z()) * GetVecFront()) : 0.f;
				FrontP += (!GetPressFront() && GetPressRear()) ? (atan2f(-GetVec().x(), GetVec().z()) * GetVecRear()) : 0.f;
				return FrontP;
			}

			void			SetSprintPer(float value) noexcept { this->m_SprintPer = value; }
			void			SetRadBufX(float x) noexcept {
				auto xbuf = this->m_rad_Buf.x();
				Easing(&xbuf, x, 0.9f, EasingType::OutExpo);
				this->m_rad_Buf.x(xbuf);
			}
			void			SetRadBufZ(float z) noexcept {
				auto zbuf = this->m_rad_Buf.z();
				Easing(&zbuf, z, 0.9f, EasingType::OutExpo);
				this->m_rad_Buf.z(zbuf);
			}
		public:
			void			ValueSet(float pxRad, float pyRad) {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
				this->m_Press_GoFront = false;
				this->m_Press_GoRear = false;
				this->m_Press_GoLeft = false;
				this->m_Press_GoRight = false;
				this->m_radAdd.clear();
				this->m_IsRun = false;
				this->m_IsSprint = false;
				this->m_RunPer = 0.f;
				this->m_RunTimer = 0.f;
				this->m_SprintPer = 0.f;
				//動作にかかわる操作
				this->m_rad_Buf.x(pxRad);
				this->m_rad_Buf.y(pyRad);
				//上記を反映するもの
				this->m_rad = this->m_rad_Buf;
			}
			void			SetInput(
				float pAddxRad, float pAddyRad,
				const VECTOR_ref& pAddRadvec,
				bool pGoFrontPress,
				bool pGoBackPress,
				bool pGoLeftPress,
				bool pGoRightPress,
				bool pRunPress,
				bool pQPress,
				bool pEPress
			) {
				this->m_Press_GoFront = pGoFrontPress;
				this->m_Press_GoRear = pGoBackPress;
				this->m_Press_GoLeft = pGoLeftPress;
				this->m_Press_GoRight = pGoRightPress;

				if (!this->m_IsRun && pRunPress) {
					this->m_RunTimer = 1.f;
				}
				if (this->m_RunTimer > 0.f) {
					this->m_RunTimer -= 1.f / FPS;
					this->m_Press_GoFront = true;
					this->m_IsRun = true;
				}
				else {
					this->m_RunTimer = 0.f;
					this->m_IsRun = pRunPress;
				}
				if (GetPressRear() || (!GetPressFront() && (GetPressLeft() || GetPressRight()))) {
					this->m_IsRun = false;
				}
				this->m_IsSprint = this->m_IsRun && (!GetPressFront() && !GetPressRear());
				{
					m_QKey.GetInput(pQPress);
					m_EKey.GetInput(pEPress);
					if (m_EKey.trigger()) {
						if (this->m_TurnRate > -1) {
							this->m_TurnRate--;
						}
						else {
							this->m_TurnRate++;
						}
					}
					if (m_QKey.trigger()) {
						if (this->m_TurnRate < 1) {
							this->m_TurnRate++;
						}
						else {
							this->m_TurnRate--;
						}
					}
					if (!GetRun()) {
						this->m_TurnRate = 0;
					}

					this->m_TurnRate = std::clamp(this->m_TurnRate, -1, 1);
					float xadd = 0.f;
					if (this->m_IsSprint) {
						xadd = 0.315f*(-this->m_TurnRate);//スプリント
					}
					else if (GetRun()) {
						xadd = 0.2f*(-this->m_TurnRate);//走り
					}
					Easing(&this->m_TurnRatePer, xadd, 0.9f, EasingType::OutExpo);
				}
				//回転
				{
					auto limchange = Leap(1.f, powf(1.f - this->GetVecFront(), 0.5f), this->m_RunPer * 0.8f);
					auto tmp = 1.f;
					Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);

					this->m_rad_Buf.x(
						std::clamp(this->m_rad_Buf.x() + pAddxRad * tmp, -deg2rad(40.f) * limchange, deg2rad(25.f) * limchange)
						+ this->m_radAdd.x()
					);
					this->m_rad_Buf.y(
						this->m_rad_Buf.y() + (pAddyRad + this->m_TurnRatePer / 100.f)*tmp
						+ this->m_radAdd.y()
					);

					Easing(&this->m_rad, this->m_rad_Buf, 0.5f, EasingType::OutExpo);
				}
			}
			void			Execute(void) noexcept {
				//移動ベクトル取得
				{
					SetVec(0, GetPressFront());
					SetVec(1, GetPressLeft());
					SetVec(2, GetPressRear());
					SetVec(3, GetPressRight());
				}
				//
				Easing(&this->m_RunPer, this->m_IsRun ? 1.f : 0.f, 0.975f, EasingType::OutExpo);
				Easing(&this->m_SprintPer, this->m_IsSprint ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
			}
		};
		class CharacterMoveFlightControl {
		private:
			std::array<float, 6>								m_Vec{ 0,0,0,0,0,0 };
			bool												m_Flightmode{ false };
			bool												m_Press_GoFront{ false };
			bool												m_Press_GoRear{ false };
			bool												m_Press_GoLeft{ false };
			bool												m_Press_GoRight{ false };
			switchs												m_QKey;
			switchs												m_EKey;
			float												m_FlightPer{ 0.f };
			VECTOR_ref											m_FradAdd_Buf, m_FradAdd;
		private: //内部
			void			SetVec(int pDir, bool Press) {
				this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
		public:
			const auto		GetVecFront() const noexcept { return  this->m_Vec[0]; }
			const auto		GetVecRear() const noexcept { return this->m_Vec[2]; }
			const auto		GetVecLeft() const noexcept { return this->m_Vec[1]; }
			const auto		GetVecRight() const noexcept { return this->m_Vec[3]; }
			const auto		GetVecLeftYaw() const noexcept { return this->m_Vec[4]; }
			const auto		GetVecRightYaw() const noexcept { return this->m_Vec[5]; }
			const auto		GetPressFront() const noexcept { return this->m_Press_GoFront; }
			const auto		GetPressRear() const noexcept { return this->m_Press_GoRear; }
			const auto		GetPressLeft() const noexcept { return this->m_Press_GoLeft; }
			const auto		GetPressRight() const noexcept { return this->m_Press_GoRight; }
			const auto		GetFlightPer() const noexcept { return this->m_FlightPer; }
			const auto		GetFradAdd() const noexcept { return this->m_FradAdd; }
		public:
			void			ValueSet() {
				this->m_FradAdd_Buf.clear();
				this->m_FradAdd.clear();
			}
			void			SetInput(
				bool pGoFrontPress,
				bool pGoBackPress,
				bool pGoLeftPress,
				bool pGoRightPress,
				bool pFlightMode,
				bool pQPress,
				bool pEPress
			) {
				this->m_Flightmode = pFlightMode;

				this->m_Press_GoFront = pGoFrontPress;
				this->m_Press_GoRear = pGoBackPress;
				this->m_Press_GoLeft = pGoLeftPress;
				this->m_Press_GoRight = pGoRightPress;
				m_QKey.GetInput(pQPress);
				m_EKey.GetInput(pEPress);
				if (m_EKey.trigger()) {
				}
				if (m_QKey.trigger()) {
				}
			}
			void			Execute(void) noexcept {
				//移動ベクトル取得
				{
					SetVec(0, GetPressFront());
					SetVec(1, GetPressLeft());
					SetVec(2, GetPressRear());
					SetVec(3, GetPressRight());

					SetVec(4, m_QKey.press());
					SetVec(5, m_EKey.press());
				}
				//
				Easing(&this->m_FlightPer, this->m_Flightmode ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
				//回転
				this->m_FradAdd_Buf.x((GetVecRear() - GetVecFront())*2.f / 100.f);
				this->m_FradAdd_Buf.y((GetVecRight() - GetVecLeft())*4.f / 100.f);
				this->m_FradAdd_Buf.z((GetVecRightYaw() - GetVecLeftYaw())*0.6f / 100.f);
				Easing(&this->m_FradAdd, this->m_FradAdd_Buf, 0.95f, EasingType::OutExpo);
			}
		};

		class CharacterClass : public ObjectBaseClass {
		private://キャラパラメーター
			const float SpeedLimit{ 2.f };
			const float UpperTimerLimit = 3.f;
		private:
			CharacterMoveGroundControl							m_InputGround;
			CharacterMoveFlightControl							m_InputSky;
			bool												m_KeyActive{ true };
			//飛行関連
			bool												m_Flightmode{ false };
			VECTOR_ref											m_FlightVecBuf;
			MATRIX_ref											m_FlightMatrix;
			//
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			VECTOR_ref											m_PosBuf;
			MATRIX_ref											m_UpperMatrix;
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float												m_NeckPer{ 0.f };

			float m_SlingHoukiPer{ 0.f };
			bool												m_TurnBody{ false };
			bool												m_ReturnStand{ false };
			bool m_RunReady{ false };
			bool m_RunReadyFirst{ false };
			bool m_Running{ false };
			CharaAnimeID										m_UpperAnimSelect, m_PrevUpperAnimSel;
			CharaAnimeID										m_BottomAnimSelect;
			MATRIX_ref											m_RightWristRot;
			float												m_Score{ 0.f };			//スコア
			float m_LeftHandPer{ 0.f };
			//入力
			bool m_Press_Shot{ false };
			bool m_Press_Reload{ false };
			bool m_Press_Aim{ false };
			bool m_ReadySwitch{ false };
			//表情
			int													m_Eyeclose{ 0 };
			float												m_EyeclosePer{ 0.f };
			//サウンド
			int													m_CharaSound{ -1 };
			SoundHandle											m_Breath;
			bool												m_HeartSoundFlag{ false };
			std::shared_ptr<HoukiClass>							m_Houki_Ptr{ nullptr };			//箒
		public://ゲッター
			//
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalMatrix(Frames[(int)frame].first); }
			const auto		GetParentFrameLocalMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalMatrix((int)this->m_obj.frame_parent(Frames[(int)frame].first)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix(Frames[(int)frame].first); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix((int)this->m_obj.frame_parent(Frames[(int)frame].first)); }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { this->m_obj.frame_Reset(Frames[(int)frame].first); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { this->m_obj.SetFrameLocalMatrix(Frames[(int)frame].first, value * Frames[(int)frame].second); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { Shapes[(int)pShape].second = Per; }

			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->m_obj.get_anime((int)anim); }
			void			SetAnimOnce(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimOnce((int)ID, speed); }
			void			SetAnimLoop(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimLoop((int)ID, speed); }

			//
			const auto		GetPressFrontGround() const noexcept { return this->m_InputGround.GetPressFront(); }
			const auto		GetPressRearGround() const noexcept { return this->m_InputGround.GetPressRear(); }
			const auto		GetPressLeftGround() const noexcept { return this->m_InputGround.GetPressLeft(); }
			const auto		GetPressRightGround() const noexcept { return this->m_InputGround.GetPressRight(); }
			const auto		GetIsRun(void) const noexcept { return this->m_InputGround.GetRun(); }
			//
			void SetHoukiPtr(std::shared_ptr<HoukiClass>& pHoukiPtr) noexcept { this->m_Houki_Ptr = pHoukiPtr; }
			void AddScore(float value) noexcept { this->m_Score += value; }
			void SetScore(float value) noexcept { this->m_Score = value; }
			auto&			GetHoukiPtr(void) noexcept { return this->m_Houki_Ptr; }
			const auto		GetCharaDir(void) const noexcept {
				if (!this->m_Flightmode) {
					return this->m_UpperMatrix * this->m_move.mat;
				}
				else {
					return this->m_UpperMatrix * this->m_move.mat;
				}
			}
			const auto		GetEyeVector(void) const noexcept { return GetCharaDir().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept { return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + this->GetEyeVector().Norm() * 1.5f; }
			const auto&		GetScore(void) const noexcept { return this->m_Score; }
			const auto&		GetFlightPer(void) const noexcept { return this->m_InputSky.GetFlightPer(); }
			const auto		GetTurnRatePer(void) const noexcept { return this->m_InputGround.GetTurnRatePer(); }
		private: //更新関連
			//以前の状態保持														//
			void			ExecuteSavePrev(void) noexcept {
				this->m_PrevUpperAnimSel = this->m_UpperAnimSelect;
				this->m_PrevRunPer2 = this->m_RunPer2;
			}
			//操作																	//0.01ms
			void			ExecuteInput(void) noexcept {
				//
				this->m_SlingHoukiPer = 1.f - GetFlightPer();
				//this->m_yrad_Upper、this->m_yrad_Bottom、this->m_rad.z()決定
				this->m_InputGround.Execute();
				this->m_InputSky.Execute();
				if (this->m_Speed <= 0.1f) {
					if (abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) > deg2rad(50.f)) {
						this->m_TurnBody = true;
					}
					if (abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) < deg2rad(0.5f)) {
						this->m_TurnBody = false;
					}
				}
				else {
					this->m_TurnBody = false;
				}
				//
				auto FrontP = this->m_InputGround.GetFrontP();
				auto TmpRunPer = Leap(0.85f, 0.7f, this->m_InputGround.GetRunPer());
				if (this->m_Flightmode) { TmpRunPer = 0.95f; }
				if (this->m_TurnBody || (this->m_Speed > 0.1f) || this->m_Flightmode) { Easing(&this->m_yrad_Upper, this->m_InputGround.GetRad().y(), TmpRunPer, EasingType::OutExpo); }
				auto OLDP = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP, TmpRunPer, EasingType::OutExpo);

				if (this->m_Flightmode) {
					this->m_InputGround.SetRadBufX(0.f);
				}
				this->m_InputGround.SetRadBufZ((this->m_yrad_Bottom - OLDP) * 2.f);
			}
			//上半身回転															//0.06ms
			void			ExecuteUpperMatrix(void) noexcept {
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->m_obj.frame_Reset(this->Frames[(int)CharaFrame::Upper].first);
				SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * Leap_Matrix(this->m_UpperMatrix, MATRIX_ref::zero(), GetFlightPer()));
			}
			//SetMat指示															//0.03ms
			void			ExecuteAnim(void) noexcept {
				auto SE = SoundPool::Instance();
				//アニメ演算
				{
					//上半身
					{
						this->m_UpperAnimSelect = CharaAnimeID::Upper_Down;
						if (this->m_ReadySwitch) {
							this->m_RunReadyFirst = false;
							GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
						}
						if (!this->m_RunReady && !this->m_Running && !GetAnime(CharaAnimeID::Upper_RunningStart).TimeEnd()) {
							this->m_RunReady = true;
						}
						if (this->m_RunReadyFirst) {
							GetAnime(CharaAnimeID::Upper_RunningStart).GoStart();
							GetAnime(CharaAnimeID::Upper_RunningStart).per = 1.f;
						}
						if (!this->m_RunReady) {
							this->m_Running = false;
						}
						else if (!this->m_Running) {
							SetAnimOnce(CharaAnimeID::Upper_RunningStart, 2.f);
							this->m_InputGround.SetSprintPer(0.f);
							if (GetAnime(CharaAnimeID::Upper_RunningStart).TimeEnd()) {
								this->m_Running = true;
								GetAnime(CharaAnimeID::Upper_Running).GoStart();
							}
							m_UpperAnimSelect = CharaAnimeID::Upper_RunningStart;
						}
						else {
							m_UpperAnimSelect = (!this->m_InputGround.GetSprint()) ? CharaAnimeID::Upper_Running : CharaAnimeID::Upper_Sprint;
						}
						//首
						Easing(&m_NeckPer, (
							m_UpperAnimSelect == CharaAnimeID::Upper_Down
							|| m_UpperAnimSelect == CharaAnimeID::Upper_Running
							) ? 0.f : 1.f, 0.9f, EasingType::OutExpo);
					}
					//下半身
					{
						if (GetPressFrontGround()) {
							this->m_BottomAnimSelect = (this->m_InputGround.GetRun()) ? CharaAnimeID::Bottom_Run : CharaAnimeID::Bottom_Walk;
						}
						else {
							this->m_BottomAnimSelect = CharaAnimeID::Bottom_Stand;
							if (GetPressLeftGround()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_LeftStep; }
							if (GetPressRightGround()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_RightStep; }
							if (GetPressRearGround()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_WalkBack; }
							if (this->m_InputGround.GetSprint()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_Run; }
						}
					}
					//
					SetAnimLoop(CharaAnimeID::Upper_Sprint, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Upper_Running, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Turn, 0.5f);
					SetAnimLoop(CharaAnimeID::Bottom_Run, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Walk, 1.15f * this->m_InputGround.GetVecFront());
					SetAnimLoop(CharaAnimeID::Bottom_LeftStep, 1.15f * this->m_InputGround.GetVecLeft());
					SetAnimLoop(CharaAnimeID::Bottom_WalkBack, 1.15f * this->m_InputGround.GetVecRear());
					SetAnimLoop(CharaAnimeID::Bottom_RightStep, 1.15f * this->m_InputGround.GetVecRight());

					Easing(&this->m_LeftHandPer,
						(
							GetAnimeBuf(CharaAnimeID::Upper_RunningStart) > 0.1f
							|| GetAnimeBuf(CharaAnimeID::Upper_Running) > 0.5f
							|| GetAnimeBuf(CharaAnimeID::Upper_Sprint) > 0.5f
							|| GetAnimeBuf(CharaAnimeID::Upper_FlightNormal) > 0.1f
							) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
				}
				//アニメセレクト
				{
					//上半身
					GetAnimeBuf(CharaAnimeID::Upper_FlightNormal) = 1.f;
					//下半身
					GetAnimeBuf(CharaAnimeID::Bottom_FlightNormal) = 1.f;
					Easing(&GetAnimeBuf(CharaAnimeID::Bottom_Turn), (this->m_TurnBody) ? abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
					//その他
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//上半身
						if (
							//i == (int)CharaAnimeID::Upper_Flight ||
							i == (int)CharaAnimeID::Upper_Down ||
							i == (int)CharaAnimeID::Upper_RunningStart ||
							i == (int)CharaAnimeID::Upper_Running ||
							i == (int)CharaAnimeID::Upper_Sprint
							)
						{
							GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_UpperAnimSelect) ? 1.f : -1.f)*3.f / FPS;
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
							GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_BottomAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						GetAnimeBuf((CharaAnimeID)i) = std::clamp(GetAnimeBuf((CharaAnimeID)i), 0.f, 1.f);
						//反映
						GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i);
						if (
							i == (int)CharaAnimeID::Bottom_FlightNormal ||
							i == (int)CharaAnimeID::Upper_FlightNormal
							) {
							GetAnime((CharaAnimeID)i).per = GetAnime((CharaAnimeID)i).per * GetFlightPer();
						}
						else {
							GetAnime((CharaAnimeID)i).per = GetAnime((CharaAnimeID)i).per * (1.f - GetFlightPer());
						}
						//*/
					}
					//
				}
				//足音
				if (!this->m_Flightmode) {
					if (this->m_BottomAnimSelect != CharaAnimeID::Bottom_Stand) {
						auto Time = GetAnime(this->m_BottomAnimSelect).time;
						if (this->m_BottomAnimSelect != CharaAnimeID::Bottom_Run) {
							//L
							if ((9.f < Time && Time < 10.f)) {
								if (this->m_CharaSound != 1) {
									this->m_CharaSound = 1;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
								}
							}
							//R
							if ((27.f < Time &&Time < 28.f)) {
								if (this->m_CharaSound != 3) {
									this->m_CharaSound = 3;
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
								if (this->m_CharaSound != 5) {
									this->m_CharaSound = 5;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
								}
							}
							//R
							if (
								(8.f < Time &&Time < 9.f) ||
								(28.f < Time &&Time < 29.f)
								) {
								if (this->m_CharaSound != 6) {
									this->m_CharaSound = 6;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
								}
							}
						}
						this->m_ReturnStand = true;
					}
					else if (this->m_ReturnStand) {
						if (this->m_CharaSound != 7) {
							this->m_CharaSound = 7;
							SE->Get((int)SoundEnum::SlideFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f, (int)(192.f * this->m_RunPer2 / SpeedLimit));
						}
						this->m_ReturnStand = false;
					}
				}
				//
				this->m_obj.work_anime();
			}
			//SetMat指示更新														//0.03ms
			void			ExecuteMatrix(void) noexcept {
				this->m_RunPer2 = Leap(0.35f, (SpeedLimit * (1.f + 0.5f * this->m_InputGround.GetSprintPer())), this->m_InputGround.GetRunPer());
				if (this->m_PrevRunPer2 == 0.f) {
					this->m_PrevRunPer2 = this->m_RunPer2;
				}

				auto OLDpos = this->m_PosBuf;
				VECTOR_ref vecBuf = MATRIX_ref::Vtrans(this->m_InputGround.GetVec(), MATRIX_ref::RotY(this->m_yrad_Upper));
				this->m_Speed = std::clamp(vecBuf.size(), 0.f, 1.f);

				if (this->m_Speed > 0.1f) {
					this->m_move.vec.x((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).x());
					this->m_move.vec.z((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).z());
				}
				else {
					this->m_move.vec.x(vecBuf.x());
					this->m_move.vec.z(vecBuf.z());
				}
				{
					auto HitResult = this->m_MapCol->CollCheck_Line(this->m_PosBuf + VECTOR_ref::up() * -1.f, this->m_PosBuf + VECTOR_ref::up() * 15.f);
					if (HitResult.HitFlag == TRUE) {
						auto yPos = this->m_PosBuf.y();
						Easing(&yPos, HitResult.HitPosition.y, 0.8f, EasingType::OutExpo);
						this->m_PosBuf.y(yPos);
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
				this->m_PosBuf += this->m_move.vec;
				if (this->m_Flightmode) {
					Easing(&this->m_FlightVecBuf, (this->GetCharaDir().zvec() * -1.f) * 55.f * (12.5f / FPS), 0.95f, EasingType::OutExpo);
				}
				else {
					Easing(&this->m_FlightVecBuf, VECTOR_ref::zero(), 0.975f, EasingType::OutExpo);
				}
				this->m_PosBuf += this->m_FlightVecBuf;
				col_wall(OLDpos, &this->m_PosBuf, *this->m_MapCol);
				if (!this->m_Flightmode) {
					auto mat_tmp = MATRIX_ref::RotZ(this->m_InputGround.GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom);
					Easing_Matrix(&this->m_move.mat, mat_tmp, 0.8f, EasingType::OutExpo);
					this->m_FlightMatrix = this->m_move.mat;
				}
				else {
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), this->m_InputSky.GetFradAdd().x());
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_InputSky.GetFradAdd().y());
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.yvec(), this->m_InputSky.GetFradAdd().z());
					Easing_Matrix(&this->m_move.mat, this->m_FlightMatrix, 0.8f, EasingType::OutExpo);
				}
				Easing(&this->m_move.pos, this->m_PosBuf, 0.9f, EasingType::OutExpo);
				UpdateMove();
				//
				if (this->m_Houki_Ptr != nullptr) {
					//持ち手探索
					VECTOR_ref yVec1, zVec1, Pos1;
					{
						auto mat = (this->m_move.mat*GetCharaDir().Inverse());
						yVec1 = mat.yvec();
						zVec1 = mat.zvec()*-1.f;
						Pos1 = GetFrameWorldMat(CharaFrame::RightHandJoint).pos();
					}
					//背負い場所探索
					VECTOR_ref yVec2, zVec2, Pos2;
					{
						yVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * GetFrameWorldMat(CharaFrame::Upper2).GetRot() * GetCharaDir().Inverse()).xvec();
						zVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * GetFrameWorldMat(CharaFrame::Upper2).GetRot() * GetCharaDir().Inverse()).yvec();
						Pos2 =
							GetFrameWorldMat(CharaFrame::Upper2).pos() +
							GetFrameWorldMat(CharaFrame::Upper2).GetRot().yvec() * -1.75f +
							GetFrameWorldMat(CharaFrame::Upper2).GetRot().zvec() * 1.75f;
					}
					auto yVec = Leap(yVec1, yVec2, this->m_SlingHoukiPer);
					auto zVec = Leap(zVec1, zVec2, this->m_SlingHoukiPer);
					auto PosBuf = Leap(Pos1, Pos2, this->m_SlingHoukiPer);
					auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, -1).Norm(), zVec);
					tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec);
					tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);

					this->m_Houki_Ptr->SetMove(tmp_gunrat.GetRot(), tmp_gunrat.pos());
				}
			}
			//顔																	//0.01ms
			void			ExecuteShape(void) noexcept {
				SetShapePer(CharaShape::None, 0.f);
				if (this->m_EyeclosePer <= 0.05f && (GetRand(100) == 0)) {
					this->m_Eyeclose = 1;
				}
				if (this->m_EyeclosePer >= 0.95f) {
					this->m_Eyeclose = 0;
				}
				Easing(&this->m_EyeclosePer, (float)this->m_Eyeclose, 0.5f, EasingType::OutExpo);
				SetShapePer(CharaShape::EYECLOSE, this->m_EyeclosePer);
			}
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				ValueSet(deg2rad(0.f), deg2rad(0.f), VECTOR_ref::vget(0.f, 0.f, 0.f));
				this->m_Houki_Ptr.reset();
			}
			~CharacterClass(void) noexcept {}
		public: //継承
			//
			void move_LeftArm() noexcept {
				auto matBase = GetFrameWorldMat(CharaFrame::Upper2).GetRot().Inverse();
				//matBase = GetCharaDir().Inverse();
				ResetFrameLocalMat(CharaFrame::LeftArm);
				ResetFrameLocalMat(CharaFrame::LeftArm2);
				ResetFrameLocalMat(CharaFrame::LeftWrist);
				matBase = GetParentFrameWorldMat(CharaFrame::LeftArm).GetRot().Inverse();

				//VECTOR_ref GunPos = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
				//VECTOR_ref Gunyvec = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - GunPos;
				//VECTOR_ref Gunzvec = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - GunPos;

				VECTOR_ref GunPos = GetFrameWorldMat(CharaFrame::LeftWrist).pos();
				VECTOR_ref Gunyvec = GetFrameWorldMat(CharaFrame::LeftWrist).yvec();
				VECTOR_ref Gunzvec = GetFrameWorldMat(CharaFrame::LeftWrist).zvec();

				moves HandAim;// = this->LEFTHAND.move;
				HandAim.pos = Leap(GunPos, GetFrameWorldMat(CharaFrame::LeftWrist).pos(), this->m_LeftHandPer);
				//基準
				auto vec_a1 = MATRIX_ref::Vtrans((HandAim.pos - GetFrameWorldMat(CharaFrame::LeftArm).pos()).Norm(), matBase);//基準
				auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(0.f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
				//vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(0.f, -1.f, 0.f)).Norm();//x=0とする
				float cos_t = GetCosFormula((GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm).pos() - HandAim.pos).size());
				auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
				//上腕
				SetFrameLocalMat(CharaFrame::LeftArm, MGetIdent());
				MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos(), matBase), vec_t);
				SetFrameLocalMat(CharaFrame::LeftArm, a1_inv);
				//下腕
				matBase = GetParentFrameWorldMat(CharaFrame::LeftArm2).GetRot().Inverse();
				SetFrameLocalMat(CharaFrame::LeftArm2, MGetIdent());
				MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos(), matBase),
					MATRIX_ref::Vtrans(HandAim.pos - GetFrameWorldMat(CharaFrame::LeftArm2).pos(), matBase));
				SetFrameLocalMat(CharaFrame::LeftArm2, a2_inv);
				//手
				matBase = GetParentFrameWorldMat(CharaFrame::LeftWrist).GetRot().Inverse();
				MATRIX_ref mat1;
				MATRIX_ref mat2 = GetFrameLocalMat(CharaFrame::LeftWrist);
				{
					auto Lmat = GetFrameLocalMat(CharaFrame::LeftWrist);
					auto Wmat = GetFrameWorldMat(CharaFrame::LeftWrist);
					auto zvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(1, -1, 0), Wmat.GetRot());
					auto yvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(1, 1, 0), Wmat.GetRot());
					mat1 = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(zvec, matBase),
						MATRIX_ref::Vtrans(Gunzvec, matBase));
					SetFrameLocalMat(CharaFrame::LeftWrist, mat1);
					mat1 = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(yvec, matBase),
						MATRIX_ref::Vtrans(Gunyvec, matBase)) * mat1;

					mat1 = Lmat * mat1;
				}
				HandAim.mat = Leap_Matrix(mat1, mat2, this->m_LeftHandPer);
				SetFrameLocalMat(CharaFrame::LeftWrist, HandAim.mat);
			}
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				GetAnime(CharaAnimeID::Bottom_Stand).per=1.f;
				GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
				//
				SetCreate3DSoundFlag(TRUE);
				this->m_Breath = SoundHandle::Load("data/Sound/SE/voice/WinningTicket/breath.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Breath.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Breath.get());
			}
			void			Execute(void) noexcept override {
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < this->m_obj.get_anime().size(); i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
					this->m_obj.work_anime();
					this->m_RightWristRot = GetFrameLocalMat(CharaFrame::RightWrist).GetRot();
				}
				ExecuteSavePrev();			//以前の状態保持
				ExecuteInput();				//操作//0.01ms
				ExecuteUpperMatrix();		//上半身回転//0.06ms
				ExecuteAnim();				//AnimUpdte//0.03ms
				ExecuteMatrix();			//SetMat指示//0.03ms
				ExecuteShape();				//顔//スコープ内0.01ms
			}
			//void			Draw(void) noexcept override { ObjectBaseClass::Draw(); }
		public:
			//
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos) {
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					GetAnimeBuf((CharaAnimeID)i) = 0.f;
				}
				this->m_Speed = 0.f;
				this->m_RunPer2 = 0.f;
				this->m_PrevRunPer2 = 0.f;
				this->m_LeftHandPer = 0.f;
				this->m_TurnBody = false;
				this->m_RunReady = false;
				this->m_Running = false;
				this->m_Press_Shot = false;
				this->m_Press_Reload = false;
				this->m_Press_Aim = false;
				this->m_KeyActive = false;
				this->m_HeartSoundFlag = false;
				//
				this->m_Score = 0.f;
				//動作にかかわる操作
				this->m_InputGround.ValueSet(pxRad, pyRad);
				this->m_InputSky.ValueSet();
				this->m_PosBuf = pPos;
				//上記を反映するもの
				this->m_yrad_Upper = this->m_InputGround.GetRad().y();
				this->m_yrad_Bottom = this->m_InputGround.GetRad().y();
				this->m_PosBuf = pPos;
				this->m_move.vec.clear();
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), this->m_PosBuf);
			}
			//
			void			SetInput(const InputControl& pInput, bool pReady) {
				this->m_ReadySwitch = (this->m_KeyActive != pReady);
				this->m_KeyActive = pReady;
				//空
				this->m_Flightmode |= pInput.m_GoFlight && this->m_KeyActive;
				m_InputSky.SetInput(
					pInput.m_GoFrontPress && this->m_Flightmode,
					pInput.m_GoBackPress && this->m_Flightmode,
					pInput.m_GoLeftPress && this->m_Flightmode,
					pInput.m_GoRightPress && this->m_Flightmode,
					this->m_Flightmode,
					pInput.m_QPress && this->m_Flightmode,
					pInput.m_EPress && this->m_Flightmode
				);
				//地
				m_InputGround.SetInput(
					pInput.m_AddxRad*(1.f - GetFlightPer()), pInput.m_AddyRad*(1.f - GetFlightPer()),
					VECTOR_ref::zero(),
					pInput.m_GoFrontPress && !this->m_Flightmode,
					pInput.m_GoBackPress && !this->m_Flightmode,
					pInput.m_GoLeftPress && !this->m_Flightmode,
					pInput.m_GoRightPress && !this->m_Flightmode,
					pInput.m_RunPress && !this->m_Flightmode,
					pInput.m_QPress && !this->m_Flightmode,
					pInput.m_EPress && !this->m_Flightmode
				);
				//
				{
					auto TmpReady = GetIsRun();
					this->m_RunReadyFirst = (TmpReady && !this->m_RunReady);
					this->m_RunReady = TmpReady;
				}
			}
			//
			void			SetEyeVec(const VECTOR_ref& camvec) noexcept {
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
