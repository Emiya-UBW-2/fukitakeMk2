#pragma once
#include"Header.hpp"

//Box2D�g��
namespace std {
	template <>
	struct default_delete<b2Body> {
		void operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
//
namespace FPS_n2 {
	//����
	class InputControl {
	public:
		float m_AddxRad; float m_AddyRad;

		bool m_GoFrontPress;
		bool m_GoBackPress;
		bool m_GoLeftPress;
		bool m_GoRightPress;

		bool m_SquatPress;
		bool m_PronePress;
		bool m_ShotPress;
		bool m_AimPress;
		bool m_RunPress;
		bool m_QPress;
		bool m_EPress;
		bool m_ReloadPress;
		bool m_GoFlight;
	public:
		void			SetInput(
			float pAddxRad, float pAddyRad,
			bool pGoFrontPress,
			bool pGoBackPress,
			bool pGoLeftPress,
			bool pGoRightPress,

			bool pSquatPress,
			bool pPronePress,
			bool pShotPress,
			bool pAimPress,
			bool pRunPress,
			bool pQPress,
			bool pEPress,
			bool pReloadPress,
			bool pGoFlight
		) {
			this->m_AddxRad = pAddxRad;
			this->m_AddyRad = pAddyRad;

			this->m_GoFrontPress = pGoFrontPress;
			this->m_GoBackPress = pGoBackPress;
			this->m_GoLeftPress = pGoLeftPress;
			this->m_GoRightPress = pGoRightPress;

			this->m_SquatPress = pSquatPress;
			this->m_PronePress = pPronePress;
			this->m_ShotPress = pShotPress;
			this->m_AimPress = pAimPress;
			this->m_RunPress = pRunPress;
			this->m_QPress = pQPress;
			this->m_EPress = pEPress;
			this->m_ReloadPress = pReloadPress;
			this->m_GoFlight = pGoFlight;
		}
	};


	//�t�H���g�v�[��
	class FontPool {
	public:
		class Fonthave {
			int size = 0;
			FontHandle handle;
		public:
			const auto& Get_size(void)const noexcept { return size; }
			const auto& Get_handle(void)const noexcept { return handle; }
			void Set(int siz_t) noexcept {
				this->size = siz_t;
				this->handle = FontHandle::Create("x14y24pxHeadUpDaisy", siz_t, DX_FONTTYPE_EDGE, -1, 2);
			}
		};
	private:
		std::vector<Fonthave> havehandle;
		size_t Add(int siz_t) noexcept {
			for (auto& h : this->havehandle) {
				if (h.Get_size() == siz_t) {
					return &h - &this->havehandle.front();
				}
			}
			this->havehandle.resize(this->havehandle.size() + 1);
			this->havehandle.back().Set(siz_t);
			return this->havehandle.size() - 1;
		}
	public:
		Fonthave& Get(int siz_t) noexcept { return this->havehandle[Add(siz_t)]; }
	};
	FontPool Fonts;
	//�G�t�F�N�g���\�[�X
	class EffectControl {
		LONGLONG Update_effect_was = 0;					//�G�t�F�N�g�̃A�b�v�f�[�g�^�C�~���O�^�C�}�[
	public:
		bool Update_effect_f{ true };					//�G�t�F�N�g�̃A�b�v�f�[�g�^�C�~���O�t���O
		std::vector<EffekseerEffectHandle> effsorce;	/*�G�t�F�N�g���\�[�X*/
		void Init(void) noexcept {
			auto data_t = GetFileNamesInDirectory("data/effect/");
			for (const auto& d : data_t) {
				std::string p = d.cFileName;
				if (p.find(".efk") != std::string::npos && p.find(".efkproj") == std::string::npos) {
					effsorce.resize(effsorce.size() + 1);
					effsorce.back() = EffekseerEffectHandle::load("data/effect/" + p);
				}
			}
			effsorce.resize(effsorce.size() + 1);
			effsorce.back() = EffekseerEffectHandle::load("data/effect/gndsmk.efk");								//��ԗp�G�t�F�N�g
			Update_effect_was = GetNowHiPerformanceCount();
		}
		void Calc(void) noexcept {
			Update_effect_f = ((GetNowHiPerformanceCount() - Update_effect_was) >= 1000000 / 60);
			if (Update_effect_f) {
				Update_effect_was = GetNowHiPerformanceCount();
			}
		}
		void Dispose(void) noexcept {
			for (auto& e : effsorce) {
				e.Dispose();
			}
		}
	};
	EffectControl effectControl;
	//�G�t�F�N�g���p�R���g���[��
	class Effect_UseControl {
		std::array<EffectS, int(Effect::effects)> effcs;	/*�G�t�F�N�g*/
		std::array<EffectS, 256> effcs_G;					/*�G�t�F�N�g*/
		int G_cnt = 0;
	public:
		void Set_FootEffect(const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept {
			this->effcs_G[this->G_cnt].Stop();
			this->effcs_G[this->G_cnt].Set(pos_t, nomal_t, scale);
			++this->G_cnt %= 256;
		}
		const auto Check_FootEffectCnt(void) noexcept {
			int cnt = 0;
			for (auto& t : this->effcs_G) {
				if (t.GetStart()) { cnt++; }
			}
			return cnt;
		}

		void Set_LoopEffect(Effect ef_, const VECTOR_ref& pos_t) noexcept {
			this->effcs[(int)ef_].Stop();
			this->effcs[(int)ef_].pos = pos_t;
			this->effcs[(int)ef_].set_loop(effectControl.effsorce[(int)ef_]);
		}
		void Update_LoopEffect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept {
			this->effcs[(int)ef_].put_loop(pos_t, nomal_t, scale);
		}

		void Set_Effect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept { this->effcs[(int)ef_].Set(pos_t, nomal_t, scale); }
		void Stop_Effect(Effect ef_) noexcept { this->effcs[(int)ef_].Stop(); }

		void SetSpeed_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Speed(value); }
		void SetScale_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Scale(value); }
		//�G�t�F�N�g�̍X�V
		void Update_Effect(void) noexcept {
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs.front();
				if (index != (int)Effect::ef_smoke) {
					t.put(effectControl.effsorce[index]);
				}
			}
			for (auto& t : this->effcs_G) {
				t.put(effectControl.effsorce[(int)Effect::ef_gndsmoke]);
			}
		}
		/*�����*/
		void Dispose_Effect(void) noexcept {
			for (auto& t : this->effcs) { t.handle.Dispose(); }
		}
	};
	//���C�g
	class LightPool {
		class Lights {
		public:
			LightHandle handle;
			LONGLONG time{ 0 };
		};
		std::array<Lights, 2> handles;
		int now = 0;
		VECTOR_ref campos;
	public:
		void Put(const VECTOR_ref& pos) noexcept {
			if ((pos - campos).size() >= 10.f) { return; }
			if (handles[now].handle.get() != -1) {
				handles[now].handle.Dispose();
			}
			handles[now].time = GetNowHiPerformanceCount();
			handles[now].handle = LightHandle::Create(pos, 2.5f, 0.5f, 1.5f, 0.5f);
			SetLightDifColorHandle(handles[now].handle.get(), GetColorF(1.f, 1.f, 0.f, 1.f));
			++now %= handles.size();
		}
		void Update(const VECTOR_ref& campos_t) noexcept {
			campos = campos_t;
			for (auto& h : handles) {
				if (h.handle.get() != -1) {
					if ((GetNowHiPerformanceCount() - h.time) >= 1000000 / 30) {
						h.handle.Dispose();
					}
				}
			}
		}
	};
	LightPool Light_pool;

	// �v���C���[�֌W�̒�`
#define PLAYER_ENUM_DEFAULT_SIZE	(1.8f * 12.5f)		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			(0.4f * 12.5f)		// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			(1.8f * 12.5f)		// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			(16)				// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * 12.5f)	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
	//�ǔ��胆�j�o�[�T��
	static bool col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos, const MV1& col_obj_t) noexcept {
		auto MoveVector = *NowPos - OldPos;
		// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
		auto HitDim = col_obj_t.CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), 0, 0);
		std::vector<MV1_COLL_RESULT_POLY*> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
		// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
		for (int i = 0; i < HitDim.HitNum; ++i) {
			auto& h_d = HitDim.Dim[i];
			//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{0.1f��荂���|���S���̂ݓ����蔻����s��
			if (
				(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
				&& (h_d.Position[0].y > OldPos.y() + 0.1f || h_d.Position[1].y > OldPos.y() + 0.1f || h_d.Position[2].y > OldPos.y() + 0.1f)
				&& (h_d.Position[0].y < OldPos.y() + 1.6f || h_d.Position[1].y < OldPos.y() + 1.6f || h_d.Position[2].y < OldPos.y() + 1.6f)
				) {
				kabe_.emplace_back(&h_d);// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
			}
		}
		bool HitFlag = false;
		// �ǃ|���S���Ƃ̓����蔻�菈��
		if (kabe_.size() > 0) {
			HitFlag = false;
			for (auto& KeyBind : kabe_) {
				if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
					if (MoveVector.size() >= 0.0001f) {	// x����y�������� 0.0001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
						// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
						*NowPos = VECTOR_ref(KeyBind->Normal).cross(MoveVector.cross(KeyBind->Normal)) + OldPos;
						bool j = false;
						for (auto& b_ : kabe_) {
							if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {
								j = true;
								break;// �������Ă����烋�[�v���甲����
							}
						}
						if (!j) {
							HitFlag = false;
							break;//�ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂ŕǂɓ��������t���O��|������Ń��[�v���甲����
						}
					}
					else {
						break;
					}
				}
			}
			//*
			if (
				HitFlag
				) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
				for (int k = 0; k < PLAYER_HIT_TRYNUM; ++k) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
					bool HitF = false;
					for (auto& KeyBind : kabe_) {
						if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
							*NowPos += VECTOR_ref(KeyBind->Normal) * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
							bool j = false;
							for (auto& b_ : kabe_) {
								if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {// �������Ă����烋�[�v�𔲂���
									j = true;
									break;
								}
							}
							if (!j) {// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
								break;
							}
							HitF = true;
						}
					}
					if (!HitF) {//�S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
						break;
					}
				}
			}
			//*/
			kabe_.clear();
		}
		MV1CollResultPolyDimTerminate(HitDim);	// ���o�����v���C���[�̎��͂̃|���S�������J������
		return HitFlag;
	}
};