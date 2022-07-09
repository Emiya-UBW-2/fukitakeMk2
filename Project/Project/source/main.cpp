#include"Header.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	OPTION::Create();
	auto* OptionParts = OPTION::Instance();
	OptionParts->Load();							//�ݒ�ǂݍ���
	DXDraw::Create("FPS_n2", FPS_n2::Frame_Rate);	//�ėp

	//MV1SetLoadModelUsePackDraw(TRUE);

#ifdef DEBUG
	DeBuG::Create(FPS_n2::Frame_Rate);
#endif // DEBUG
	OptionParts->Set_useVR(DXDraw::Instance()->use_vr);

	auto scene = std::make_unique<FPS_n2::Sceneclass::SceneControl>();
	//�V�F�[�_�[
	PostPassEffect::Create();
	//�V�[��
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//
	FPS_n2::effectControl.Init();																						//�G�t�F�N�g
	//�J�ڐ�w��
	MAINLOOPscene->Set_Next(MAINLOOPscene, FPS_n2::Sceneclass::scenes::MAIN_LOOP);
	//�J�n���J�ڐ�
	scene->ChangeScene(FPS_n2::Sceneclass::scenes::MAIN_LOOP, (std::shared_ptr<FPS_n2::Sceneclass::TEMPSCENE>&)MAINLOOPscene);
	//�J��Ԃ�
	do {
		scene->StartScene();
		while (true) {
			if (scene->Execute()) { break; }
			if (!scene->isPause()) {
				FPS_n2::effectControl.Calc();			//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
			}
			scene->Draw();
			//60FPS��ҋ@
			scene->Vsync();
		}
		//�I������
		scene->NextScene();
	} while (!scene->isEnd());
	FPS_n2::effectControl.Dispose();
	return 0;
}
