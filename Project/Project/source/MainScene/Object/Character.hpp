#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//����
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
		private: //����
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
			void			SetRadBufY(float y) noexcept {
				this->m_rad_Buf.y(y);
				this->m_rad.y(y);
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
				//����ɂ�����鑀��
				this->m_rad_Buf.x(pxRad);
				this->m_rad_Buf.y(pyRad);
				//��L�𔽉f�������
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
				bool pFlightMode,
				bool pQPress,
				bool pEPress
			) {
				this->m_Press_GoFront = pGoFrontPress && !pFlightMode;
				this->m_Press_GoRear = pGoBackPress && !pFlightMode;
				this->m_Press_GoLeft = pGoLeftPress && !pFlightMode;
				this->m_Press_GoRight = pGoRightPress && !pFlightMode;

				if (!this->m_IsRun && (pRunPress && !pFlightMode)) {
					this->m_RunTimer = 1.f;
				}
				if (this->m_RunTimer > 0.f) {
					this->m_RunTimer -= 1.f / FPS;
					this->m_Press_GoFront = true;
					this->m_IsRun = true;
				}
				else {
					this->m_RunTimer = 0.f;
					this->m_IsRun = pRunPress && !pFlightMode;
				}
				if (GetPressRear() || (!GetPressFront() && (GetPressLeft() || GetPressRight()))) {
					this->m_IsRun = false;
				}
				this->m_IsSprint = this->m_IsRun && (!GetPressFront() && !GetPressRear());
				{
					m_QKey.GetInput(pQPress && !pFlightMode);
					m_EKey.GetInput(pEPress && !pFlightMode);
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
						xadd = 0.315f*(-this->m_TurnRate);//�X�v�����g
					}
					else if (GetRun()) {
						xadd = 0.2f*(-this->m_TurnRate);//����
					}
					Easing(&this->m_TurnRatePer, xadd, 0.9f, EasingType::OutExpo);
				}
				//��]
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
				//�ړ��x�N�g���擾
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
			std::array<float, 7>								m_Vec{ 0,0,0,0,0,0,0 };
			bool												m_Flightmode{ false };
			bool												m_Press_Flight{ false };
			bool												m_Press_GoFront{ false };
			bool												m_Press_GoRear{ false };
			bool												m_Press_GoLeft{ false };
			bool												m_Press_GoRight{ false };
			switchs												m_QKey;
			switchs												m_EKey;
			bool												m_Press_Accel{ false };
			bool												m_Press_Brake{ false };
			float												m_FlightPer{ 0.f };
			VECTOR_ref											m_FradAdd_Buf, m_FradAdd;
			float												m_PressFlightTime{ 0.f };
		private: //����
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
			const auto		GetGoFlight() const noexcept { return this->m_Vec[6]; }
			const auto		GetPressFront() const noexcept { return this->m_Press_GoFront; }
			const auto		GetPressRear() const noexcept { return this->m_Press_GoRear; }
			const auto		GetPressLeft() const noexcept { return this->m_Press_GoLeft; }
			const auto		GetPressRight() const noexcept { return this->m_Press_GoRight; }
			const auto		GetFlightPer() const noexcept { return this->m_FlightPer; }
			const auto		GetFradAdd() const noexcept { return this->m_FradAdd; }

			const auto		GetPressAccel() const noexcept { return this->m_Press_Accel; }
			const auto		GetPressBrake() const noexcept { return this->m_Press_Brake; }

			const auto		GetIsFlightMode() const noexcept { return this->m_Flightmode; }
			void			SetIsFlightMode(bool value) noexcept { this->m_Flightmode = value; }

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
				bool pEPress,
				bool pPress_Accel,
				bool pPress_Brake
			) {
				if (this->m_PressFlightTime == 0.f && pFlightMode) {
					this->m_PressFlightTime = 5.f;
				}
				this->m_Press_Flight = (this->m_PressFlightTime > (5.f - 1.f));
				this->m_Flightmode |= this->m_Press_Flight;

				this->m_Press_GoFront = pGoFrontPress && this->m_Flightmode;
				this->m_Press_GoRear = pGoBackPress && this->m_Flightmode;
				this->m_Press_GoLeft = pGoLeftPress && this->m_Flightmode;
				this->m_Press_GoRight = pGoRightPress && this->m_Flightmode;
				m_QKey.GetInput(pQPress && this->m_Flightmode);
				m_EKey.GetInput(pEPress && this->m_Flightmode);
				this->m_Press_Accel = pPress_Accel && this->m_Flightmode;
				this->m_Press_Brake = pPress_Brake && this->m_Flightmode;
				if (m_EKey.trigger()) {
				}
				if (m_QKey.trigger()) {
				}
			}
			void			Execute(void) noexcept {
				//�ړ��x�N�g���擾
				{
					SetVec(0, GetPressFront());
					SetVec(1, GetPressLeft());
					SetVec(2, GetPressRear());
					SetVec(3, GetPressRight());

					SetVec(4, m_QKey.press());
					SetVec(5, m_EKey.press());

					SetVec(6, this->m_Press_Flight);
				}
				//
				this->m_PressFlightTime = std::max(this->m_PressFlightTime - 1.f / FPS, 0.f);
				//
				Easing(&this->m_FlightPer, this->m_Flightmode ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
				//��]
				this->m_FradAdd_Buf.x((GetVecRear() - GetVecFront())*2.f / 100.f);
				this->m_FradAdd_Buf.y((GetVecRight() - GetVecLeft())*4.f / 100.f);
				this->m_FradAdd_Buf.z((GetVecRightYaw() - GetVecLeftYaw())*0.6f / 100.f);
				Easing(&this->m_FradAdd, this->m_FradAdd_Buf, 0.95f, EasingType::OutExpo);
			}
		};

		class CharacterClass : public ObjectBaseClass {
		private://�L�����p�����[�^�[
			const float SpeedLimit{ 2.f };
			const float UpperTimerLimit = 3.f;

			const float FlightSpeedMin = 20.f;
			const float FlightSpeedMax = 200.f;
		private:
			CharacterMoveGroundControl							m_InputGround;
			CharacterMoveFlightControl							m_InputSky;
			bool												m_KeyActive{ true };
			//��s�֘A
			VECTOR_ref											m_FlightVecBuf;
			MATRIX_ref											m_FlightMatrix;
			VECTOR_ref											m_FlightKickReturnVecBuf;
			VECTOR_ref											m_FlightKickReturnVec;
			float												m_FlightKickReturnSpeed;
			bool												m_FlightKickReturn{ false };
			bool												m_FlightKickReturnAnimOne{ false };
			bool												m_FlightKickReturnAnimLR{ false };
			float												m_FlightSpeedAdd{ 0.f };
			float												m_FlightSpeed_r{ 0.f };
			float												m_FlightSpeed{ 0.f };
			float												m_FlightReturn{ 0.f };
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
			float												m_Score{ 0.f };			//�X�R�A
			//����
			bool m_Press_Shot{ false };
			bool m_ReadySwitch{ false };
			//�\��
			int													m_Eyeclose{ 0 };
			float												m_EyeclosePer{ 0.f };
			//�T�E���h
			int													m_CharaSound{ -1 };

			std::shared_ptr<HoukiClass>							m_Houki_Ptr{ nullptr };			//�
		public://�Q�b�^�[
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
			const auto		GetIsRun(void) const noexcept { return this->m_InputGround.GetRun(); }
			const auto		GetCharaDir(void) const noexcept { return this->m_UpperMatrix * this->m_move.mat; }
			const auto		GetEyeVecMat(void) const noexcept {
				MATRIX_ref Matrix;

				float per = Leap(1.f, 0.5f, this->m_FlightSpeed / (FlightSpeedMax - FlightSpeedMin))*10.f;
				Matrix *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), this->m_InputSky.GetFradAdd().x()*per);
				Matrix *= MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_InputSky.GetFradAdd().y()*per);
				Matrix *= MATRIX_ref::RotAxis(this->m_move.mat.yvec(), this->m_InputSky.GetFradAdd().z()*per);

				return (GetCharaDir()*Matrix);
			}
			const auto		GetEyePosition(void) const noexcept { return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + this->GetEyeVecMat().zvec() * -1.5f; }
			const auto		GetTurnRatePer(void) const noexcept { return this->m_InputGround.GetTurnRatePer(); }
			const auto&		GetScore(void) const noexcept { return this->m_Score; }
			auto&			GetHoukiPtr(void) noexcept { return this->m_Houki_Ptr; }
			const auto&		GetFlightPer(void) const noexcept { return this->m_InputSky.GetFlightPer(); }
			const auto&		GetFlightMode(void) const noexcept { return this->m_InputSky.GetIsFlightMode(); }
			const auto&		GetFlightSpeed(void) const noexcept { return this->m_FlightSpeed; }
			void			SetHoukiPtr(std::shared_ptr<HoukiClass>& pHoukiPtr) noexcept { this->m_Houki_Ptr = pHoukiPtr; }
			void			AddScore(float value) noexcept { this->m_Score += value; }
			void			SetScore(float value) noexcept { this->m_Score = value; }
		private: //�X�V�֘A
			//�ȑO�̏�ԕێ�														//
			void			ExecuteSavePrev(void) noexcept {
				this->m_PrevUpperAnimSel = this->m_UpperAnimSelect;
				this->m_PrevRunPer2 = this->m_RunPer2;
			}
			//����																	//0.01ms
			void			ExecuteInput(void) noexcept {
				//
				this->m_SlingHoukiPer = 1.f - GetFlightPer();
				//this->m_yrad_Upper�Athis->m_yrad_Bottom�Athis->m_rad.z()����
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
				if (m_InputSky.GetIsFlightMode()) { TmpRunPer = 0.95f; }
				if (this->m_TurnBody || (this->m_Speed > 0.1f) || m_InputSky.GetIsFlightMode()) { Easing(&this->m_yrad_Upper, this->m_InputGround.GetRad().y(), TmpRunPer, EasingType::OutExpo); }
				auto OLDP = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP, TmpRunPer, EasingType::OutExpo);

				if (m_InputSky.GetIsFlightMode()) {
					this->m_InputGround.SetRadBufX(0.f);
				}
				this->m_InputGround.SetRadBufZ((this->m_yrad_Bottom - OLDP) * 2.f);
			}
			//�㔼�g��]															//0.06ms
			void			ExecuteUpperMatrix(void) noexcept {
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->m_obj.frame_Reset(this->Frames[(int)CharaFrame::Upper].first);
				SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * Leap_Matrix(this->m_UpperMatrix, MATRIX_ref::zero(), GetFlightPer()));
			}
			//SetMat�w��															//0.03ms
			void			ExecuteAnim(void) noexcept {
				auto SE = SoundPool::Instance();
				//�A�j�����Z
				{
					//�㔼�g
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
						//��
						Easing(&m_NeckPer, (
							m_UpperAnimSelect == CharaAnimeID::Upper_Down
							|| m_UpperAnimSelect == CharaAnimeID::Upper_Running
							) ? 0.f : 1.f, 0.9f, EasingType::OutExpo);
					}
					//�����g
					{
						if (this->m_InputGround.GetPressFront()) {
							this->m_BottomAnimSelect = (this->m_InputGround.GetRun()) ? CharaAnimeID::Bottom_Run : CharaAnimeID::Bottom_Walk;
						}
						else {
							this->m_BottomAnimSelect = CharaAnimeID::Bottom_Stand;
							if (this->m_InputGround.GetPressLeft()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_LeftStep; }
							if (this->m_InputGround.GetPressRight()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_RightStep; }
							if (this->m_InputGround.GetPressRear()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_WalkBack; }
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
				}
				//�A�j���Z���N�g
				{
					//�㔼�g
					GetAnimeBuf(CharaAnimeID::Upper_FlightNormal) = 1.f;
					//�����g
					if (!this->m_FlightKickReturnAnimOne) {
						GetAnimeBuf(CharaAnimeID::Bottom_FlightNormal) = 1.f;

						GetAnimeBuf(CharaAnimeID::Bottom_FlightKickLeft) = 0.f;
						GetAnime(CharaAnimeID::Bottom_FlightKickLeft).GoStart();

						GetAnimeBuf(CharaAnimeID::Bottom_FlightKickRight) = 0.f;
						GetAnime(CharaAnimeID::Bottom_FlightKickRight).GoStart();
					}
					else {
						GetAnimeBuf(CharaAnimeID::Bottom_FlightNormal) = 0.f;
						if (this->m_FlightKickReturnAnimLR) {
							GetAnimeBuf(CharaAnimeID::Bottom_FlightKickLeft) = 1.f;
							SetAnimOnce(CharaAnimeID::Bottom_FlightKickLeft, 2.f);
							if (GetAnime(CharaAnimeID::Bottom_FlightKickLeft).TimeEnd()) {
								this->m_FlightKickReturnAnimOne = false;
							}
						}
						else {
							GetAnimeBuf(CharaAnimeID::Bottom_FlightKickRight) = 1.f;
							SetAnimOnce(CharaAnimeID::Bottom_FlightKickRight, 2.f);
							if (GetAnime(CharaAnimeID::Bottom_FlightKickRight).TimeEnd()) {
								this->m_FlightKickReturnAnimOne = false;
							}
						}
					}

					Easing(&GetAnimeBuf(CharaAnimeID::Bottom_Turn), (this->m_TurnBody) ? abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
					//���̑�
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//�㔼�g
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
						//�����g
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
						//���f
						GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i);
						if (
							i == (int)CharaAnimeID::Bottom_FlightKickLeft ||
							i == (int)CharaAnimeID::Bottom_FlightKickRight ||
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
				//����
				if (!m_InputSky.GetIsFlightMode()) {
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
			//SetMat�w���X�V														//0.03ms
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
							if (this->m_FlightSpeed_r <= FlightSpeedMin + (FlightSpeedMax - FlightSpeedMin) *0.3f) {
								if (m_InputSky.GetIsFlightMode()) {
									if (this->m_FlightReturn == 0.f) {
										this->m_FlightReturn = 5.f;
									}
									//
									VECTOR_ref cross = VECTOR_ref::front().cross(this->m_FlightMatrix.zvec());
									float dotFront = VECTOR_ref::front().dot(this->m_FlightMatrix.zvec());
									float dotUp = VECTOR_ref::up().dot(cross);
									this->m_yrad_Upper = std::atan2f(cross.size()*(dotUp >= 0.f ? 1.f : -1.f), dotFront);
									this->m_yrad_Bottom = this->m_yrad_Upper;
									this->m_InputGround.SetRadBufY(this->m_yrad_Upper);
								}
								m_InputSky.SetIsFlightMode(false);
							}
							else {
								if (!this->m_FlightKickReturn) {
									this->m_FlightKickReturn = true;
									this->m_FlightKickReturnSpeed = 1.f;
									this->m_FlightKickReturnVecBuf = HitResult.Normal;
									this->m_FlightKickReturnAnimOne = true;

									VECTOR_ref cross = this->GetCharaDir().yvec().cross(this->m_FlightKickReturnVecBuf);
									this->m_FlightKickReturnAnimLR = (this->GetCharaDir().zvec().dot(cross) > 0.f);
								}
							}
						}
						this->m_move.vec.y(0.f);
					}
					else {
						this->m_FlightKickReturn = false;
						if (!m_InputSky.GetIsFlightMode()) {
							this->m_move.vec.yadd(M_GR / (FPS * FPS));
						}
						else {

							if (this->m_FlightSpeed_r >= FlightSpeedMin) {
								this->m_move.vec.y(0.01f);
							}
							else {
								this->m_move.vec.yadd(M_GR / (FPS * FPS));
							}
						}
					}
				}
				this->m_PosBuf += this->m_move.vec;
				//
				if (m_InputSky.GetPressAccel()) {
					this->m_FlightSpeedAdd = std::clamp(this->m_FlightSpeedAdd + 0.1f / FPS, 0.f, 0.3f);
				}
				else if (m_InputSky.GetPressBrake() && (this->m_FlightSpeed_r >= FlightSpeedMin)) {
					this->m_FlightSpeedAdd = std::clamp(this->m_FlightSpeedAdd - 10.f / FPS, -0.75f, 0.f);
				}
				else {
					float per = GetCharaDir().zvec().y();
					Easing(&this->m_FlightSpeedAdd, (per>=0) ? (per*0.2f) : (per*0.05f), 0.95f, EasingType::OutExpo);
				}
				this->m_FlightSpeed_r = std::clamp(this->m_FlightSpeed_r + this->m_FlightSpeedAdd*60.f / FPS, 0.f, FlightSpeedMax);
				Easing(&this->m_FlightSpeed, this->m_FlightSpeed_r, 0.95f, EasingType::OutExpo);
				//
				{
					Easing(&this->m_FlightKickReturnVec, this->m_FlightKickReturnVecBuf*this->m_FlightKickReturnSpeed*200.f, 0.8f, EasingType::OutExpo);
					this->m_FlightKickReturnSpeed = std::max(this->m_FlightKickReturnSpeed - 1.f / FPS, 0.f);
				}
				//
				if (m_InputSky.GetIsFlightMode()) {
					Easing(&this->m_FlightVecBuf,
						(
							(this->GetCharaDir().zvec() * -1.f) * (this->m_FlightSpeed)
							+ VECTOR_ref::up() * (this->m_FlightSpeed * m_InputSky.GetGoFlight())
						) * 1000.f / 3600.f * (12.5f / FPS)
						, 0.95f, EasingType::OutExpo);
				}
				else {
					Easing(&this->m_FlightSpeedAdd, 0.f, 0.95f, EasingType::OutExpo);
					this->m_FlightSpeed_r = FlightSpeedMin + 30.f;
					Easing(&this->m_FlightSpeed, this->m_FlightSpeed_r, 0.95f, EasingType::OutExpo);

					Easing(&this->m_FlightVecBuf, VECTOR_ref::zero(), 0.975f, EasingType::OutExpo);
				}
				this->m_PosBuf += this->m_FlightVecBuf + this->m_FlightKickReturnVec * 1000.f / 3600.f* (12.5f / FPS);
				col_wall(OLDpos, &this->m_PosBuf, *this->m_MapCol);
				if (!m_InputSky.GetIsFlightMode()) {
					auto mat_tmp = MATRIX_ref::RotZ(this->m_InputGround.GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom);
					Easing_Matrix(&this->m_move.mat, mat_tmp, 0.8f, EasingType::OutExpo);
					this->m_FlightMatrix = this->m_move.mat;
				}
				else {
					float per = Leap(1.f, 0.5f, this->m_FlightSpeed / (FlightSpeedMax - FlightSpeedMin));

					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), this->m_InputSky.GetFradAdd().x()*per);
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_InputSky.GetFradAdd().y()*per);
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.yvec(), this->m_InputSky.GetFradAdd().z()*per);
					Easing_Matrix(&this->m_move.mat, this->m_FlightMatrix, 0.8f, EasingType::OutExpo);
				}
				Easing(&this->m_move.pos, this->m_PosBuf, 0.9f, EasingType::OutExpo);
				UpdateMove();
				//
				if (this->m_Houki_Ptr != nullptr) {
					//������T��
					VECTOR_ref yVec1, zVec1, Pos1;
					{
						auto mat = (this->m_move.mat*GetCharaDir().Inverse());
						yVec1 = mat.yvec();
						zVec1 = mat.zvec()*-1.f;
						Pos1 = GetFrameWorldMat(CharaFrame::RightHandJoint).pos();
					}
					//�w�����ꏊ�T��
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
			//��																	//0.01ms
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
		public: //�R���X�g���N�^�A�f�X�g���N�^
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				ValueSet(deg2rad(0.f), deg2rad(0.f), VECTOR_ref::vget(0.f, 0.f, 0.f));
				this->m_Houki_Ptr.reset();
			}
			~CharacterClass(void) noexcept {}
		public: //�p��
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				GetAnime(CharaAnimeID::Bottom_Stand).per=1.f;
				GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
			}
			void			Execute(void) noexcept override {
				//����̂ݍX�V������e
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < this->m_obj.get_anime().size(); i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
					this->m_obj.work_anime();
					this->m_RightWristRot = GetFrameLocalMat(CharaFrame::RightWrist).GetRot();
				}
				ExecuteSavePrev();			//�ȑO�̏�ԕێ�
				ExecuteInput();				//����//0.01ms
				ExecuteUpperMatrix();		//�㔼�g��]//0.06ms
				ExecuteAnim();				//AnimUpdte//0.03ms
				ExecuteMatrix();			//SetMat�w��//0.03ms
				ExecuteShape();				//��//�X�R�[�v��0.01ms
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
				this->m_TurnBody = false;
				this->m_RunReady = false;
				this->m_Running = false;
				this->m_Press_Shot = false;
				this->m_KeyActive = false;
				this->m_FlightKickReturn = false;
				this->m_FlightKickReturnAnimOne = false;
				this->m_FlightKickReturnAnimLR = true;
				//
				this->m_Score = 0.f;
				//����ɂ�����鑀��
				this->m_InputGround.ValueSet(pxRad, pyRad);
				this->m_InputSky.ValueSet();
				this->m_PosBuf = pPos;
				//��L�𔽉f�������
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

				bool pressFront = false;
				bool pressFlight = false;

				auto prev = this->m_FlightReturn;
				this->m_FlightReturn = std::max(this->m_FlightReturn - 1.f / FPS, 0.f);
				if (this->m_FlightReturn > 0.f) {
					pressFront = true;
				}
				if (prev > 0 && this->m_FlightReturn == 0.f) {
					pressFlight = true;
				}

				//��
				m_InputSky.SetInput(
					pInput.m_GoFrontPress,
					pInput.m_GoBackPress,
					pInput.m_GoLeftPress,
					pInput.m_GoRightPress,
					pInput.m_GoFlight || pressFlight,
					pInput.m_QPress,
					pInput.m_EPress,
					pInput.m_Press_Accel,
					pInput.m_Press_Brake
				);
				//�n
				m_InputGround.SetInput(
					pInput.m_AddxRad*(1.f - GetFlightPer()), pInput.m_AddyRad*(1.f - GetFlightPer()),
					VECTOR_ref::zero(),
					pInput.m_GoFrontPress && !pressFront,
					pInput.m_GoBackPress && !pressFront,
					pInput.m_GoLeftPress && !pressFront,
					pInput.m_GoRightPress && !pressFront,
					pInput.m_RunPress || pressFront,
					m_InputSky.GetIsFlightMode(),
					pInput.m_QPress,
					pInput.m_EPress
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
