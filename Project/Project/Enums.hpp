#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//�萔
	static const float Frame_Rate{ 60.f };

	enum class Effect {
		ef_fire,	//���C��
		ef_reco,	//�����a���e
		ef_smoke,	//�e�̋O��
		ef_gndsmoke,//�n�ʂ̋O��
		ef_fire2,	//���C��
		ef_hitblood,//�����Ԃ�
		ef_greexp,	//�O������
		ef_greexp2,	//�O������2
		effects,	//�ǂݍ���
	};
	namespace Sceneclass {
		enum class scenes
		{
			NONE_SCENE,
			LOAD_SCENE,
			MAIN_LOOP
		};
		enum class ObjType {
			Human,
			Gun,
			Target,
			Magazine,
			Circle,
			Gate,
			ShootingMat,
			Cart,
		};
		enum class SoundEnum {
			Shot_Gun,
			Trigger,
			Cocking0,
			Cocking1,
			Cocking2,
			Cocking3,
			RunFoot,
			SlideFoot,
			StandupFoot,
			Heart,
			GateOpen,
			Num,
		};
	};
	namespace Sceneclass {
		enum class CharaAnimeID : int {
			RightHand,
			LeftHand,

			Upper_Aim,
			Upper_Shot,
			Upper_Cocking,
			Upper_RunningStart,
			Upper_Running,
			Upper_Sprint,

			Mid_Squat,

			Bottom_Stand,
			Bottom_Walk,
			Bottom_WalkBack,
			Bottom_LeftStep,
			Bottom_RightStep,
			Bottom_Turn,
			Bottom_Run,

			All_Prone,
			All_ProneShot,
			All_ProneCocking,

			All_ProneWalk,

			All_PronetoStand,

			Upper_ReloadStart,
			Upper_ReloadOne,
			Upper_ReloadEnd,

			All_Prone_ReloadStart,
			All_Prone_ReloadOne,
			All_Prone_ReloadEnd,

			Upper_Ready,
			Upper_Down,

			AnimeIDMax,
		};
		enum class CharaFrame {
			Center,
			Upper,
			Upper2,
			Head,
			LeftEye,
			RightEye,
			LeftFoot,
			RightFoot,
			RightWrist,
			RightHandJoint,
			RightThumb,
			RightPointer,
			LeftArm,
			LeftArm2,
			LeftWrist,
			LeftHandJoint,
			Max,
		};
		const char* CharaFrameName[(int)CharaFrame::Max] = {
			"�Z���^�[",
			"�㔼�g",
			"�㔼�g2",
			"��",
			"���ڐ�",
			"�E�ڐ�",
			"������",
			"�E����",
			"�E���",
			"�E�_�~�[",
			"�E�l�w�R",
			"�E�e�w�Q",
			"���r",
			"���Ђ�",
			"�����",
			"���_�~�[",
		};
		enum class CharaShape {
			None,
			A,
			I,
			U,
			E,
			O,
			MAZIME,
			EYECLOSE,
			Max,
		};
		const char* CharaShapeName[(int)CharaShape::Max] = {
			"",
			"��",
			"��",
			"��",
			"��",
			"��",
			"�^�ʖ�",
			"�܂΂���",
		};
	};
	namespace Sceneclass {
		enum class GunAnimeID : int {
			Cocking,
			ReloadStart,
			ReloadOne,
			ReloadEnd,
			AnimeIDMax,
		};
		enum class GunFrame {
			Center,
			Cart,
			CartVec,
			Muzzle,
			Eyepos,
			Lens,
			LensSize,
			Magpos,
			LeftHandPos,
			LeftHandYvec,
			LeftHandZvec,
			Max,
		};
		const char* GunFrameName[(int)GunFrame::Max] = {
			"�Z���^�[",
			"cart",
			"cartvec",
			"muzzle",
			"EyePos",
			"lens",
			"lenssize",
			"magpos",
			"lefthand",
			"lefthand_yvec",
			"lefthand_zvec",
		};
	};
	namespace Sceneclass {
		enum class TargetAnimeID : int {
			Cocking,
			ReloadStart,
			ReloadOne,
			ReloadEnd,
			AnimeIDMax,
		};
		enum class TargetFrame {
			Center,
			Cart,
			CartVec,
			Muzzle,
			Eyepos,
			Lens,
			LensSize,
			Magpos,
			Max,
		};
		const char* TargetFrameName[(int)TargetFrame::Max] = {
			"�Z���^�[",
			"cart",
			"cartvec",
			"muzzle",
			"EyePos",
			"lens",
			"lenssize",
			"magpos",
		};
	};
};
