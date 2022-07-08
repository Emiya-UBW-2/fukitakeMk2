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
			Circle,
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
			Bottom_Stand,//0
			Bottom_Walk,//1
			Bottom_WalkBack,//2
			Bottom_LeftStep,//3
			Bottom_RightStep,//4
			Bottom_Turn,//5
			Bottom_Run,//6

			Upper_Normal,//7
			Upper_Running,//8

			Bottom_Flight,//9
			Upper_Flight,//10

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
